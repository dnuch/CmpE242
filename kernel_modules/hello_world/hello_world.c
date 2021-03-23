#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");

static int hello_world_init(void) {
	pr_alert("hello_world: init\n");
	return 0;
}


static void hello_world_exit(void) {
	pr_alert("hello_world: exit\n");
}

module_init(hello_world_init);
module_exit(hello_world_exit);

