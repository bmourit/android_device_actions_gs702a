package com.actions.sensor.calib;

import android.app.Activity;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

public class SensorActivity extends Activity
implements SensorEventListener {
    private final String TAG = "SensorActivity";
    private View.OnClickListener mBtnCalibListener = new View.OnClickListener() {
        public void onClick(View paramAnonymousView) {
            SensorActivity.this.mButtonCalib.setClickable(false);
            SensorActivity.access$102(SensorActivity.this, true);
            SensorActivity.this.sc.resetCalib();
            SensorActivity.this.mHandler.postDelayed(new Runnable() {
                public void run() {
                    SensorActivity.this.mButtonCalib.setClickable(true);
                    SensorActivity.access$102(SensorActivity.this, false);
                    SensorActivity.this.sc.runCalib();
                    String str1 = SensorActivity.this.sc.getCalibValue();
                    Log.i("SensorActivity", "Calib: " + str1);
                    String str2 = str1 + "\n";
                    SensorActivity.this.sc.writeCalibFile(str2);
                    Toast.makeText(SensorActivity.this, info_calib, 1).show();
                }
            }
            , 1000L);
        }
    };

    private View.OnClickListener mBtnResetListener = new View.OnClickListener() {
        public void onClick(View paramAnonymousView) {
            SensorActivity.this.mButtonReset.setClickable(false);
            SensorActivity.access$102(SensorActivity.this, true);
            SensorActivity.this.sc.resetCalib();
            SensorActivity.this.mHandler.postDelayed(new Runnable() {
                public void run() {
                    SensorActivity.this.mButtonReset.setClickable(true);
                    SensorActivity.access$102(SensorActivity.this, false);
                    String str1 = SensorActivity.this.sc.getCalibValue();
                    Log.i("SensorActivity", "Calib: " + str1);
                    String str2 = str1 + "\n";
                    SensorActivity.this.sc.writeCalibFile(str2);
                    Toast.makeText(SensorActivity.this, info_reset, 1).show();
                }
            }
            , 1000L);
        }
    };

    private Button mButtonCalib;
    private final int mButtonDelay = 1000;
    private Button mButtonReset;
    private boolean mCalibMode = false;
    private Handler mHandler;
    private SensorHost mSensorHost;
    private TextView mTextView;
    private SensorControl sc = null;
    private SensorManager sm = null;

    private void findViews() {
        this.mTextView = ((TextView)findViewById(mTextView));
        this.mButtonCalib = ((Button)findViewById(buttonCalib));
        this.mButtonReset = ((Button)findViewById(buttonReset));
        this.mSensorHost = ((SensorHost)findViewById(mSensorHost));
    }

    private void setListensers() {
        this.mButtonCalib.setOnClickListener(this.mBtnCalibListener);
        this.mButtonReset.setOnClickListener(this.mBtnResetListener);
    }

    public void onAccuracyChanged(Sensor paramSensor, int paramInt) {
    }

    public void onCreate(Bundle paramBundle) {
        super.onCreate(paramBundle);
        setContentView(main);
        findViews();
        this.mHandler = new Handler();
        this.sm = ((SensorManager)getSystemService("sensor"));
        this.sc = new SensorControl(this);
        setListensers();
    }

    protected void onDestroy() {
        super.onDestroy();
        this.mHandler = null;
        this.sm = null;
        this.sc = null;
    }

    protected void onPause() {
        super.onPause();
        this.sm.unregisterListener(this);
    }

    protected void onResume() {
        super.onResume();
        this.sm.registerListener(this, this.sm.getDefaultSensor(1), 3);
    }

    public void onSensorChanged(SensorEvent paramSensorEvent) {
        if ((paramSensorEvent == null) || (paramSensorEvent.values.length != 3) || (this.mCalibMode));
        do {
            return;
            if (this.mTextView != null) {
                Object[] arrayOfObject = new Object[3];
                arrayOfObject[0] = Float.valueOf(paramSensorEvent.values[0]);
                arrayOfObject[1] = Float.valueOf(paramSensorEvent.values[1]);
                arrayOfObject[2] = Float.valueOf(paramSensorEvent.values[2]);
                String str = String.format("X: %.3f, Y: %.3f, Z: %.3f", arrayOfObject);
                this.mTextView.setText(str);
                this.mTextView.setTextColor(-16711681);
            }
        }

        while (this.mSensorHost == null);
        this.mSensorHost.onSensorChanged(paramSensorEvent);
    }
}
