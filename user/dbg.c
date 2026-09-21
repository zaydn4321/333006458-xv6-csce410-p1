#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/debug.h"
#include "user/user.h"

static void
usage(void)
{
  printf("usage:\n");
  printf("  dbg                 show current setting\n");
  printf("  dbg on <cat>...     enable categories\n");
  printf("  dbg off <cat>...    disable categories\n");
  printf("  dbg only <cat>...   enable exactly these\n");
  printf("  dbg level <lvl>     set detail level\n");

  printf("categories:");
  for (int i = 0; i < DBG_NCATS; i++)
    printf(" %s", dbg_cats[i].name);
  printf(" all\n");

  printf("levels:");
  for (int i = 0; i < DBG_NLEVELS; i++)
    printf(" %s", dbg_levelnames[i]);
  printf("\n");
}

// 0 if the name isnt a category
static uint64
catbit(char *s)
{
  if (strcmp(s, "all") == 0)
    return DBG_ALL;
  for (int i = 0; i < DBG_NCATS; i++)
    if (strcmp(s, dbg_cats[i].name) == 0)
      return dbg_cats[i].bit;
  return 0;
}

// -1 if the name isnt a level
static int
levelnum(char *s)
{
  for (int i = 0; i < DBG_NLEVELS; i++)
    if (strcmp(s, dbg_levelnames[i]) == 0)
      return i;
  return -1;
}

static void
show(void)
{
  uint64 mask = debugctl(DBGCTL_GETMASK, 0);
  int level = debugctl(DBGCTL_GETLEVEL, 0);

  printf("mask  0x%lx", mask);
  if (mask == 0)
    printf(" (none)");
  for (int i = 0; i < DBG_NCATS; i++)
    if (mask & dbg_cats[i].bit)
      printf(" %s", dbg_cats[i].name);
  printf("\n");

  printf("level %d", level);
  if (level >= 0 && level < DBG_NLEVELS)
    printf(" (%s)", dbg_levelnames[level]);
  printf("\n");
}

// or together the bits named in argv[2..argc), 0 if any name is bad
static uint64
collect(int argc, char **argv)
{
  uint64 bits = 0;

  if (argc < 3) {
    printf("dbg: %s needs a category\n", argv[1]);
    return 0;
  }

  for (int i = 2; i < argc; i++) {
    uint64 b = catbit(argv[i]);
    if (b == 0) {
      printf("dbg: unknown category %s\n", argv[i]);
      return 0;
    }
    bits |= b;
  }
  return bits;
}

int
main(int argc, char *argv[])
{
  uint64 mask, bits;
  int level;

  if (argc == 1) {
    show();
    exit(0);
  }

  if (strcmp(argv[1], "level") == 0) {
    if (argc != 3) {
      usage();
      exit(1);
    }
    level = levelnum(argv[2]);
    if (level < 0) {
      printf("dbg: unknown level %s\n", argv[2]);
      exit(1);
    }
    debugctl(DBGCTL_SETLEVEL, level);
    show();
    exit(0);
  }

  if (strcmp(argv[1], "on") != 0 && strcmp(argv[1], "off") != 0 &&
      strcmp(argv[1], "only") != 0) {
    usage();
    exit(1);
  }

  bits = collect(argc, argv);
  if (bits == 0)
    exit(1);

  mask = debugctl(DBGCTL_GETMASK, 0);
  if (strcmp(argv[1], "on") == 0)
    mask |= bits;
  else if (strcmp(argv[1], "off") == 0)
    mask &= ~bits;
  else
    mask = bits;

  debugctl(DBGCTL_SETMASK, mask);
  show();
  exit(0);
}
