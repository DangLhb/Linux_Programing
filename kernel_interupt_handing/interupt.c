#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/of.h>
/* Device tree
* interupt_device :interupt_device@0 {
*	compatibale = "interupt-data";
*	reg = <0x0 0x100000>
*	interrupts = <RK_PD3 IRQ_TYPE_LEVEL_LOW>;
*	interrupt-parent = <&gpio4>;
* }
*/
#define MIN_REQUIRED_FIFO_SIZE 10
#define MASK_IRQ_DISABLE 0x1
#define REG_STATUS_OFFSET 0x2

struct private_struct {
	int counter;
	struct work_struct my_work;
	void __iomem *reg_base;
	spinlock_t lock;
	int irq;
};

static void work_handle(struct work_struct *work){
	int i;
	unsigned long flags;
	struct private_struct* my_data = container_of(work, struct private_struct, my_work);
	for (i = 0; i < MIN_REQUIRED_FIFO_SIZE; i++) {
		// device_pop_and_process_data_buffer();
		if (i == MIN_REQUIRED_FIFO_SIZE / 2)
		{

		}
		// enable_irq_at_device_level(my_data); // Bat lai ngat
	}
	spin_lock_irqsave(&my_data->lock, flags);
		my_data->counter -= MIN_REQUIRED_FIFO_SIZE;
	spin_unlock_irqrestore(&my_data->lock, flags);
	pr_info("Work handler interupt done!!! - Niceeee\n");
}

static irqreturn_t my_interupt_handle(int irq, void * dev_id)
{
	// u32 status;
	unsigned long flags;	
	struct private_struct *my_data = dev_id;
	// status = readl(my_data->reg_base + REG_STATUS_OFFSET);
	// writel(my_data->reg_base + REG_STATUS_OFFSET, status | MASK_IRQ_ACK);
	spin_lock_irqsave(&my_data ->lock, flags);
	my_data->counter ++;
	spin_unlock_irqrestore(&my_data->lock, flags);
	if(my_data->counter != MIN_REQUIRED_FIFO_SIZE)
	{
		return IRQ_HANDLED;	// bat lai ngat
	}
	else
	{
		// writel(my_data->reg_base + REG_STATUS_OFFSET, MASK_IRQ_DISABLE);	//disable ngat
		writel(MASK_IRQ_DISABLE, (void *)(my_data->reg_base + REG_STATUS_OFFSET));

		schedule_work(&my_data->my_work);	// start a global workqueue
	}
	return IRQ_HANDLED;


}

static int interupt_probe(struct platform_device *pdev)
{
	struct resource * mem;
	struct private_struct * my_data;
	my_data = kmalloc(sizeof(struct private_struct), GFP_KERNEL);	// Cap phat bo nho dong cho con tro
	/* De lay thong tin tai nguyen tu device tree 
	*  nhu vung nho, IO, IRQ
	*  Lay thong tin trong device tree
	*/ 
	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	/* Sau khi lay duoc thong tin thi vung nho thi se anh xa vung vat li
	* len vung nho ao
	*/
	my_data->reg_base = ioremap(mem->start, resource_size(mem));
	if(IS_ERR(my_data->reg_base))
	{
		return PTR_ERR(my_data->reg_base);
	}
	INIT_WORK(&my_data->my_work, work_handle);

	spin_lock_init(&my_data->lock);
	/* ham platform_get_irq lay thong tin ve so IRQ */
	my_data->irq = platform_get_irq(pdev, 0);
	if(devm_request_irq(&pdev->dev, my_data->irq, my_interupt_handle, 0, pdev->name, my_data))
	{
		// handle_this_error();
	}
	pr_info("%s sucess!!\n",__func__);
	return 0;
}

static int interupt_remove(struct platform_device *pdev)
{
	return 0;
}

static const struct of_device_id interupt_of_match[] = {
	{ .compatible = "interupt-data", },
	{ }
};

static struct platform_driver interupt_driver = {
	.probe		= interupt_probe,
	.remove		= interupt_remove,
	.driver 	= {
			.name = "interupt-data",
			.of_match_table = of_match_ptr(interupt_of_match),
	}
};

static int __init module_driver_init(void)
{
	return platform_driver_register(&interupt_driver);
}

static void __exit module_driver_exit(void)
{
	platform_driver_unregister(&interupt_driver);
}

module_init(module_driver_init);
module_exit(module_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DangLHb <haidanglai23@gmail.com");
MODULE_DESCRIPTION("Designing and registering an interupt handler");