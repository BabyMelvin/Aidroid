package com.melvin.appbinderdemo;

import android.os.Parcel;
import android.os.Parcelable;

public class MyData implements Parcelable{
    private int data1;
    private int data2;
    public MyData(){
    }
    protected MyData(Parcel in){
        readFromParcel(in);
    }

    public static final Creator<MyData> CREATOR=new Creator<MyData>(){

        @Override
        public MyData createFromParcel(Parcel in){
            return new MyData(in);
        }

        @Override
        public MyData[] newArray(int size){
            return new MyData[size];
        }
    };

    @Override
    public int describeContents(){
        return 0;
    }
    //将数据写入到Parcel
    @Override
    public void writeToParcel(Parcel dest,int flags){
        dest.writeInt(data1);
        dest.writeInt(data2);
    }

    //从Parcel读取数据
    public void readFromParcel(Parcel in){
        data1=in.readInt();
        data2=in.readInt();
    }
    public int getData2(){
        return data2;
    }

    public int getData1(){
        return data1;
    }
    public void setData1(int data){
        data1=data;
    }
    public void setData2(int data){
        data2=data;
    }
    @Override
    public String toString(){
        return "data1="+data1,"data2="+data2;
    }
}
