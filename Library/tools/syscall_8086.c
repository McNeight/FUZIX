/*
 *	Generate the syscall functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "syscall_name.h"

static char namebuf[128];

static void write_call(int n)
{
  int args;
  FILE *fp;
  snprintf(namebuf, 128, "fuzix8086/syscall_%s.s", syscall_name[n]);
  fp = fopen(namebuf, "w");
  if (fp == NULL) {
    perror(namebuf);
    exit(1);
  }
  /* The kernel stubs expect a stack frame with the extras on the front.
     TODO; use stubs maybe ?? */
  fprintf(fp, "\t.text\n\n"
	      "\t.globl %1$s\n\n"
	      "%1$s:\n", syscall_name[n]);

  fprintf(fp, "\tmovb $%d,%%al\n\tint $0x80\n",n);
  fprintf(fp, "\tcmpw $0,%%bx\n\tje noerr\n");
  fprintf(fp, "\tmovw %%bx,errno\n");
  fprintf(fp, "noerr:\n");
  fprintf(fp, "\tret\n");
  fclose(fp);
}

static void write_call_table(void)
{
  int i;
  for (i = 0; i < NR_SYSCALL; i++)
    write_call(i);
}

static void write_makefile(void)
{
  int i;
  FILE *fp = fopen("fuzix8086/Makefile", "w");
  if (fp == NULL) {
    perror("Makefile");
    exit(1);
  }
  fprintf(fp, "# Autogenerated by tools/syscall_8086\n");
  fprintf(fp, "CROSS_AS=ia16-elf-as\nCROSS_LD=ia16-elf-ld\nCROSS_AR=ia16-elf-ar\n");
  fprintf(fp, "ASOPTS=\n\n");
  fprintf(fp, "ASRCS = syscall_%s.s\n", syscall_name[0]);
  for (i = 1; i < NR_SYSCALL; i++)
    fprintf(fp, "ASRCS += syscall_%s.s\n", syscall_name[i]);
  fprintf(fp, "\n\nASRCALL = $(ASRCS)\n");
  fprintf(fp, "\nAOBJS = $(ASRCALL:.s=.o)\n\n");
  fprintf(fp, "syslib.lib: $(AOBJS)\n");
  fprintf(fp, "\techo $(AOBJS) >syslib.l\n");
  fprintf(fp, "\t$(CROSS_AR) rc syslib.lib $(AOBJS)\n\n");
  fprintf(fp, "$(AOBJS): %%.o: %%.s\n");
  fprintf(fp, "\t$(CROSS_AS) $(ASOPTS) -o $*.o $<\n\n");
  fprintf(fp, "clean:\n");
  fprintf(fp, "\trm -f $(AOBJS) $(ASRCS) syslib.lib syslib.l *~\n\n");
  fclose(fp);
}

int main(int argc, char *argv[])
{
  write_makefile();
  write_call_table();
  exit(0);
}
