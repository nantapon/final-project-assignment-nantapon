#include <cjson/cJSON.h>
#include <stdbool.h>

bool netsim_status_init(void);
void netsim_status_lock(void);
void netsim_status_unlock(void);
cJSON *netsim_status_get(void);
bool netsim_status_put(cJSON **json);

bool netsim_profiles_init(void);
void netsim_profiles_lock(void);
void netsim_profiles_unlock(void);
cJSON *netsim_profiles_get(void);
bool netsim_profiles_put(cJSON **json);