package com.hytech.phone.blt_phone_pi;

import android.app.Activity;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.Window;
import android.view.WindowManager;

import com.github.pavlospt.CircleView;

public class MainActivity extends Activity implements SensorEventListener {

    private BluetoothCommService btCommService = null;

    private CircleView odometer;

    private SensorManager sensorManager;
    private Sensor accelerometer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_main);

        btCommService = new BluetoothCommService(btHandler);
        btCommService.start();

        initSensors();

        odometer = (CircleView) findViewById(R.id.odometer);
    }

    private final Handler btHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            String text = new String((byte[]) msg.obj, 0, msg.arg1);
            BluetoothCommService.log("Message received");
            odometer.setTitleText(text);
        }
    };

    @Override
    public void onSensorChanged(SensorEvent event) {
        if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
            float x = event.values[0];
            float y = event.values[1];
            float angle = (float) Math.toDegrees(Math.atan2(-y, x));
            float viewRotation = odometer.getRotation();
            viewRotation += 0.5 * (angle - viewRotation);
            odometer.setRotation(viewRotation);
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
        btCommService.stop();
        sensorManager.unregisterListener(this, accelerometer);
    }
}