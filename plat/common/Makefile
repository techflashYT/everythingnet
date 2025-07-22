obj-$(CONFIG_PLAT_NEEDS_INTERNAL_PRINTF) += sane_printf.o
obj-$(CONFIG_PLAT_NEEDS_INTERNAL_LIBC)   += libc/
ifeq ($(ARCH),aarch64)
obj-y					 += arm-cpu-list.o
else ifeq ($(ARCH),arm)
obj-y					 += arm-cpu-list.o
endif
