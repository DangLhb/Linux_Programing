/*
 * Device tree : Node, Alias, Lable va phandle
 * Node : Dai dien cho mot thiet bi - vd <node_name>[@address]. Address co the co hoac khong
 * Lable : La cach de dinh danh node bang mot ten duy nhat, dung de tham chieu den node - vd lable : <node_name>[@address] . lable co the co hoac khong
 * Phandle : Co the hieu la mot con tro tro den mot node thong qua lable - vd : <&lable>
 */

/* device tree example va cach doc gia tri tu device tree
node_lable : nodename@0x20 {
	compatible = "test,device_tree";
	 reg = <0x4a064000 0x800>,
 		   <0x4a064800 0x200>,
 		   <0x4a064c00 0x200>;
 	reg-names = "ohci", "ehci", "danglhb";
	interrupts= = <0 66 IRQ_TYPE_LEVEL_HIGH>,
				  <0 67 IRQ_TYPE_LEVEL_HIGH>;
	interrupt-names = "ohci", "ehci";
	string-property = "a string";
	string-list = "danglhb", "somebody";
	one-int-property = <2000>;
	int-list-property = <2000>, <2001>, <2002>;
	mixed-list-property = <2000>, "a string", [0x01 0x02 0x03];
	byte-array-property = [0x01 0x23 0x45 0x67];
	boolean-property;
	status = "okay";
	child_node : childnode@1 {
		reg = <0x1>;
		value = <23>;
	};
};

 */


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/io.h>
#include <linux/interrupt.h>


struct private_data {
	struct device_node *np;
	struct device_node *child_node;
	void __iomem *reg_base;
	const char *string;
	const char *string_list;
	const char **name;
	int one_int_property;
	bool bool_property;
};

static irqreturn_t interupt_handle(int irq, void* dev_id)
{
	return IRQ_HANDLED;
}

static int device_tree_probe(struct platform_device *pdev)
{
	struct resource *mem;
	struct resource *irq_1;
	int count;
	int value;

	struct private_data *mydata;
	mydata = kmalloc(sizeof(struct private_data), GFP_KERNEL);
	mydata->np = pdev->dev.of_node;
	mem = platform_get_resource_byname(pdev, IORESOURCE_MEM, "danglhb");
	mydata->reg_base = ioremap(mem->start, resource_size(mem));
	irq_1 = platform_get_resource_byname(pdev, IORESOURCE_IRQ, "ohci");
	if(!devm_request_irq(&pdev->dev, irq_1->start, interupt_handle, 0, pdev->name, mydata))
	{
		//handle error
	}

	of_property_read_string(mydata->np, "string-property", &mydata->string);
	of_property_read_string_index(mydata->np, "string-list", 1, &mydata->string_list);
	count = of_property_read_string_array(mydata->np, "string-list", mydata->name, 3);
	mydata->bool_property = of_property_read_bool(mydata->np, "boolean-property");

	for_each_child_of_node(mydata->np, mydata->child_node){
		of_property_read_u32(mydata->np, "value", &value);
	}
	return 0;
}

static int device_tree_remove(struct platform_device *pdev)
{
	return 0;
}

static const struct of_device_id device_tree_ids[] = {
	{ .compatible = "test,device_tree"},
	{ }
};

MODULE_DEVICE_TABLE(of, device_tree_ids);

static struct platform_driver device_tree_driver = {
	.driver = {
		.name = "device_tree-info",
		.of_match_table = of_match_ptr(device_tree_ids)
	},
	.probe = device_tree_probe,
	.remove = device_tree_remove,
	
};

static int __init device_tree_init(void)
{
	return platform_driver_register(&device_tree_driver);
}

static void __exit device_tree_exit(void)
{
	platform_driver_unregister(&device_tree_driver);
}
module_init(device_tree_init);
module_exit(device_tree_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("danglhb <haidanglai23@gmail.com>");
MODULE_DESCRIPTION("Module driver example get info from device tree");