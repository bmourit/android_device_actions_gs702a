package com.actions.server;

import java.util.ArrayList;

import android.util.Log;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.BroadcastReceiver;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.os.IBinder;
import android.os.PowerManager;
import android.view.WindowManagerPolicy;
import android.os.SystemProperties;
//import android.app.KeyguardManager;
import android.os.RemoteException;
import com.actions.hardware.IDisplayService;
import com.actions.hardware.ICableStatusListener;

//import com.android.server.InputManager;
/**
 * The implementation of the display service.
 * <p>
 * This implementation focuses on manage the tvout and lcd display. Most methods 
 * are synchronous to external calls. 
 * @author Actions
 * @version V2.0
 */
public class DisplayService extends IDisplayService.Stub {

    static {
        System.load("/system/lib/libactions_runtime.so");
    }

    private static final String TAG = "DisplayService";
    /**HDMI cable status*/
    public static final int CABLE_HDMI_ON = 0x1;
    /** tvout cable status*/
    public static final int CABLE_TVOUT_ON = 0x2;
    /**hot plug status*/
    public static final int HOTPLUG = 1;
    /**CVBS plug in value*/
    public static final int CABLE_CVBS_PLUG_IN = 0x1;
    /**cvbs plug out value*/
    public static final int CABLE_CVBS_PLUG_OUT = 0x2;
    /**ypbpr plug in value*/
    public static final int CABLE_YPBPR_PLUG_IN = 0x4;
    /**ypbpr plug out value*/
    public static final int CABLE_YPBPR_PLUG_OUT = 0x8;
    /**hdmi plug in value*/
    public static final int CABLE_HDMI_PLUG_IN = 0x100;
    /**hdmi plug out value*/
    public static final int CABLE_HDMI_PLUG_OUT = 0x200;
    /**cable status*/
    public static int mCableState = 0;
    /**
     * Binder context for this service
     */
    private Context mContext;
    private int mDisplayerCount;
    private int wakeLockFlag = 0x0;
    /* list of listerner */
    private ArrayList<Listener> mListeners = new ArrayList<Listener>();

    private PowerManager.WakeLock mTvoutEventWakeLock;

    // private KeyguardManager keyguardManager;
    // private KeyguardManager.KeyguardLock keyLock;

    //for hdmi
    private boolean mHdmiPluggedIn = false;
    //for cvbs
    private boolean mCvbsPluggedIn = false;
    //indicate onoff mode when hdmi or cvbs is on
    private String onoffMode; 
    //indicate if video is playing
    private boolean mVideoPlay = false;
    
    private HandlerThread mHandlerThread;

    private final class ScreenReceiver extends BroadcastReceiver {
	@Override
        public void onReceive(Context context, Intent intent) {
            synchronized (this) {
                String action = intent.getAction();		
                if (action.equals(Intent.ACTION_SCREEN_ON)) {
                      Log.d(TAG, "ACTION_SCREEN_ON");
                      if (!getHdmiDisconnectFlag() && !mVideoPlay) {
                          setSwitchStatus(1);
                      }
                } else if (action.equals(Intent.ACTION_SCREEN_OFF)) {
                      Log.d(TAG, "ACTION_SCREEN_OFF");
                      if (!getHdmiDisconnectFlag() && !mVideoPlay) {
                          setSwitchStatus(0);
                      }

                }   
            }	    

        }

    }

    private final class HdmiReceiver extends BroadcastReceiver {
	@Override
        public void onReceive(Context context, Intent intent) {
            synchronized (this) {
                if (WindowManagerPolicy.ACTION_HDMI_PLUGGED.equals(intent.getAction())) {
                    mHdmiPluggedIn = intent.getBooleanExtra(WindowManagerPolicy.EXTRA_HDMI_PLUGGED_STATE, false);
                    if (onoffMode.equals("alwayson")) {
                        if (mHdmiPluggedIn) {
                            if (wakeLockFlag == 0x0) {
                                mTvoutEventWakeLock.acquire();
                                wakeLockFlag = 0x1;
                            }
                        } else {
                            if (wakeLockFlag == 0x1) {
                                mTvoutEventWakeLock.release();
                                wakeLockFlag = 0x0;
                            }
                        }
                    }
                }	    
            }
                    Log.d(TAG, "here receive HDMI_PLUGGED broadcast,plug flag=" + mHdmiPluggedIn);

        }
    }

    private final class CvbsReceiver extends BroadcastReceiver {
	@Override
        public void onReceive(Context context, Intent intent) {
            synchronized (this) {
                if (WindowManagerPolicy.ACTION_CVBS_PLUGGED.equals(intent.getAction())) {
                    mCvbsPluggedIn = intent.getBooleanExtra(WindowManagerPolicy.EXTRA_CVBS_PLUGGED_STATE, false);
                    if (onoffMode.equals("alwayson")) {
                        if (mCvbsPluggedIn) {
                            if (wakeLockFlag == 0x0) {
                                mTvoutEventWakeLock.acquire();
                                wakeLockFlag = 0x1;
                            }
                        } else {
                            if (wakeLockFlag == 0x1) {
                                mTvoutEventWakeLock.release();
                                wakeLockFlag = 0x0;
                            }
                        }
                    }
                }	    
                    Log.d(TAG, "here receive CVBS_PLUGGED broadcast,plug flag=" + mHdmiPluggedIn);

            }

        }
    }


   	
    /**
     * Constructs a new DisplayService instance.
     * 
     * @param context
     *            Binder context for this service
     */
    public DisplayService(Context context) {
        mContext = context;
        _init();
        mDisplayerCount = _getDisplayerCount();
        onoffMode = SystemProperties.get("ro.hdmi.onoffmode", "alwayson");
        // boot with lcd display
        // setOutputDisplayer(-1);
        // create a monitor thread.
        // new Thread(new CableMonitorThread()).start();
        PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
        mTvoutEventWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "handleTvoutEvent");

        // keyguardManager = (KeyguardManager)
        // context.getSystemService(Context.KEYGUARD_SERVICE);
        // keyLock = keyguardManager.newKeyguardLock(TAG);
        
        IntentFilter filter = new IntentFilter();
        filter.addAction(WindowManagerPolicy.ACTION_HDMI_PLUGGED);
        mContext.registerReceiver(new HdmiReceiver(), filter);
        filter = new IntentFilter();
        filter.addAction(WindowManagerPolicy.ACTION_CVBS_PLUGGED);
        mContext.registerReceiver(new CvbsReceiver(), filter);
        if (onoffMode.equals("auto")) {
            filter = new IntentFilter();
            filter.addAction(Intent.ACTION_SCREEN_ON);
            mContext.registerReceiver(new ScreenReceiver(), filter);
            filter = new IntentFilter();
            filter.addAction(Intent.ACTION_SCREEN_OFF);
            mContext.registerReceiver(new ScreenReceiver(), filter);
        }
        
    }

    /**
     * used to test
     */
    public void testInit() {
        synchronized (this) {
            _init();
        }
    }
    /**
     * get displayer count 
     * @return actual displayer  count 
     */
    public int getDisplayerCount() {
        return mDisplayerCount;
    }
    /**
     * get one displayer's information
     * @param id displayer that represent one displayer
     * @return displayer's information
     */
    public String getDisplayerInfo(int id) {
        return _getDisplayerInfo(id);
    }
    
    /**
     * get current displayer which are displaying.
     * @return displayer's id which are current displayers
     */
    public int getOutputDisplayer() {
        synchronized (this) {
            return _getOutputDisplayer();
        }
    }
    /**
     * get current displayer(string format) which are displaying.
     * @return displayer's id which are current displayers
     */
    public String getOutputDisplayerStr() {
        synchronized (this) {
            return _getOutputDisplayerStr();
        }
    }
    /**
     * set current displayers which will display at once.
     * @param id displayers' id which will display
     * @return whether displayer set is successful or not.
     */
    public boolean setOutputDisplayer(int id) {
        synchronized (this) {
            boolean ret;
            ret = _setOutputDisplayer(id);
            // InputManager.updateTvCalibrationParam();
            return ret;
        }
    }
    /**
     * set current displayers(string format) which will display at once.
     * @param id displayers' id(string format) which will display
     * @return whether displayer set is successful or not.
     */
    public boolean setOutputDisplayer1(String id) {
        Log.d(TAG, "[enter setOutputDisplayer1]\n");

        synchronized (this) {
            boolean ret;
        
            ret = _setOutputDisplayer1(id);
            if (onoffMode.equals("alwayson")) {
                if ((id.indexOf("cvbs") >= 0) || (id.indexOf("hdmi") >= 0)) {
                    if (wakeLockFlag == 0x0) {
                        mTvoutEventWakeLock.acquire();
                        wakeLockFlag = 0x1;
                    }
                } else {
                    if (wakeLockFlag == 0x1) {
                       mTvoutEventWakeLock.release();
                       wakeLockFlag = 0x0;
                    }
                }
            }
		
            return ret;
        }

    }
    /**
     * enable devices output ,which are set by {@link #setOutputDisplayer}
     * @param enable enable or disable displayer output
     * @return whether displayer set is successful or not.
     */
    public boolean enableOutput(boolean enable) {
        synchronized (this) {
            return _enableOutput(enable);
        }
    }
    /**
     * enable devices(string format) output ,which are set by {@link #setOutputDisplayer}
     * @param id displayer's id
     * @param enable enable or disable displayer(string format) output
     * @return whether displayer set is successful or not.
     */
    public boolean enableOutput1(String id, boolean enable) {
        synchronized (this) {
            return _enableOutput1(id, enable);
        }
    }
    /**
     * set dual displayers output at the same time, which will display at once.
     * @param dualOutput displayers' id which will display
     * @return whether displayer set is successful or not.
     */
    public boolean setDualDisplay(boolean dualOutput) {
        synchronized (this) {
            return _setDualDisplay(dualOutput);
        }
    }
    /**
     * set display mode.include 7 display
     * modes.ref:android/device/actions/hardware/include/de.h
     * 
     * @param mode     the mode which you want to set.the mode could be
     *            interpretationed to one of 3 bandth width.
     */
    public void setDisplayMode(int mode) {
        Log.d(TAG, "[enter DisplayService.setDisplayMode]\n");
        synchronized (this) {
            _setDisplayMode(mode);
        }
    }
    /**
     * set display mode.include 7 display
     * modes.ref:android/device/actions/hardware/include/de.h
     * 
     * @param mode the mode which you want to set.
     */
    public void setDisplaySingleMode(int mode) {
        Log.d(TAG, "[enter DisplayService.setDisplaySingleMode]\n");
        synchronized (this) {
            _setDisplaySingleMode(mode);
        }
    }

    /**
     * get hdmi's video id,equal with getHdmiParam.
     * @return hdmi's vid currently.
     */
    public int getHdmiVid() {
        Log.d(TAG, "[enter DisplayService.getDisplayVid]\n");
        
        synchronized (this) {
            int vid = _getHdmiVid();
            return vid;

        }
    }
    
    /**
     * set hdmi's uevent switch
     * 
     * @param status hdmi uevent that you want set.
     */
    public void setSwitchStatus(int status) {
        Log.d(TAG, "[enter DisplayService.setSwitchStatus]\n");
        synchronized (this) {
             _setSwitchStatus(status);
        }
    }
    
    /**
     * get hdmi's disconnectd flag
     * @return if hdmi funtion is closed.
     */
    public boolean getHdmiDisconnectFlag() {
        Log.d(TAG, "[enter DisplayService.getHdmiDisconnectFlag]\n");
        
        synchronized (this) {
            boolean flag = _getHdmiDisconnectFlag();
            return flag;

        }
    }
	
    /**
     * when plug in and choose hdmi disconnect, could trigger irq, 
     * in order discard these irq, set this flag
     * @param flag plug in and choose hdmi disconnect state, set true, otherwise set false
     */
    public void setHdmiDisconnectFlag(boolean flag) {
        Log.d(TAG, "[enter DisplayService.setHdmiDisconnectFlag]\n");
        synchronized (this) {
             _setHdmiDisconnectFlag(flag);
        }
    }
    
    /**
     * set hdmi's video id,equal with setHdmiParam.
     * 
     * @param vid hdmi video id which you want to set.
     */
    public void setHdmiVid(int vid) {
        Log.d(TAG, "[enter DisplayService.setDisplayVid]\n");
        synchronized (this) {
             _setHdmiVid(vid);
        }
    }
    /**
     * get hdmi's capability,including res,aspect,vedio id,audio info of tv.
     * 
     * @return information of hdmi which tv supports.
     */
    public String getHdmiCap() {
        synchronized (this) {
            return _getHdmiCap();
        }
    }
    /**
     * set displayers' parameters,which include hdmi's res,hz,aspect and cvbs's
     * mode.
     * 
     * @param param
     *            displayers's information.
     * @return if set is successful,return true,otherwise false.
     */
    public boolean setDisplayerParam(String param) {
        boolean result;
        synchronized (this) {
            result = _setDisplayerParam(param);
            if (param != null) {
                if (param.indexOf("scale-x=") >= 0 && param.indexOf("scale-y=") >= 0) {
                    // InputManager.updateTvCalibrationParam();
                }
            }

            return result;
        }
    }
    /**
     * get displayers' parameters,which include hdmi's res,hz,aspect and cvbs's
     * mode.
     * 
     * @return all displayers' infomation.
     */
    public String getDisplayerParam() {
        return _getDisplayerParam();
    }
    /**
     * get displayers' parameters,which include hdmi's res,hz,aspect and cvbs's
     * mode.
     * 
     * @param displayer
     *            one displayers's information.
     * @return some displayer's parameters.
     */
    public String getDisplayerParamStr(String displayer) {
        return _getDisplayerParamStr(displayer);
    }
    /**
     * get hdmi and cvbs's cable state.
     * 
     * @return cable state value.
     */
    public int getCableState() {
        synchronized (this) {
            return _getCableState();
        }
    }

    Handler eventHander = new Handler() {
        public void handleMessage(Message msg) {
            // to do
            Intent intent = new Intent("com.actions.intent.cablechanged");
            String displayer;
            String action;

            Log.d(TAG, "Handler get new events:" + msg);

            if (msg.what == DisplayService.HOTPLUG) {
                switch (msg.arg1) {
                    case CABLE_CVBS_PLUG_IN: { // cvbs event
                        displayer = "cvbs";
                        action = "plugin";
                    }
                        break;
                    case CABLE_CVBS_PLUG_OUT: { // cvbs event
                        displayer = "cvbs";
                        action = "plugout";
                    }
                        break;
                    case CABLE_YPBPR_PLUG_IN: { // ypbpr event
                        displayer = "ypbpr";
                        action = "plugin";
                    }
                        break;
                    case CABLE_YPBPR_PLUG_OUT: { // ypbpr event
                        displayer = "ypbpr";
                        action = "plugout";
                    }
                        break;
                    case CABLE_HDMI_PLUG_IN: {
                        displayer = "hdmi";
                        action = "plugin";
                    }
                        break;
                    case CABLE_HDMI_PLUG_OUT: {
                        displayer = "hdmi";
                        action = "plugout";
                    }
                        break;
                    default:
                        return;
                }
                intent.putExtra("action", action);
                intent.putExtra("type", displayer);
                // intent.addFlags(Intent.FLAG_RECEIVER_REGISTERED_ONLY);
                if (mContext != null) {
                    mContext.sendBroadcast(intent);
                    Log.v(TAG, "Send sendBroadcast:" + intent + msg.arg1);
                    return;
                }
                notifyStatusListener(displayer, action);

            }
        }
    };

    private void notifyStatusListener(String displayer, String action) {
        synchronized (mListeners) {
            int size = mListeners.size();
            for (int i = 0; i < size; i++) {
                Listener listener = mListeners.get(i);
                try {
                    listener.mListener.onStatusChanged(displayer, action);
                } catch (RemoteException e) {
                    Log.w(TAG, "RemoteException in reportNmea");
                    mListeners.remove(listener);
                }
            }

        }
    }
    /**
     * listen cable plug status
     * @param listener be charge of listening cable plug status
     * @throws RemoteException exception
     */
    public void addStatusListener(ICableStatusListener listener) throws RemoteException {
        if (listener == null) {
            throw new NullPointerException("listener is null in addStatusListener");
        }
        synchronized (mListeners) {
            IBinder binder = listener.asBinder();
            int size = mListeners.size();
            for (int i = 0; i < size; i++) {
                Listener tmp = mListeners.get(i);
                if (binder.equals(tmp.mListener.asBinder())) {
                    // listener already added
                    return;
                }
            }
            Listener l = new Listener(listener);
            binder.linkToDeath(l, 0);
            mListeners.add(l);
        }
    }

    class CableMonitorThread extends Thread {
        public CableMonitorThread() {
            super("cableMonitorThread");
        }

        public void run() {
            // register the native method here and start to run.
            _cable_monitor();
        }

        /**
         * notify the hdmi plugin events what: hdmi/tvout cabled connected
         * value: plugin/plugout action
         * 
         * 
         */
        public void plugEvent(int value) {
            Message message = new Message();
            message.what = HOTPLUG; // 1 means hdmi events
            message.arg1 = value;
            Log.d(TAG, "Start to send message Pluged:" + message);
            DisplayService.this.eventHander.sendMessage(message);
        }

        private native void _cable_monitor();
    }

    private final class Listener implements IBinder.DeathRecipient {
        final ICableStatusListener mListener;
        int mSensors = 0;

        Listener(ICableStatusListener listener) {
            mListener = listener;
        }

        public void binderDied() {
            Log.w(TAG, "DisplayManager status listener died");
            synchronized (mListeners) {
                mListeners.remove(this);
            }
            if (mListener != null) {
                mListener.asBinder().unlinkToDeath(this, 0);
            }
        }
    }

    private static native boolean _init();

    private native int _getDisplayerCount();

    private native String _getDisplayerInfo(int id);

    private native boolean _setOutputDisplayer(int id);

    private native boolean _setOutputDisplayer1(String id);

    private native int _getOutputDisplayer();

    private native String _getOutputDisplayerStr();

    private native boolean _enableOutput1(String id, boolean enable);

    private native boolean _enableOutput(boolean enable);

    private native boolean _setDualDisplay(boolean dualOutput);

    private native boolean _setDisplayerParam(String param);

    private native String _getDisplayerParam();

    private native String _getDisplayerParamStr(String displayer);

    private native int _getCableState();

    private native void _setDisplayMode(int mode);

    private native void _setDisplaySingleMode(int mode);

    private native void _setSwitchStatus(int status);

    private native boolean _getHdmiDisconnectFlag();
    private native void _setHdmiDisconnectFlag(boolean flag);
    private native void _setHdmiVid(int vid);
    private native int _getHdmiVid();
    private native String _getHdmiCap();

}
