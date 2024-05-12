
##############################################################
#
# NETSIM_UI
#
##############################################################

NETSIM_UI_VERSION = 1.0
NETSIM_UI_SITE = ../netsim_ui
NETSIM_UI_SITE_METHOD = local

define NETSIM_UI_INSTALL_TARGET_CMDS
	$(INSTALL) -d $(TARGET_DIR)/www $(TARGET_DIR)/www/assets
	$(INSTALL) -m 0644 $(@D)/dist/index.html $(TARGET_DIR)/www
	$(INSTALL) -m 0644 $(@D)/dist/*.png $(TARGET_DIR)/www
	$(INSTALL) -m 0644 $(@D)/dist/*.svg $(TARGET_DIR)/www
	$(INSTALL) -m 0644 $(@D)/dist/assets/* $(TARGET_DIR)/www/assets
endef

$(eval $(generic-package))
