# ThreadPool
根据[知乎大佬的code](https://www.zhihu.com/question/27908489)，实现了一个可以携带任意参数的线程池。且规避了答主的提到的不能拷贝构造的对象的bug。

目前发现 Shutdown 函数在 Windows 下不能很好的工作，请谨慎使用。