ROOT_DIR := $(CURDIR)
KERNEL_DIR := $(ROOT_DIR)/../linux
TEST_DIR := $(ROOT_DIR)/test
STUB_DIR := $(ROOT_DIR)/stub

V ?= @
.SECONDARY:

LINUXINCLUDE_STRING    = $(shell make -C $(KERNEL_DIR) -qp | grep "^LINUXINCLUDE :=")
KBUILD_CPPFLAGS_STRING = $(shell make -C $(KERNEL_DIR) -qp | grep "^KBUILD_CPPFLAGS :=")
KBUILD_CFLAGS_STRING   = $(shell make -C $(KERNEL_DIR) -qp | grep "^KBUILD_CFLAGS :=")

$(eval $(subst -I.,-I$(KERNEL_DIR),$(LINUXINCLUDE_STRING)))
$(eval $(KBUILD_CPPFLAGS_STRING))
$(eval $(KBUILD_CFLAGS_STRING))

INCLUDE_FLAGS = $(LINUXINCLUDE) $(KBUILD_CPPFLAGS) $(KBUILD_CFLAGS) \
		-I$(ROOT_DIR)/include -I$(KERNEL_DIR) -I$(KERNEL_DIR)/drivers/cxl \
		-fno-sanitize=shadow-call-stack -Wno-frame-larger-than -D__NO_FORTIFY -O0 \
		$(subst __NAME__,$(notdir $*),$(KBUILD_NAME_STRING))

LINK_FLAGS = $(KBUILD_CPPFLAGS) $(KBUILD_CFLAGS) -lcunit

KBUILD_NAME_STRING = -DKBUILD_BASENAME='"__NAME__"' -DKBUILD_MODNAME='"__NAME__"'

KERNEL_SRC = \
	drivers/base/bus.c \
	drivers/base/driver.c \
	lib/kobject.c \
	lib/klist.c \
	lib/refcount.c \
	lib/xarray.c \
	lib/find_bit.c \
	lib/hweight.c \
	lib/radix-tree.c \
	lib/uuid.c \
	lib/hexdump.c \
	lib/kstrtox.c \
	lib/sort.c

WRAPPED_KERNEL_SRC = $(patsubst %.c,build/%.c,$(KERNEL_SRC))
WRAPPED_KERNEL_OBJ = $(patsubst build/%.c,build/%.o,$(WRAPPED_KERNEL_SRC))

all: $(TEST_DIR)/drivers/cxl/core/region_ut

clean:
	$(V)rm -f $(TEST_DIR)/drivers/cxl/core/region_ut $(TEST_DIR)/drivers/cxl/core/region_ut.o
	$(V)rm -rf build

$(ROOT_DIR)/%_ut: $(ROOT_DIR)/%_ut.o $(WRAPPED_KERNEL_OBJ)
	$(V)echo [LINK] $*_ut
	$(V)gcc -o $@ $^ $(LINK_FLAGS)

# lib/kstrtox.c generates asm constraint error if compiled at -O0
build/lib/kstrtox.o: INCLUDE_FLAGS += -O2

build/%.o: build/%.c
	$(V)echo [OBJ] $@
	$(V)gcc -o $@ -c $< $(INCLUDE_FLAGS)

$(ROOT_DIR)/%.o: $(ROOT_DIR)/%.c
	$(V)echo [OBJ] $*.o
	$(V)gcc -o $@ -c $< $(INCLUDE_FLAGS) -DMODULE

build/%.c:
	$(V)echo [SRC] $@
	$(V)mkdir -p build/$(dir $*)
	$(V)echo \#include \"pre.h\" > $@
	$(V)echo \#include \"$(KERNEL_DIR)/$*.c\" >> $@
	$(V)echo \#include \"post.h\" >> $@
