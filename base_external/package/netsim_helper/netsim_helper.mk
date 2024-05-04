
##############################################################
#
# NETSIM_HELPER 
#
##############################################################

NETSIM_HELPER_VERSION = 1.0
NETSIM_HELPER_SITE = ../netsim_helper
NETSIM_HELPER_SITE_METHOD = local

define NETSIM_HELPER_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D) clean 
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D) all
endef

define NETSIM_HELPER_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 $(@D)/netsim_helper $(TARGET_DIR)/bin
endef

$(eval $(generic-package))
