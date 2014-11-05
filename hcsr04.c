#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/delay.h>

#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/interrupt.h> 

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Sergio Tanzilli");
MODULE_DESCRIPTION("Driver for HC-SR04 ultrasonic sensor");

#define HCSR04_TRIGGER	95 // J4.32 -   PC31
#define HCSR04_ECHO  	91 // J4.30 -   PC27
 
static int gpio_irq=-1;
static ktime_t echo_start;
static ktime_t echo_end;
 
// This function is called when you write something on /sys/class/hcsh04/trigger
// passing in *buf the incoming content

static ssize_t hcshr04_trigger(struct class *class, struct class_attribute *attr, const char *buf, size_t len) {
	printk(KERN_INFO "Buffer len %d bytes\n", len);
	gpio_set_value(HCSR04_TRIGGER,0);
	
	udelay(10);
	
	gpio_set_value(HCSR04_TRIGGER,1);
	return len;
}
	
// Sysfs definitions for hcsr04 class
static struct class_attribute hcsr04_class_attrs[] = {
   __ATTR(trigger,   0200, NULL, hcsr04_trigger),
   __ATTR_NULL,
};

// Name of directory created in /sys/class
static struct class hcsr04_class = {
  .name =        "hcsr04",
  .owner =       THIS_MODULE,
  .class_attrs = hcsr04_class_attrs,
};

static irqreturn_t gpio_isr(int irq, void *data)
{
	if (gpio_get_value(HCSR04_ECHO)==1) {
		echo_start=ktime_get();
		printk(KERN_INFO "%lld\n",ktime_to_ms(echo_start));
	}
	if (gpio_get_value(HCSR04_ECHO)==0) {
		echo_end=ktime_get();
		printk(KERN_INFO "%lld\n",ktime_to_ms(echo_end));
		printk(KERN_INFO "%lld\n",ktime_to_ms(ktime_sub(echo_end,echo_start)));
	}
	return IRQ_HANDLED;
}

static int hcsr04_init(void)
{	
	int rtc;
	
    printk(KERN_INFO "HC-SR04 driver v0.05 initializing.\n");

	if (class_register(&hcsr04_class)<0) goto fail;
          
	rtc=gpio_request(HCSR04_TRIGGER,"TRIGGER");
	if (rtc!=0) {
		printk(KERN_INFO "Error %d\n",__LINE__);
		goto fail;
	}

	rtc=gpio_request(HCSR04_ECHO,"ECHO");
	if (rtc!=0) {
		printk(KERN_INFO "Error %d\n",__LINE__);
		goto fail;
	}
	
	rtc=gpio_direction_output(HCSR04_TRIGGER,0);
	if (rtc!=0) {
		printk(KERN_INFO "Error %d\n",__LINE__);
		goto fail;
	}
	
	rtc=gpio_direction_input(HCSR04_ECHO);
	if (rtc!=0) {
		printk(KERN_INFO "Error %d\n",__LINE__);
		goto fail;
	}

	/* http://lwn.net/Articles/532714/ */
	rtc=gpio_to_irq(HCSR04_ECHO);
	if (rtc<0) {
		printk(KERN_INFO "Error %d\n",__LINE__);
		goto fail;
	} else {
		printk(KERN_INFO "gpio_to_irq(HCSR04_ECHO)=%d\n",rtc);
		gpio_irq=rtc;
	}
	
    gpio_set_value(HCSR04_TRIGGER,1);


	rtc = request_irq(gpio_irq, gpio_isr, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_DISABLED, "hc-sr04.trigger", NULL);

	if(rtc) {
		printk(KERN_ERR "Unable to request IRQ: %d\n", rtc);
		goto fail;
	}


	return 0;

fail:
	return -1;

}
 
static void hcsr04_exit(void)
{
	if (gpio_irq!=-1) {	
		free_irq(gpio_irq, NULL);
	}
	gpio_free(HCSR04_TRIGGER);
	gpio_free(HCSR04_ECHO);
	class_unregister(&hcsr04_class);
    printk(KERN_INFO "HC-SR04 disabled.\n");
}
 
module_init(hcsr04_init);
module_exit(hcsr04_exit);
