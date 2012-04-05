#ifndef PROTO_H
#define PROTO_H

#include <idt.h>
#include <inode.h>
#include <tty.h>
#include <vm.h>

struct trap;
struct seg_desc;
struct inode;
struct pde;
struct super;

// tty.c
void    tty_init();
void    cls();
void    putch(char ch);
void    puts(char *ch);
void    printk(char *fmt, ...);

// sched.c
void wakeup(uint chan);
void sleep(uint chan, int pri);

// sysent.c
int syserr(uint err);
uint suser();
int nosys(struct trap *tf);

// trap.c
void idt_init();
void irq_enable(uchar irq);
void set_hwint(int nr, int (*func)(struct trap *tf));

// seg.c
uint    get_seg_limit(struct seg_desc *seg);
extern  struct seg_desc     gdt[];

// mm/pm.c
int do_pgfault(struct trap *tf);
struct page* pgalloc();
struct page* pgfind(uint pn);
int pgfree(struct page *pp);
struct pte* pgattach(struct pde *pgd, uint vaddr, struct page *pp, uint flag);

// mm/pgfault.c
void do_no_page(uint vaddr);
void do_wp_page(uint vaddr);

// timer.c
void do_timer();
uint time();
void timer_init();

// kern/syscall.c
void    do_syscall();

void    ltr(uint n);
void    lldt(uint n);
void    ljmp(ushort seg, uint offset);

// kern/exec.c
int upush(uint *esp, char *buf, int len);
int upush_argv(uint *esp, char **tmp);

// kern/signal.c
int sigsend(int pid, int n, int priv);

// mm/vm.c
int vm_clone(struct vm *to);
struct vma* find_vma(uint addr);
int vma_init(struct vma *vp, uint base, uint size, uint flag, struct inode *ip, uint ioff);

// mm/pte.c
int pgd_init(struct pde *pgd);
struct pte* find_pte(struct pde *pgd, uint vaddr, uint creat);
int pt_free(struct pde *pgd);
int pt_copy(struct pde *npgd, struct pde *opgd);
void flmmu();

// mm/malloc.c
void* kmalloc(uint size);
int kfree(void* addr, uint size);

// drv/bio.c
int     nodev();
int     nulldev();

// blk/buf.c
struct buf* bread(int dev, uint blkno);
void brelse(struct buf *bp);
void bwrite(struct buf *bp);
struct buf* getblk(int dev, uint blkno);
void iodone(struct buf *bp);
void iowait(struct buf *bp);
void notavail(struct buf *bp);

// blk/hd.c
int hd_wait_ready();
void hd_start();

// fs/rdwri.c
int readi(struct inode *ip, char *buf, uint off, uint cnt);
int writei(struct inode *ip, char *buf, uint off, uint cnt);

// fs/bmap.c
int bmap(struct inode *ip, ushort nr, uchar creat);
int itrunc(struct inode *ip);

// fs/inode.c
struct inode* iget(ushort dev, uint num);
void iput(struct inode *ip);
int iload(struct inode *ip);
void iupdate(struct inode *ip);
void lock_ino(struct inode *ip);
void unlk_ino(struct inode *ip);
void dump_inode(struct inode *ip);

// fs/alloc.c
int balloc(ushort dev);
int bfree(ushort dev, uint nr);
int bzero(ushort dev, uint bn);
int ialloc(ushort dev);
void ifree(ushort dev, uint ino);
int find_bit(char *bm, int size);

// fs/namei.c
struct inode* namei(char *path, uchar creat);
struct inode* namei_parent(char *path, char **name);
uint find_entry(struct inode* dip, char *name, uint len);
uint link_entry(struct inode *dip, char *name, uint len, uint ino);
int unlink_entry(struct inode *dip, char *name, int len);

// fs/file.c 
struct file* falloc(int fd);

// fs/super.c
struct super* getsp(ushort dev);
int spload(struct super *sp);
void spupdate(struct super *sp);
void unlk_sp(struct super *sp);
void putsp(struct super *sp);

// fs/open.c
int do_open(char *path, uint flag, uint mode);
int do_dup(int fd);
int do_close(int fd);
int do_dup(int fd);
int do_dup2(int fd, int newfd);
int ufalloc();
struct file* falloc(int fd);

// chr/tty.c
int tty_read(ushort dev, char *buf, uint cnt);
int tty_write(ushort dev, char *buf, uint cnt);
int tty_open(ushort dev);
int tty_input(struct tty *tp, char ch);

/* --------------------------------------------------- */
/* panic */
#define panic(str)  \
    do {            \
        printk("\nPANIC: %s:%d: %s\n", __FILE__, __LINE__, str);\
        asm volatile("cli");\
        for(;;);\
    } while(0)

#define max(a, b) (((a)>(b))?(a):(b))
#define min(a, b) (((a)<(b))?(a):(b))

#endif
