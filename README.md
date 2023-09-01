# TinyThreadPool

Hands-on implementation of a small thread pool

本项目是基于Linux系统下是用Clang构造的使用C++11语法的小型线程池项目，其中必须是用linux系统，在win下会报很多死锁的问题。

1. 基于本项目，我们构造Task结构体，并利用回调函数来实现任务。

2. 并且我们构建ThreadPool类，其中有任务队列，一把互斥锁，一个管理者线程，多个工作者线程。在构造函数中构建管理者线程和工作者线程，我们把主要重点放在管理者线程和工作者线程中
3. 在一开始任务队列是空的工作者线程将会发送条件变量阻塞该线程，当有新任务加进来时将会唤醒所有进程，此时即可执行Task.
4. 管理者线程将会当任务个数>正在工作者线程的时候创建线程。这时就可以有足够的线程去工作啦
5. 最后当工作执行完成后，唤醒之前的条件变量将所有线程return，并且在析构函数中释放资源。

## Cloning this Repository

你可以通过一下操作来Clone该项目

1.在你的git终端输入一下命令克隆项目：

```
$ git clone git@github.com:wannan123/TinyThreadPool.git
```

2.进入到你的TinyThreadPool文件，创建build

```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

3.进入你的build文件夹，里面有可执行文件test.exe

```
$ ./test
```

