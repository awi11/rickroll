#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "rickroll"

static int dev_open(struct inode*, struct file*);
static int dev_release(struct inode*, struct file*);
static ssize_t dev_read(struct file*, char*, size_t, loff_t*);
static ssize_t dev_write(struct file*, const char*, size_t, loff_t*);

static struct file_operations fops = {
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

// offset into the kernel's device driver table, which tells the kernel what 
// kind of device it is
static int major;

// runs as soon as the module is loaded into the kernel
static int __init rickroll_init(void) {

    // register a character device, 0 means dynamically allocate me a major #
    major = register_chrdev(0, DEVICE_NAME, &fops);

    if (major < 0) {
        printk(KERN_ALERT "Rickroll load failed\n");
        return major;
    }

    printk(KERN_INFO "Rickroll module has been loaded: %d\n", major);
    return 0;
}

// runs as soon as the module is unloaded into the kernel
static void __exit rickroll_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Rickroll module has been unloaded\n");
}

static int dev_open(struct inode *inodep, struct file *filep) {
   printk(KERN_INFO "Rickroll device opened\n");
   return 0;
}

static ssize_t dev_write(struct file *filep, const char *buffer,
                         size_t len, loff_t *offset) {

   printk(KERN_INFO "Sorry, rickroll is read only\n");
   return -EFAULT;
}

static int dev_release(struct inode *inodep, struct file *filep) {
   printk(KERN_INFO "Rickroll device closed\n");
   return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    int errors = 0;
    char *message = "never gonna give you up, never gonna let you down... ";
    int message_len = strlen(message);

    errors = copy_to_user(buffer, message, message_len);

    return errors == 0 ? message_len : -EFAULT;
}

module_init(rickroll_init);
module_exit(rickroll_exit);
