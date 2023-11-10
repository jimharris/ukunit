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

INCLUDE_FLAGS = -I$(ROOT_DIR)/include -I$(ROOT_DIR)/build/include -I$(KERNEL_DIR) \
		$(LINUXINCLUDE) $(KBUILD_CPPFLAGS) $(KBUILD_CFLAGS) \
		-fno-sanitize=shadow-call-stack -Wno-frame-larger-than -D__NO_FORTIFY -O0 \
		$(subst __NAME__,$(notdir $*),$(KBUILD_NAME_STRING))

LINK_FLAGS = $(KBUILD_CPPFLAGS) $(KBUILD_CFLAGS) -lcunit

KBUILD_NAME_STRING = -DKBUILD_BASENAME='"__NAME__"' -DKBUILD_MODNAME='"__NAME__"' \
		     -DKBUILD_MODFILE='"__NAME__"'

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

MODIFIED_HEADERS = build/include/linux/compiler_attributes.h

UNIT_TEST_SRC = \
	$(TEST_DIR)/drivers/cxl/core/region_ut.c \
	$(TEST_DIR)/drivers/cxl/core/hdm_ut.c
UNIT_TEST_BIN = $(patsubst $(TEST_DIR)/%.c,build/%,$(UNIT_TEST_SRC))

all: $(UNIT_TEST_BIN)

clean:
	$(V)rm -f $(TEST_DIR)/drivers/cxl/core/region_ut $(TEST_DIR)/drivers/cxl/core/region_ut.o
	$(V)rm -f $(MODIFIED_HEADERS)
	$(V)rm -rf build

build/%_ut: build/%_ut.o $(WRAPPED_KERNEL_OBJ)
	$(V)echo [LINK] $(subst build/,,$@)
	$(V)gcc -o $@ $^ $(LINK_FLAGS)

# lib/kstrtox.c generates asm constraint error if compiled at -O0
build/lib/kstrtox.o: INCLUDE_FLAGS += -O2

$(TEST_DIR)/drivers/cxl/%.o build/drivers/cxl/%.o: INCLUDE_FLAGS += -I$(KERNEL_DIR)/drivers/cxl

build/%.o: build/%.c Makefile
	$(V)echo [OBJ] $(subst build/,,$@)
	$(V)gcc -o $@ -c $< $(INCLUDE_FLAGS)

build/%_ut.o: $(TEST_DIR)/%_ut.c $(ROOT_DIR)/stub.c Makefile
	$(V)echo [OBJ] $(subst build/,,$@)
	$(V)mkdir -p $(dir $@)
	$(V)gcc -o $@ -c $< $(INCLUDE_FLAGS) -DMODULE

build/%.c: $(MODIFIED_HEADERS)
	$(V)echo [SRC] $(subst build/,,$@)
	$(V)mkdir -p $(dir $@)
	$(V)echo \#include \"pre.h\" > $@
	$(V)echo \#include \"$(KERNEL_DIR)/$*.c\" >> $@
	$(V)echo \#include \"post.h\" >> $@

# compiler_attributes.h uses __attribute__(__error__) for static asserts - this depends
# on the compiler optimizing away the function, so for unit test builds with -O0 we
# create a new version of that header to ensure the attribute doesn't get used.
build/include/linux/compiler_attributes.h: $(KERNEL_DIR)/include/linux/compiler_attributes.h
	$(V)echo {HEADER] $(subst build/,,$@)
	$(V)mkdir -p $(dir $@)
	$(V)sed 's/__has_attribute(__error__)/& \&\& defined(__OPTIMIZE__)/' $< > $@
