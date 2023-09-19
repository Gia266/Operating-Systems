#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/ktime.h>


#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
#define HAVE_PROC_OPS
#endif

static int lab1_show(struct seq_file *m, void *v) {

  
  //output:
  seq_printf(m, "Current Process PCB Information \n");
  
  //print out name 
  seq_printf(m, "Name = %s \n", current->comm);

  //print out Process identifier (PID)
  seq_printf(m, "PID = %ld \n", current->pid); 
  
  //print out Parent process identifier (PPID)
  seq_printf(m,"PPID = %d \n", task_ppid_nr(current));
  
  // Based on the output value, print out the correct state of the process
  if (current->state <= 0){
  //state of the process is waiting if its equal to 0 according to sched.h
  seq_printf(m, "State = Running  \n");

  }else if (current->state <= 2){
  //State of the process is waiting if its less than or equal to 2 according to sched.h
  seq_printf(m, "State = Waiting \n");

  }else{
  // else, the state has stopped according to sched.h
  seq_printf(m, "State = Stopped \n");
  }
  
  //print out User Identifier of the user 
  seq_printf(m, "Real UID = %ld \n", current->cred->uid);
  
  //print out effective user id
  seq_printf(m, "Effective UID = %ld \n",  current->cred->euid);
 
 //print out saved user id
  seq_printf(m, "Saved UID = %ld \n", current->cred->suid);
  
  //print out real group id
  seq_printf(m, "Real GID = %ld \n", current->cred->gid);
  
  //print out effective group id
  seq_printf(m, "Effective GID = %ld \n", current->cred->egid);
  
  //print out saved group id
  seq_printf(m, "Saved GID = %ld \n", current->cred->sgid);
  

  //current-> points to task_struct of current proccess
  //current->state;

  return 0;
}

static int lab1_open(struct inode *inode, struct  file *file) {
  return single_open(file, lab1_show, NULL);
}

#ifdef HAVE_PROC_OPS
static const struct proc_ops lab1_fops = {
    //operating map
  .proc_open = lab1_open,
  .proc_read = seq_read,
  .proc_lseek = seq_lseek,
  .proc_release = single_release,

};
#else
static const struct file_operations lab1_fops = {
  //operating map
  .owner = THIS_MODULE,
  .open = lab1_open,
  .read = seq_read,
  .llseek = seq_lseek,
  .release = single_release,

};
#endif

static int __init lab1_init(void) {

  if( proc_create("lab1", 0, NULL, &lab1_fops) == NULL){

    // if function returns NULL, then it should return enomem
    return "ENOMEM";
  }
  //create lab1 file
  printk(KERN_INFO "lab1mod in\n");
  return 0;
}

static void __exit lab1_exit(void) {

   remove_proc_entry("lab1", NULL);
   //remove lab1 file
  printk(KERN_INFO "lab1mod out\n");
}

MODULE_LICENSE("GPL");

//loading and removing modules
module_init(lab1_init);
module_exit(lab1_exit);