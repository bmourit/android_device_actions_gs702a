package com.actions.sensor.calib;

import android.content.Context;
import android.util.Log;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;

public class SensorControl {
private final String CALIBFILE = "gsensor_calib.txt";
  public final String CALIB_RESET_ATTR = "calibration_reset";
  public final String CALIB_RUN_ATTR = "calibration_run";
  public final String CALIB_VAL_ATTR = "calibration_value";
  private final String INPUT_DIR = "/sys/class/input";
  private final String SENSORNAME = "bma220,bma222,bma250,mma7660,mma8452,dmard06,dmard10,mc3210,mc3230,mc6420,bmc050,lis3dh";
  public final String TAG = "SensorControl";
  private String classPath = null;
  private String dataPath = null;

  public SensorControl(Context paramContext) {
    this.dataPath = paramContext.getFilesDir().getAbsolutePath();
  }

  private String getClassPath() {
    File[] arrayOfFile;
    if (this.classPath == null)
    arrayOfFile = new File("/sys/class/input").listFiles();
    for (int i = 0; ; i++)
    if (i < arrayOfFile.length) {
      if ((arrayOfFile[i].isDirectory()) && (arrayOfFile[i].getName().contains("input"))) {
        String str = readFile(arrayOfFile[i].getAbsolutePath() + "/name");
        if ((str != null) && ("bma220,bma222,bma250,mma7660,mma8452,dmard06,dmard10,mc3210,mc3230,mc6420,bmc050,lis3dh".contains(str.trim()))) {
          this.classPath = arrayOfFile[i].getAbsolutePath();
          Log.i("SensorControl", "classPath: " + this.classPath);
        }
      }
    }
    else
    return this.classPath;
  }

  private String getDataPath(String paramString) {
    return this.dataPath + "/" + paramString;
  }

  private String getDevPath(String paramString) {
    return getClassPath() + "/" + paramString;
  }

  private String readFile(String paramString) {
    byte[] arrayOfByte = null;
    try {
      FileInputStream localFileInputStream = new FileInputStream(paramString);
      arrayOfByte = new byte[localFileInputStream.available()];
      localFileInputStream.read(arrayOfByte);
      localFileInputStream.close();
      return new String(arrayOfByte);
    }
    catch (Exception localException) {
      while (true) {
        Log.e("SensorControl", "read " + paramString + " error!");
        localException.printStackTrace();
      }
    }
  }

  private void writeFile(String paramString1, String paramString2) {
    try {
      FileOutputStream localFileOutputStream = new FileOutputStream(paramString1);
      localFileOutputStream.write(paramString2.getBytes());
      localFileOutputStream.close();
      return;
    }
    catch (Exception localException) {
      Log.e("SensorControl", "write " + paramString1 + " error!");
      localException.printStackTrace();
    }
  }

  public String getCalibValue() {
    return readFile(getDevPath("calibration_value")).trim();
  }

  public void resetCalib() {
    writeFile(getDevPath("calibration_reset"), "1");
  }

  public void runCalib() {
    writeFile(getDevPath("calibration_run"), "1");
  }

  public void writeCalibFile(String paramString) {
    writeFile(getDataPath("gsensor_calib.txt"), paramString);
  }
}
