###Title: The Art of War: Offensive Techniques in Binary Analysis

###:IEEE Symposium on Security and Privacy’15
###主要内容：

随着物联网的普及，智能设备广泛进入大家生活，智能设备系统的安全越来越广泛收到重视。发现固件中的bug和安全问题变得更加受到重视。固件通常都是编译好的二进制程序，并且格式架构等往往不同，难以分析。这篇文章介绍了对于二进制程序的分析方法，以及作者实现了一个对于二进制程序的一个分析框架angr。它主要是一个符号执行引擎，并且提供对不同架构不同系统的支持和优化。

对于只能设备，通常有两种方法，一种是在linux上运行程序，第二种是将所有程序编译到一起成为固件成为一个嵌入式系统，这种固件被称为binaryblob。binaryblob通常难以分析，因为其可能来自多个架构和不同平台，每个嵌入式设备也有很多不同的外设，难以模拟执行。

作者开发了angr框架用于binaryblob的分析。并开发几个相对独立的模块Binary Loading CLE，Intermediate Representation VEX/pyVEX，Program State Representation/Modification simuVEX，Data Model claripy，Full-Program Analysis(Symbolic Execuction Analysis) angr，通过这些实现可以自动化或者辅助人工进行binaryblob的分析。作者将其运用到CGC比赛，并且在github上开源。
