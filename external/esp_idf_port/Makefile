###########################################################################
#
# Copyright 2019 Samsung Electronics All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
# either express or implied. See the License for the specific
# language governing permissions and limitations under the License.
#
###########################################################################

-include $(TOPDIR)/.config
-include $(TOPDIR)/Make.defs

#DEPPATH	= --dep-path .
VPATH = .
CXXSRCS =

ESP32_CSRCS += esp32/esp32_wifi_os_adapter.c esp32/event_groups.c esp32/esp32_queue_api.c esp32/mq_tryreceive.c 

CXXOBJS = $(CXXSRCS:.cpp=$(OBJEXT))
COBJS   = $(CSRCS:.c=$(OBJEXT))

BIN       = ../libexternal$(LIBEXT)
OBJS        = $(CXXOBJS) $(COBJS)

# Common build
all: $(SUBDIRS)	.built

.PHONY: clean distclean


.built: $(OBJS)
	$(call ARCHIVE, $(BIN), $(OBJS))

$(CXXOBJS) : %$(OBJEXT): %.cpp
	$(call COMPILEXX, $<, $@)

$(COBJS): %$(OBJEXT): %.c 
    $(call COMPILE, $<, $@)

clean:
	$(call DELFILE, .built)
	$(call DELFILE, esp32/*.o)
	$(call CLEAN)
	rm -f $(OBJS)

distclean: clean
	$(call DELFILE, Make.dep)
	$(call DELFILE, .depend)

-include Make.dep
