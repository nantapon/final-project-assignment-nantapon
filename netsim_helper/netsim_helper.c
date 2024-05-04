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

#define STATUS_FILE "/var/run/netsim.cfg"
#define SAVE_DIR "/etc/netsim/configs"

static const char *arg_configfile = NULL;
static const char *arg_dev = "eth0";
static long arg_delay = -1;
static long arg_loss = -1;
static long arg_bandwidth = -1;

static bool process_args(int argc, char **argv)
{
  int opt;

  while ((opt = getopt(argc, argv, "d:")) != -1) {
    switch (opt) {
    case 'd':
      arg_dev = optarg;
      break;
    default: /* '?' */
      fprintf(stderr, "Usage: %s [-d dev] netsim-file\n", argv[0]);
      return false;
    }
  }

  if (optind >= argc) {
     fprintf(stderr, "Expected argument after options\n");
     return false;
  }

  arg_configfile = argv[optind];
  return true;
}

static bool to_long(const char *value, long *out_value, long min_value, long max_value)
{
  char *end = NULL;
  long v = 0;



  v = strtol(value, &end, 10);
  if (errno == ERANGE) {
    return false;
  }
  if (!*value || *end) {
    return false;
  }

  if ((v < min_value) || (v > max_value)) {
    syslog(LOG_ERR, "value '%ld' out of range [%ld, %ld]", v, min_value, max_value);
    return false;
  }

  *out_value = v;
  return true;
}

static bool process_config(const char *key, const char *value)
{
  if (!strcasecmp(key, "delay")) {
    return to_long(value, &arg_delay, 0, LONG_MAX);
  } else if (!strcasecmp(key, "loss")) {
    return to_long(value, &arg_loss, 0, 100);
  } else if (!strcasecmp(key, "bandwidth")) {
    return to_long(value, &arg_bandwidth, 0, LONG_MAX);
  } else {
    syslog(LOG_ERR, "Unknown key: %s", key);
    return false;
  }
}

static char *read_identifier(char **p)
{
  char *start = NULL;

  // skip ws
  while (**p && isspace(**p)) {
    (*p)++;
  }

  if (!**p) {
    return NULL;
  }

  start = *p;
  while (**p && (isalnum(**p) || (**p == '_'))) {
    (*p)++;
  }

  if (start == *p) {
    return NULL;
  }

  return strndup(start, *p - start);
}

static bool expect_symbol(char **p, char sym)
{
  // skip ws
  while (**p && isspace(**p)) {
    (*p)++;
  }

  if (!**p) {
    return false;
  }

  if (**p != sym) {
    return false;
  }

  (*p)++;
  return true;
}

static bool expect_eol(char **p)
{
  // skip ws
  while (**p && isspace(**p)) {
    (*p)++;
  }

  if (**p) {
    return false;
  }

  return true;
}

static bool process_line(char *line)
{
  bool success = false;
  char *p = line;
  char *key = NULL;
  char *value = NULL;

  if (!(key = read_identifier(&p))) {
    if (!*p) {
      success = true;
      goto exit;
    }
    goto exit;
  }

  if (!expect_symbol(&p, '=')) {
    goto exit;
  }

  if (!(value = read_identifier(&p))) {
    goto exit;
  }

  if (!expect_eol(&p)) {
    goto exit;
  }

  if (!process_config(key, value)) {
    goto exit;
  }

  success = true;

exit:
  if (!success) {
    syslog(LOG_ERR, "Invalid line: %s", line);
    fprintf(stderr, "Invalid line: %s\n", line);
  }
  free(key);
  free(value);
  return success;
}

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

static void clear_settings(void)
{
  char buffer[512];
  snprintf(buffer, sizeof(buffer), "tc qdisc del dev %s root", arg_dev);
  execute(buffer);
  unlink(STATUS_FILE);
}

static bool apply_settings(void)
{
  bool success = false;
  const char *parent = "root";
  char buffer[512];

  clear_settings();

  if ((arg_bandwidth == -1) && (arg_delay == -1) && (arg_loss == -1)) {
    success = true;
    goto exit;
  }

  // Apply new settings
  if (arg_bandwidth != -1) {
    snprintf(buffer, sizeof(buffer), "tc qdisc add dev %s %s handle 1: tbf rate %ld burst %ld latency 4s",
        arg_dev, parent, arg_bandwidth, arg_bandwidth / 250);
    parent = "parent 1:0";
    if (!execute(buffer)) {
      goto exit;
    }
  }

  if ((arg_delay != -1) || (arg_loss != -1)) {
    snprintf(buffer, sizeof(buffer), "tc qdisc add dev %s %s handle 10: netem", arg_dev, parent);
    if (arg_delay != -1) {
      snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), " delay %ldms", arg_delay);
    }
    if (arg_loss != -1) {
      snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), " loss %ld%%", arg_loss);
    }
    parent = "parent 10:0";
    if (!execute(buffer)) {
      goto exit;
    }
  }

  snprintf(buffer, sizeof(buffer), "tc qdisc add dev %s %s pfifo", arg_dev, parent);
  if (!execute(buffer)) {
    goto exit;
  }

  success = true;
exit:
  if (!success) {
    syslog(LOG_ERR, "Apply new status failed! Clear the settings!");
    clear_settings();
  }

  return success;
}

static bool save_config(void)
{
  FILE *fp = NULL;

  if ((fp = fopen(STATUS_FILE, "w")) == NULL) {
    return false;
  }

  if (arg_delay != -1) {
    fprintf(fp,  "delay=%ld\n", arg_delay);
  }

  if (arg_loss != -1) {
    fprintf(fp,  "loss=%ld\n", arg_loss);
  }

  if (arg_bandwidth != -1) {
    fprintf(fp,  "bandwidth=%ld\n", arg_bandwidth);
  }

  if(fp) {
    fclose(fp);
  }
  return true;
}

static bool read_config_file(void)
{
  bool success = false;
  FILE *fp = NULL;
  char *buffer = NULL;
  size_t sz = 0;
  ssize_t nread = 0;

  fp = fopen(arg_configfile, "r");
  if (!fp) {
    syslog(LOG_ERR, "Cannot open file '%s': %s", arg_configfile, strerror(errno));
    fprintf(stderr, "Cannot open file '%s': %s\n", arg_configfile, strerror(errno));
    goto exit;
  }

  while ((nread = getline(&buffer, &sz, fp)) != -1) {
    // trim new line
    if (buffer[nread-1] == '\n') {
      buffer[nread-1] = 0;
    }

    if (!process_line(buffer)) {
      goto exit;
    }
  }

  if (!apply_settings()) {
    goto exit;
  }

  if (!save_config()) {
    goto exit;
  }

  success = true;

exit:
  free(buffer);
  if (fp) {
    fclose(fp);
  }
  return success;
}

int main(int argc, char **argv)
{
  int rc = 1;
  openlog(NULL, 0, LOG_USER);

  if (!process_args(argc, argv)) {
    goto exit;
  }

  if (!read_config_file()) {
    syslog(LOG_ERR, "Cannot apply netsim config");
    fprintf(stderr, "Cannot apply netsim config\n");
    goto exit;
  }

  rc = 0;
  printf("Successfully applied netsim config!\n");

exit:
  closelog();
  return rc;
}
