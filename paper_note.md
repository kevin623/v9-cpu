###mTCP: 针对多核系统的高扩展性用户层TCP协议栈
###来源：发表于NSDI2014
###内容简介：
随着移动网络和物联网的不断发展，网络数据包中short TCP connections的占比越来越大，比如在celular网络中，90%的TCP连接数据量都小于32KB。针对短连接的高效处理对于用户在线交互应用和网络middlebox来说显得更为重要。但传统的内核层协议栈在处理TCP数据时占据了70-80%的CPU时间，应用程序难于针对数据特点进行效率优化。同时，内核层协议栈也存在以下问题：

缺乏connection locality：存在多线程争用套接字的accept queue的问题；同时，协议栈中接收处理TCP数据的CPU core并不一定是实际处理数据的应用程序所运行的CPU core。这些都导致了cache miss和cache-line sharing的问题。

共享的file descriptor空间：根据POSIX标准，file descriptor在进程内是共享资源。套接字以file descriptor的形式存在，需处理多线程争用的问题，同时也引入了file descriptor在VFS中的额外开销。

per-packet处理的低效性：socket API的逐包处理效率低，不能有效的进行batching。

System Call开销：BSD socket API对于短连接存在大量user/kernel的切换开销。

因此，本文作者提出了用户层mTCP协议栈，实现在多核系统上的良好扩展性，同时具备应用兼容性，且无需修改内核以方便部署。

mTCP的主要技术特点有：

user-level Packet I/O library：支持event-driven packet I/O接口，基于RSS实现网卡数据的负载均衡，避免内核/应用层切换开销；同时利用flow实现CPU处理数据的affinity问题。
cache-aware thread placement：优化数据结构，以cache line对齐，并将常访问结构体分量放入相邻区域，减少TLB的不命中性。
bached event handling：利用队列对数据包进行批量处理。
efficent per-core resource management：采用lock-free data structure：减少因lock引入的CPU开销；实现flow-level级的线程处理core局部性；进行高效的TCP计时器管理等。
短连接优化：基于优先级的包队列，轻量级的connection setup。
BSD套接字兼容性：易于mTCP的推广部署。
最后，作者通过对lighttpd、ab、SSLShader、WebReplay等在mTCP上进行性能评估，性能最高提升到320%。
