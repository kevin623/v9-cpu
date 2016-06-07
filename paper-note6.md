##Title：
###Scalable Address Spaces Using RCU Balanced Trees
##Author：
###Austin T. Clements M. Frans Kaashoek Nick Zeldovich
MIT CSAIL
###摘要：
软件开发者通过开发多线程软件来利用多核处理器，这些同一应用软件的多个线程共享同一个地址空间。共享同一地址是有代价的：内核虚拟内存操作, 如处理分页失败；增加地址空间；文件映射等会限制这些应用的可扩展性。在广泛使用的操作系统，所有的这些操作都是依靠一个进程锁同步。论文给出了一个新的设计，利用RCU来增加在一个共享地址空间上的内核操作的并发数。这样，分页错误错误能通过更换相同地址空间和，避免和其它却也错误竞争共享缓存实现并发运行。为实现并发，论文介绍了基于RCU的二叉平衡树来存储内存映射。通过3个多线程应用在linux2.6.37  ，80个内核上的实验表明，性能改善了。基于RCU二叉树使得分页错误故障，以固定成本运行，显示这个设计在超过80个内核的机器上可扩展性很好。

###论文主要内容：
常见的并行程序设计模式是共享内存的多线程设计，一个应用的所有线程共享一个地址空间。这个共享地址空间要求内核虚拟存储系统也要并发，内核并发地址空间操作严重影响多线程的VM密集型应用。实际上，应用程序经常用多个进程代替多个线程来避免单一，共享的地址空间，但是这让共享变得复杂。应用程序也能不声不响的频繁使用VM：libc可以调用nmap或者munmap来增加地址空间或者内存映射文件，甚至一个天真的内存访问因为修改应用程序的共享页表能引起页表分页错误错误。本文全部目标是让密集使用虚拟内存的多线程应用具有好的扩展性。
在大部分常用操作系统，地址空间是由一套内存映射区域和一个页表树组成。见图1.每个内存映射区域描述虚拟地址空间和存储关于映射相关信息，比如保护位和后备文件。大部分广泛应用操作系统利用树来存储内存区域是因为应用程序常有成千的内存区域（比如，因为动态链接），树能帮助操作系统快速找到包含特定虚拟聂存的区域。页表记录冲虚拟页面到物理页面的特定银蛇结构。与区域树不同，区域树通过硬件定义。论文中假定使用x8664位四级页表，但是这种思想并只能用于x86。
操作系统提供3中地址空间操作：mmap和munmap系统调用；分页错误，它出现在当硬件不能映射虚拟地址到一个物理地址。mmap创建内存映射区域和把它们加入区域树。mummap从树中删除区域和令其在硬件页表结构中失效。分页错误在区域树中查询失败的虚拟地址，如果虚拟地址被映射了，在硬件页面表中安装一个实体，继续执行应用。我们特别关注软页表失败，它可能指派新页面，新页表或者内存中已经存在的映射页面，但它不会置入来自外部驱动的数据。
在同一进程中的所有线程共享区域树和硬件页表，确保当几个内核正确执行mmap，munmap和页面失败处理，大部分操作系统使用几个锁。常用操作系统Solaris，Linux每个进程使用一把读写锁，它对页面失败和内存映射操作序列处理（见图2），这限制了多线程密集使用虚拟内存的应用程序的可扩展性。在这种操作系统中，一个进程在一个时刻只能处理一个内存映射操作，这些操作也耽误分页错误处理。不同虚拟地址分页错误处理同时运行，阻塞其它执行内存映射操作的线程。
我们工作目标是让分页错误处理扩展到具有大量内核的情形。这要求解决两个基本形式的序列化。首先， 必须允许软分页错误同时运行mmap和munmap操作，剔除在每个进程读写锁的序列化。这是非常困难的，因为3种操作都要改变地址空间数据结构（区域树，页表或者两者并存）。此外，我们必须保证在不修改共享缓存线的情况下运行分页失败操作，以便去除因为处理器独自缓存请求引发的序列化。在读模式下，因为保护锁存在，相求修改共享缓存很困难。
为了实现目标，论文提出了新的并发地址空间设计，通过采用RCU在虚拟内存系统提出前者内容，引入BONSAI tree，一种基于RCU并发平衡树作为设计的一部分。当RCU广泛应用与LINUX内核，它不会用到内核地址空间结构。这是因为两个重大挑战：地址空间结构要遵循复杂约束条件。在地址空间结构全面应用RCU需要一个兼容RCU的并发平衡树，这很复杂。
作者在linux2.6.37 部署并发地址空间设计来测试他们提出的方法。该设计利用BONSAI tree在页表和树操作部署RCU，允许分页处理虚拟地址空间，不需要考虑保护锁或者专属的缓存访问。子啊3个虚拟内存密集应用平台Metis and
Psearchy，Dedup在80个内核机器上，吞吐量提高到3.4倍，1.8倍，1.7倍。此外，一个微测试平台显示，以我们设计进行页面处理的代价恒定，不依赖并发的分页故障或者内存映射操作。作为对比，在linux采用缺省方案负担增长很快。
本论文提出了一种新的兼容RCU平衡树的设计，这种新的地址空间设计利用BONSAI实现分页故障处理的高扩展性；在linux下设计了一个工具；用实验进行苹果。尽管我们基于linux设计，但我们认为我们的设计可以用于其它OS。尽管不同内核地址空间管理的细节不同，大部分共享内存的内核使用相似的地址空间描述方案。