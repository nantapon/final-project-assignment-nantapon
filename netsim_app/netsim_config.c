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
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include "netsim.h"

static cJSON *s_jconfig = NULL;

bool
netsim_addr_to_name(struct sockaddr *sa, int salen, char **out_addr)
{
  char host[1025];
  char serv[32];
  int err = 0;
  char *output = NULL;
  char *ch = NULL;

  if ((err = getnameinfo(sa, salen, host, sizeof(host), serv, sizeof(serv), NI_NUMERICHOST | NI_NUMERICSERV)) != 0) {
    syslog(LOG_ERR, "getnameinfo returns error: %s", gai_strerror(err));
    return false;
  }

  ch = strchr(host, '%');
  if (ch) {
    *ch = 0;
  }

  output = strdup(host);
  if (!output) {
    return false;
  }

  *out_addr = output;
  return true;
}

static bool
netsim_get_interfaces_info(const char *arg_dev, char **addr, char **addr6)
{
  bool found = false;
  struct ifaddrs *ifaddr = NULL;
  struct ifaddrs *ifa = NULL;

  *addr = NULL;
  *addr6 = NULL;
  if (getifaddrs(&ifaddr) == -1) {
    syslog(LOG_ERR, "Cannot get interface list: %s", strerror(errno));
    goto exit;
  }

  for (ifa = ifaddr; ifa ; ifa = ifa->ifa_next) {

    if (strcmp(ifa->ifa_name, arg_dev) != 0) {
      continue;
    }

    if (!ifa->ifa_addr) {
      continue;
    }

    found = true;

    switch (ifa->ifa_addr->sa_family) {
      case AF_INET:
        if (*addr) {
          break;
        }
        if (!netsim_addr_to_name(ifa->ifa_addr, sizeof(struct sockaddr_in), addr)) {
          goto exit;
        }
        break;

      case AF_INET6:
        if (*addr6) {
          break;
        }
        if (!netsim_addr_to_name(ifa->ifa_addr, sizeof(struct sockaddr_in6), addr6)) {
          goto exit;
        }
        // Don't use link-local address
        if (!strncmp(*addr6, "fe80::", 6)) {
          free(*addr6);
          *addr6 = NULL;
          break;
        }
        break;

      default:
        break;
    }
  }

  if (!found) {
    goto exit;
  }

  freeifaddrs(ifaddr);
  return true;

exit:
  if (*addr) {
    free(*addr);
    *addr = NULL;
  }
  if (*addr6) {
    free(*addr6);
    *addr = NULL;
  }

  if (ifaddr) {
    freeifaddrs(ifaddr);
  }

  return false;
}


bool netsim_config_init(const char *arg_dev)
{
  cJSON *jconfig = NULL;
  char *addr = NULL;
  char *addr6 = NULL;

  if (!netsim_get_interfaces_info(arg_dev, &addr, &addr6)) {
    return false;
  }

  jconfig = cJSON_CreateObject();
  cJSON_AddStringToObject(jconfig, "addr", addr ? addr : "<netsim-ip-addr>");
  cJSON_AddStringToObject(jconfig, "addr6", addr6 ? addr6 : "<netsim-ipv6-addr");

  s_jconfig = jconfig;
  free(addr);
  free(addr6);
  return true;
}

cJSON *netsim_config_get(void)
{
  return s_jconfig;
}

