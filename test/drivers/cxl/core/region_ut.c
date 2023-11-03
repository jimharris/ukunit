#include "pre.h"
#include "drivers/cxl/core/region.c"
#include "post.h"
#include "../stub.c"

#include <CUnit/Basic.h>
#include <stdlib.h>

static void
uuid_show_test(void)
{
	struct cxl_region region = { .dev.type = &cxl_region_type };
	char buf[128];

	region.mode = CXL_DECODER_RAM;
	CU_ASSERT(uuid_show(&region.dev, NULL, buf) == 1);
}

static void
is_cxl_pmem_region_test(void)
{
	struct device dev;

	dev.type = &cxl_pmem_region_type;
	CU_ASSERT(is_cxl_pmem_region(&dev) == true);

	dev.type = &cxl_dax_region_type;
	CU_ASSERT(is_cxl_pmem_region(&dev) == false);
}

const struct device_type dummy_region_type = {
	.name = "dummy"
};

static void
is_dup_test(void)
{
	struct device dummy = { .type = &dummy_region_type };
	struct cxl_region region = { .dev.type = &cxl_region_type };
	uuid_t uuid = { 0 };

	/* non-region devices should always return 0 */
	CU_ASSERT(is_dup(&dummy, NULL) == 0);

	/*
	 * uuid matches, indicates the specified uuid duplicates
	 * the uuid for an existing region
	 * return -EBUSY
	 */
	CU_ASSERT(is_dup(&region.dev, &uuid) == -EBUSY);

	/*
	 * uuid does not match
	 */
	uuid.b[0] = 1;
	CU_ASSERT(is_dup(&region.dev, &uuid) == 0);
}

static void
interleave_ways_store_test(void)
{
	struct cxl_region *region = calloc(1, sizeof(*region));
	const char *str0 = "0";
	const char *str1 = "1";
	const char *str16 = "16";
	char buf[32];
	struct cxl_root_decoder *root_decoder = calloc(1, sizeof(*root_decoder));

	region->dev.type = &cxl_region_type;
	region->dev.parent = &root_decoder->cxlsd.cxld.dev;
	root_decoder->cxlsd.cxld.interleave_ways = 1;
	region->params.interleave_ways = 0xFF;

	CU_ASSERT(interleave_ways_store(&region->dev, NULL, str0, strlen(str0)) < 0);
	CU_ASSERT(region->params.interleave_ways == 0xFF);

	CU_ASSERT(interleave_ways_store(&region->dev, NULL, str1, strlen(str1)) == strlen(str1));
	CU_ASSERT(region->params.interleave_ways == 1);
	/* interleave_ways_show appends a newline to the value string */
	CU_ASSERT(interleave_ways_show(&region->dev, NULL, buf) == strlen(str1) + 1);
	CU_ASSERT(strncmp(buf, str1, strlen(str1)) == 0);

	region->params.interleave_ways = 0xFF;
	CU_ASSERT(interleave_ways_store(&region->dev, NULL, str16, strlen(str16)) == strlen(str16));
	CU_ASSERT(region->params.interleave_ways == 16);
	/* interleave_ways_show appends a newline to the value string */
	CU_ASSERT(interleave_ways_show(&region->dev, NULL, buf) == strlen(str16) + 1);
	CU_ASSERT(strncmp(buf, str16, strlen(str16)) == 0);

	free(root_decoder);
	free(region);
}

int
main(int argc, char **argv)
{
	CU_pSuite suite = NULL;
	unsigned int num_failures;

	CU_set_error_action(CUEA_ABORT);
	CU_initialize_registry();

	suite = CU_add_suite("app_suite", NULL, NULL);
	CU_ADD_TEST(suite, uuid_show_test);
	CU_ADD_TEST(suite, is_dup_test);
	CU_ADD_TEST(suite, is_cxl_pmem_region_test);
	CU_ADD_TEST(suite, interleave_ways_store_test);

	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	num_failures = CU_get_number_of_failures();
	CU_cleanup_registry();

	return num_failures;
}
