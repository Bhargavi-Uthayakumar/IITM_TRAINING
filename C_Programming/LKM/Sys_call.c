#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Arya");
MODULE_DESCRIPTION("Kernel module to log file open syscalls using kprobes");

static struct kprobe kp;

static int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
    const char __user *filename = (const char __user *)regs->di;
    char fname[256];

    if (strncpy_from_user(fname, filename, sizeof(fname)) > 0)
        printk(KERN_INFO "[sys_openat] File opened: %s\n", fname);

    return 0;
}

static int __init kprobe_init(void)
{
    kp.symbol_name = "do_sys_openat2"; // internal syscall helper
    kp.pre_handler = handler_pre;

    if (register_kprobe(&kp) < 0) {
        pr_err("Failed to register kprobe.\n");
        return -1;
    }

    pr_info("Kprobe registered at %p\n", kp.addr);
    return 0;
}

static void __exit kprobe_exit(void)
{
    unregister_kprobe(&kp);
    pr_info("Kprobe unregistered.\n");
}

module_init(kprobe_init);
module_exit(kprobe_exit);