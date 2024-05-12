#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <civetweb.h>
#include <cjson/cJSON.h>
#include "netsim.h"

static struct mg_callbacks callbacks;
static struct mg_context *ctx;

static bool arg_daemon = false;
static const char *arg_dir="/var/lib/netsim";
static const char *arg_dev="eth0";
static const char *arg_www_dir="/www";
static const char *arg_ports="8080";

struct netsim_handlers
{
  int (*do_get)(struct mg_connection *conn, void *arg);
  int (*do_put)(struct mg_connection *conn, void *arg);
  int (*do_delete)(struct mg_connection *conn, void *arg);
};

static int
netsim_send_json(struct mg_connection *conn, cJSON *json_obj)
{
  char *json_str = cJSON_PrintUnformatted(json_obj);
  size_t json_str_len = strlen(json_str);

  mg_send_http_ok(conn, "application/json; charset=utf-8", json_str_len + 1);
  mg_write(conn, json_str, json_str_len);
  mg_write(conn, "\n", 1);

  cJSON_free(json_str);
  return (int)json_str_len;
}

// Already locked
static int netsim_handle_status_get_internal(struct mg_connection *conn, void *arg)
{
  cJSON *jstatus = netsim_status_get();
  if (!jstatus) {
    mg_send_http_error(conn, 500, "Server error");
    return 500;
  }
  netsim_send_json(conn, jstatus);
  return 200;
}

static int netsim_handle_status_get(struct mg_connection *conn, void *arg)
{
  int retcode = 0;
  netsim_status_lock();
  retcode = netsim_handle_status_get_internal(conn, arg);
  netsim_status_unlock();
  return retcode;
}

#define READ_BODY \
  char buffer[1024]; \
  int nread = 0; \
  size_t total = 0; \
  char *p = buffer; \
  char *p_end = p + sizeof(buffer); \
  char *alloc_buffer = NULL; \
\
  for (;;) { \
    nread = mg_read(conn, p + total, p_end - p - total); \
    if (nread <= 0) { \
      break; \
    } else { \
      total += nread; \
      if (p + total == p_end) { \
        size_t new_size = 2 * (p_end - p); \
        if (new_size > 1024*1024) { \
          syslog(LOG_ERR, "Too big input"); \
          goto exit; \
        } \
        char *new_buf = malloc(new_size); \
        if (!new_buf) { \
          goto exit; \
        } \
\
        memcpy(new_buf, p, total); \
        free(alloc_buffer); \
        alloc_buffer = new_buf; \
        p = alloc_buffer; \
        p_end = p + new_size; \
      } \
    } \
  }


static int netsim_handle_status_put(struct mg_connection *conn, void *arg)
{
  int retcode = 0;
  cJSON *jstatus = NULL;

  // Read to p and total
  READ_BODY

  jstatus = cJSON_ParseWithLength(p, total);
  if (!jstatus) {
    syslog(LOG_ERR, "Cannot parse json");
    goto exit;
  }

  netsim_status_lock();
  if (!netsim_status_put(&jstatus)) {
    netsim_status_unlock();
    goto exit;
  }
  free(alloc_buffer);

  retcode = netsim_handle_status_get_internal(conn, arg);
  netsim_status_unlock();
  return retcode;

exit:
  cJSON_free(jstatus);
  free(alloc_buffer);
  mg_send_http_error(conn, 500, "Server error");
  return 500;
}

static struct netsim_handlers status_handlers = {
  netsim_handle_status_get,
  netsim_handle_status_put,
  NULL
};

static int netsim_handle_profiles_get(struct mg_connection *conn, void *arg)
{
  cJSON *jprofiles = netsim_profiles_get();
  if (!jprofiles) {
    mg_send_http_error(conn, 500, "Server error");
    return 500;
  }
  netsim_send_json(conn, jprofiles);
  return 200;
}

static int netsim_handle_profiles_put(struct mg_connection *conn, void *arg)
{
  cJSON *jprofiles = NULL;

  // Read to p and total
  READ_BODY

  jprofiles = cJSON_ParseWithLength(p, total);
  if (!jprofiles) {
    syslog(LOG_ERR, "Cannot parse json");
    goto exit;
  }

  if (!netsim_profiles_put(&jprofiles)) {
    goto exit;
  }
  free(alloc_buffer);

  return netsim_handle_profiles_get(conn, arg);

exit:
  cJSON_free(jprofiles);
  free(alloc_buffer);
  mg_send_http_error(conn, 500, "Server error");
  return 500;
}

static struct netsim_handlers profiles_handlers = {
  netsim_handle_profiles_get,
  netsim_handle_profiles_put,
  NULL
};

static int netsim_handle(struct mg_connection *conn, void *arg)
{
  struct netsim_handlers *handlers = arg;
  const struct mg_request_info *ri = mg_get_request_info(conn);

  /* According to method */
  if (!strcmp(ri->request_method, "GET")) {
    if (handlers->do_get) {
      return handlers->do_get(conn, arg);
    }
  } else if (!strcmp(ri->request_method, "PUT")
      || !strcmp(ri->request_method, "POST")
      || !strcmp(ri->request_method, "PATCH")) {
    if (handlers->do_put) {
      return handlers->do_put(conn, arg);
    }
  } else if (!strcmp(ri->request_method, "DELETE")) {
    if (handlers->do_delete) {
      return handlers->do_delete(conn, arg);
    }
  }

  mg_send_http_error(conn, 405, "method not allowed\n");
  return 405;
}

static void init_callbacks(void)
{
}

int start_web(void)
{
  struct mg_init_data init;
  struct mg_error_data error;
  char error_text[256];
  const char *options[] = {
    "document_root", arg_www_dir,
    "listening_ports", arg_ports,
    NULL
  };

  init_callbacks();
  mg_init_library(0);

  memset(&init, 0, sizeof(init));
  init.callbacks = &callbacks;
  init.user_data = NULL;
  init.configuration_options = options;

  memset(&error, 0, sizeof(error));
  error.text = error_text;
  error.text_buffer_size = sizeof(error_text);

  ctx = mg_start2(&init, &error);
  if (!ctx) {
    syslog(LOG_ERR, "Fail to start: %s", error_text);
    fprintf(stderr, "Fail to start: %s\n", error_text);
    return 1;
  }

  mg_set_request_handler(ctx, "/status", netsim_handle, &status_handlers);
  mg_set_request_handler(ctx, "/profiles", netsim_handle, &profiles_handlers);
  for(;;) {
    sleep(10);
  }

  return 0;
}

static bool process_args(int argc, char **argv)
{
  int opt;

  while ((opt = getopt(argc, argv, "de:s:w:p:")) != -1) {
    switch (opt) {
    case 'd':
      arg_daemon = true;
      break;

    case 'e':
      arg_dev = optarg;
      break;

    case 's':
      arg_dir = optarg;
      break;

    case 'w':
      arg_www_dir = optarg;
      break;

    case 'p':
      arg_ports = optarg;
      break;

    default: /* '?' */
      fprintf(stderr, "Usage: %s [-d] [-e dev] [-s dir] [-w www_dir] [-p ports]\n", argv[0]);
      return false;
    }
  }
  return true;
}

int main(int argc, char **argv)
{
  openlog(NULL, 0, LOG_USER);

  process_args(argc, argv);

  if (arg_daemon) {
    pid_t childpid = fork();
    if (childpid != 0) {
      // quit parent
      _exit(0);
    }
    setsid();
    if (chdir("/") == -1) {
      syslog(LOG_ERR, "chdir error: %s", strerror(errno));
      return 1;
    }
    if ((freopen("/dev/null", "r", stdin) == NULL) ||
        (freopen("/dev/null", "w", stdout) == NULL) ||
        (freopen("/dev/null", "r", stderr) == NULL)) {
      syslog(LOG_ERR, "Error in redirecting i/o!: %s", strerror(errno));
      return 1;
    }
  }

  netsim_status_init(arg_dev, arg_dir);
  netsim_profiles_init(arg_dir);
  return start_web();
}
