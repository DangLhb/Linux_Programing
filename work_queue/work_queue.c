#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>            // Required for workqueues
#include <linux/err.h>


// DECLARE_WORK(name, function);
// DECLARE_DELAYED_WORK(name, function)
// INIT_WORK(work, func );
// INIT_DELAYED_WORK( work, func);
#define TIME_DELAY_WORK 5000
struct workqueue_struct *my_wq;
struct work_struct my_work;
struct delayed_work my_delay_work;
static void work_function_handle(struct work_struct *work);
struct object_queue *work_init;

struct object_queue {
	struct work_struct ac_work;
	struct workqueue_struct *queue_work;
	struct delayed_work work_delay;
};



static void work_function_handle(struct work_struct *work)
{
	pr_emerg("%s enter\n",__func__);
}


static int __init module_driver_init(void)
{
	int ret = 0;
	work_init = kmalloc(sizeof(struct object_queue), GFP_KERNEL);
	INIT_WORK(&work_init->ac_work, work_function_handle);	//dang ky ham xu ly voi mot cong viec
	INIT_DELAYED_WORK(&work_init->work_delay, work_function_handle);

/***Tao mot hang doi cong viec : gioi thieu cac cach***/
	// work_init->queue_work = create_singlethread_workqueue("object_queue");

	work_init->queue_work = create_workqueue("object_queue");

	// work_init->queue_work = alloc_ordered_workqueue("object_queue",
	// 					  WQ_MEM_RECLAIM);
	if(work_init->queue_work == NULL)
	{
		ret = -EINVAL;
		pr_info("%s Failed to creat workqueue\n",__func__);
		goto free_queue;
	}
	//Dang Ki cong viec vao danh sach cho
	queue_work(work_init->queue_work, &work_init->ac_work);
	queue_delayed_work(work_init->queue_work, &work_init->work_delay, msecs_to_jiffies(TIME_DELAY_WORK));
/****************************************************/


	/*************success****************/
	DECLARE_WORK(my_work, work_function_handle);
	DECLARE_DELAYED_WORK(my_delay_work, work_function_handle);
	my_wq = create_singlethread_workqueue("my_queue");
	// my_wq = alloc_ordered_workqueue("pm2xxx_charger_wq",
	// 					  WQ_MEM_RECLAIM);
	if(!my_wq)
	{
		pr_info("Failed to creat workqueue\n");
		return -ENOMEM;
	}
	queue_work(my_wq, &my_work);
	queue_delayed_work(my_wq, &my_delay_work, msecs_to_jiffies(TIME_DELAY_WORK));
	/************************************/
	pr_info("call flush_workqueue\n");
	// flush_workqueue(work_init->queue_work);

	// kfree(work_init);
	pr_info("init done\n");
	return ret;
free_queue:

	return ret;
}

static void __exit module_driver_exit(void)
{
	pr_info("module remove\n");
	cancel_work_sync(&my_work);
	cancel_delayed_work_sync(&my_delay_work);
	destroy_workqueue(my_wq);
	cancel_work_sync(&work_init->ac_work);
	cancel_delayed_work_sync(&work_init->work_delay);
	destroy_workqueue(work_init->queue_work);
	kfree(work_init);
}

module_init(module_driver_init);
module_exit(module_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DangLHb <haidanglai23@gmail.com>");
MODULE_DESCRIPTION("Workqueue test module driver");
