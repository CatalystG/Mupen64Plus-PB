ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

USEFILE=

# Extra include path for libfreetype and for target overrides and patches
EXTRA_INCVPATH+=$(QNX_TARGET)/usr/include/freetype2 \
	$(QNX_TARGET)/../target-override/usr/include

# Extra library search path for target overrides and patches
EXTRA_LIBVPATH+=$(QNX_TARGET)/../target-override/$(CPUVARDIR)/lib \
	$(QNX_TARGET)/../target-override/$(CPUVARDIR)/usr/lib
	
# Compiler options for enhanced security and recording the compiler options in release builds
CCFLAGS+=-fstack-protector-all -D_FORTIFY_SOURCE=2 \
	$(if $(filter g so shared,$(VARIANTS)),,-fPIE) \
	$(if $(filter g,$(VARIANTS)),,-frecord-gcc-switches)
	
CFLAGS += -Wall -ffast-math -fno-strict-aliasing -fvisibility=hidden

# Linker options for enhanced security
LDFLAGS+=-Wl,-z,relro -Wl,-z,now $(if $(filter g so shared,$(VARIANTS)),,-pie)

# Add your required library names, here
LIBS+= GLESv2 png

include $(MKFILES_ROOT)/qmacros.mk

# Suppress the _g suffix from the debug variant
BUILDNAME=../../../libs/$(IMAGE_PREF_$(BUILD_TYPE))$(NAME)$(IMAGE_SUFF_$(BUILD_TYPE))
BUILDNAME_SAR=$(IMAGE_PREF_AR)$(NAME)S$(IMAGE_SUFF_AR)

include $(MKFILES_ROOT)/qtargets.mk

OPTIMIZE_TYPE_g=none
OPTIMIZE_TYPE=$(OPTIMIZE_TYPE_$(filter g, $(VARIANTS)))
