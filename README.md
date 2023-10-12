# ThreadPool

[参考文章 : C++线程池实现](https://zhuanlan.zhihu.com/p/367309864)

主要代码在于如何将所有任务：

1. 普通函数
2. 成员函数
3. lambda
4. std::bind

封装成一个通用函数任务提交给任务队列。

在次使用一个``submit`模板函数来实现将用户的任务封装成一个通用函数来提交。

使用`c11`的`可变模板参数特性`，`F`为用户提交的函数指针，`Args`为函数参数。

`future`：提供一个异步操作结果查询途径，`st::future的 wait()`可以阻塞线程，`get()`方法可以查询返回结果.

```c++
template<typename F, typename... Args>
auto submit(F &&f, Args &&...args) -> std::future<decltype(f(args...))>
```

结合`auto`，`decltype`与`尾返回类型`技巧，自动推断该函数返回类型。

```c++
std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
```

`std::bind`：可以将调用函数时的部分参数事先指定好，留下一部分真正调用时确定，即将`args`绑定到`f`中，调用时直接`f()`就可以执行任务。

`std::forward`：对参数进行完美转发，以保证对参数的正确引用。

```c++
auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
```

这里我们使用`std::make_shared<>()`方法，声明了一个`std::packaged_task<decltype(f(args...))()>`类型的智能指针，并将前面`std::function`方法声明的特殊函数包装`func`传入作为`std::packaged_task`的实例化参数。智能指针将更方便我们对该`std::packaged_task`对象进行管理。
`std::packaged_task`可以用来封装任何可以调用的目标，从而用于实现异步的调用。

```c++
std::function<void()> warpper_func = [task_ptr](){ (*task_ptr)(); };
```

`function`：再次调用，封装成通过函数`void f()`加入队列。



## CAS

对于工作队列，使用CAS算法实现多个线程之间对于任务队列的访问，以提高并发量。
