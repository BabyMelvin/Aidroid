## JNI类型和数据结构

解释JNI如果映射JAVA类型到c类型。

### 原始类型

|java type|native type|description|
|--|--|--|
|boolean|jboolean|unsigned 8bits|
|byte|jbyte|signed 8bits|
|char|jchar|unsigned 16bits|
|short|jshort|signed 16bits|
|int|jint|signed 32bits|
|long|jlong|signed 64 bits|
|float|jfloat|32 bits|
|double|jdouble|64 bits|
|void|void|N/A|

下面定义提供方便：

```c
#define JNI_FALSE 0
#define JNI_TRUE  1
```

## 应用数据类型
JNI对应java对象的类型。

<image src="image/02-01.png"/>
在C，所有其他JNI引用类型被定义成同一个对象

```c
typedef jobject jclass
```
在C++中，JNI引用一系列空类，强迫子类关系：

```cpp
class _jobject{};
class _jclass:public _jobject{};

typedef _jobject *jobject;
typedef _jclass  *jclass;
```

## 属性和方法ID
方法和属性ID通常是C指针类型；

```c
struct _jfieldID;
typedef struct _jfieldID *jfieldID;

struct _jmethodID;
typedef strcut _jmethodID *jmethodID;
```

## 值类型
jvalue是一个联合体类型，参数组中，声明如下:

```cpp
typedef union jvalue{
	jboolean z;
	jbyte 	 b;
	jchar    c;
	jshort   s;
	jint     i;
	jfloat   f;
	jdouble  d;
	jobject  l;
}jvalue;
```

## 类型签名

JNI使用JavaVM下面签名类型：

|type signature|java type|
|--|--|
|Z|boolean|
|B|byte|
|C|char|
|S|short|
|I|int|
|J|long|
|F|float|
|D|double|
|L fully-qualified-class;|fully-qualified-class|
|[type|type[]|
|(arg-types)ret-type|method type|

例如，java方法：

```java
long f (int n,String s,int[] arr);
```
使用虾下面签名：

```cpp
(ILjava/lang/String;[I)J
```
