package com.melvin.frameworkBinder;

import android.os.IBinder;
import android.os.RemoteException;

public class MyServiceProxy implements IMyService{
    private android.os.IBinder mRemote;

    public MyServiceProxy(android.os.IBinder remote){
        mRemote=remote;
    }
    
    @Override
    public IBinder asBinder(){
        return mRemote;
    }
    public java.lang.String getInterfaceDescriptor(){
        return DESCRIPTOR;
    }
    @Override
    public void sayHello(String str) throws RemoteExcepions{
        android.os.Parcel _data=android.os.Parcel.obtain();
        android.os.Parcel _reply=android.os.Parcel.obtain();
        try {
            _data.writeInterfaceToken(DESCRIPTON);
            _data.writeString(str);
            mRemote.transact(TRANSACTION_say, _data, _reply, 0);
            _reply.readException();
        } finally {
            _reply.recycle();
            _data.recycle();
        }
    }
}
