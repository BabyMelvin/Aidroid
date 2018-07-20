# RefBase
RefBase是Android native底层的对象。结合`sp`和`wp`实现一套引用计数的方法类控制对象声明周期。

* RefBase中有个对象，对象内部进行强弱引用计数
* sp化后，强弱引用计数各增加1，sp析构后，强弱引用各减1.
* wp化后，弱引用计数增加1，wp析构后，弱引用计数减1.

```
class A:public RefBase{
    
}
```
完全消灭ReBase对象，包括实际对象和包含对象。强弱引用计数控制，还要考虑flag取值。
flag为0时，结论：

* 强引用为0，导致实际对象被delete
* 弱引用为0，导致内部对象被delete

## 1 sp赋值过程

StrongPointer.h中，有四种方式初始化sp对象。

```
//括号方式，调用目标对象incStrong方法
sp(T* other); //1
sp(const sp<T>& other);//2
//等号方式，调用目标incStrong方法，再调用旧对象decStrong方法
sp&operator=(T *other);//3
sp&operator=(const sp<T> &other);//4
```

Android大量Binder通信，ProcessStat便是最常见sp对象，初始化sp实例：

```
sp<ProcessState> proc(ProcessState::self());//采用括号方式2
sp<ProcessState> gProcess=new ProcessState;//等号方式4
```

**注意**:首次调用对象incStrong()，则会调用该对象onFirstRef()，调用decStrong()的最后一次，则会调用该对象onLastStrongRef().
弱引用不能直接操作目标对象的根本原因是若指针没有重载`*`和`->`操作符号。

可通过promote()函数,将弱引用提升为强引用对象：

* promote作用试图增加目标对象的强引用计数
* 由于目标对象可能被delete掉了，或者是其它原因到时提升失败。

## 2.生命周期

* flag为OBJECT_LIFETIME_STRONG,强引用计数控制对象生命周期，弱引用计数控制weak_impl对象生命周期
    * 强引用计数为0后，实际对象被delete掉。这种情况应使用wp要由弱升强。
* flag为LIFETIME_WEAK:强引用计数为0，弱引用计数不为0，实际对象不会被delete
    * 当弱引用计数为0时，实际对象和weakref_impl对象会同时被delete
* flag为LIFETIME_FOREVER:对象不受强弱引用计数控制，永不会被回收。
