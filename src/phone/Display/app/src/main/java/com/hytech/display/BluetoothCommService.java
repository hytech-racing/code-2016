package com.hytech.display;

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

    private static final String LOG_TAG = "BT_COMM";
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
        if (btCommunicationThread != null) {
            btCommunicationThread.interrupt();
            btCommunicationThread = null;
        }
        btSetupConnection = new SetupConnection();
//        log("Connecting");
        btSetupConnection.start();
    }

    public synchronized void connected() {
        if (btSetupConnection != null) {
            btSetupConnection.interrupt();
            btSetupConnection = null;
        }
        btCommunicationThread = new CommunicationThread();
//        log("Talking");
        btCommunicationThread.start();
    }

    public synchronized void disconnected() {
//        log("Disconnected");
        start();
    }

    private class SetupConnection extends Thread {
        @Override
        public void run() {
            if (!btAdapter.isEnabled()) btAdapter.enable();
            while (!btAdapter.isEnabled());
            BluetoothDevice btServer = btAdapter.getRemoteDevice(SERVER_ADDR);
            btAdapter.cancelDiscovery();
            while (btSocket == null || !btSocket.isConnected()) {
                try {
                    btSocket = btServer
                            .createInsecureRfcommSocketToServiceRecord(
                            UUID.fromString(B_UUID));
                } catch (Exception e) {
                    continue;
                }
                try {
                    btSocket.connect();
                } catch (IOException e) {
                    try {
                        btSocket = (BluetoothSocket) btServer.getClass()
                                .getMethod("createRfcommSocket",
                                        new Class[] {int.class})
                                .invoke(btServer, PORT);
                        btSocket.connect();
                    }
                    catch (Exception e2) {
//                        log("Connection Failed");
                        try {
                            btSocket.close();
                        } catch (IOException e3) {
//                            log("Couldn't close socket");
                        }
                    }
                }
            }
//            log("Socket created and connected");
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
//                log("IO streams creation failed");
            }
            btReader = tmpIn;
            btWriter = tmpOut;
//            log("IO streams created");
        }

        @Override
        public void run() {
            byte[] buffer = new byte[DATA_LENGTH];
            int bytes;
            boolean running = true;
            while (running) {
                try {
                    bytes = btReader.read(buffer);
                    btHandler.obtainMessage(0, bytes, 0, buffer).sendToTarget();
                } catch (IOException e) {
                    buffer[0] = MainActivity.BT_DISCONNECT;
                    btHandler.obtainMessage(0, 1, 0, buffer).sendToTarget();
                    try {
                        btReader.close();
                        btWriter.close();
                    } catch (IOException e1) {
                        log("Couldn't close streams");
                    }
                    running = false;
                }
            }
            disconnected();
        }

        public void write(byte[] buffer) {
            try {
                btWriter.write(buffer);
            } catch (IOException e) { }
        }
    }

    public void write(byte[] out) {
        if (btCommunicationThread != null) {
            btCommunicationThread.write(out);
        } else {
            log("BT_COMM_THREAD is null");
        }
    }

    public static void log(String message) {
        Log.d(LOG_TAG, message);
    }
}
