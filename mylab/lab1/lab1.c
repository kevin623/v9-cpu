// os0.c -- simple timer isr test

#include <u.h>

int current;
int count;

enum { // processor fault codes
  FMEM,   // bad physical address
  FTIMER, // timer interrupt
  FKEYBD, // keyboard interrupt
  FPRIV,  // privileged instruction
  FINST,  // illegal instruction
  FSYS,   // software trap
  FARITH, // arithmetic trap
  FIPAGE, // page fault on opcode fetch
  FWPAGE, // page fault on write
  FRPAGE, // page fault on read
  USER=16 // user mode exception
};

trap(int b, int a, int fc, int pc)
{

    switch(fc) {
        case FTIMER:
            current++;
            break;
        case FKEYBD:
            count++;
	    break;
        default:
            ;
      }
}

out(port, val)  { asm(LL,8); asm(LBL,16); asm(BOUT); }
ivec(void *isr) { asm(LL,8); asm(IVEC); }
stmr(int val)   { asm(LL,8); asm(TIME); }
halt(val)       { asm(LL,8); asm(HALT); }

alltraps()
{
  asm(PSHA);
  asm(PSHB);

 
  trap();

  asm(POPB);
  asm(POPA);
  asm(RTI);
}

main()
{
  current = 0;
  count = 0;

  stmr(10000);
  ivec(alltraps);

  asm(STI);

  while (current <  10) {
        if (count & 1) out(1, 'K'); else out(1, '0');
  }

  halt(0);
}
