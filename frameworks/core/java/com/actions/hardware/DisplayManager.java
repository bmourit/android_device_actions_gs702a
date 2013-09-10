/*
 * Copyright (c) 2012 Actions-semi co. All Rights Reserved.
 * Author Actions
 * Version V1.0  
 * Date:  2012-9-12
 * Comment:this class lets you access the Display Engine,cvbs and hdmi.
 */

package com.actions.hardware;

import java.util.StringTokenizer;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.util.Log;

import com.actions.hardware.IDisplayService;
import com.actions.hardware.ICableStatusListener;

/**
 * This class lets you access the Display Engine,cvbs and hdmi.
 * <p>You can set which displayer output with {@link #setOutputDisplayer}.Also,you<br>
 * can set cvbs mode and hdmi using {@link #setDisplayPara} and {@link #setHdmiVid} <br>
 * respectively etc..
 * @author Actions
 * @version V2.0  
 */
public class DisplayManager {
    private static final String TAG = "DisplayManager";

    /**
     * display mode bitmask: multi display port(hdmi/cvbs) output image at the
     * same time
     */
    private static final int FLAG_TVOUT_SIMULTANEOUS = 0x1;

    /**
     * display mode bitmask: lcd and tvout display are extract the same, at this
     * circumstance bandwidth is high some time we have only videos on tv to
     * decrease bandwidth consumption .
     */
    private static final int FLAG_LCD_HAS_VIDEO = 0x2;

    /**
     * display mode bitmask: when FLAG_MINUM_BANDWIDTH is set, performane is the<br>
     * first consideration,
     */
    private static final int FLAG_MIN_BANDWIDTH = 0x4;
    /**whether if the hdmi cable is connected or not*/
    public static final int CABLE_HDMI_CONNECTED = 0x1;
    /**whether if the cvbs cable is connected or not*/
    public static final int CABLE_CVBS_CONNECTED = 0x2;
    /**whether if the ypbpr cable is connected or not*/
    public static final int CABLE_YPBPR_CONNECTED = 0x4;
    /**full screen scale mode*/
    public static final String SCALE_FULLSCREEN = "fullscreen";
    /**uniform screen scale mode*/
    public static final String SCALE_UNIFORM = "uniform";
    /**origin screen scale mode*/
    public static final String SCALE_ORIGIN = "origin";
    /**indicate the pixel scan mode,interleave*/
    public static final int NON_PROGRESSIVE = 0;
    /**indicate the pixel scan mode,progressive*/
    public static final int PROGRESSIVE = 1;
    /**indicate the tv's width:height,4:3*/
    public static final int ASPECT_4VS3 = 0;
    /**indicate the tv's width:height,16:9*/
    public static final int ASPECT_16VS9 = 1;

    /** display mode,low bandwidth means use LEQ 2 layers of de*/
    public static final int DISPLAY_MODE_LOW_BANDWIDTH = FLAG_MIN_BANDWIDTH;
    /** display mode,low bandwidth means use 3 layers of de*/
    public static final int DISPLAY_MODE_MEDIUM_BANDWIDTH = FLAG_MIN_BANDWIDTH | FLAG_LCD_HAS_VIDEO;
    /** display mode,low bandwidth means use 4 layers of de*/
    public static final int DISPLAY_MODE_HIGH_BANDWIDTH = FLAG_TVOUT_SIMULTANEOUS | FLAG_LCD_HAS_VIDEO | FLAG_MIN_BANDWIDTH;

    private IDisplayService mDisplayService;
    private int mDisplayNum = 0;
    /**Constructor,complete initialiazation, get Bp of DisplayService*/
    public DisplayManager() {

        mDisplayService = IDisplayService.Stub.asInterface(ServiceManager.getService("actions.display"));
        if (mDisplayService != null) {
            try {
                Log.i(TAG, mDisplayService.toString());
                mDisplayNum = mDisplayService.getDisplayerCount();
            } catch (RemoteException e) {
                Log.e(TAG, "RemoteException when get Number of Displayer", e);
                mDisplayNum = 0;
            }
        } else {
            Log.e(TAG, "error! can not get the display service!");
            mDisplayNum = 0;
        }
         Log.e(TAG, "enter DisplayManager !");
    }
    /**test*/
    public void testInit() {
        try {
            mDisplayService.testInit();
        } catch (Exception e) {
            Log.e(TAG, "error! mDisplayService.testInit() fail");
        }
    }
    /**
     * get count of displayers which de supports
     * @return displayers' count 
     * */
    public int getDisplayerCount() {
        return mDisplayNum;
    }

    /**
     * get displayers including hdmi's and cvbs's info,such as reslution,aspect,Hz etc..
     * 
     * @param id displayer's id number.
     * 
     * @return displayer's display infomation.
     */
    public DisplayerInfo getDisplayerInfo(int id) {
        String info;
        DisplayerInfo d;
        if (id >= mDisplayNum || mDisplayService == null) {
            return new DisplayerInfo();
        }

        try {
            info = mDisplayService.getDisplayerInfo(id);
        } catch (Exception e) {
            return new DisplayerInfo();
        }

        d = new DisplayerInfo();
        d.unflatten(info);

        return d;
    }
    /**
     * set which diplayer to display
     * @param dev_id displayer's device number.
     * @return whether displayer set is successful or not.
     */
    public boolean setOutputDisplayer(int dev_id) {
        boolean result = false;
        if (mDisplayService == null) {
            Log.e(TAG, "Display service  connected failed or mDisplayNum==0!");
            return false;
        }
        try {
            result = mDisplayService.setOutputDisplayer(dev_id);
        } catch (Exception e) {

        }
        return result;
    }
    /**
     * set which diplayer to display
     * @param dev_id displayer's name which type is string.
     * @return whether displayer set is successful or not.
     */
    public boolean setOutputDisplayer(String dev_id) {
        Log.d(TAG, "[enter DisplayManger.setOutputDisplayer]\n");
        if (mDisplayService == null) {
            Log.e(TAG, "Display service  connected failed or mDisplayNum==0!");
            return false;
        }
        try {
            return mDisplayService.setOutputDisplayer1(dev_id);
        } catch (Exception e) {
            Log.e(TAG, "Display service setOutputDisplayer failed", e);
        }

        return false;

    }
    /**
     * get current displayers which are output.
     * @return current displayers' id
     */
    public int getOutputDisplayer() {
        int id = -1;
        if (mDisplayService == null) {
            Log.e(TAG, "Display service  connected failed or mDisplayNum==0!");
            return id;
        }
        try {
            id = mDisplayService.getOutputDisplayer();
        } catch (Exception e) {

        }

        return id;

    }
    /**
     * get current displayers which are output.
     * @return current displayers' names,separated by "&&".
     */
    public String getOutputDisplayerStr() {
        String param = null;
        if (mDisplayService == null) {
            Log.e(TAG, "Display service  connected failed or mDisplayNum==0!");
            return param;
        }
        try {
            param = mDisplayService.getOutputDisplayerStr();
        } catch (Exception e) {
            Log.e(TAG, "Display service getOutputDisplayerStr failed", e);
        }
        return param;

    }
    /**
     * enable or disable displayers output.Displayers are set by {@link #setOutputDisplayer}.
     * @param enable if true means enable,othersize means disable.
     * @return if set is successful,return true,otherwise false.  
     */
    public boolean enableOutput(boolean enable) {
        boolean result = false;
        if (mDisplayService == null) {
            Log.e(TAG, "Display service  connected failed or mDisplayNum==0!");
            return false;
        }
        try {
            result = mDisplayService.enableOutput(enable);
        } catch (Exception e) {
            Log.e(TAG, "Display service enableOutput failed", e);
        }
        return result;

    }
    /**
     * enable or disable displayers output.Displayers are set by {@link #setOutputDisplayer}.
     * @param id displayers' name.
     * @param enable if true means enable,othersize means disable.
     * @return if set is successful,return true,otherwise false.  
     */
    public boolean enableOutput1(String id, boolean enable) {
        boolean result = false;
        if (mDisplayService == null) {
            Log.e(TAG, "Display service  connected failed or mDisplayNum==0!");
            return false;
        }
        try {
            result = mDisplayService.enableOutput1(id, enable);
        } catch (Exception e) {
            Log.e(TAG, "Display service enableOutput1 failed", e);
        }
        return result;

    }
    /**
     * set two displayers to display,which include lcd and one of the tv displayers(cvbs/hdmi).
     * @param dualOutput enalbe or disable dual output.
     * @return if set is successful,return true,otherwise false.  
     */
    public boolean setDualDisplay(boolean dualOutput) {
        boolean result = false;
        if (mDisplayService == null) {
            Log.e(TAG, "Display service  connected failed or mDisplayNum==0!");
            return false;
        }
        try {
            result = mDisplayService.setDualDisplay(dualOutput);
        } catch (Exception e) {
            Log.e(TAG, "Display service setDualDisplay failed", e);
        }

        return result;
    }
    /**
     * set displayers' parameters,which include hdmi's res,hz,aspect and cvbs's mode.
     * @param param displayers's information.
     * @return if set is successful,return true,otherwise false.  
     */
    public boolean setDisplayerParam(String param) {
        Log.e(TAG, "enter setDisplayerParam!");
        if (mDisplayService == null) {
            Log.e(TAG, "Display service  connected failed or mDisplayNum==0!");
            return false;
        }
        try {
            return mDisplayService.setDisplayerParam(param);
        } catch (Exception e) {
            Log.e(TAG, "Display service setDisplayerParam failed", e);
        }

        return false;
    }
    /**
     * get displayers' parameters,which include hdmi's res,hz,aspect and cvbs's mode.
     * @return all displayers' infomation.  
     */
    public String getDisplayerParam() {
        String param = null;
        if (mDisplayService == null) {
            Log.e(TAG, "Display service  connected failed or mDisplayNum==0!");
            return param;
        }
        try {
            param = mDisplayService.getDisplayerParam();
        } catch (Exception e) {
            Log.e(TAG, "Display service getDisplayerParam failed", e);
        }
        return param;
    }
    /**
     * get displayers' parameters,which include hdmi's res,hz,aspect and cvbs's mode.
     * @param displayer one displayers's information.
     * @return some displayer's parameters.  
     */
    public String getDisplayerParam(String displayer) {
        String param = null;
        if (mDisplayService == null) {
            Log.e(TAG, "Display service  connected failed or mDisplayNum==0!");
            return param;
        }
        try {
            param = mDisplayService.getDisplayerParamStr(displayer);
        } catch (Exception e) {
            Log.e(TAG, "Display service getDisplayerParam failed", e);
        }
        return param;
    }
    /**
     * set scale mode {@link #SCALE_FULLSCREEN} and other scale mode.
     * @param mode scale mode, including {@link #SCALE_FULLSCREEN},<br>
     * {@link #SCALE_UNIFORM} and {@link #SCALE_ORIGIN}.
     * @return if success return true,or false.
     */
    public boolean setScaleMode(String mode) {

        return setDisplayerParam(ConfigInfo.KEY_VIDEO_SCALE + "=" + mode);
    }
    
    /**
     * get scale mode {@link #SCALE_FULLSCREEN} and other scale mode.
     * {@link #SCALE_UNIFORM} and {@link #SCALE_ORIGIN}.
     * @return current scale mode.
     */
    public String getScaleMode() {

        String param = getDisplayerParam();
        if (param != null) {
            ConfigInfo cfg = new ConfigInfo();
            cfg.unflatten(param);
            return cfg.scale;
        }

        return "";
    }
    /**
     * set cvbs's format,which is ntsc or pal.
     * @param format the format which you want,"ntsc" or "pal".
     * @return if success return true,or false.
     */
    public boolean setFormat(String format) {
        return setDisplayerParam(ConfigInfo.KEY_FORMAT + "=" + format);
    }

    /**
     * set display parameter of hdmi display
     * 
     * @param width x res
     * @param height y res
     * @param hz refresh (hz)
     * @return true sucess or failed
     */
    public boolean setHdmiParam(int width, int height, float hz) {
        return setHdmiParam(width, height, hz, -1, -1);
    }
    
    /**
     * set hdmi's display parameters.
     * @param width x res.
     * @param height y res.
     * @param hz refresh rate.
     * @param pg interleave(0) or progressive(1).
     * @return if success return true,or false.
     */
    public boolean setHdmiParam(int width, int height, float hz, int pg) {
        return setHdmiParam(width, height, hz, pg, -1);
    }
    /**
     * set hdmi's display parameters.
     * @param width x res.
     * @param height y res.
     * @param hz refresh rate.
     * @param pg interleave(0) or progressive(1).
     * @param aspect tv's width/height.
     * @return if success return true,or false.
     */
    public boolean setHdmiParam(int width, int height, float hz, int pg, int aspect) {
        return setDisplayerParam(ConfigInfo.KEY_HDMI_RES_WIDTH + "=" + width + ";" + ConfigInfo.KEY_HDMI_RES_HEIGHT + "=" + height + ";"
                + ConfigInfo.KEY_HDMI_RES_HZ + "=" + hz + ";" + ConfigInfo.KEY_HDMI_RES_PG + "=" + pg + ";"
                + ConfigInfo.KEY_HDMI_RES_ASPECT + "=" + aspect);
    }

    /**
     * get hdmi's video id,equal with getHdmiParam.
     * @return hdmi's vid currently.
     */
    public int getHdmiVid() {
        Log.d(TAG, "[enter DisplayManger.getHdmiVid]\n");
	 int vid = -1;
        try {
            vid = mDisplayService.getHdmiVid();
        } catch (Exception e) {
            Log.e(TAG, "Display service getHdmiVid failed", e);
        }
        return vid;

    }
	
    /**
     * when plug in and choose hdmi disconnect, could trigger irq, 
     * in order discard these irq, set this flag
     * @param flag plug in and choose hdmi disconnect state, set true, otherwise set false
     */
    public void setHdmiDisconnectFlag(boolean flag) {
        Log.d(TAG, "[enter DisplayManger.setHdmiDisconnectFlag]\n");
        try {
            mDisplayService.setHdmiDisconnectFlag(flag);
        } catch (Exception e) {
            Log.e(TAG, "Display service setHdmiDisconnectFlag failed", e);
        }

    }
    
    /**
     * set hdmi's video id,equal with setHdmiParam.
     * @param vid hdmi video id which you want to set.
     * @return if success return true,or false.
     */
    public boolean setHdmiVid(int vid) {
        Log.d(TAG, "[enter DisplayManger.setHdmiVid]\n");
        try {
            mDisplayService.setHdmiVid(vid);
        } catch (Exception e) {
            Log.e(TAG, "Display service setHdmiVid failed", e);
        }
        return true;

    }

    /**
     * get hdmi's display parameters.
     * @param info save params of hdmi.
     * @return if success return true,or false.
     */
    public boolean getHdmiParam(DisplayParam info) {

        String param = getDisplayerParam();
        if (param != null && info != null) {
            ConfigInfo cfg = new ConfigInfo();
            cfg.unflatten(param);
            info.width = cfg.hdmiResWidth;
            info.height = cfg.hdmiResHeight;
            info.hz = cfg.hdmiResHz;
            return true;
        }

        return false;
    }
    /**
     * get hdmi's capability,including res,aspect,vedio id,audio info of tv.
     * @return information of hdmi which tv supports.
     */
    public String getHdmiCap() {
        String param = null;
        //Log.d(TAG, "[enter getHdmiCap]:" );
        //RuntimeException e = new RuntimeException("here");
        //e.fillInStackTrace();
        if (mDisplayService == null) {
            Log.e(TAG, "Display service  connected failed or mDisplayNum==0!");
            return param;
        }
        try {
            param = mDisplayService.getHdmiCap();
        } catch (Exception e) {
            Log.e(TAG, "Display service getHdmiCap failed", e);
        }
        return param;
    }

    /**
     * set display parameter of ypbpr display
     * @param width x res
     * @param height y res
     * @param hz refresh (hz)
     * @return true sucess or failed
     */
    public boolean setYpbprParam(int width, int height, float hz) {
        return setYpbprParam(width, height, hz, -1, -1);
    }
    /**
     * set ypbpr's parameters
     * @param width x res.
     * @param height y res.
     * @param hz refresh rate.
     * @param pg interleave(0) or progressive(1).
     * @return true sucess or failed.
     */
    public boolean setYpbprParam(int width, int height, float hz, int pg) {
        return setYpbprParam(width, height, hz, pg, -1);
    }
    /**
     * set ypbpr's display parameters.
     * @param width x res.
     * @param height y res.
     * @param hz refresh rate.
     * @param pg interleave(0) or progressive(1).
     * @param aspect tv's width/height.
     * @return if success return true,or false.
     */
    public boolean setYpbprParam(int width, int height, float hz, int pg, int aspect) {
        return setDisplayerParam(ConfigInfo.KEY_YPBPR_RES_WIDTH + "=" + width + ";" + ConfigInfo.KEY_YPBPR_RES_HEIGHT + "=" + height + ";"
                + ConfigInfo.KEY_YPBPR_RES_HZ + "=" + hz + ";" + ConfigInfo.KEY_YPBPR_RES_PG + "=" + pg + ";"
                + ConfigInfo.KEY_YPBPR_RES_ASPECT + "=" + aspect);
    }

    /**
     * get ypbpr's display parameters.
     * @param info save params of hdmi.
     * @return if success return true,or false.
     */
    public boolean getYpbprParam(DisplayParam info) {

        String param = getDisplayerParam();
        if (param != null && info != null) {
            ConfigInfo cfg = new ConfigInfo();
            cfg.unflatten(param);
            info.width = cfg.ypbprResWidth;
            info.height = cfg.ypbprResHeight;
            info.hz = cfg.ypbprResHz;
            return true;
        }

        return false;
    }
    
    /**
     * set tv's scale rate.
     * @param xscale x direction scale rate.
     * @param yscale y direction scale rate.
     * @return if success return true,or false.
     */
    public boolean setTvDisplayScale(int xscale, int yscale) {
        boolean result = setDisplayerParam(ConfigInfo.KEY_SCALE_X + "=" + xscale + ";" + ConfigInfo.KEY_SCALE_Y + "=" + yscale);
        return result;
    }
    
    /**
     * get tv's scale rate.
     * @param info x and y direction scale rates.
     * @return if success return true,or false.
     */
    public boolean getTvDisplayScale(ScaleInfo info) {
        String param = getDisplayerParam();
        if (param != null && info != null) {
            ConfigInfo cfg = new ConfigInfo();
            cfg.unflatten(param);
            info.scale_x = cfg.scale_x;
            info.scale_y = cfg.scale_y;
            return true;
        }

        return false;
    }
    /**
     * setColor
     * @param color color
     * @return if success return true,or false.
     */
    public boolean setColor(int color) {
        return setDisplayerParam(ConfigInfo.KEY_COLOR + "=" + color);
    }

    /**
     * setOpacity.
     * @param alpha alpha.
     * @return if success return true,or false.
     */
    public boolean setOpacity(int alpha) {
        return setDisplayerParam(ConfigInfo.KEY_ALPHA + "=" + alpha);
    }
    
    /**
     * getColor.
     * @return value.
     */
    public int getColor() {

        String param = getDisplayerParam();
        if (param != null) {
            ConfigInfo cfg = new ConfigInfo();
            cfg.unflatten(param);
            return cfg.color;
        }
        return 0;
    }
    
    /**
     * enableColorKey
     * @param enabled enable or disable.
     * @return if success return true,or false.
     */
    public boolean enableColorKey(boolean enabled) {

        return setDisplayerParam(ConfigInfo.KEY_COLORKEY + "=" + enabled);
    }

    /**
     * get hdmi and cvbs's cable state.
     * @return cable state value.
     */
    public int getCableState() {
        int state = 0;
        if (mDisplayService == null) {
            Log.e(TAG, "Display service  connected failed");
            return state;
        }
        try {
            state = mDisplayService.getCableState();
        } catch (Exception e) {
            Log.e(TAG, "Display service getCableState failed", e);
        }
        return state;
    }

    /**
     * set display mode.include 7 display modes.ref:android/device/actions/hardware/include/de.h
     * @param mode the mode which you want to set.the mode could be interpretationed to one of 3 bandth width.
     * @return if success return true,or false.
     */
    public boolean setDisplayMode(int mode) {
        Log.d(TAG, "[enter DisplayManger.setDisplayMode]\n");
        try {
            mDisplayService.setDisplayMode(mode);
        } catch (Exception e) {
            Log.e(TAG, "Display service setDisplayerMode failed", e);
        }
        return true;

    }
    /**
     * set display mode.include 7 display modes.ref:android/device/actions/hardware/include/de.h
     * @param mode the mode which you want to set.
     * @return if success return true,or false.
     */
    public boolean setDisplaySingleMode(int mode) {
        Log.d(TAG, "[enter DisplayManger.setDisplaySingleMode]\n");
        try {
            mDisplayService.setDisplaySingleMode(mode);
        } catch (Exception e) {
            Log.e(TAG, "Display service setDisplayerSingleMode failed", e);
        }
        return true;

    }
    /**
     * set status listener.
     * @param listener the listener which will be set.
     */
    public void setStatusListener(IStatusListener listener) {
        if (mDisplayService == null || listener == null) {
            Log.e(TAG, "setStatusListener invliad argument listener is null ");
            return;
        }
        try {
            mDisplayService.addStatusListener(new StatusListenerImp(listener));
        } catch (Exception e) {
            Log.e(TAG, "Display service getCableState failed", e);
        }

    }

    /**
     * this interface listen cable status.
     * @author Actions
     * @version v1.0.
     */
    public interface IStatusListener {
        /**
         * the action when cable status changes.
         * @param displayer device name.
         * @param status displayer cable status.
         */
        void onStatusChanged(String displayer, String status);

    }

    private class StatusListenerImp extends ICableStatusListener.Stub {
        private IStatusListener mListener;

        StatusListenerImp(IStatusListener listener) {
            mListener = listener;
        }

        public void onStatusChanged(String displayer, String status) {
            Log.d(TAG, "StatusListener displayer= " + displayer + "  status= " + status);

        }
    }
    
    /**
     * configure information
     * @author Actions
     * 
     */
    public class ConfigInfo extends Object {
        /**cvbs's format*/
        public String format;
        /**audio channel num*/
        public String audio_chan;
        /**scale mode*/
        public String scale;
        /***/
        public boolean colorkey;
        /***/
        public int color;
        /***/
        public int alpha;
        /**hdmi width res*/
        public int hdmiResWidth;
        /**hdmi height res*/
        public int hdmiResHeight;
        /**hdmi  refresh rate*/
        public float hdmiResHz;
        /**hdmi scan mode*/
        public int hdmiResPg;
        /**hdmi width/height,4:3 or 16:9*/
        public int hdmiResAspect;

        /**x scale rate*/
        public int scale_x;
        /**y scael rate*/
        public int scale_y;
        /**ypbpr x res*/
        public int ypbprResWidth;
        /**ypbpr y res*/
        public int ypbprResHeight;
        /**ypbpr refresh rate*/
        public float ypbprResHz;
        /**ypbpr scan mode*/
        public int ypbprResPg;
        /**ypbpr res aspect*/
        public int ypbprResAspect;
        /**
         * configure info interpratation.
         * @param flattened params which will be interprataed.
         */
        public void unflatten(String flattened) {
            StringTokenizer tokenizer = new StringTokenizer(flattened, ";");
            while (tokenizer.hasMoreElements()) {
                String kv = tokenizer.nextToken();
                int pos = kv.indexOf('=');
                if (pos == -1) {
                    continue;
                }
                String k = kv.substring(0, pos);
                String v = kv.substring(pos + 1);
                if (k.equals(KEY_FORMAT)) {
                    format = v;
                } else if (k.equals(KEY_AUDIO_CHAN)) {
                    audio_chan = v;
                } else if (k.equals(KEY_VIDEO_SCALE)) {
                    scale = v;
                } else if (k.equals(KEY_COLORKEY)) {
                    colorkey = Boolean.parseBoolean(v);
                } else if (k.equals(KEY_COLOR)) {
                    color = Integer.parseInt(v);
                } else if (k.equals(KEY_ALPHA)) {
                    alpha = Integer.parseInt(v);
                } else if (k.equals(KEY_HDMI_RES_WIDTH)) {
                    hdmiResWidth = Integer.parseInt(v);
                } else if (k.equals(KEY_HDMI_RES_HEIGHT)) {
                    hdmiResHeight = Integer.parseInt(v);
                } else if (k.equals(KEY_HDMI_RES_HZ)) {
                    hdmiResHz = Float.parseFloat(v);
                } else if (k.equals(KEY_HDMI_RES_PG)) {
                    hdmiResPg = Integer.parseInt(v);
                } else if (k.equals(KEY_HDMI_RES_ASPECT)) {
                    hdmiResAspect = Integer.parseInt(v);
                } else if (k.equals(KEY_SCALE_X)) {
                    scale_x = Integer.parseInt(v);
                } else if (k.equals(KEY_SCALE_Y)) {
                    scale_y = Integer.parseInt(v);
                } else if (k.equals(KEY_YPBPR_RES_WIDTH)) {
                    ypbprResWidth = Integer.parseInt(v);
                } else if (k.equals(KEY_YPBPR_RES_HEIGHT)) {
                    ypbprResHeight = Integer.parseInt(v);
                } else if (k.equals(KEY_YPBPR_RES_HZ)) {
                    ypbprResHz = Float.parseFloat(v);
                } else if (k.equals(KEY_YPBPR_RES_PG)) {
                    ypbprResPg = Integer.parseInt(v);
                } else if (k.equals(KEY_YPBPR_RES_ASPECT)) {
                    ypbprResAspect = Integer.parseInt(v);
                } else {
                    Log.w(TAG, "unknown display info" + k + v);
                }
            }
        }

        /*keep sync with base/services/jni/DisplayParameter.cpp*/
        /**cvbs's format*/
        public static final String KEY_FORMAT = "format";
        /**audio channel*/
        public static final String KEY_AUDIO_CHAN = "audio-chan";
        /***/
        public static final String KEY_VIDEO_SCALE = "scale";
        /***/
        public static final String KEY_COLORKEY = "colorkey";
        /***/
        public static final String KEY_COLOR = "color";
        /***/
        public static final String KEY_ALPHA = "alpha";
        /***/
        public static final String KEY_HDMI_RES_WIDTH = "hdmi-res-width";
        /***/
        public static final String KEY_HDMI_RES_HEIGHT = "hdmi-res-height";
        /***/
        public static final String KEY_HDMI_RES_HZ = "hdmi-res-hz";
        /***/
        public static final String KEY_HDMI_RES_PG = "hdmi-res-pg";
        /***/
        public static final String KEY_HDMI_RES_ASPECT = "hdmi-res-aspect";
        /***/
        public static final String KEY_SCALE_X = "scale-x";
        /***/
        public static final String KEY_SCALE_Y = "scale-y";
        /***/
        public static final String KEY_YPBPR_RES_WIDTH = "ypbpr-res-width";
        /***/
        public static final String KEY_YPBPR_RES_HEIGHT = "ypbpr-res-height";
        /***/
        public static final String KEY_YPBPR_RES_HZ = "ypbpr-res-hz";
        /***/
        public static final String KEY_YPBPR_RES_PG = "ypbpr-res-pg";
        /***/
        public static final String KEY_YPBPR_RES_ASPECT = "ypbpr-res-aspect";

    }
    
    /**
     * describe display's display param.
     * @author Actions
     *
     */
    public class DisplayerInfo extends Object {
        /**x res*/
        public int mWidth;
        /**y res*/
        public int mHeight;
        /**displayer name*/
        public String mName;
        /***/
        public String mDes;
        /***/
        public int mId;
        /**
         * interpratation for string.
         * @param flattened the string which will be interprated.
         */
        public void unflatten(String flattened) {
            StringTokenizer tokenizer = new StringTokenizer(flattened, ";");
            while (tokenizer.hasMoreElements()) {
                String kv = tokenizer.nextToken();
                int pos = kv.indexOf('=');
                if (pos == -1) {
                    continue;
                }
                String k = kv.substring(0, pos);
                String v = kv.substring(pos + 1);
                if (k.equals(KEY_WIDTH)) {
                    mWidth = Integer.parseInt(v);
                } else if (k.equals(KEY_HEIGHT)) {
                    mHeight = Integer.parseInt(v);
                } else if (k.equals(KEY_NAME)) {
                    mName = v;
                } else if (k.equals(KEY_DES)) {
                    mDes = v;
                } else if (k.equals(KEY_ID)) {
                    mId = Integer.parseInt(v);
                } else {
                    Log.w(TAG, "unknown display info" + k + v);
                }
            }
        }

        // keep sync with base/services/jni/DisplayParameter.cpp
        private static final String KEY_WIDTH = "width";
        private static final String KEY_HEIGHT = "height";
        private static final String KEY_NAME = "name";
        private static final String KEY_DES = "description";
        private static final String KEY_ID = "id";
    }
    /**
     * describe scale information
     * @author chenbo
     *
     */
    public static class ScaleInfo extends Object {
        /**x scale rate*/
        public int scale_x;
        /**y scale rate*/
        public int scale_y;
        /**max x scale rate*/
        public static final int MAX_SCALE_X = 75;
        /**max y scale rate*/
        public static final int MAX_SCALE_Y = 75;
    }
    /**
     * display parameters
     * @author Actions
     *
     */
    public static class DisplayParam extends Object {
        /**x res*/
        public int width;
        /**y res*/
        public int height;
        /**refresh rate*/
        public float hz;
    }

}
