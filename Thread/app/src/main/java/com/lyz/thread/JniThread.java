package com.lyz.thread;

/**
 * Created by admin on 2018/9/11.
 */

public class JniThread {
    static {
        System.loadLibrary("native-lib");
    }

    public interface OnErrorListener
    {
        void onError(int code, String msg);
    }

    private OnErrorListener onErrorListener;

    //1.创建线程
    public native void normalThread();
    //2.生产者消费者问题
    public native void mutexThread();
    public native void threadPool();

    public void setOnErrorListener(OnErrorListener onErrorListener){
        this.onErrorListener=onErrorListener;
    }
    //jni调用此方法，把结果返回到java层
    public void onError(int code, String msg)
    {
        if(onErrorListener != null)
        {
            onErrorListener.onError(code, msg);
        }
    }

}
