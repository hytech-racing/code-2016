package com.hytech.phone.blt_phone_pi;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.Window;
import android.view.WindowManager;
import android.widget.TextView;

public class MainActivity extends Activity {

    private BluetoothCommService btCommService = null;

    private TextView tempTextView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_main);

        btCommService = new BluetoothCommService(btHandler);
        btCommService.start();

        tempTextView = (TextView) findViewById(R.id.tempTextView);
    }

    private final Handler btHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            String text = new String((byte[]) msg.obj);
            tempTextView.setText(text);
        }
    };

    @Override
    public void onDestroy() {
        super.onDestroy();
        btCommService.stop();
    }
}