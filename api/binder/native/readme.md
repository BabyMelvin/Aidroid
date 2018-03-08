## Native

相关类主要在`IInterface.h`中实现,其中IInterface类声明如下:
```c++
class IInterface:public virtual ReBase{
    public:
        IInterface();
        sp<IBinder>         asBinder();
        sp<const IBinder>   asBinder() const;
    protected:
        virtual             ~IInterface();
        virtual  IBinder*   onAsBinder()=0;
};
```
`DECLARE_META_INTERFACE`宏在`IInterface.h` 中声明
```c++
#define DECLARE_META_INTERFACE(INTERFACE)   \
    static const android::String16 descriptor; \
    static android::sp<I##INTERFACE> asInterface(const android::sp<android::IBinder> &obj); \
    virtual const android::String16& getInterfaceDescriptor() const; \
    I##INTERFACE(); \
    virtual ~I##INTERFACE(); 
```
`IMPLEMENT_META_INTERFACE` 宏的实现
```c++
#define IMPLEMETN_META_INTERFACE(INTERFACE,NAME) \
    const android::String16 I##INTERFACE::descriptor(NAME); \
    const android::String16&  I##INTERFACE::getInterfaceDescriptor() const{ \
        return I##INTERFACE::descriptor; \
    }\
    android::sp<I##INTERFACE> I##INTERFACE::asInterface(const android::sp<android::IBinder> &obj){\
        android::sp<I##INTERFACE> intr; \
        if(obj!=null){ \
            intr=static_cast<I##INTERFACE*>(obj->queryLocalInterface(I##INTEFACE::descriptor).get()); \ if(intr==NULL){\ intr=new Bp##INTERFACE(obj); \
            }\
        } \
        return intr;\
    }\
    I##INTERFACE::I##INTERFACE(){}\
    I##INTERFACE::~I##INTERFACE(){}
```
`BnInterface` 声明
```c++
template<typename INTERFACE>
class BnInterface:public INTERFACE,public BBinder{
    public:
        virtual sp<IInterface>  queryLocalInterface(const String16& descriptor);
        virtual const String16& getInterfaceDescriptor() const;
    protected:
        virtual IBinder*        onAsBinder();
};
```
`BpInterface`声明
``` c++
template<typename INTERFACE>
class BpInterface:public INTERFACE,public BpRefBase{
    public:
        BpInterface(const sp<IBinder>& IBinder);
    protected:
        virtual IBinder*  onAsBinder();
}
```
