package com.hytech.display;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.Gravity;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.util.Log;

import com.github.lzyzsd.circleprogress.ArcProgress;
import com.github.nkzawa.emitter.Emitter;
import com.github.nkzawa.socketio.client.IO;
import com.github.nkzawa.socketio.client.Socket;

import java.net.URISyntaxException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;


public class MainActivity extends Activity implements SensorEventListener {

    // region # CONSTANTS
    public int GREEN;
    public int YELLOW;
    public int ORANGE;
    public int RED;
    public int[] PROGRESS_COLORS;

    //public static final byte BT_BATT_SOC = 0;
    public static final byte BT_PCU_TEMP = 1;
    public static final byte BT_PCU_STATUS = 2;
    public static final byte BT_MOTOR_TEMP = 3;
    public static final byte BT_MOTOR_SPEED = 4;
    public static final byte BT_MOTOR_CURRENT = 5;
    public static final byte BT_PCU_VOLTAGE = 6;
    public static final byte BT_PEDALS = 7;
    public static final byte BT_TCU_STATUS = 8;
    public static final byte BT_DISCONNECT = 0x7F;
    // endregion

    private int currentProgressColor = RED;
    private ArcProgress chargeMeter;
    private TextView speedView;

    private ImageView connectedView;

    private TextView motorCurrent;
    private TextView battTempHigh;
    private TextView battTempAvg;
    private TextView motorTemp;
    private TextView rearVoltage;

    private ProgressBar pedalsAccel;
    private ProgressBar pedalsBrake;

    private SensorManager sensorManager;
    private Sensor accelerometer;

    private float previousRotation = 0;

    private AlertDialog infoDisplay;
    private byte currentPCUState = -1;
    private byte currentTCUState = -1;
    private byte errorState = -1;

    private Socket socketIO;
    private static final String MSG_NAMESPACE = "telemetry";

    ByteBuffer btByteBuffer = ByteBuffer.allocate(7);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(R.layout.activity_main);

        // region # SETUP COLORS
        GREEN = getResources().getColor(R.color.green);
        YELLOW = getResources().getColor(R.color.yellow);
        ORANGE = getResources().getColor(R.color.orange);
        RED = getResources().getColor(R.color.red);
        PROGRESS_COLORS = new int[] {RED, ORANGE, YELLOW, GREEN};
        // endregion

        // region # SETUP SOCKET_IO
        try {
            socketIO = IO.socket("http://hytech-telemetry.herokuapp.com/");
        } catch (URISyntaxException e) {
            BluetoothCommService.log("SocketIO creation failed: " + e.getMessage());
        }

        socketIO.on(Socket.EVENT_CONNECT, new Emitter.Listener() {
            @Override
            public void call(Object... args) {
                BluetoothCommService.log("SocketIO Connected");
            }
        }).on(Socket.EVENT_DISCONNECT, new Emitter.Listener() {
            @Override
            public void call(Object... args) {
                BluetoothCommService.log("SocketIO Disconnected");
            }
        });

        socketIO.connect();
        // endregion

        BluetoothCommService bcs = new BluetoothCommService(handler,
                new BluetoothCommService.OnConnectedListener() {
            @Override
            public void onConnectedChange(final boolean connected) {
                MainActivity.this.runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (connected) {
                            connectedView.setVisibility(View.VISIBLE);
                        } else {
                            connectedView.setVisibility(View.GONE);
                            setDefaultValues();
                        }
                    }
                });
            }
        });
        bcs.start();
        btByteBuffer.order(ByteOrder.LITTLE_ENDIAN);

        sensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        bindUIElements();
    }

    // region [ BT DATA HANDLER ]
    private final Handler handler = new Handler(new Handler.Callback() {
        @Override
        public boolean handleMessage(Message message) {
            Log.d("BT_DISP", "Message received");
            byte[] data = (byte[]) message.obj;
            btByteBuffer.clear();
            btByteBuffer.put(data, 1, 7);
            short value = btByteBuffer.getShort(0);
            switch (data[0]) {
                /*
                 * As of Apr 30 2017, State of Charge is not implemented on BMS
                 * This code will work once it is implemented
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
                 */
                case BT_PCU_TEMP:
                    battTempHigh.setText(String.valueOf(btByteBuffer.get(1) << 8 | btByteBuffer.get(0)));
                    battTempAvg.setText(String.valueOf(btByteBuffer.get(4) << 8 | btByteBuffer.get(3)));
                    break;
                case BT_PCU_STATUS:
                    handlePCUMessage(btByteBuffer.get(0), btByteBuffer.get(1));
                    break;
                case BT_MOTOR_TEMP:
                    motorTemp.setText(String.valueOf(value));
                    break;
                case BT_MOTOR_SPEED:
                    speedView.setText(String.valueOf(value));
                    break;
                case BT_MOTOR_CURRENT:
                    motorCurrent.setText(String.valueOf(value / 10));
                    break;
                case BT_PCU_VOLTAGE:
                    rearVoltage.setText(String.valueOf(btByteBuffer.get(1) << 8 | btByteBuffer.get(0)));
                    break;
                case BT_PEDALS:
                    //pedalsAccel.setProgress(btByteBuffer.get(1));
                    //pedalsBrake.setProgress(btByteBuffer.get(0));
                    break;
                case BT_TCU_STATUS:
                    handleTCUMessage(btByteBuffer.get(0), btByteBuffer.get(1));
                    break;
                case BT_DISCONNECT:
                    setDefaultValues();
                    break;
                default:
                    return true;
            }
            socketIO.emit(MSG_NAMESPACE, data);
            return true;
        }

    });
    // endregion

    public void handlePCUMessage(byte state, byte flags) {
        errorState = -1;
        if (currentPCUState == state)
            return;

        currentPCUState = state;

        String message = null;
        switch (state) {
            case 1:
                message = "Waiting for BMS & IMD";
                break;
            case 2:
                message = "Press START button to begin";
                break;
            case 3:
                message = "Latching AIRs...";
                break;
            case 4:
                if (currentTCUState < 4) {
                    message = "Tractive system energized, but TCU not in expected state";
                }
                break;
        }
        int bms_fault = (flags & 0x8) >> 3;
        int imd_fault = (flags & 0x4) >> 2;
        if (bms_fault > 0) {
            message = "BMS Fault";
        }
        if (imd_fault > 0) {
            message = "IMD Fault";
        }
        if (bms_fault > 0 && imd_fault > 0) {
            message = "BMS & IMD Fault";
        }

        if (message != null) {
            btErrorDialog(message, false);
        }
    }

    public void handleTCUMessage(byte state, byte flags) {
        errorState = -1;
        if (currentTCUState == state) {
            return;
        }

        currentTCUState = state;

        String message = null;
        switch (state) {
            case 4:
                message = "Press brake pedal and START button to start car";
                break;
            case 5:
                message = "Enabling inverter...";
                break;
            case 7:
                if (infoDisplay != null && infoDisplay.isShowing()) {
                    infoDisplay.dismiss();
                }
                break;
        }
        int throttle_error = (flags & 0x8) >> 3;
        int brake_error = (flags & 0x8) >> 1;
        if (throttle_error > 0) {
            message = "Throttle pedal error (implausibility)";
        }
        if (brake_error > 0) {
            message = "Brake pedal error (implausibility)";
        }

        if (message != null) {
            btErrorDialog(message, false);
        }
    }

    public void handleStartupCode(byte startupCode) {
        errorState = -1;
        if (currentPCUState == startupCode) {
            return;
        }

        currentPCUState = startupCode;

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
            btErrorDialog(message, false);
        }
    }

    public void handleErrorCode(byte errorCode, byte startupCode) {
        if (errorState == errorCode) {
            return;
        }

        errorState = errorCode;

        String message = null;
        switch (errorCode) {
            case 1:
                message = "BMS TIMEOUT";
                break;
            case 2:
                message = "EVDC TIMEOUT";
                break;
            case 3:
                message = "Motor Controller TIMEOUT";
                break;
            case 4:
                message = "Rear Arduino TIMEOUT";
                break;
            case 5:
                message = "Main Battery LOW";
                break;
            case 6:
                message = "Main Battery CRITICAL";
                break;
            case 7:
                message = "Main Battery OVERHEAT";
                break;
            case 9:
                message = "Main Battery Cell LOW";
                break;
            case 10:
                message = "Current Draw HIGH";
                break;
            case 11:
                message = "Motor Controller OVERHEAT";
                break;
            case 12:
                message = "Motor OVERHEAT";
                break;
            case 13:
                message = "Pedal ERROR";
                break;
            case 20:
                message = "Rear Battery OVERHEAT";
                break;
            case 21:
                message = "Rear Battery CRITICAL";
                break;
            case 22:
                message = "Rear Battery Temperature HIGH";
                break;
            case 23:
                message = "Rear Battery LOW";
                break;
            case 26:
                message = "IMD ERROR";
                break;
            case 27:
                message = "Coolant Temperature HIGH";
                break;
            case 28:
                message = "Coolant OVERHEAT";
                break;
            case 29:
                message = "12V DC-DC Temperature HIGH";
                break;
            case 30:
                message = "12V DC-DC OVERHEAT";
                break;
            case 31:
                message = "5V DC-DC Temperature HIGH";
                break;
            case 32:
                message = "5V DC-DC OVERHEAT";
                break;
            case 33:
                message = "5V1 DIP";
                break;
            case 34:
                message = "5V2 DIP";
                break;
            case 0:
                handleStartupCode(startupCode);
                return;
        }

        currentTCUState = -1;
        if (message != null) {
            btErrorDialog(message, true);
        }
    }

    public void btErrorDialog(String message, boolean error) {
        if (infoDisplay != null && infoDisplay.isShowing()) {
            infoDisplay.dismiss();
        }
        TextView tv = new TextView(this);
        tv.setPadding(100, 100, 100, 100);
        tv.setGravity(Gravity.CENTER);
        if (error) {
            tv.setTextColor(RED);
        }
        tv.setText(message);
        tv.setTextSize(24);
        infoDisplay = new AlertDialog.Builder(MainActivity.this)
            .setView(tv)
                .setCancelable(false).create();
        infoDisplay.show();
    }

    private void bindUIElements() {
        chargeMeter = (ArcProgress) findViewById(R.id.charge_meter);
        speedView = (TextView) findViewById(R.id.speed_view);
        battTempHigh = (TextView) findViewById(R.id.batt_temp_high);
        battTempAvg = (TextView) findViewById(R.id.batt_temp_avg);
        motorTemp = (TextView) findViewById(R.id.motor_temp);
        motorCurrent = (TextView) findViewById(R.id.motor_current_draw);
        rearVoltage = (TextView) findViewById(R.id.rear_voltage);
        pedalsAccel = (ProgressBar) findViewById(R.id.pedals_accel);
        pedalsBrake = (ProgressBar) findViewById(R.id.pedals_brake);
        connectedView = (ImageView) findViewById(R.id.bt_connected);
    }

    private void setDefaultValues() {
        String defaultString = getResources().getString(R.string.default_value);
        motorCurrent.setText(defaultString);
        battTempHigh.setText(defaultString);
        battTempAvg.setText(defaultString);
        motorTemp.setText(defaultString);
        speedView.setText(defaultString);
        rearVoltage.setText(defaultString);
        chargeMeter.setBottomText(defaultString);
        chargeMeter.setProgress(0);
        pedalsAccel.setProgress(0);
        pedalsBrake.setProgress(0);
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

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {}

    @Override
    public void onResume() {
        super.onResume();
        accelerometer = sensorManager.getDefaultSensor(
                Sensor.TYPE_ACCELEROMETER);
        sensorManager.registerListener(this, accelerometer,
                SensorManager.SENSOR_DELAY_FASTEST);
    }

    @Override
    public void onPause() {
        super.onPause();
        sensorManager.unregisterListener(this, accelerometer);
    }
}
