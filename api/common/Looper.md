# Looper，Message和Handler
Java应用依靠消息驱动，大致工作原理：

* 一个消息队列，可以往消息队列中投递消息。
* 一个消息循环，不断从队列中取出消息然后处理。
主要由Looper和Handler来实现：

* Looper类：封装消息循环，并且有一个消息队列
	* 保证一个线程只有一个Looper对象，即一个消息队列。
	* 调用Handler的dispatchMessage处理消息
* Message类
	* Message类中有一个Handler，处理Message。
* Handler类：消息投递，消息处理接口。

## 1.Looper类
分析Looper的例子：

```java
class LooperThread extends Thread{
	public Handler mHandler;
	public void run(){
		//1.调用prepare
		Looper.prepare();
		mHandler=new Handler(){
			public void handleMessage(Message msg){
				//process incoming messages here
			}
		}
		//2.进入消息循环
		Looper.loop();
	}
}
//应用启动使用LooperThread,运行run函数
new LooperThread().start();
```

### 1.1 其中`prepare()`函数保证：

```
public static final void prepare(){
	//一个线程只能一个Looper
	if(sThreadLocal.get()!=null){
		throw new RuntimeException("only one Looper may be create per thread");
	}
	//构建一个Looper对象，设置调用线程局部变量
	sThreadLocal.set(new Looper());
}
//sThread定义
private static final ThreadLocal sThreadLocal=new ThreadLocal();
```
实现一个线程本地存储，也就是一个变量对每个线程都有自己的值。所有线程共享相同`ThreadLocal<T>`对象，但是每个线程访问它将得到不同的值，并且线程之间修改，相互不影响，并且支持`null`值。
一个线程只能有一个Looper.通过`ThreadLocal<T>`进行保证。Looper构造函数

```
private Looper(){
	//构造一个消息队列
	mQueue=new MessageQueue();
	mRun=true();
	//得到当前线程的Thread对象	
	mThread=Thread.currentThread();
}
```

### 1.2Looper循环

```java
public static final void loop(){	
	// return (Looper)sThreadLocal.get()
	Looper me=myLooper();//返回TLV中保存Looper对象
	//取出这个Looper消息队列
	MessageQueue queue=me.mQueue;
	while(true){
		Message msg=queue.next();
		//处理消息，Message对象中有一个target，它是Handler类型
		//Handler为空，退出循环
		if(msg!=null){
			if(msg.target==null){
				return;
			}
			//调用消息的Hanlder，交给它dispatchMessage函数处理
			msg.target.dispatchMessage(msg);
			msg.recycle();
		}
	}
}
```

## 2.Handler分析

```
//成员变量
final MessageQueue mQueue;//handler中也有一个消息队列
final Looper mLooper；//也有一个Looper
final Callback mCallback;//也有一个回调函数
public Handler(){
	//获取当前线程Handler
	mLooper=Looper.myLooper();
	if(mLooper==null){
		throw new RuntimeException();
	}
	//得到Looper的消息队列
	mQueue=mLooper.mQueue;
	//无callback设置
	mCallback=null;
}

public Handler(Callback callback){
	mLooper =Looper.myLooper();
	if(mLooper==null){
		throw new RuntimeException();
	}
	mQueue=mLooper.mQueue;
	mCallback=callback;
}
public Handler(Looper loope){
	mLooper=looper;
	mQueue=looper.mQueue;
	mCallback=null;
}
public Handler(Looper looper,Callback callback){
	mLooper=looper;
	mQueue=looper.mQueue;
	mCallback=callback;
}
```

### 2.1 Handler和Message
Handler提供一些列完成创建和插入消息队列工作：

* 是否有what的消息：`final boolean hasMessage(int what)`
* 创建一个消息码是what消息：`final Message obtainMessage(int what)`
* 移除what消息:`final void removeMessage(int what)`
* 发送一个只填充消息码消息：`final boolean sendEmptyMessage(int what)`
* 发送一个消息到队列头，优先级很高`final boolean sendMessageFrontOfQueue(Message msg)`
* `public final boolean sendMessageDelayed(Message msg,long delayMillis)`

```
public final boolean sendMessage(Message msg){
	return sendMessageDelayed(msg,0);
}
public boolean sendMessageDelayed(Message msg,long uptineMills){
	if(delayMills<0) delayMills=0;
	return sendMessageAtTime(msg,SystemClock.uptimeMills()+delayMillis);
}
public boolean sendMessageAtTime(Message msg,long uptimeMills){
	boolean send =false;
	MessageQueue queue=mQueue;
	if(queue!=null){
		//把Message target设置为自己，然后加入到队列消息中
		msg.target=this;
		sent=queue.enqueueMessage(msg,uptimeMills);
	}
}
```
Handler把Message设置为自己，因为Handler除了封装消息添加功能还封装小消息处理接口

### 2.2 Handler消息处理
Looper消息队列中加入一个消息，Looper消息处理规则获取消息还会调用target的dispatchMessage函数。

```
public void dispatchMessage(Message msg){
	//如果Message本身有callback,则直接交给Messge的callback处理
	if(msg.callback!=null){
		//优先级：1
		handleCallback(msg);
	}else{
		//如果Handler设置mCallback，则交给mCallback处理
		if(mCallback!=null){
			//优先级：2
			if(mCallback.handleMessage(msg){
				return ;
			}
		}
		//优先级：3
		//最后交给子类处理
		handleMessage(msg);
	}
}
```

## Looper和Handler同步关系

```
//线程1
class LooperThread extends Thread{
	public Looper myLooper=null;//定义一个public
	public void run(){
		Looper.prepare();
		//myLooper必须这个线程中赋值
		myLooper=Looper.myLooper();
		Looper.loop();
	}
}
//下面在线程1中执行，并且创建线程2
{
	LooperThread lpThread=new LooperThread;
	lpThread.start();//start后会创建线程2
	Looper looper=lpThread.myLooper;
	//线程1和2挂钩
	Handler thread2Handler=new Handler(looper);
	//sendMessage发送雄安喜
	threadHandler.sendMessage()...;
}
```