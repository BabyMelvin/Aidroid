# JNI function

注意：使用"must"术语描述在JNI编程中的限制。例如当看到某JNI函数必须接收一个非空对象，让你来保证不要让NULL 传递到JNI函数中。因此JNI本身不会对空进行检查。

## 接口函数表
每个函数通过JNIEnv参数一个固定偏移获得。JNIEnv类型是一个指针，指向所有JNI函数指针存储位置。定义:

```cpp
typedef const struct JNINativeInterface *JNIEnv;
```
VM初始化函数表如下，注意前三个入口保留给未来兼容COM。

```cpp
const struct JNINativeInterface ...={
	NULL,
    NULL,
    NULL,
    NULL,
    GetVersion,

    DefineClass,
    FindClass,

    FromReflectedMethod,
    FromReflectedField,
    ToReflectedMethod,

    GetSuperclass,
    IsAssignableFrom,

    ToReflectedField,

    Throw,
    ThrowNew,
    ExceptionOccurred,
    ExceptionDescribe,
    ExceptionClear,
    FatalError,

    PushLocalFrame,
    PopLocalFrame,

    NewGlobalRef,
    DeleteGlobalRef,
    DeleteLocalRef,
    IsSameObject,
    NewLocalRef,
    EnsureLocalCapacity,

    AllocObject,
    NewObject,
    NewObjectV,
    NewObjectA,

    GetObjectClass,
    IsInstanceOf,

    GetMethodID,

    CallObjectMethod,
    CallObjectMethodV,
    CallObjectMethodA,
    CallBooleanMethod,
    CallBooleanMethodV,
    CallBooleanMethodA,
    CallByteMethod,
    CallByteMethodV,
    CallByteMethodA,
    CallCharMethod,
    CallCharMethodV,
    CallCharMethodA,
    CallShortMethod,
    CallShortMethodV,
    CallShortMethodA,
    CallIntMethod,
    CallIntMethodV,
    CallIntMethodA,
    CallLongMethod,
    CallLongMethodV,
    CallLongMethodA,
    CallFloatMethod,
    CallFloatMethodV,
    CallFloatMethodA,
    CallDoubleMethod,
    CallDoubleMethodV,
    CallDoubleMethodA,
    CallVoidMethod,
    CallVoidMethodV,
    CallVoidMethodA,

    CallNonvirtualObjectMethod,
    CallNonvirtualObjectMethodV,
    CallNonvirtualObjectMethodA,
    CallNonvirtualBooleanMethod,
    CallNonvirtualBooleanMethodV,
    CallNonvirtualBooleanMethodA,
    CallNonvirtualByteMethod,
    CallNonvirtualByteMethodV,
    CallNonvirtualByteMethodA,
    CallNonvirtualCharMethod,
    CallNonvirtualCharMethodV,
    CallNonvirtualCharMethodA,
    CallNonvirtualShortMethod,
    CallNonvirtualShortMethodV,
    CallNonvirtualShortMethodA,
    CallNonvirtualIntMethod,
    CallNonvirtualIntMethodV,
    CallNonvirtualIntMethodA,
    CallNonvirtualLongMethod,
    CallNonvirtualLongMethodV,
    CallNonvirtualLongMethodA,
    CallNonvirtualFloatMethod,
    CallNonvirtualFloatMethodV,
    CallNonvirtualFloatMethodA,
    CallNonvirtualDoubleMethod,
    CallNonvirtualDoubleMethodV,
    CallNonvirtualDoubleMethodA,
    CallNonvirtualVoidMethod,
    CallNonvirtualVoidMethodV,
    CallNonvirtualVoidMethodA,

    GetFieldID,

    GetObjectField,
    GetBooleanField,
    GetByteField,
    GetCharField,
    GetShortField,
    GetIntField,
    GetLongField,
    GetFloatField,
    GetDoubleField,
    SetObjectField,
    SetBooleanField,
    SetByteField,
    SetCharField,
    SetShortField,
    SetIntField,
    SetLongField,
    SetFloatField,
    SetDoubleField,

    GetStaticMethodID,

    CallStaticObjectMethod,
    CallStaticObjectMethodV,
    CallStaticObjectMethodA,
    CallStaticBooleanMethod,
    CallStaticBooleanMethodV,
    CallStaticBooleanMethodA,
    CallStaticByteMethod,
    CallStaticByteMethodV,
    CallStaticByteMethodA,
    CallStaticCharMethod,
    CallStaticCharMethodV,
    CallStaticCharMethodA,
    CallStaticShortMethod,
    CallStaticShortMethodV,
    CallStaticShortMethodA,
    CallStaticIntMethod,
    CallStaticIntMethodV,
    CallStaticIntMethodA,
    CallStaticLongMethod,
    CallStaticLongMethodV,
    CallStaticLongMethodA,
    CallStaticFloatMethod,
    CallStaticFloatMethodV,
    CallStaticFloatMethodA,
    CallStaticDoubleMethod,
    CallStaticDoubleMethodV,
    CallStaticDoubleMethodA,
    CallStaticVoidMethod,
    CallStaticVoidMethodV,
    CallStaticVoidMethodA,

    GetStaticFieldID,

    GetStaticObjectField,
    GetStaticBooleanField,
    GetStaticByteField,
    GetStaticCharField,
    GetStaticShortField,
    GetStaticIntField,
    GetStaticLongField,
    GetStaticFloatField,
    GetStaticDoubleField,

    SetStaticObjectField,
    SetStaticBooleanField,
    SetStaticByteField,
    SetStaticCharField,
    SetStaticShortField,
    SetStaticIntField,
    SetStaticLongField,
    SetStaticFloatField,
    SetStaticDoubleField,

    NewString,

    GetStringLength,
    GetStringChars,
    ReleaseStringChars,

    NewStringUTF,
    GetStringUTFLength,
    GetStringUTFChars,
    ReleaseStringUTFChars,

    GetArrayLength,

    NewObjectArray,
    GetObjectArrayElement,
    SetObjectArrayElement,

    NewBooleanArray,
    NewByteArray,
    NewCharArray,
    NewShortArray,
    NewIntArray,
    NewLongArray,
    NewFloatArray,
    NewDoubleArray,

    GetBooleanArrayElements,
    GetByteArrayElements,
    GetCharArrayElements,
    GetShortArrayElements,
    GetIntArrayElements,
    GetLongArrayElements,
    GetFloatArrayElements,
    GetDoubleArrayElements,

    ReleaseBooleanArrayElements,
    ReleaseByteArrayElements,
    ReleaseCharArrayElements,
    ReleaseShortArrayElements,
    ReleaseIntArrayElements,
    ReleaseLongArrayElements,
    ReleaseFloatArrayElements,
    ReleaseDoubleArrayElements,

    GetBooleanArrayRegion,
    GetByteArrayRegion,
    GetCharArrayRegion,
    GetShortArrayRegion,
    GetIntArrayRegion,
    GetLongArrayRegion,
    GetFloatArrayRegion,
    GetDoubleArrayRegion,
    SetBooleanArrayRegion,
    SetByteArrayRegion,
    SetCharArrayRegion,
    SetShortArrayRegion,
    SetIntArrayRegion,
    SetLongArrayRegion,
    SetFloatArrayRegion,
    SetDoubleArrayRegion,

    RegisterNatives,
    UnregisterNatives,

    MonitorEnter,
    MonitorExit,

    GetJavaVM,

    GetStringRegion,
    GetStringUTFRegion,

    GetPrimitiveArrayCritical,
    ReleasePrimitiveArrayCritical,

    GetStringCritical,
    ReleaseStringCritical,

    NewWeakGlobalRef,
    DeleteWeakGlobalRef,

    ExceptionCheck,

    NewDirectByteBuffer,
    GetDirectBufferAddress,
    GetDirectBufferCapacity,

    GetObjectRefType
};
```

## 版本信息
### GetVersion

```cpp
jint GetVersion(JEIEnv* env);
```
获得本地接口的版本信息。

* JNIEnv接口表中索引为4.
* JNI接口指针为参数
* 返回值
	* jdk/jre1.1-》0x00010001
	* jdk/jre1.2->0x00010002
	* jdk/jre1.4->0x00100004
	* jdk/jre1.6->0x00100006
	
### 常量
#### 从jdk/jre1.2

```cpp
#define JNI_VERSION_1_1 0X00010001
#define JNI_VERSION_1_2 0X00010002
#define JNI_VERSION_1_4 0X00010004
#define JNI_VERSION_1_6 0x00010006
//error codes
#define JNI_EDETACHED (-2)
#define JNI_EVERSION  (-3)
```

## 类操作
### DefineClass
从原生类数据缓冲加载一个类。DefineClass调用返回后，原生类数据缓冲不是VM引用，有可能被丢弃。

```cpp
jclass DefineClass(JNIEnv *env,const char*name,jobject loader,const jbyte *buf,jsize buflen);
```

### FindClass
加载本地已定义的方法。搜索CLASSPATH环境变量，对特定名称的类。

```cpp
jclass FindClass(JNIEnv* env,const char*name);
```

## 全局和局部引用

### 全局引用
创建一个全局应用，`obj`可能是全局或者是本地。

```cpp
jobject NewGlobalRef(JNIEnv* env,jobject obj);
```
删除全局引用

```cpp
void DeleteGlobalRef(JNIEnv* env,jobject globalRef)
```
### 本地引用
在本地方法中调用本地引用有效。删除本地引用方法：

```cpp
void DeleteLocalRef(JNIEnv*env,jobject localRef);
```
新建本地引用 

```cpp
jobject NewLocalRef(JNIEnv* env,jobject ref);
```

## 对象操作
### 多有对象

分配一个java对象没有任何构造，返回一个对象引用。

```cpp
jobject AllocObject(JNIEnv*env,jclass clazz);
```

### NewObject、NewObjectA和NewObjectV
有构造函数创建一个java类。methID是构造函数被调用的。这个ID必须通过`GetMethdID()`通过`<init>`函数名`void(V)`作为函数类型。

```cpp
jobject NewObejct(JNIEnv*env,jclass clazz,jmethodID methdID,...);
jobject NewObejctA(JNIEnv*env,jclass clazz,jmethodID methdoID,const jvalue,*args);
jobject NewObjectV(JNIEnv*env,jclass clazz,jmethodID methodID,va_list args);
```

### GetObjectClass
获得一个java类。

```cpp
jclass GetObejctClass(JNIEnv*env,jobject obj);
```

### GetObjectRefType

获得obj参数对象的类型。

```cpp
jobjectRefType GetObjectRefType(JNIEnv*env,jobject obj);
```

* 返回：jobjectRefType

```cpp
JNIInvalidRefType=0,
JNILocalRefType=1,
JNIGlobalRefType=2,
JNIWeakGlobalRefType=3
```

### IsInstanceOf

如果能够转换成clazz，则返回JNI_TRUE。否则，返回未JNI_FALSE。空指针NULL，能够转换成任何类型。

### IsSameObject
检测两个引用是否指向同一个对象。

```cpp
jboolean IsSameObject(JNIEnv*env,jobject ref1,jobject ref2);
```

## 获得对象属性

### 获得属性ID

```cpp
jfieldID GetFieldID(JNIEnv* env,jclass clazz,const char*name,const char*sig);
```

### Get<type>Field Routines

```cpp
NativeType Get<type>Field(JNIEnv*env,jobject obj,jfieldID fieldID);

//GetBooleanField() jboolean
```

### Set<type>Field Routines

```cpp
void Set<type>Filed(JNIEnv*env,jobject obj,jfieldID fieldID,NativeType value);
```

## 调用实例的方法
### GetMethodID

```cpp
jmethodID GetMethodID(JNIEnv *env,jclass clazz,const char*name,const char*sig);
```

### Call<type>Method,Call<type>MethodA和Call<type>MethodV

```cpp
NativeType Call<type>Method(JNIEnv*env,jobject obj,jmethodID methodID,...);
NativeType Call<type>MethodA(JNIEnv*env,jobject obj,jmethodID methodID,const jvalue*args);
NativeType Call<type>MethodV(JNIEnv*env,jobject obj,jmethodID methodID,va_list args);
```

## 获得静态属性

```cpp
jfieldID GetStaticFieldID(JNIEnv*env,jclass clazz,const char*name,const char*sig);
```

### GetStatic<type>fieldID

```cpp
jboolean GetStaticBooleanField();
```
## 获得静态方法

```cpp
jmethodID GetStaticMethodID(JNIEnv*env,jclass clazz,const char*name,const char*sig);
```

### CallStatic<type>Method、CallStatic<type>MethodA和CallStatic<type>MethodV

# 字符串操作
### NewString
构造一个新的`java.lang.String`对象从编码字符串。

```cpp
jstring NewString(JNIEnv *env,const jchar*unicodeChars);
```
### GetStringLength

返回一个javaString的长度。

```cpp
jsize GetStringLength(JNIEnv*env,jstring string):
```

### GetStringChars

获得一个字符集编码字符串。这个指针直到`ReleaseStringchars()`有效。

```cpp
const jchar* GetStringChars(JNIEnv*env,jstring string,jboolean*isCopy);
```

### NewStringUTF

构建一个以UTF8编码的`java.lang.String`对象。

```cpp
jstring NewStringUTF(JNIEnv*env,const char*bytes);
```

### GetStringUTFLength

```cpp
jsize GetStringUTFLength(JNIEnv*env,jstring string);
```

### GetStringUTFChars
返回一个指针，指向一个字符数组UTF8格式的。直到`ReleaseStringUTFChars()`有效。

## java VM接口

### GetJavaVM
返回javaVM和当前线程相关的接口。

```cpp
jint GetJavaVM(JNIEnv*env,JavaVM**vm);
```