import java.util.*;

public class HelloJni {
    static {
        System.loadLibrary("hello");
    }

    public native static void sayHello();
    public native void setInt(int i);
    public native int getInt();
    
    public native void setUser(String userName);
    public native User getUser();
    public static void main (String[] args) {
        System.out.println("Hello world in java");
        HelloJni.sayHello ();

        // 传入int后再返回
        HelloJni hello = new HelloJni();
        hello.setInt(2);
        System.out.println("get int form jni:" + hello.getInt());

        hello.setUser("melvin cao");
        User user = hello.getUser();
        System.out.println("user form c/c++");
        System.out.println("name:"+user.getUserName());
        System.out.println("isMan?"+user.isMan());
        System.out.println("age:"+user.getAge());
    }
}
