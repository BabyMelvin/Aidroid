public class User {
    private long id;
    private String userName;
    private boolean isMan;
    private int age;

    public User() {};
    public User(long id, String userName, boolean isMan, int age) {
        super();
        this.id = id;
        this.userName = userName;
        this.isMan = isMan;
        this.age = age;
    }

    public long getId() {
        return id;
    }

    public void setId(long id) {
        this.id = id;
    }

    public String getUserName() {
        return userName;
    }

    public void setUserName(String userName) {
        this.userName = userName;
    }

    public boolean isMan () {
        return isMan;
    }

    public int getAge() {
        return age;
    }

    public void setAge (int age) {
        this.age = age;
    }
}
