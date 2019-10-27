#
# Makefile for Goldfish virtual platform
#

EXTRA_CFLAGS += -Wno-error

CFLAGS_goldfish-platform.o	= -I$(src)/../../../scripts/dtc/libfdt

obj-$(CONFIG_MIPS_RANCHU)	+= goldfish-platform.o
obj-$(CONFIG_MIPS_RANCHU)	+= goldfish-interrupt.o
obj-$(CONFIG_MIPS_RANCHU)	+= goldfish-time.o
obj-$(CONFIG_MIPS_RANCHU)	+= pm.o
obj-$(CONFIG_MIPS_RANCHU)	+= goldfish-reset.o

obj-$(CONFIG_MIPS_GOLDFISH_SWITCH)	+= switch.o

ifneq ($(CONFIG_MIPS_APPENDED_DTB), y)
obj-$(CONFIG_MIPS_RANCHU)        += ranchu.dtb.o

$(obj)/%.dtb: $(obj)/%.dts
       $(call if_changed,dtc)
endif
