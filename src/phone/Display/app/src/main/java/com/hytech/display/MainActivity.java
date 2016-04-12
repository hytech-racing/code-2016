package com.hytech.display;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
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
import android.widget.TextView;

import com.github.pavlospt.CircleView;


public class MainActivity extends Activity implements SensorEventListener {

    private CircleView odometer;
    private TextView stateOfCharge;
    private TextView batteryTemps;

    private SensorManager sensorManager;
    private Sensor accelerometer;

    private float previousRotation = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(R.layout.activity_main);

        BluetoothCommService bcs = new BluetoothCommService(handler);
        bcs.start();

        initSensors();
        odometer = (CircleView) findViewById(R.id.odometer);
        stateOfCharge = (TextView) findViewById(R.id.button1);
        batteryTemps = (TextView) findViewById(R.id.button3);
    }

    private final Handler handler = new Handler(new Handler.Callback() {
        @Override
        public boolean handleMessage(Message message) {
            byte[] data = (byte[]) message.obj;
            switch (data[0]) {
                case 1:
                    int soc = data[1] & 0xFF;
                    String sLabel = "Battery: " + soc + "%";
                    stateOfCharge.setText(sLabel);
                    break;
                case 4:
                    int high = data[4] & 0xFF;
                    int avg = data[5] & 0xFF;
                    String bTempLabel = "Battery Temps:\n Avg: " + avg + "° C\n High: " + high
                            + "° C";
                    batteryTemps.setText(bTempLabel);
                    break;
                case 10:
                    int speed = ((data[11] & 0xFF) << 8) | (data[10] & 0xFF);
                    odometer.setTitleText("" + speed);
                    break;
                case 0x7F:
                    btErrorDialog();
            }
            return false;
        }
    });

    public void btErrorDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
        builder.setMessage(R.string.bt_err_msg).setTitle(R.string.bt_error);
        builder.setPositiveButton(R.string.ok, new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.cancel();
            }
        });
        AlertDialog error = builder.create();
        error.show();
    }

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
