/*** Tom tat ve character device driver
 * Là driver đại diện cho các thiết bị giao tiếp dựa vào tốc độ và cách nó truyền dữ liệu, từng byte một. ví dụ như chuột, bàn phím,... 
 * Major number va minor number : Major number đại diện cho driver , còn minor đại diện cho thiết bị cho một driver.
 * Cấu trúc đại diện cho character device la cdev duoc declared trong /linux/cdev.h
	struct cdev {
		struct kobject kobj;
		struct module *owner;
		const struct file_operations *ops;
		dev_t dev;
		[...]
	};

*/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/kernel.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/fs.h>


#define FIRST_MINOR 0
#define MAX_MINOR 1

dev_t dev = 0;
static struct class *class;
static struct cdev cdev;

static ssize_t danglhb_dev_read(struct file *file, char __user *buf, size_t count,
		       loff_t *ppos)
{
	pr_info("enter %s\n",__func__);
	return count;
}

static ssize_t danglhb_dev_write(struct file *file, const char __user *buf,
			size_t count, loff_t *ppos)
{
	pr_info("enter %s\n",__func__);
	return count;	
}

static int danglhb_dev_open(struct inode *inode, struct file *file)
{
	pr_info("%s enter\n",__func__);
	return 0;
}

static int etx_release(struct inode *inode, struct file *file)
{
        pr_info("Driver Release Function Called...!!!\n");
        return 0;
}

static struct file_operations danglhb_dev_fops = {
	.owner  	= THIS_MODULE,
	.read 		= danglhb_dev_read,
	.write 		= danglhb_dev_write,
	.open 		= danglhb_dev_open,
	.release    = etx_release,	//Neu khong co call back nay thi se bi loop trong cac ham call back khac.
};

static int __init module_driver_init(void)
{
	//Dang ki character device number 
	// dev_t curr_dev = MKDEV(MAJOR(dev),MINOR(dev));
	pr_info("%s enter\n",__func__);
	if(alloc_chrdev_region(&dev, 0, 1, "danglhb_character") < 0)
	{
		pr_err("Cant alloc major number for device\n");
		return -1;
	}

	cdev_init(&cdev, &danglhb_dev_fops);

	if(cdev_add(&cdev, dev, 1) < 0)
	{
		pr_err("Cant not add the device to system\n");
		goto r_class;
	}

	class = class_create("danglhb_class");
	if(IS_ERR(class))
	{
		pr_err("Cant not creat class device\n");
		goto r_class;
	}

	if(IS_ERR(device_create(class, NULL, dev, NULL, "danglhb_device")))	//device_create(class, NULL, MKDEV(MAJOR(first_devt),MINOR(first_devt) + i), NULL, "mynull%d", i);
	{
		pr_err("Cant not creat device\n");
		goto r_device;
	}

	pr_info("%s done\n",__func__);

	return 0;
r_class:
	unregister_chrdev_region(dev, 1);
r_device:
	class_destroy(class);
	return -1;

}

static void __exit module_driver_exit(void)
{
	device_destroy(class, MKDEV(MAJOR(dev),MINOR(dev)));	
	// class_unregister(class);
	class_destroy(class);
	cdev_del(&cdev);
	unregister_chrdev_region(dev, 1);
	pr_info("module driver exit\n");
	
}

module_init(module_driver_init);
module_exit(module_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DangLHb <haidanglai23@gmail.com");
MODULE_DESCRIPTION("Simple character device driver");