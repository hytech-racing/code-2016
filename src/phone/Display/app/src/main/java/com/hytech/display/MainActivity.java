package com.hytech.display;

import android.app.Activity;
import android.app.AlertDialog;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.Window;
import android.view.WindowManager;
import android.widget.TextView;

import com.github.lzyzsd.circleprogress.ArcProgress;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;


public class MainActivity extends Activity implements SensorEventListener {

    // region #CONSTANTS
    public int GREEN;
    public int YELLOW;
    public int ORANGE;
    public int RED;
    public int[] PROGRESS_COLORS;

    public static final byte BT_BATT_SOC = 0;
    public static final byte BT_BATT_HI_AVG_TEMP = 1;
    public static final byte BT_CAR_STARTUP_STATE = 2;
    public static final byte BT_MOTOR_TEMP = 3;
    public static final byte BT_CAR_SPEED = 4;
    public static final byte BT_CAR_TORQUE = 5;
    public static final byte BT_CAR_CURRENT_DRAW = 6;
    public static final byte BT_DISCONNECT = 0x7F;
    // endregion

    private int currentProgressColor = RED;
    private ArcProgress chargeMeter;
    private TextView speedView;

    private TextView motorCurrent;
    private TextView motorTorque;
    private TextView battTempHigh;
    private TextView battTempAvg;
    private TextView motorTemp;

    private SensorManager sensorManager;
    private Sensor accelerometer;

    private float previousRotation = 0;

    private AlertDialog infoDisplay;
    private byte currentState = -1;

    ByteBuffer btByteBuffer = ByteBuffer.allocate(2);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(R.layout.activity_main);

        // region #SETUP COLORS
        GREEN = getResources().getColor(R.color.green);
        YELLOW = getResources().getColor(R.color.yellow);
        ORANGE = getResources().getColor(R.color.orange);
        RED = getResources().getColor(R.color.red);
        PROGRESS_COLORS = new int[] {RED, ORANGE, YELLOW, GREEN};
        // endregion

        BluetoothCommService bcs = new BluetoothCommService(handler);
        bcs.start();
        btByteBuffer.order(ByteOrder.LITTLE_ENDIAN);

        initSensors();
        initDefaultUI();
    }

    // region [ BT DATA HANDLER ]
    private final Handler handler = new Handler(new Handler.Callback() {

        @Override

        public boolean handleMessage(Message message) {
            byte[] data = (byte[]) message.obj;
            btByteBuffer.clear();
            btByteBuffer.put(data, 1, 2);
            short value = btByteBuffer.getShort(0);
            switch (data[0]) {
                case BT_BATT_SOC:
                    int soc = btByteBuffer.get(0);
                    chargeMeter.setProgress(soc);
                    chargeMeter.setBottomText(soc + "\uFE6A");
                    int newColor = PROGRESS_COLORS[Math.min(3, soc / 25)];
                    if (newColor != currentProgressColor) {
                        chargeMeter.setFinishedStrokeColor(newColor);
                        chargeMeter.setTextColor(newColor);
                    }
                    break;
                case BT_BATT_HI_AVG_TEMP:
                    battTempHigh.setText(String.valueOf(btByteBuffer.get(0)));
                    battTempAvg.setText(String.valueOf(btByteBuffer.get(1)));
                    break;
                case BT_CAR_STARTUP_STATE:
                    handleStartupCode(btByteBuffer.get(0));
                    break;
                case BT_MOTOR_TEMP:
                    motorTemp.setText(String.valueOf(value));
                    break;
                case BT_CAR_SPEED:
                    speedView.setText(String.valueOf(value));
                    break;
                case BT_CAR_TORQUE:
                    break;
                case BT_CAR_CURRENT_DRAW:
                    motorCurrent.setText(String.valueOf(value / 10));
                    break;
                case BT_DISCONNECT:
                    initDefaultUI();
                    break;
            }
            return true;
        }

    });
    // endregion

    public void handleStartupCode(byte startupCode) {
        if (currentState == startupCode) {
            return;
        }
        currentState = startupCode;

        String message = null;
        switch (startupCode) {
            case 0:
                message = "Dude, ur in a dope-ass car. This thing lit";
                break;
            case 1:
                message = "Press progress button to begin startup sequence";
                break;
            case 2:
                message = "Testing for BMS Error and loading food supply...";
                break;
            case 3:
                message = "Press brake pedal and progress button to start car";
                break;
            case 4:
                message = "Pre-charging capacitors and crossing fingers...";
                break;
            case 5:
                if (infoDisplay != null && infoDisplay.isShowing()) {
                    infoDisplay.dismiss();
                }
                break;
            case 8:
                message = "Shutdown Complete! You did fine I guess";
                break;
        }

        if (message != null) {
            btErrorDialog(message);
        }
    }

    public void btErrorDialog(String message) {
        if (infoDisplay != null && infoDisplay.isShowing()) {
            infoDisplay.dismiss();
        }
        infoDisplay = new AlertDialog.Builder(MainActivity.this)
            .setMessage(message).setIcon(R.drawable.ic_launcher)
                .setCancelable(false).create();
        infoDisplay.show();
    }

    private void initDefaultUI() {
        chargeMeter = (ArcProgress) findViewById(R.id.charge_meter);
        speedView = (TextView) findViewById(R.id.speed_view);
        battTempHigh = (TextView) findViewById(R.id.batt_temp_high);
        battTempAvg = (TextView) findViewById(R.id.batt_temp_avg);
        motorTemp = (TextView) findViewById(R.id.motor_temp);
        motorCurrent = (TextView) findViewById(R.id.motor_current_draw);
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
            float x = event.values[0];
            float y = event.values[1];
            float angle = (float) Math.toDegrees(Math.atan2(-y, x));
            float viewRotation = speedView.getRotation();
            viewRotation += 0.5 * (angle - viewRotation);
            if (Math.abs(viewRotation - previousRotation) > 0.2) {
                speedView.setRotation(viewRotation);
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
