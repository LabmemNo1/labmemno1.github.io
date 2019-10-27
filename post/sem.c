#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/ioctl.h>
#include <linux/slab.h>
#include <mach/map.h>
#include <mach/gpio.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-core.h>
#include <plat/gpio-cfg.h>
#include <plat/gpio-cfg-helpers.h>

#define IOCTL_GPIO_ON 1 /*LED亮控制字，具体控制哪支灯由arg给出*/
#define IOCTL_GPIO_OFF 0 /*LED灭控制字，对应的LED 灯会熄灭*/
#define LED_MAJOR 0 /*预设的主设备号*/
/* 用来指定LED所用的GPIO引脚*/

static unsigned long gpio_table [] =
{
	
	S5PV210_GPG2(2),
	S5PV210_GPG2(3),
	S5PV210_GPG2(5),
	S5PV210_GPG2(0),
	S5PV210_GPG2(1),
};
/* 用来指定GPIO引脚的功能：输出*/
static unsigned int gpio_cfg_table [] =
{
	S3C_GPIO_SFN(1),//对应引脚的功能配置，这里为输出。
	S3C_GPIO_SFN(1),
	S3C_GPIO_SFN(1),//对应引脚的功能配置，这里为输出。
	S3C_GPIO_SFN(1),
	S3C_GPIO_SFN(1),
};
static int LED_major = LED_MAJOR;
static struct class *ioctrl_class;
/*LED设备结构体*/
struct LED_dev                                     
{                                                        
  struct cdev cdev; /*cdev结构体*/  
  struct semaphore lock;                           
};
struct LED_dev *LED_devp; /*设备结构体指针*/
/*文件打开函数*/
int LED_open(struct inode *inode, struct file *filp)
{

	int i;
	int err;
	filp->private_data = LED_devp;  /*将设备结构体指针赋值给文件私有数据指针*/	
	


//信号量的操作
if(!down_interruptible(&LED_devp->lock))
{
	
printk(KERN_INFO " leds opened\n");
//申请gpio。	
	err = gpio_request(gpio_table[0], "GPJ2_2");
	if(err) //成功返回0
	{
		printk(KERN_ERR "failed to request GPJ2_0 for led pin\n");
		return err;
	}
	err = gpio_request(gpio_table[1], "GPG2_3");
	if(err)
	{
		printk(KERN_ERR "failed to request GPG2_1 for led pin\n");
		return err;
	}
	err = gpio_request(gpio_table[2], "GPG2_5");
	if(err)
	{
		printk(KERN_ERR "failed to request GPG2_2 for led pin\n");
		return err;
	}	
	err = gpio_request(gpio_table[3], "GPG2_5");
	if(err)
	{
		printk(KERN_ERR "failed to request GPG2_3 for led pin\n");
		return err;
	}
	err = gpio_request(gpio_table[4], "GPG2_5");
	if(err)
	{
		printk(KERN_ERR "failed to request GPG2_4 for led pin\n");
		return err;
	}
//初始化引脚。
	for (i = 0; i < sizeof(gpio_table)/sizeof(unsigned long); i++)
	{
		s3c_gpio_cfgpin(gpio_table[i], gpio_cfg_table[i]);//由上面定义可知这里将所有的引脚配置成输出
		gpio_direction_output(gpio_table[i], 0);//指定的引脚输出0.（初始状态为0），led 为灭
	}		





	

}
else
{
	printk("正在被使用\n");
	return -EBUSY;

}







  return 0;
}
/*文件释放函数*/
int LED_release(struct inode *inode, struct file *filp)
{
	gpio_free(gpio_table[0]);/*释放资源，与gpio_request对应。*/
	gpio_free(gpio_table[1]);
	gpio_free(gpio_table[2]);/*释放资源，与gpio_request对应。*/
	gpio_free(gpio_table[3]);	
	gpio_free(gpio_table[4]);
	printk(KERN_INFO "LEDs driver successfully close\n");
	up(&LED_devp->lock);//释放信号量
  return 0;
}
/*读函数*/
static ssize_t LED_read(struct file *filp, char __user *buf, size_t size,
  loff_t *ppos)
{
  int ret = 0;
  struct LED_dev *dev = filp->private_data; /*获得设备结构体指针*/
  return ret;
}

/*写函数*/
static ssize_t LED_write(struct file *filp, const char __user *buf,
  size_t size, loff_t *ppos)
{
  int ret = 0;
  struct LED_dev *dev = filp->private_data; /*获得设备结构体指针*/
  return ret;
}
static int LED_ioctl(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg)
{ 
//ioctl函数接口
	if (arg > sizeof(gpio_table)/sizeof(unsigned long))
	{
		return -EINVAL;
	}
	switch(cmd)
	{
		case IOCTL_GPIO_ON:// 设置指定引脚的输出电平为0，由电路图可知，输出0时为亮
			gpio_direction_output(gpio_table[arg], 1);
			return 0;
		case IOCTL_GPIO_OFF:// 设置指定引脚的输出电平为1，由电路图可知，输出1时为灭
			gpio_direction_output(gpio_table[arg], 0);
			return 0;
		default:
		return -EINVAL;
	}
}

/*文件操作结构体*/
static const struct file_operations LED_fops =
{
  .owner = THIS_MODULE,
  .read = LED_read,
  .write = LED_write,
  .open = LED_open,
  .release = LED_release,
  .ioctl =LED_ioctl,
   /*填写代码：实现主要控制功能*/
};
/*初始化并注册cdev*/
static void LED_setup_cdev(struct LED_dev *dev, int index)
{
  int err, devno = MKDEV(LED_major, index);
  cdev_init(&dev->cdev, &LED_fops);
  dev->cdev.owner = THIS_MODULE;
  dev->cdev.ops = &LED_fops;
  err = cdev_add(&dev->cdev, devno, 1);
  if (err)
    printk(KERN_ALERT "Error %d adding LED%d", err, index); 

}
/*设备驱动模块加载函数*/
int LED_init(void)
{
  int result;
  dev_t devno = MKDEV(LED_major, 0);
  /* 申请设备号*/
  if (LED_major)
    result = register_chrdev_region(devno, 1, "MSB_SEM");
  else  /* 动态申请设备号 */
  {
    result = alloc_chrdev_region(&devno, 0, 1, "MSB_SEM");
    LED_major = MAJOR(devno);
  }  
  if (result < 0)
    return result;
	LED_devp=kmalloc(sizeof(struct LED_dev),GFP_KERNEL);
   
  
  if (!LED_devp)    /*申请失败*/
  {
    result =  - ENOMEM;
    goto fail;
  }
  memset(LED_devp, 0, sizeof(struct LED_dev));
  LED_setup_cdev(LED_devp, 0);
	ioctrl_class = class_create(THIS_MODULE, "MSB_SEM");
	device_create(ioctrl_class, NULL, MKDEV(LED_major, 0), NULL, "MSB_SEM");
	init_MUTEX(&LED_devp->lock);//初始化互斥体
  fail: unregister_chrdev_region(devno, 1);
  return result;
}
/*模块卸载函数*/
void LED_exit(void)
{
	cdev_del(&LED_devp->cdev);
	kfree(LED_devp);
	unregister_chrdev_region(MKDEV(LED_major,0),1);


      
  device_destroy(ioctrl_class, MKDEV(LED_major, 0));
	class_destroy(ioctrl_class);
}
MODULE_AUTHOR("MAOMAO");
MODULE_LICENSE("Dual BSD/GPL");
module_param(LED_major, int, S_IRUGO);
module_init(LED_init);
module_exit(LED_exit);
