package com.melvin.server;
import android.util.Config;
import android.util.Log;
import android.content.Context;
import android.os.Binder;
import android.os.RemoteException;
import adnroid.os.IBinder;
import melvin.hardware.ILedService;

public final class LedService extends ILedService.Stub{
    private final String TAG="LedService";
    static {
        System.load("/system/lib/libmelvin_runtime.so");
    }

    public LedService(){
        Log.i("LedService","Go to get LED Stub...");
        _init();
    }
    public boolean setOn(int led){
        Log.i(TAG,"LED ON");
        return _set_on(led,5.1);
    }
    public boolean setOff(int led){
        Log.i(TAG,"LED OFF");
        return _set_off(led);
    }
    public boolean setAllOn(){
        return false;
    }
    private static native boolean _init();
    private static native boolean _set_on(int led,double d);
    private static native boolean _set_off(int led);

}
