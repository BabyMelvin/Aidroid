package melvin.hardware;
import java.util.ArrayList;

import melvin.hardware.ILedService;
public class LedManager{
    public LedManager(){
        mLedService=ILedService.Stub.asInterface(ServiceManager.getService("led"));
        if(mLedService!=null){
            Log.i(TAG,"The LedManager object is ready.");
        }
    }
    private ILedService mLedService;
    static final ArrayList<LedDelegate> sLeds=new ArrayList<LedDelegate>();
    public void registerListener(LedEventListener context){
        LedDelegate l=new LedDelegate(context);
        sLeds.add(l);
    }
    public boolean LedOn(int n){
        mThread.start();
        return true;
    }
    public boolean LedOff(int n){
        boolean result=false;
        try{
            result=mLedService.setOff(n);
        }catch(RemoteException e){
            Log.e(TAG,"");
        }
        return result;
    }
    public boolean AllOn(int n){
        mAllOnThread.start();
        return true;
    }
    private Thread mThread=new MyThread();
    private Thread mAllOnThread=new AllOnThread();
    private Handler mHandler=new MyHandler();

    private class MyThread extends Thread{
        @Override
        public void run(){
            super.run();
            try{
                mLedService.setOn(1);
                Message msg=new Message();
                msg.what=MSG_TYPE_SET_ON;
                mHandler.sendMessage(msg);
            }catch(RemoteException e){
                e.printStackTrace();
            }
        }
    }
    private class AllOnThread extends Thread{
        @Override
        public void run(){
            super.run();
            try{
                mLedService.setAllOn(1);
                Message msg=new Message();
                msg.what=MSG_TYPE_SET_ALL_ON;
                mHandler.sendMessage(msg);
            }catch(RemoteException e){
                e.printStackTrace();
            }
        }
    }
    private void doLedChanged(){
        final int size=sLeds.size();
        for(int i=0;i<size;i++){
            LedDelegate led=sLeds.get(i);
            led.onLedChanged();
        }
    }
    private void doLedReady(){
        final int size=sLeds.size();
        for(int i=0;i<size;i++){
            LedDelegate led=sLeds.get(i);
            led.onLedReady();
        }
    }
    private class MyHandler extends Handler{
        @Override
        public void handleMessage(Message msg){
            switch(msg.what){
                case LedManager.MSG_TYPE_LED_SET_ON:
                    doLedChanged();
                    break;
                case LedManager.MSG_TYPE_SET_ALL_ON:
                    doLedReady();
                    break;
            }
            super.handleMessage(msg);
        }
    }

    private class LedDelegate extends Binder{
        private LedEventListener mContext;
        LedDelegate(LedEventListener ctx){
            mContext=ctx;
        }

        public void onLedReady(){
            mContext.onLedReady();
        }

        public void onLedChanged(){
            mContext.onLedChanged();
        }
    }
}
