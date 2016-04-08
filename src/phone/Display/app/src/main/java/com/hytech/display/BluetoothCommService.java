package com.hytech.display;

import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.os.Handler;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

public class BluetoothCommService {

    private final BluetoothAdapter btAdapter;
    private BluetoothSocket btSocket;

    private final Handler btHandler;
    private SetupConnection btSetupConnection;
    private CommunicationThread btCommunicationThread;

    private static final String LOG_TAG = "RETRYING";
    // Laptop Address
    // private static final String SERVER_ADDR = "A0:A8:CD:B5:52:97";
    // RPi Address
    private static final String SERVER_ADDR = "00:1A:7D:DA:71:11";
    private static final String B_UUID = "00001101-0000-1000-8000-00805F9B34FB";
    private static final int PORT = 10;

    private static final int DATA_LENGTH = 16;

    public BluetoothCommService(Handler handler) {
        btAdapter = BluetoothAdapter.getDefaultAdapter();
        btHandler = handler;
    }

    public synchronized void start() {
        btSetupConnection = new SetupConnection();
        btSetupConnection.start();
    }

    public synchronized void connected() {
        btSetupConnection = null;
        btCommunicationThread = new CommunicationThread();
        btCommunicationThread.start();
    }

    private class SetupConnection extends Thread {
        @Override
        public void run() {
            if (!btAdapter.isEnabled()) btAdapter.enable();
            while (!btAdapter.isEnabled());
            log("Bluetooth enabled");
            BluetoothDevice btServer = btAdapter.getRemoteDevice(SERVER_ADDR);
            btAdapter.cancelDiscovery();
            try {
                btSocket = btServer.createInsecureRfcommSocketToServiceRecord(
                        UUID.fromString(B_UUID));
            } catch (Exception e) {
                log("Socket creation failed");
                return;
            }
            try {
                btSocket.connect();
            } catch (IOException e) {
                log("Socket connection method #1 failed");
                try {
                    btSocket = (BluetoothSocket) btServer.getClass()
                            .getMethod("createRfcommSocket",
                                    new Class[] {int.class}).invoke(btServer,
                                    PORT);
                    btSocket.connect();
                }
                catch (Exception e2) {
                    log("Socket connection method #2 failed");
                    return;
                }
            }
            log("Socket created and connected");
            connected();
        }
    }

    private class CommunicationThread extends Thread {
        private final InputStream btReader;
        private final OutputStream btWriter;

        public CommunicationThread() {
            InputStream tmpIn = null;
            OutputStream tmpOut = null;
            try {
                tmpIn = btSocket.getInputStream();
                tmpOut = btSocket.getOutputStream();
            } catch (IOException e) {
                log("IO streams creation failed");
            }
            btReader = tmpIn;
            btWriter = tmpOut;
            log("IO streams created");
        }

        @Override
        public void run() {
            byte[] buffer = new byte[DATA_LENGTH];
            int bytes;
            while (true) {
                try {
                    bytes = btReader.read(buffer);
                    btHandler.obtainMessage(0, bytes, 0, buffer)
                            .sendToTarget();
                } catch (IOException e) {
                    log("disconnected? " + e.getMessage());
                    break;
                }
            }
            buffer[0] = 0x7F;
            btHandler.obtainMessage(0, 1, 0, buffer)
                    .sendToTarget();
        }

        public void write(byte[] buffer) {
            try {
                btWriter.write(buffer);
                btHandler.obtainMessage(0, 0, 0, buffer)
                        .sendToTarget();
            } catch (IOException e) {
            }
        }
    }

    public void write(byte[] out) {
        CommunicationThread r;
        r = btCommunicationThread;
        r.write(out);
    }

    public static void log(String message) {
        Log.d(LOG_TAG, message);
    }
}
