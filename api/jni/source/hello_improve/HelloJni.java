import java.util.*;

public class HelloJni {

    static {
        System.loadLibrary("userbean");
    }

    public native static void sayHello();
    public native void setInt(int i);
    public native int getInt();
    
    public native void setUser(String userName);
    public native User getUser();

    //TODO 使用list
    public native void setUserList(ArrayList<User> userList);
    public native ArrayList<User> getUserList();
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

        //TODO 使用list
        ArrayList<User> userList = new ArrayList<User>(); 
        for (int i = 0; i < 10; i++) {
            User u = new User((long)(100 + i), "melvin" + i, true, 21);
            userList.add(u);
        }
        hello.setUserList(userList);

        userList = null;
        userList = hello.getUserList();
        System.out.println("ArrayList<User> construct form c/C++, then Java print it ...");
        for (User u: userList) {
           System.out.println("id:" + u.getId() + "name:" + u.getUserName()); 
        }
    }
}
