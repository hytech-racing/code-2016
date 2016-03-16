package com.hytech.display;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Window;
import android.view.WindowManager;

import com.github.pavlospt.CircleView;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

public class MainActivity extends Activity implements SensorEventListener {

    private CircleView odometer;

    private SensorManager sensorManager;
    private Sensor accelerometer;

    private float previousRotation = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_main);

        BluetoothCommService bcs = new BluetoothCommService(handler);
        bcs.start();

        initSensors();
        odometer = (CircleView) findViewById(R.id.odometer);
    }

    private final Handler handler = new Handler(new Handler.Callback() {
        @Override
        public boolean handleMessage(Message message) {
            Log.d("RETRYING", "Got callbacked");
            String str = new String((byte[]) message.obj, 0, message.arg1);
            odometer.setTitleText(str);
            return false;
        }
    });

    @Override
    public void onSensorChanged(SensorEvent event) {
        if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
            float x = event.values[0];
            float y = event.values[1];
            float angle = (float) Math.toDegrees(Math.atan2(-y, x));
            float viewRotation = odometer.getRotation();
            viewRotation += 0.5 * (angle - viewRotation);
            if (Math.abs(viewRotation - previousRotation) > 0.2) {
                odometer.setRotation(viewRotation);
            }
            previousRotation = viewRotation;
        }
    }

    private void initSensors() {
        sensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        accelerometer = sensorManager.getDefaultSensor(
                Sensor.TYPE_ACCELEROMETER);
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {}

    @Override
    public void onResume() {
        super.onResume();
        sensorManager.registerListener(this, accelerometer,
                SensorManager.SENSOR_DELAY_FASTEST);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        sensorManager.unregisterListener(this, accelerometer);
    }
}