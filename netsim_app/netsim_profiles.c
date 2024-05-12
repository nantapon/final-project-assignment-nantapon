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

// configuration
static const char *profiles_file = "profiles.json";

static cJSON *s_jprofiles = NULL;
static pthread_mutex_t profiles_mutex = PTHREAD_MUTEX_INITIALIZER;

#define MUTEX_LOCK() pthread_mutex_lock(&profiles_mutex)
#define MUTEX_UNLOCK() pthread_mutex_unlock(&profiles_mutex)

static bool netsim_load_profiles(const char *filename, cJSON **new_jprofiles)
{
  bool retval = false;
  cJSON *jprofiles = NULL;
  int fd = -1;
  char *buffer = NULL;
  struct stat sb;
  ssize_t nread = 0;

  if ((fd = open(filename, O_RDONLY)) == -1) {
    // ok if profile not exists
    *new_jprofiles = NULL;
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

  jprofiles = cJSON_ParseWithLength(buffer, sb.st_size);
  if (!jprofiles) {
    goto exit;
  }

  *new_jprofiles = jprofiles;
  jprofiles = NULL;
  retval = true;

exit:
  cJSON_Delete(jprofiles);
  free(buffer);
  if (fd != -1) {
    close(fd);
  }
  return retval;
}

static bool netsim_write_profiles(const char *filename, cJSON *jprofiles)
{
  bool retval = false;
  int fd = -1;
  char *buffer = NULL;
  ssize_t nwritten = 0;

  buffer = cJSON_Print(jprofiles);
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

bool netsim_profiles_init(const char *arg_dir)
{
  bool retval = false;
  cJSON *jprofiles = NULL;
  char buffer[PATH_MAX];

  snprintf(buffer, sizeof(buffer), "%s/profiles.json", arg_dir);
  profiles_file = strdup(buffer);
  if (!profiles_file) {
    goto exit;
  }

  // load profile
  if (!netsim_load_profiles(profiles_file, &jprofiles)) {
    goto exit;
  }

  if (!jprofiles) {
    jprofiles = cJSON_CreateObject();
    if (!netsim_write_profiles(profiles_file, jprofiles)) {
      goto exit;
    }
  }

  s_jprofiles = jprofiles;
  jprofiles = NULL;
  retval = true;

exit:
  cJSON_Delete(jprofiles);
  return retval;
}

void netsim_profiles_lock(void)
{
  MUTEX_LOCK();
}

void netsim_profiles_unlock(void)
{
  MUTEX_UNLOCK();
}

cJSON *netsim_profiles_get(void)
{
  return s_jprofiles;
}

bool netsim_profiles_put(cJSON **jprofiles)
{
  bool retval = false;

  if (!cJSON_IsObject(*jprofiles)) {
    syslog(LOG_ERR, "profiles is not object");
    goto exit;
  }

  if (!netsim_write_profiles(profiles_file, *jprofiles)) {
    goto exit;
  }

  s_jprofiles = *jprofiles;
  *jprofiles = NULL;
  retval = true;

exit:
  return retval;
}

