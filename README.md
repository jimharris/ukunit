# ukunit (Userspace kernel unit testing)

This is a prototype for unit testing Linux kernel source code
using cunit.

cunit package must be installed to build ukunit.

ukunit directory should be a peer of the Linux kernel source
tree under test.

Simply `make` and then run `test/drivers/cxl/core/region_ut`
to execute a few unit tests of the CXL region.c code.
