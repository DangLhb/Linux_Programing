/* A Platform driver demo */
/* Device tree sample*/
/*
demo {
	compatible = "simple-bus";

	demo_pdev: demo-pdev@0 {
		compatible = "danglhb,demo-pdev";
		reg = <0x2008000 0x400>;
		interrupts = <0 31 IRQ_TYPE_LEVEL_HIGH>;
	};
};
*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>

#define DEV_BASE 0x2008000
#define PDEV_IRQ 31

static struct resource pdev_resoure[] = {
	[0] = {
		.start = DEV_BASE,
		.end = DEV_BASE + 0x4000,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = PDEV_IRQ,
		.end = PDEV_IRQ,
		flags = IORESOURCE_IRQ,
	},
};

struct platform_device demo_pdev = {
	.name = "demo_pdev",
	.id = 0,
	.num_resource = ARRAY_SIZE(pdev_resoure);
	.resource = pdev_resoure,
	/*.dev = {
		.platform_data = (void*)&big_struct1,
	},*/
};

static int demo_pdev_init()
{
	return platform_device_register(&demo_pdev);
}

static void demo_pdev_exit()
{
	platform_device_unregister(&demo_pdev);
}

module_init(demo_pdev_init);
module_exit(demo_pdev_exit);
