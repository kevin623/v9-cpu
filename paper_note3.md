##论文1：Scalable Kernel TCP Design and Implementation for Short-Lived Connections
##作者：Xiaofeng Lin，Yu Chen，Xiaodong Li等
##主要内容：
随着网络带宽的不断增长，CPU内核的增多以及移动应用带来更多的的短期连接需求，具有可扩展性的TCP协议栈日益成为系统性能的关键。尽管人们已经提出许多新的设计，但是生产环境仍然需要自底向上，兼容现存应用的协议栈设计。  

作者提出了Fastsocket，兼容BSD套接字，并且可有可扩展性的内核套接字的新设计。它可以在TCP协议栈实现桌面级的连接划分，确保本地连接，包括主动连接和被动连接的情况。Fastsocket架构是一个分块的设计，它能去除在整个栈内的各种竞争锁定。此外，Fastsocket保持所有TCP协议栈基本功能和BSDsocket API的兼容，应用程序不需做任何修改。  

在Fastsocket，它主要做了三个方面的改动：1）对全局共享数据结构进行划分，引入listen table 和established 。2） 正确引导进入的数据包实现本地连接。3）在VFS中为socket提供一个快速路径，用于解决VFS中的兼容可扩展性问题和对BSD socket API的兼容。  

图1描述了Fastsocket架构，它由三部分构成。TCB数据结构的分割（local listen Table ,local established Table）;Receive Flow Deliver;Fastsocket-aware VFS.这三部分通过协作为每一个连接提供一个Per-Core 进程区域。为实现 the Per-Core Process Zone,作者对共享数据结构，连接内核绑定和VFS抽象实现进行了改进：  

1. Fastsocket对数据结构分割和管理全局TCB tables。这样当NET_RX请求到达TCP层，Fastsocket使用per-core Local Listen Table 和per-core Local Established Table进行整个TCB管理。  

2. 在一个数据包进入网络协议栈之前，Fastsocket使用Receive Flow Deliver来引导数据包进入一个对应local Table的适当的CPU内核。利用这种方式，Fastsocket能实现最大的本地连接和最小的CPU cache反弹。  
  
3.  内核的VFS层是套接字API抽象和兼容的关键。支持Fastsocket的VFS旁路所有不必要的和集中锁定的程序；使用专门的套接字快速路径去消除可扩展性瓶颈。  

在设计中，作者提出用Local Listen Table来实现桌面级的listen socket分割来保证TCP协议栈的鲁棒性。在内核TCP设置阶段，Fastsocket为每一个CPU内核指派一个local listen table。在启动阶段，服务器端的应用程序启动第一个进程监听某一个端口，等待客户请求。内核在global listen table响应创建一个监听套接字。接着，服务器创建多个process分支，并把它们和不同CPU进行绑定。这些子进程继承了监听套接字，准备接受新的连接和并行处理所有已接受的链接。  

Local ListenTable用于实现本地被动连接，而Receive Flow      Deliver用来解决主动本地连接的问题。要建立本地的主动连接，关键在于把当前内核对源端口编码。在此利用哈希函数把端口映射到CPU内核。当运行在CPU内核c上的应用试图建立一个主动连接。RFD选择一个源端口，此端口可以通过哈希函数对应到内核C。当收到响应报文，RFD从中检出目的端口，此端口就是RFD先前为内核选定的，它决定哪个内核处理这个数据包。当此内核当前没有处理数据时，RFD控制数据包到内核交由其处理。通过这个办法RFD保证每一个活动的连接总是能被同一个内存处理。  

    支持Fastsocket的VFS。socket与磁盘上的文件有很大不同：1.Socket存在于内存之中，和存在于磁盘并能利用cache加速的普通文件相比，socket并不能从缓存机制中收益。2.socket通过文件数据结构的private_data field直接访问，不存在普通文件对应的目录路径的概念。目录项和索引节点对于sockets都是无效的。作者提出利用支持fastsocket的VFS来解决VFS平台的可扩展性问题，同时保持Fast-socket对传统VFS的兼容。 


    为了验证真实应用能否从FAStsocket中收益？相对于基本linux内核中的TCP协议栈，Fastsocket的可扩展性如何？作者在一个拥有24核，工作是提供短期连接的系统上做测试，采用Fastsocket之后，速度提升到原来的20.4倍。当扩展到24核后，与采用基本linux 内核相比，Fastsocket能够增加NGinx的吞吐量到267%，HAProxy吞吐量增加到621%。目前，Fastsocket已经在Sina WeiBo这个商业系统部署，每天服务5000万活跃用户的上亿次请求。
