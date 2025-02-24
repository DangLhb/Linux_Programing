#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>

static u32 *reg_base;
static struct resource * reg_irq;

/*** Match by device tree - match by compatible***/
static const struct of_device_id danglhb_dt_ids [] = {
	{
		.compatible = "danglhb,demo-pdev",
		/*.data = (void*)&big_struct1, */
	},
	{
		.compatible = "danglhb, other-pdev",
		./* .data = (void*)&big_struct2, */
	},
	{}
};
MODULE_DEVICE_TABLE(of, danglhb_dt_ids);

/*** Match by platform id table - Match by name***/

static const struct platform_device_id danglhb_ids [] = {
	{
		.name = "demo-pdev",
		/*.driver_data = &big_struct1, */
	},
	{
		.name = "other-pdev",
		/*.driver_data = &big_struct2, */
	},
	{}
};
MODULE_DEVICE_TABLE(platform, danglhb_ids);

/******Ham tham do - probe function ***/

static int danglhb_probe(struct platform_device *pdev)
{
	struct resource *regs;

/**Neu thiet driver chi lay thong tin tu device tree va thiet bi cung duoc khoi tao tu device tree**/

	struct deivce_node *np = pdev->dev.of_node;

	const struct of_device_id *of_id = of_match_device(danglhb_dt_ids, &pdev->dev);

	// struct big_struct *pdata_struct = (big_struct*)of_id->data;

	regs = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if(!regs) {
		dev_err(&pdev->dev, "Khong the lay IO memory\n");
		return = -ENXIO;
	}
/****************************************************************************************************/

	/* map the base register */
	
	reg_base = devm_ioremap(&pdev->dev, regs->start, resource_size(resgs));
	if(!reg_base) {
		dev_err(&pdev->dev, "Khong the map memory\n");
		return 0;
	} 

	reg_irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);

	dev_request_irq(&pdev->dev, res_irq->start, top_half, IRQF_TRIGGER_FALLING, "demo-pdev", NULL);

	return 0;

}

int danglhb_remove(struct platform_device * pdev)
{
	free_irq(res_irq->start, NULL);
	iounmap(reg_base);
	return 0;
}

static struct platform_driver danglhb_driver = {
	.probe = danglhb_probe,
	.remove = danglhb_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = "demo-pdev",
	},
};
module_platform_driver(danglhb_driver);