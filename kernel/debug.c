#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "debug.h"

// no lock, reads cant tear
uint64 dbg_mask = 0;
int dbg_level = DBG_INFO;

char *
dbg_catname(uint64 cat)
{
  for (int i = 0; i < DBG_NCATS; i++)
    if (dbg_cats[i].bit & cat)
      return dbg_cats[i].label;
  return "?      ";
}

// -1 if no process
int
dbg_curpid(void)
{
  struct proc *p = myproc();

  return p ? p->pid : -1;
}
