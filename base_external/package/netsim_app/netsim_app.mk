
##############################################################
#
# NETSIM_APP
#
##############################################################

NETSIM_APP_VERSION = 1.0
NETSIM_APP_SITE = ../netsim_app
NETSIM_APP_SITE_METHOD = local

define NETSIM_APP_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D) clean 
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D) all
endef

define NETSIM_APP_INSTALL_TARGET_CMDS
  $(INSTALL) -d $(TARGET_DIR)/var/lib/netsim
	$(INSTALL) -m 0755 $(@D)/netsim $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 0755 $(@D)/netsim-start-stop $(TARGET_DIR)/etc/init.d/S99netsim
	$(INSTALL) -m 0644 $(@D)/status.json.install $(TARGET_DIR)/var/lib/netsim/
	$(INSTALL) -m 0644 $(@D)/profiles.json.install $(TARGET_DIR)/var/lib/netsim/
endef

$(eval $(generic-package))
