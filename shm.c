#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

struct {
  struct spinlock lock;
  struct shm_page {
    uint id;
    char *frame;
    int refcnt;
  } shm_pages[64];
} shm_table;

void shminit() {
  int i;
  initlock(&(shm_table.lock), "SHM lock");
  acquire(&(shm_table.lock));
  for (i = 0; i< 64; i++) {
    shm_table.shm_pages[i].id =0;
    shm_table.shm_pages[i].frame =0;
    shm_table.shm_pages[i].refcnt =0;
  }
  release(&(shm_table.lock));
}

int shm_open(int id, char **pointer) {

//you write this
	acquire(&(shm_table.lock));
	unsigned int i;
	int f=0;
	for (i = 0; i < 64; i++){
		if(shm_table.shm_pages[i].id == id){
			f = 1;
			break;
		}
	}
	if (!f){
		for (i = 0; i < 64; i++){
			if(shm_table.shm_pages[i].id == 0&& shm_table.shm_pages[i].frame == 0&& shm_table.shm_pages[i].refcnt == 0)
				break;
		}
		char* Nmem;
		Nmem = kalloc();
		memset(Nmem, 0, PGSIZE);
		shm_table.shm_pages[i].id = id;
		shm_table.shm_pages[i].frame = Nmem;	
	}
	shm_table.shm_pages[i].refcnt++;
	struct proc *cur = myproc();
	mappages(cur->pgdir, (void *)PGROUNDUP(cur->sz), PGSIZE, V2P(shm_table.shm_pages[i].frame), PTE_W | PTE_U);
        *pointer = (char *)PGROUNDUP(cur->sz);
        cur->sz = PGROUNDUP(cur->sz) + PGSIZE;
        release(&(shm_table.lock));
	return 0; //added to remove compiler warning -- you should decide what to return
}


int shm_close(int id) {
//you write this too!
	acquire(&(shm_table.lock));
	unsigned int i;
  	for(i = 0; i < 64; i++) {
    		if(shm_table.shm_pages[i].id == id) {
      			if(shm_table.shm_pages[i].refcnt > 1) {
        			shm_table.shm_pages[i].refcnt -= 1;
      			}
      			else {
        			shm_table.shm_pages[i].id = 0;
        			shm_table.shm_pages[i].frame = 0;
        			shm_table.shm_pages[i].refcnt = 0;
      			}
    		}
  	}	
	release(&(shm_table.lock));
	return 0; //added to remove compiler warning -- you should decide what to return
}
