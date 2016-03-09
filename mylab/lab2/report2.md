Lab2 实验报告
实验目的

实现代码，支持建立页表

功能分析

课程提供的os2.c代码中实现了中断处理及建立页表，lab2.c增加了写入、读取页表映射范围内数据的测试项。

中断功能

首先定义了中断码，也就是v9文档中所述的错误码fault code。

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
trap()函数依然和lab1一样，根据堆栈中的fc中断码进行相应处理。 在本实验中主要

trap(int c, int b, int a, int fc, int pc)
{
  printf("TRAP: ");
  switch (fc) {
  case FINST:  printf("FINST"); break;
  case FRPAGE: printf("FRPAGE [0x%08x]",lvadr()); break;
  case FWPAGE: printf("FWPAGE [0x%08x]",lvadr()); break;
  case FIPAGE: printf("FIPAGE [0x%08x]",lvadr()); break;
  case FSYS:   printf("FSYS"); break;
  case FARITH: printf("FARITH"); break;
  case FMEM:   printf("FMEM [0x%08x]",lvadr()); break;
  case FTIMER: printf("FTIMER"); current = 1; stmr(0); break;
  case FKEYBD: printf("FKEYBD [%c]", in(0)); break;
  default:     printf("other [%d]",fc); break;
  }
}

alltraps()
{
  asm(PSHA);
  asm(PSHB);
  asm(PSHC);
  trap();
  asm(POPC);
  asm(POPB);
  asm(POPA);
  asm(RTI);
}
分页机制

分页机制的目的在于实现虚拟地址和物理地址之间的映射关系，映射关系由页目录和页表定义。

分配内存用于存放页目录和页表项

char pg_mem[6 * 4096]; // page dir + 4 entries + alignment
int *pg_dir, *pg0, *pg1, *pg2, *pg3;
页目录和页表由setup_paging()函数建立

setup_paging()
{
  int i;
  //对齐页目录地址
  pg_dir = (int *)((((int)&pg_mem) + 4095) & -4096);
  //页表0的地址为页目录加1024
  pg0 = pg_dir + 1024;
  //后续页表地址顺序偏移1024
  pg1 = pg0 + 1024;
  pg2 = pg1 + 1024;
  pg3 = pg2 + 1024;

 //设置页目录中的页表属性为对应页present、可写和用户态
  pg_dir[0] = (int)pg0 | PTE_P | PTE_W | PTE_U;  // identity map 16M
  pg_dir[1] = (int)pg1 | PTE_P | PTE_W | PTE_U;
  pg_dir[2] = (int)pg2 | PTE_P | PTE_W | PTE_U;
  pg_dir[3] = (int)pg3 | PTE_P | PTE_W | PTE_U;
  //剩余页表项清0
  for (i=4;i<1024;i++) pg_dir[i] = 0;

  //建立页表项，4K为一页的基址，依次递增，并设置页属性为present、可写和用户态，共映射了4096*4096即16M的地址空间
  for (i=0;i<4096;i++) pg0[i] = (i<<12) | PTE_P | PTE_W | PTE_U;  // trick to write all 4 contiguous pages
  //设置页目录地址
  pdir(pg_dir);
  //分页使能
  spage(1);
}
分页功能测试

main()函数中测试了不同的中断异常，如非法指令、整除0等，下面主要分析分页功能测试。

设置堆栈，并启动分页

// 设置栈指针SP为4M
  asm(LI, 4*1024*1024); // a = 4M
  asm(SSP); // sp = a
// 开启分页
  setup_paging();
  printf("identity map...ok\n");
内存访问

//开启分页后，正常测试
  printf("test page read...");
  t = *(int *)(50<<12);
  printf("...ok\n");
  printf("test page write...");
  *(int *)(50<<12) = 5;
  printf("...ok\n");
//将页表项设置为0，从而访问时将引发异常，进入中断处理函数，打印异常。 
  printf("test page fault read...");
  pg0[50] = 0;
  pdir(pg_dir);
  t = *(int *)(50<<12);
  printf("...ok\n");

  printf("test page fault write...");
  *(int *)(50<<12) = 5;
  printf("...ok\n");
其余辅助函数

以下函数均利用了栈帧进行参数传递，第一个参数的地址距sp的偏移为8，第二个参数的偏移为16，加载到寄存器A、B后，调用对应CPU指令。

// 读外设，从终端获取一个输入字符
int in(port)    { asm(LL,8); asm(BIN); }
// 写外设，将字符写出到终端
out(port, val)  { asm(LL,8); asm(LBL,16); asm(BOUT); }
//设置中断处理函数地址
ivec(void *isr) { asm(LL,8); asm(IVEC); }
//获取访问异常的地址
lvadr()         { asm(LVAD); }
//设置时钟中断间隔到时值
stmr(int val)   { asm(LL,8); asm(TIME); }
//设置页目录起始地址为value所指地址
pdir(value)     { asm(LL,8); asm(PDIR); }
//页使能或屏蔽
spage(value)    { asm(LL,8); asm(SPAG); }
//停机
halt(value)     { asm(LL,8); asm(HALT); }
//内存拷贝，利用了a、b、c寄存器传递源地址、目的地址和长度，完成后将拷贝长度放入寄存器A
void *memcpy() { asm(LL,8); asm(LBL, 16); asm(LCL,24); asm(MCPY); asm(LL,8); }
//内存清0函数
void *memset() { asm(LL,8); asm(LBLB,16); asm(LCL,24); asm(MSET); asm(LL,8); }
//从字符串中查找特定字符
void *memchr() { asm(LL,8); asm(LBLB,16); asm(LCL,24); asm(MCHR); }
//
write(fd, char *p, n) { while (n--) out(fd, *p++); }
//扫描字符串中的0后返回与字符串首地址之差，及字符串长度
int strlen(char *s) { return memchr(s, 0, -1) - (void *)s; }

enum { BUFSIZ = 32 };
//字符串格式化函数
int vsprintf(char *s, char *f, va_list v)
{
}
//基于write和vsprint实现标准printf函数
int printf(char *f) { static char buf[4096]; return write(1, buf, vsprintf(buf, f, &f)); } // XXX remove static from buf

