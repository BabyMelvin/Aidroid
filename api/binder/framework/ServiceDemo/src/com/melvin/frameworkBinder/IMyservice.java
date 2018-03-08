package com.melvin.frameworkBinder;
import android.os.IInterface;
import android.os.RemoteException;

public interface IMyService extends IInterface{
    static final java.lang.String DESCRIPTOR=""com.melvin.frameworkBinder.MySerivce";
    public void sayHello() throws RemoteException;
    static final int THREANSACTION_say=android.os.IBinder.FIRST_CALL_TRANSCATION;
}
