#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <syslog.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include "netsim.h"

#define CONFIG_FILE "netsim.json"

// configuration
static const char *eth_dev = "wlp61s0";
static const char *status_file = "status.json";
//static const char *profiles_file = "profiles.json";

struct tc_settings {
  long delay, jitter, delay_corr;
  const char *delay_dist;
  const char *delay_unit;
  const char *jitter_unit;

  const char *loss_model;
  long loss; // random loss
  long p13, p31, p32, p23, p14;  // state loss
  long p, r, h_bar, k_bar; // gemodel
  bool ecn;
  long corrupt, corrupt_corr; // corruption
  long duplicate, duplicate_corr; // duplication
  long reorder, reorder_corr, reorder_gap; // reordering
  long bandwidth; // bandwidth
};

static cJSON *s_jstatus = NULL;
static bool s_enabled = false;
static pthread_mutex_t status_mutex = PTHREAD_MUTEX_INITIALIZER;

#define MUTEX_LOCK() pthread_mutex_lock(&status_mutex)
#define MUTEX_UNLOCK() pthread_mutex_unlock(&status_mutex)

static bool execute(char *command)
{
  printf("Executing: %s\n", command);
  int rc = system(command);
  if (!WIFEXITED(rc)) {
    return false;
  }

  if (WEXITSTATUS(rc) != 0) {
    return false;
  }

  return true;
}

static void netsim_clear_settings(void)
{
  char buffer[512];
  snprintf(buffer, sizeof(buffer), "tc qdisc del dev %s root", eth_dev);
  execute(buffer);
}

static bool netsim_apply_settings(struct tc_settings *tcs)
{
  bool success = false;
  const char *parent = "root";
  char buffer[512];
  bool tbf_applied = false;
  bool netem_applied = false;
  char *p = buffer;
  char *end = buffer + sizeof(buffer);

  netsim_clear_settings();

  // bandwidth
  if (tcs->bandwidth != -1) {
    snprintf(buffer, sizeof(buffer), "tc qdisc add dev %s %s handle 1: tbf rate %ldbit burst %ld latency 4s",
        eth_dev, parent, tcs->bandwidth, (tcs->bandwidth * 8) / 250);
    parent = "parent 1:0";
    if (!execute(buffer)) {
      goto exit;
    }
    tbf_applied = true;
  }

  // delay
  snprintf(p, end-p, "tc qdisc add dev %s %s handle 10: netem", eth_dev, parent); p += strlen(p);
  if (tcs->delay != -1) {
    snprintf(p, end-p, " delay %ld%s", tcs->delay, tcs->delay_unit); p += strlen(p);
    if (tcs->jitter != -1) {
      snprintf(p, end-p, " %ld%s", tcs->jitter, tcs->jitter_unit); p += strlen(p);
      if (tcs->delay_corr != -1) {
        snprintf(p ,end-p, " %ld%%", tcs->delay_corr); p += strlen(p);
      }
    }
    netem_applied = true;
  }

  // loss
  if (!strcmp(tcs->loss_model, "random") && (tcs->loss != -1)) {
    snprintf(p, end-p, " loss random %ld%%", tcs->loss); p += strlen(p);
    netem_applied = true;
  } else if (!strcmp(tcs->loss_model, "state") && (tcs->p13 != -1)) {
    snprintf(p, end-p, " loss state %ld%%", tcs->p13); p += strlen(p);
    if (tcs->p31 != -1) {
      snprintf(p, end-p, " %ld%%", tcs->p31); p += strlen(p);
      if (tcs->p32 != -1) {
        snprintf(p, end-p, " %ld%%", tcs->p32); p += strlen(p);
        if (tcs->p23 != -1) {
          snprintf(p, end-p, " %ld%%", tcs->p23); p += strlen(p);
          if (tcs->p14 != -1) {
            snprintf(p, end-p, " %ld%%", tcs->p14); p += strlen(p);
          }
        }
      }
    }
    netem_applied = true;
  } else if (!strcmp(tcs->loss_model, "gemodel") && (tcs->p != -1)) {
    snprintf(p, end-p, " loss gemodel %ld%%", tcs->p); p += strlen(p);
    if (tcs->r != -1) {
      snprintf(p, end-p, " %ld%%", tcs->r); p += strlen(p);
      if (tcs->h_bar != -1) {
        snprintf(p, end-p, " %ld%%", tcs->h_bar); p += strlen(p);
        if (tcs->k_bar != -1) {
          snprintf(p, end-p, " %ld%%", tcs->k_bar); p += strlen(p);
        }
      }
    }
    netem_applied = true;
  }

  // corrupt
  if (tcs->corrupt != -1) {
    snprintf(p, end-p, " corrupt %ld%%", tcs->corrupt); p += strlen(p);
    if (tcs->corrupt_corr != -1) {
      snprintf(p, end-p, " %ld%%", tcs->corrupt_corr); p += strlen(p);
    }
    netem_applied = true;
  }

  // duplicate
  if (tcs->duplicate != -1) {
    snprintf(p, end-p, " duplicate %ld%%", tcs->duplicate); p += strlen(p);
    if (tcs->duplicate_corr != -1) {
      snprintf(p, end-p, " %ld%%", tcs->duplicate_corr); p += strlen(p);
    }
    netem_applied = true;
  }

  // reorder
  if (tcs->reorder != -1) {
    snprintf(p, end-p, " reorder %ld%%", tcs->reorder); p += strlen(p);
    if (tcs->reorder_corr != -1) {
      snprintf(p, end-p, " %ld%%", tcs->reorder_corr); p += strlen(p);
    }
    if (tcs->reorder_gap != -1) {
      snprintf(p, end-p, " gap %ld%%", tcs->reorder_gap); p += strlen(p);
    }
    netem_applied = true;
  }

  if (netem_applied) {
    parent = "parent 10:0";
    if (!execute(buffer)) {
      goto exit;
    }
  }

  if (netem_applied && tbf_applied) {
    snprintf(buffer, sizeof(buffer), "tc qdisc add dev %s %s pfifo", eth_dev, parent);
    if (!execute(buffer)) {
      goto exit;
    }
  }

  success = true;

exit:
  if (!success) {
    syslog(LOG_ERR, "Apply new status failed! Clear the settings!");
    netsim_clear_settings();
  }

  return success;
}

static bool netsim_get_long(cJSON *json, const char *name, long default_value,
    long min_value, long max_value, long *output)
{
  cJSON *item = cJSON_GetObjectItem(json, name);
  double v = 0;

  if (!item) {
    *output = default_value;
    return true;
  }

  if (!cJSON_IsNumber(item)) {
    syslog(LOG_ERR, "%s is not a number", name);
    return false;
  }

  v = cJSON_GetNumberValue(item);
  if ((v != default_value) && ((v < min_value) || (v > max_value))) {
    syslog(LOG_ERR, "value '%ld' out of range [%ld, %ld]", (long)v, min_value, max_value);
    return false;
  }

  *output = (long) v;
  return true;
}

static bool netsim_get_string(cJSON *json, const char *name, const char *default_value, const char **output)
{
  cJSON *item = cJSON_GetObjectItem(json, name);
  if (!item) {
    *output = default_value;
    return true;
  }

  if (!cJSON_IsString(item)) {
    syslog(LOG_ERR, "%s is not a string", name);
    return false;
  }

  *output = cJSON_GetStringValue(item);
  return true;
}

static bool netsim_get_bool(cJSON *json, const char *name, bool default_value, bool *output)
{
  cJSON *item = cJSON_GetObjectItem(json, name);
  if (!item) {
    *output = default_value;
    return true;
  }

  if (!cJSON_IsBool(item)) {
    syslog(LOG_ERR, "%s is not a bool", name);
    return false;
  }

  *output = cJSON_IsTrue(item) ? true : false;
  return true;
}

static bool netsim_get_time_unit(cJSON *json, const char *name, const char *default_value, const char **output)
{
  if (!netsim_get_string(json, name, default_value, output)) {
    return false;
  }

  if (strcmp(*output, "s") &&
      strcmp(*output, "ms") &&
      strcmp(*output, "us") &&
      strcmp(*output, "ns")) {
    return false;
  }

  return true;
}

#if 0
static long netsim_time_multipler(const char *s)
{
  if (!strcmp(s, "s")) {
    return 1000000L;
  } else if (!strcmp(s, "ms")) {
    return 1000L;
  } else if (!strcmp(s, "us")) {
    return 1L;
  } else {
    return 1000L;
  }
}
#endif

static long netsim_bandwidth_multiplier(const char *s)
{
  if (!strcmp(s, "bps")) {
    return 1L;
  } else if (!strcmp(s, "kbps")) {
    return 1000L;
  } else if (!strcmp(s, "Mbps")) {
    return 1000000L;
  } else if (!strcmp(s, "Gbps")) {
    return 1000000000L;
  } else if (!strcmp(s, "Bps")) {
    return 8L;
  } else if (!strcmp(s, "kBps")) {
    return 8000L;
  } else if (!strcmp(s, "MBps")) {
    return 8000000L;
  } else if (!strcmp(s, "GBps")) {
    return 8000000000L;
  } else {
    return 1000000L;
  }
}

static bool netsim_apply_profile(cJSON *jprofile)
{
  struct tc_settings tcs;
  memset(&tcs, 0, sizeof(tcs));
  const char *bandwidth_unit = NULL;

  if (// delay
      !netsim_get_long(jprofile, "delay", -1, 0, LONG_MAX, &tcs.delay) ||
      !netsim_get_time_unit(jprofile, "delay_unit", "ms", &tcs.delay_unit) ||
      !netsim_get_long(jprofile, "jitter", -1, 0, LONG_MAX, &tcs.jitter) ||
      !netsim_get_time_unit(jprofile, "jitter_unit", "ms", &tcs.jitter_unit) ||
      !netsim_get_long(jprofile, "delay_corr", -1, 0, 100, &tcs.delay_corr) ||
      !netsim_get_string(jprofile, "delay_dist", "normal", &tcs.delay_dist) ||

      // loss
      !netsim_get_string(jprofile, "loss_model", "random", &tcs.loss_model) ||
      !netsim_get_long(jprofile, "loss", -1, 0, 100, &tcs.loss) ||
      // state loss model
      !netsim_get_long(jprofile, "p13", -1, 0, 100, &tcs.p13) ||
      !netsim_get_long(jprofile, "p31", -1, 0, 100, &tcs.p31) ||
      !netsim_get_long(jprofile, "p32", -1, 0, 100, &tcs.p32) ||
      !netsim_get_long(jprofile, "p23", -1, 0, 100, &tcs.p23) ||
      !netsim_get_long(jprofile, "p14", -1, 0, 100, &tcs.p14) ||
      // gemodel loss model
      !netsim_get_long(jprofile, "p", -1, 0, 100, &tcs.p) ||
      !netsim_get_long(jprofile, "r", -1, 0, 100, &tcs.r) ||
      !netsim_get_long(jprofile, "h_bar", -1, 0, 100, &tcs.h_bar) ||
      !netsim_get_long(jprofile, "k_bar", -1, 0, 100, &tcs.k_bar) ||
      // ecn
      !netsim_get_bool(jprofile, "ecn", false, &tcs.ecn) ||
      // corrupt
      !netsim_get_long(jprofile, "corrupt", -1, 0, 100, &tcs.corrupt) ||
      !netsim_get_long(jprofile, "corrupt_corr", -1, 0, 100, &tcs.corrupt_corr) ||
      // duplicate
      !netsim_get_long(jprofile, "duplicate", -1, 0, 100, &tcs.duplicate) ||
      !netsim_get_long(jprofile, "duplicate_corr", -1, 0, 100, &tcs.duplicate_corr) ||
      // reordering
      !netsim_get_long(jprofile, "reorder", -1, 0, 100, &tcs.reorder) ||
      !netsim_get_long(jprofile, "reorder_corr", -1, 0, 100, &tcs.reorder_corr) ||
      !netsim_get_long(jprofile, "reorder_gap", -1, 0, LONG_MAX, &tcs.reorder_gap) ||
      // bandwidth
      !netsim_get_long(jprofile, "bandwidth", -1, 0, LONG_MAX, &tcs.bandwidth) ||
      !netsim_get_string(jprofile, "bandwidth_unit", "MBps", &bandwidth_unit)) {
    return false;
  }

  if (tcs.delay != -1) {
    if (((tcs.delay_corr != -1) && (tcs.jitter == -1))) {
      syslog(LOG_ERR, "delay parameters invalid: [%ld [%ld [%ld]]]",
          tcs.delay, tcs.jitter, tcs.delay_corr);
      return false;
    }

    if (strcmp(tcs.delay_dist, "uniform") &&
        strcmp(tcs.delay_dist, "normal") &&
        strcmp(tcs.delay_dist, "pareto") &&
        strcmp(tcs.delay_dist, "paretonormal")) {
      syslog(LOG_ERR, "Invalid delay distribution: %s", tcs.loss_model);
      return false;
    }
  }

  if (!strcmp(tcs.loss_model, "random")) {

  } else if (!strcmp(tcs.loss_model, "state")) {
    if (((tcs.p31 != -1) && (tcs.p13 == -1)) ||
        ((tcs.p32 != -1) && ((tcs.p13 == -1) || (tcs.p31 == -1))) ||
        ((tcs.p23 != -1) && ((tcs.p13 == -1) || (tcs.p31 == -1) || (tcs.p32 == -1))) ||
        ((tcs.p14 != -1) && ((tcs.p13 == -1) || (tcs.p31 == -1) || (tcs.p32 == -1) || (tcs.p23 == -1)))) {
      syslog(LOG_ERR, "state loss parameters invalid: [%ld [%ld [%ld [%ld [%ld]]]]]",
          tcs.p13, tcs.p31, tcs.p32, tcs.p23, tcs.p14);
      return false;
    }
  } else if (!strcmp(tcs.loss_model, "gemodel")) {
    if (((tcs.r != -1) && (tcs.p == -1)) ||
        ((tcs.h_bar != -1) && ((tcs.p == -1) || (tcs.r == -1))) ||
        ((tcs.k_bar != -1) && ((tcs.p == -1) || (tcs.r == -1) || (tcs.h_bar == -1)))) {
      syslog(LOG_ERR, "gemodel loss parameters invalid: [%ld [%ld [%ld [%ld]]]]",
          tcs.p, tcs.r, tcs.h_bar, tcs.k_bar);
      return false;
    }
  } else {
    syslog(LOG_ERR, "Invalid loss model: %s", tcs.loss_model);
    return false;
  }

  if (tcs.bandwidth != -1) {
    tcs.bandwidth *= netsim_bandwidth_multiplier(bandwidth_unit);
  }

  if (!netsim_apply_settings(&tcs)) {
    return false;
  }

  return true;
}

static bool netsim_make_status_json(bool enabled, cJSON *jprofile, cJSON **new_jstatus)
{
  cJSON *jstatus = cJSON_CreateObject();
  if (!jstatus) {
    return false;
  }

  cJSON_AddBoolToObject(jstatus, "enabled", enabled);
  if (jprofile) {
    cJSON_AddItemToObject(jstatus, "profile", jprofile);
  }

  *new_jstatus = jstatus;
  return true;
}

static void netsim_status_set_current(cJSON *jstatus)
{
  if (s_jstatus) {
    cJSON_Delete(s_jstatus);
  }
  s_jstatus = jstatus;
}

static void netsim_status_set_enabled(cJSON *jstatus, bool enabled)
{
  cJSON_DeleteItemFromObject(jstatus, "enabled");
  cJSON_AddBoolToObject(jstatus, "enabled", enabled);
}

static bool netsim_load_profile(const char *filename, cJSON **new_jprofile)
{
  bool retval = false;
  cJSON *jprofile = NULL;
  int fd = -1;
  char *buffer = NULL;
  struct stat sb;
  ssize_t nread = 0;

  if ((fd = open(filename, O_RDONLY)) == -1) {
    // ok if profile not exists
    *new_jprofile = NULL;
    retval = true;
    goto exit;
  }

  if (fstat(fd, &sb) == -1) {
    syslog(LOG_ERR, "fstat: %s", strerror(errno));
    goto exit;
  }

  buffer = malloc(sb.st_size);
  if (!buffer) {
    goto exit;
  }

  if ((nread = read(fd, buffer, sb.st_size)) == -1) {
    syslog(LOG_ERR, "read: %s", strerror(errno));
    goto exit;
  }

  jprofile = cJSON_ParseWithLength(buffer, sb.st_size);
  if (!jprofile) {
    goto exit;
  }

  *new_jprofile = jprofile;
  jprofile = NULL;
  retval = true;

exit:
  cJSON_Delete(jprofile);
  free(buffer);
  if (fd != -1) {
    close(fd);
  }
  return retval;
}

static bool netsim_write_profile(const char *filename, cJSON *jprofile)
{
  bool retval = false;
  int fd = -1;
  char *buffer = NULL;
  ssize_t nwritten = 0;

  buffer = cJSON_Print(jprofile);
  if (!buffer) {
    syslog(LOG_ERR, "Cannot stringify json");
    goto exit;
  }

  if ((fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1) {
    syslog(LOG_ERR, "open (write): %s", strerror(errno));
    goto exit;
  }

  if ((nwritten = write(fd, buffer, strlen(buffer))) == -1) {
    syslog(LOG_ERR, "write: %s", strerror(errno));
    goto exit;
  }

  retval = true;

exit:
  cJSON_free(buffer);
  if (fd != -1) {
    close(fd);
  }
  return retval;
}

bool netsim_status_init(void)
{
  bool retval = false;
  cJSON *jstatus = NULL;
  cJSON *jprofile = NULL;

  // Turn off everything
  netsim_clear_settings();

  // load profile
  if (!netsim_load_profile(status_file, &jprofile)) {
    goto exit;
  }

  if (!jprofile) {
    jprofile = cJSON_CreateObject();
    if (!netsim_write_profile(status_file, jprofile)) {
      goto exit;
    }
  }

  if (!netsim_make_status_json(s_enabled, jprofile, &jstatus)) {
    goto exit;
  }
  jprofile = NULL;

  netsim_status_set_current(jstatus);
  jstatus = NULL;
  retval = true;

exit:
  cJSON_Delete(jstatus);
  cJSON_Delete(jprofile);
  return retval;
}

void netsim_status_lock(void)
{
  MUTEX_LOCK();
}

void netsim_status_unlock(void)
{
  MUTEX_UNLOCK();
}

cJSON *netsim_status_get(void)
{
  return s_jstatus;
}

bool netsim_status_put(cJSON **jstatus)
{
  bool retval = false;
  bool enabled = false;
  cJSON *elem = NULL;
  cJSON *jprofile = NULL;

  if (!cJSON_IsObject(*jstatus)) {
    syslog(LOG_ERR, "status is not object");
    goto exit;
  }

  elem = cJSON_GetObjectItem(*jstatus, "enabled");
  if (!elem) {
    enabled = s_enabled;
    cJSON_AddBoolToObject(*jstatus, "enabled", enabled);
  } else if (cJSON_IsTrue(elem)) {
    enabled = true;
  } else if (cJSON_IsFalse(elem)) {
    enabled = false;
  } else {
    syslog(LOG_ERR, "enabled is not bool");
    goto exit;
  }

  jprofile = cJSON_GetObjectItem(*jstatus, "profile");

  if (enabled) {
    if (!netsim_apply_profile(jprofile)) {
      netsim_status_set_enabled(*jstatus, false);
      enabled = false;
      goto save_and_exit;
    }
  } else {
    netsim_clear_settings();
  }

  retval = true;

save_and_exit:
  s_enabled = enabled;
  netsim_status_set_current(*jstatus);
  *jstatus = NULL;
  if (!netsim_write_profile(status_file, jprofile)) {
    retval = false;
    goto exit;
  }

exit:
  return retval;
}

