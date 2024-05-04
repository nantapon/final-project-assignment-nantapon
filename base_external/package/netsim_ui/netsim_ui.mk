
##############################################################
#
# NETSIM_UI
#
##############################################################

NETSIM_UI_VERSION = 1.0
NETSIM_UI_SITE = ../netsim_ui
NETSIM_UI_SITE_METHOD = local

define NETSIM_UI_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 $(@D)/netsim_ui $(TARGET_DIR)/bin
endef

$(eval $(generic-package))
