package com.lyz.thread;

import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    JniThread jniThread;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        jniThread=new JniThread();
        jniThread.setOnErrorListener(new JniThread.OnErrorListener(){
            @Override
            public void onError(int code,String msg){
                Log.d("lyz", "code: " + code + ", msg: " + msg);
                Message message = Message.obtain();
                message.what = code;
                message.obj = msg;
                handler.sendMessage(message);
            }
        });

    }

    public void normalThread(View view) {
        jniThread.normalThread();
    }

    public void mutexThread(View view) {
        jniThread.mutexThread();
    }

    public void threadPool(View view) {
        jniThread.threadPool();
    }

    Handler handler = new Handler()
    {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            Toast.makeText(MainActivity.this, (String)msg.obj, Toast.LENGTH_SHORT).show();
        }
    };

}
