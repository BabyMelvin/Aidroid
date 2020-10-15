# JNI调用

* C ：`(*env)->NewStringUTF(env, "Hello from JNI")`;
* C++: `env->NewStringUTF("Hello from JNI")`;

# 1. 数据类型

## 1.1 基本数据类型

透明的无需转换

| java type | native type | description     |
| --------- | ----------- | --------------- |
| boolean   | jboolean    | unsigned 8bits  |
| byte      | jbyte       | signed 8bits    |
| char      | jchar       | unsigned 16bits |
| short     | jshort      | signed 16bits   |
| int       | jint        | signed 32bits   |
| long      | jlong       | signed 64 bits  |
| float     | jfloat      | 32 bits         |
| double    | jdouble     | 64 bits         |
| void      | void        | N/A             |

## 1.2 引用数据类型


| java type           | native type   |
| ------------------- | ------------- |
| java.lang.Class     | jclass        |
| java.lang.Throwable | jthrowable    |
| java.lang.String    | jstring       |
| Other objects       | jobject       |
| java.lang.Object[]  | jobjecArray   |
| boolean[]           | jbooleanArray |
| byte[]              | jbyteArray    |
| char[]              | jcharArray    |
| short[]             | jshortArray   |
| int[]               | jintArray     |
| long[]              | jlongArray    |
| float[]             | jfloatArray   |
| double[]            | jdoubleArray  |
| Other arrays        | jarray        |


签名:

| java类型            | 签名                  |
| ------------------- | --------------------- |
| Boolean             | Z                     |
| Byte                | B                     |
| Char                | C                     |
| Short               | S                     |
| Int                 | I                     |
| Long                | J                     |
| Float               | F                     |
| Double              | D                     |
| fully-quality-class | Lfully-quality-class; |
| type[]              | [type                 |
| methodtype          | (arg-type)ret-type    |

# 2 对引用数据类型的操作

引用数据类型，不透明不能直接操作。JNIEnv提供了一套原声接口函数。

简单介绍API：

* 字符串
* 数组
* NIO缓冲区
* 字段
* 方法

## 2.1 字符串操作
JNI支持Unicode编码格式和UTF-8编码格式的字符串，还提供两组函数通过JNIEnv接口指针处理这些字符串编码。
### 2.1.1 创建字符串

```java
jstring javaString = env->NewStringUTF("Hello world!");
```

内存溢出，将函数返回NULL，通知原声代码虚拟机抛出异常。

### 2.1.2 把java字符串转换为C字符串

java字符串转换为C字符串，有两种：

* Unicode:`GetStringChars`
* UTF-8:`GetStringUTFChars`

```java
const jbyte *str;
jboolean isCopy;

str = env->GetStringUTFChars(javaString, &isCopy);
if (0 != str) {
	printf ("Java string: %s", str);
	if (JNI_TRUE == isCopy) {
		printf("C string is a copy of the Java string.");
	} else {
		printf("C string points to actual string.");
	}
}
```
### 2.1.3 释放字符串
原声代码用完需要释放，否则会内存泄露

`env->ReleaseStringUTFChars(javaString, str);`

## 2.2 数组操作

JNI 把java数组当成引用类型处理，提供函数接口处理java数组。

### 2.2.1 创建数组

`New<Type>Array`原生代码中创建数组实例。

```java
jintArray javaArray;
javaArray = env->NewIntArray(10);
if (0 != javaArray) {
	// 可以使用数组
}
```
### 2.2.2 访问数组元素
两种方法

* 将数组代码复制成C数组
* JNI提供直接指向数组的指针

#### 2.2.2.1 对副本的操作

* 全部复制给C数组，`Get<Type>ArrayRegion`

```java
jint nativeArray[10];
env->GetIntArrayRegion(javaArray, 0, 10, nativeArray);
```
原声代码可以像使用普通C数组一样使用和修改数组元素。

当原生代码所做修改提价给Java数组时，使用Set<Type>ArrayRegion将C数组复制给Java数组中

```java
env->SetIntArrayRegion(javaArray, 0, 10, nativeArray);
```

数组很大，不适合。应该只获取某个元素

#### 2.2.2.2 对直接指针的操作
Get<Type>ArrayElements

* isCopy返回C字符串地址指向副本还是指向堆中的固定对象

```java
jint *nativeDirectArray;
nativeDirectArray = env->GetIntArrayElements(javaArray, &isCopy);
```

需要释放获取的C指针

```java
env->ReleaseIntArrayElements(javaArray, nativeDirectArray, 0);
```

第4个是释放模式：

* 0： 将内容复制回来并释放原生数组
* JNI_COMMIT： 将内容复制回来，但不释放原生数组，一般用于周期性更新java数组
* JNI_ABORT：释放原生数组但不用将内容复制回来


## 2.3 NIO 操作
NIO 缓冲区管理。比数组性能好，适合java和native转递大量数据

### 2.3.1 创建直接字节缓冲区
原声代码可以创建Java应用程序直接使用字节缓冲区，该过程提供一个原生C字节数组为基础。 NewDirectByteBuffer

```java
unsigned char *buffer = (unsigned char *)malloc(1024);
jobject directBuffer;
directBuffer = env->NewDirectByteBuffer(buffer, 1024);
```
### 2.3.2 直接字节缓冲区获取

Java 应用程序也可以创建直接缓冲区，原生代码中调用GetDirectBufferAddress函数,获取原声字节数组的内存地址

```c
unsigned char *buffer;
buffer = (unsigned char *)env->GetDirectBufferAddress(directBuffer);
```
## 2.4 访问域

Java域：

* 实例域
* 静态域

```java
public class JavaClass {
	// 实例域
	private String instanceField = "Instance Field";
	
	// 静态域
	private static String staticField = "Static Field";
	...
}
```
JNI 提供了访问**两类域**的函数

### 2.4.1 获取域ID

* 1.通过给定的class对象获取域ID，**GetObjectClass**函数获取class对象

* 用对象引用获得类

```cpp
jclass clazz;
clazz = env->GetObjectClass(instance);
```

* 获取实例域的域ID

```java
jfieldID instanceFieldId;
instanceFiledId = env->GetFieldID(clazz, "instanceField","Ljava/lang/String;");
```

* 获取静态域的域ID

```java
jfiledID staticFieldId;
staticFieldId = env->GetStaticFiledID(clazz, "staticFiledId", "Ljava/lang/String;");
```

注意：对访问频繁的ID，可以缓存域ID

### 2.4.2 获取域
获得域ID后，可以通过`Get<Type>Field`函数获得实际域。

* 获得实例域

```java
jstring instanceField;
instanceFiled = env->GetObjectField(instance, instanceFieldId);
```

* 获得静态域

```java
jstring staticFiled;
staticField = env->GetStaticObjectField(clazz, staticFieldId);
```

注意：建议将原生方法调用传递，不是将原生代码回到Java中

## 2.5 调用方法

java中两种方法：实例方法和静态方法。

```java
public class JavaClass {
	//实例方法
	private String instanceMethod() {
		return "instance Method";
	}

	private static String staticMethod() {
		return "Static Method";
	}
	...
}
```
### 2.5.1 获取方法ID

* 获取实例的方法

```java
jmethodID instanceMethodId;
instanceMethodId = env->GetMethodID(clazz, "instanceMethod", "()L/java/lang/String;");
```

* 获取静态方法ID

```java
jmethodID staticMethodId;
staticMethodId = env->GetStaticMethodID(clazz, "staticMethod", "()Ljava/lang/String;");
```

获得良好的性能，可以将MethodId进行缓存
### 2.5.2 调用方法

* 调用实例方法

`Call<Type>Method`

```java
jstring instanceMethodResult;
instanceMethodResutl = env->CallStringMethod(instance, instanceMethodId);
```

* 调用静态类方法

`CallStatic<Type>Field`

```java
jstring staticMethodResult;
staticMethodResult = env->CallStaticStringMethod(clazz, staticMethod);
```

# 3.异常处理
JNI需要开发人员在异常发生后显示地实现异常处理

```java
public class JavaClass {
	// 抛出方法
	private void throwingMethod() throws NullPointerException {
		throw new NullPointerException("Null pointer");
	}

	// 访问方法
	private native void accessMethods();
}
```
## 3.1 捕获异常

调用throwingMethod方法时，accessMethod需要显示的处理异常。JNI提供ExceptionOccurred函数查询虚拟机中是否有挂起的异常。

处理完异常处理程序ExceptionClear函数显示清楚异常

```java
jthrowable ex;

env->CallVoidMethod(instance, throwingMethodId);
if (0 != ex) {
	env->ExceptionClear(env);

	// Exception handler
	...
}
```
## 3.2 抛出异常
原生代码抛出异常。

```java
jclass clazz;

clazz = env->FindClass("java/lang/NullPointerException");
if (0 != clazz) {
	env->ThrowNew(clazz, "Exception message");
}
```

原生代码不受虚拟机控制，抛出异常并不会停止原声函数的执行并把控制权转交给异常处理程序。

抛出异常，应该自己释放资源。 

通过JNIEnv接口获得的引用是局部引用且一旦返回原生函数，它们就自动被虚拟机释放

# 4.局部和全局引用

## 4.1 局部引用
显示删除局部引用

```java
jclass clazz;

clazz = env->FindClass("java/lang/String");
env->DeleteLocalRef(clazz);
```

## 4.2 全局引用

### 4.2.1 创建全局引用
NewGlobalRef将局部引用初始化为全局引用

```java
jclass localClazz;
jclass globalClazz;

localClazz = env->FindClass("java/lang/String");
globalClazz = env->NewGlobalRef(localClazz);

env->DeleteLocalRef(localClazz);
```
### 4.2.2 删除全局引用

`env->DeleteGlobalRef(globalClazz);`
## 4.3 弱全局引用

弱全局引用在返回仍有效，并不阻止潜在的对象垃圾收回

### 4.3.1 创建弱全局引用

NewWeakGlobalRef 函数对象弱全局引用初始化
jclass weakGloablClazz = env->NewWeakGlobalRef(localClazz);
### 4.3.2 弱全局引用的有效性检验
IsSameObject 检查弱全局引用是否仍然指向活动的实例。

```java
if (JNI_FALSE == env->IsSameObject(weakGlobalClazz, NULL)) {
	// 对象处理活动状态
} else {
	//对象被回收，不可使用
}
```
### 4.3.3 删除弱全局引用

* 只在原生方法执行期间及正在执行原生方法线程下局部引用是有效的。只有全局变量可以被多个线程共享
```java
env->DeleteWeakGlobalRef(weakGlobalRef);
```
# 5.线程

```java

```
## 5.1 同步

java同步

```java
sychronized (obj) {
}
```

等价的原生

```c
if (JNI_OK == env->MoniterEnter(obj)) {
}

// 同步线程安全代码块

if (JNI_OK == env->MoniterExit(obj)) {
}
```
## 5.2 原生线程

为了与应用交互，原生线程应该先附着在虚拟机上。

```c
JavaVM *cachedJvm;

JNIEnv *env;

cachedJvm->AttachCurrentThread(cachedJvm, &env, NULL);

// 可以用JNIEnv 接口实现线程与Java应用程序的通信
cachedJvm->DettachCurrentThread(cachedJvm, &env, NULL);
```