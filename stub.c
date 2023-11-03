#include "pre.h"
#include "linux/cpuhotplug.h"
#include "linux/rwsem.h"
#include "linux/kobject.h"
#include "linux/device/bus.h"

#include "drivers/cxl/cxl.h"
#include "post.h"

#include <CUnit/Basic.h>
#include <stdlib.h>

/* kernel/locking/spinlock.c */
void _raw_spin_lock(raw_spinlock_t *lock) { }
void _raw_spin_lock_irq(raw_spinlock_t *lock) { }
void _raw_spin_lock_bh(raw_spinlock_t *lock) { }
void _raw_spin_unlock(raw_spinlock_t *lock) { }
void _raw_spin_unlock_irq(raw_spinlock_t *lock) { }
void _raw_spin_unlock_bh(raw_spinlock_t *lock) { }
unsigned long _raw_spin_lock_irqsave(raw_spinlock_t *lock) { return 0; }
void _raw_spin_unlock_irqrestore(raw_spinlock_t *lock, unsigned long flags) { }

/* kernel/locking/mutex.c */
void mutex_lock(struct mutex *lock) { }
void mutex_unlock(struct mutex *lock) { }
void __mutex_init(struct mutex *lock, const char *name, struct lock_class_key *key) { }

/* kernel/locking/rwsem.c */
void __init_rwsem(struct rw_semaphore *sem, const char *name, struct lock_class_key *key) { }
void up_read(struct rw_semaphore *sem) { }
void down_read(struct rw_semaphore *sem) { }
int down_read_interruptible(struct rw_semaphore *sem) { return 0; }
void up_write(struct rw_semaphore *sem) { }
void down_write(struct rw_semaphore *sem) { }
int down_write_killable(struct rw_semaphore *sem) { return 0; }

/* kernel/rcu/tree_plugin.h */
void __rcu_read_lock(void) { }
void __rcu_read_unlock(void) { }

/* kernel/rcu/tree.c */
void call_rcu(struct rcu_head *head, rcu_callback_t func) { }

/* kernel/notifier.c */
int blocking_notifier_chain_register(struct blocking_notifier_head *nh, struct notifier_block *n) { return 0; }
int blocking_notifier_chain_unregister(struct blocking_notifier_head *nh, struct notifier_block *n) { return 0; }
int blocking_notifier_call_chain(struct blocking_notifier_head *nh, unsigned long val, void *v) { return 0; }

/* fs/kernfs/dir.c */
void kernfs_get(struct kernfs_node *kn) { }
void kernfs_put(struct kernfs_node *kn) { }

/* mm/usercopy.c */
void __check_object_size(const void *ptr, unsigned long n, bool to_user) { }

/* drivers/char/random.c */
void get_random_bytes(void *buf, size_t len) { }

struct rw_semaphore cxl_dpa_rwsem;
struct attribute_group cxl_base_attribute_group;
struct bus_type cxl_bus_type;
struct cxl_port *to_cxl_port(const struct device *dev)
{
	return container_of(dev, struct cxl_port, dev);
}
struct cxl_root_decoder *to_cxl_root_decoder(struct device *dev)
{
	return container_of(dev, struct cxl_root_decoder, cxlsd.cxld.dev);
}
struct cxl_switch_decoder *to_cxl_switch_decoder(struct device *dev) { return NULL; }
struct cxl_endpoint_decoder *to_cxl_endpoint_decoder(struct device *dev) { return NULL; }
struct cxl_decoder *to_cxl_decoder(struct device *dev) { return NULL; }
int cxl_mem_get_poison(struct cxl_memdev *cxlmd, u64 offset, u64 len,
		struct cxl_region *cxlr) { return 0; }
struct cxl_nvdimm_bridge *cxl_find_nvdimm_bridge(struct cxl_memdev *cxlmd) { return NULL; }
bool is_endpoint_decoder(struct device *dev) { return true; }
bool is_root_decoder(struct device *dev) { return true; }
bool is_switch_decoder(struct device *dev) { return true; }
int __cxl_driver_register(struct cxl_driver *cxl_drv, struct module *owner,
		const char *modname) { return 0; }
void cxl_driver_unregister(struct cxl_driver *cxl_drv) { }
struct module __this_module;

/* mm/percpu.c */
unsigned long __per_cpu_offset[NR_CPUS];

/* kernel/sched/core.c */
void dynamic_preempt_schedule(void) { }
int dynamic_might_resched(void) { return 0; }

/* kernel/cpu.c */
int __cpuhp_setup_state(enum cpuhp_state state, const char *name, bool invoke,
		int (*startup)(unsigned int cpu),
		int (*teardown)(unsigned int cpu), bool multi_instance) { return 0; }

/* drivers/base/devres.c */
int __devm_add_action(struct device *dev, void (*action)(void *), void *data, const char *name) { return 0; }
void devm_release_action(struct device *dev, void (*action)(void *), void *data) { }

/* lib/bitmap.c */
void __bitmap_clear(unsigned long *map, unsigned int start, int len) { }

/* mm/slab_common.c */
void *__kmalloc(size_t size, gfp_t flags) { return malloc(size); }
void kfree(const void *objp) { free((void *)objp); }
void kfree_const(const void *x) { free((void *)x); }
struct kmem_cache {
	unsigned int obj_size;
};
struct kmem_cache *kmem_cache_create(const char *name, unsigned int size, unsigned int align,
		slab_flags_t flags, void (*ctor)(void *))
{
	struct kmem_cache *s = calloc(1, sizeof(*s));
	s->obj_size = size;
	return s;
}
void *kmem_cache_alloc(struct kmem_cache *s, gfp_t flags) { return malloc(s->obj_size); }
void *kmem_cache_alloc_lru(struct kmem_cache *s, struct list_lru *lru, gfp_t gfpflags) { return malloc(s->obj_size); }
void kmem_cache_free(struct kmem_cache *s, void *objp) { free(objp); }

/* lib/memregion.c */
int memregion_alloc(gfp_t gfp) { return 0; }
void memregion_free(int id) { }

/* mm/maccess.c */
void __copy_overflow(int size, unsigned long count) { }

/* kernel/resource.c */
int insert_resource(struct resource *parent, struct resource *new) { return 0; }
int remove_resource(struct resource *old) { return 0; }
struct resource *alloc_free_mem_region(struct resource *base, unsigned long size,
		unsigned long align, const char *name) { return NULL; }

/* include/linux/dev_printk.h, lib/dynamic_debug.c */
void _dev_err(const struct device *dev, const char *fmt, ...) { }
void _dev_warn(const struct device *dev, const char *fmt, ...) { }
void __dynamic_dev_dbg(struct _ddebug *descriptor, const struct device *dev,
		const char *fmt, ...) { }
void __dynamic_pr_debug(struct _ddebug *descriptor, const char *fmt, ...) { }
void __warn_printk(const char *fmt, ...) { }
int _printk(const char *fmt, ...) { return 0; }
const char *kvasprintf_const(gfp_t gfp, const char *fmt, va_list ap) { return NULL; }

/* fs/sysfs/file.c */
int sysfs_create_file_ns(struct kobject *kobj, const struct attribute *attr, const void *ns) { return 0; }
void sysfs_remove_file_ns(struct kobject *kobj, const struct attribute *attr, const void *ns) { }
int sysfs_create_dir_ns(struct kobject *kobj, const void *ns) { return 0; }
int sysfs_move_dir_ns(struct kobject *kobj, struct kobject *new_parent_kobj, const void *new_ns) { return 0; }
int sysfs_rename_dir_ns(struct kobject *kobj, const char *new_name, const void *new_ns) { return 0; }
int sysfs_create_groups(struct kobject *kobj, const struct attribute_group **groups) { return 0; }
void sysfs_remove_groups(struct kobject *kobj, const struct attribute_group **groups) { }
int sysfs_update_group(struct kobject *kobj, const struct attribute_group *grp) { return 0; }
int sysfs_create_link(struct kobject *kobj, struct kobject *target, const char *name) { return 0; }
void sysfs_remove_link(struct kobject *kobj, const char *name) { }
void sysfs_remove_dir(struct kobject *kobj) { }
int sysfs_emit(char *buf, const char *fmt, ...)
{
	va_list argptr;
	int rc;

	va_start(argptr, fmt);
	rc = vsprintf(buf, fmt, argptr);
	va_end(argptr);

	return rc;
}
bool sysfs_streq(const char *s1, const char *s2) { return true; }

unsigned long __arch_copy_from_user(void *to, const void *from, unsigned long n) { return n; }
int walk_iomem_res_desc(unsigned long desc, unsigned long flags, u64 start, u64 end,
		void *arg, int (*func)(struct resource *, void *)) { return 0; }

int device_add(struct device *dev) { return 0; }
int device_attach(struct device *dev) { return 0; }
struct device *get_device(struct device *dev) { return NULL; }
void put_device(struct device *dev) { }
void device_unregister(struct device *dev) { }
int device_for_each_child(struct device *dev, void *data,
		int (*fn)(struct device *dev, void *data)) { return 0; }
struct device *device_find_child(struct device *dev, void *data,
		int (*match)(struct device *dev, void *data)) { return NULL; }
struct device *device_find_child_by_name(struct device *parent, const char *name)
{ return NULL; }
void device_release_driver(struct device *dev) { }
const char *dev_driver_string(const struct device *dev) { return NULL; }
void device_del(struct device *dev) { }
int device_match_name(struct device *dev, const void *name) { return 0; }
void device_initialize(struct device *dev) { }
int dev_set_name(struct device *dev, const char* name, ...) { return 0; }
void device_remove_groups(struct device *dev, const struct attribute_group **groups) { }
int device_driver_attach(struct device_driver *drv, struct device *dev) { return 0; }
void device_driver_detach(struct device *dev) { }
int driver_attach(struct device_driver *drv) { return 0; }
void driver_detach(struct device_driver *drv) { }
int device_register(struct device *dev) { return 0; }
int device_add_groups(struct device *dev, const struct attribute_group **groups) { return 0; }
void device_initial_probe(struct device *dev) { }
struct kobject *virtual_device_parent(struct device *dev) { return NULL; }
void deferred_probe_extend_timeout(void) { }

int wake_up_process(struct task_struct *p) { return 0; }
void schedule(void) { }
void module_add_driver(struct module *mod, struct device_driver *drv) { }
void module_remove_driver(struct device_driver *drv) { }

struct kset *devices_kset;

struct kmem_cache *kmalloc_caches[NR_KMALLOC_TYPES][KMALLOC_SHIFT_HIGH + 1];
void *kmalloc_trace(struct kmem_cache *s, gfp_t flags, size_t size) { return NULL; }

DECLARE_BITMAP(system_cpucaps, ARM64_NCAPS);

const unsigned char _ctype[1];

struct static_key_false gic_nonsecure_priorities;

unsigned long __stack_chk_guard;
void alt_cb_patch_nops(struct alt_instr *alt, __le32 *origptr,
		       __le32 *updptr, int nr_inst)
{
}

char *kstrdup(const char *s, gfp_t gfp) { return strdup(s); }
const char *kstrdup_const(const char *s, gfp_t gfp) { return strdup(s); }
char *kstrndup(const char *s, size_t max, gfp_t gfp) { return strndup(s, max); }
char *strreplace(char *str, char old, char new) { return str; }

char *strnchr(const char *s, size_t count, int c) { return NULL; }
int __pm_runtime_idle(struct device *dev, int rpmflags) { return 0; }

asmlinkage void dump_stack_lvl(const char *log_lvl) { }

int kobject_uevent(struct kobject *kobj, enum kobject_action) { return 0; }
int kobject_uevent_env(struct kobject *kobj, enum kobject_action, char *envp[]) { return 0; }
int kobject_synth_uevent(struct kobject *kobj, const char *buf, size_t count) { return 0; }
