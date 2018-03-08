package com.melvin.appbinderdemo;

import android.os.Serice;
import android.content.Intent;
import android.os.IBinder;
import android.os.Parcel;
import android.os.RemoteException;
import android.util.Log;

public class RemoteService extends Service{
    private static final String TAG ="BinderSimple";
    MyData mMydata;

    private final IRemoteService.Stub mBinder=new IRemoteService.Stub(){
        @Override
        public int getPid() throws RemoteException{
            Log.i(TAG,"RemoteService getPid()="+android.os.Process.myPid());
            return android.os.Process.myPid();
        }

        @Override
        public MyData getMyData() throws RemoteException{
            Log.i(TAG,"RemoteService getMyData()="+mMyData.toString());
            return mMyData;
        }
        //可用于权限拦截
        @Override
        public boolean onTransact(int code,Parcel data,Parcel reply,int flags) throws RemoteException{
            return super.onTransact(code,data,reply,flags);
        }
    }
    @Override
    public void onCreate(){
        super.onCreate();
        Log.i(TAG,"RemoteService onCreate");
        initMyData();
    }
    private void initMyData(){
        mMyData=new MyData();
        mMyData.setData1(10);
        mMyData.setData2(20);
    }
    @Override
    public IBinder onBind(){
        Log.i(TAG,"RemoteService onBinder");
        return mBinder;
    }
    @Override
    public boolean onUnbind(Intent intent){
        Log.i(TAG,"RemoteService onUnbind");
        return super.onUnbind(intent);
    }

    @Override
    public void onDestroy(){
        Log.i(TAG,"RemoteService onDestroy");
        super.onDestroy();
    }
}
