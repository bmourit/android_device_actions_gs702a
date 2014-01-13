package com.actions.sensor.calib;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Paint.Style;
import android.graphics.RectF;
import android.hardware.SensorEvent;
import android.util.AttributeSet;
import android.view.Display;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.MeasureSpec;
import android.view.View.OnTouchListener;
import android.view.WindowManager;

public class SensorHost extends View
implements View.OnTouchListener {
  public final String TAG = "SensorHost";
  Display mDisplay = null;
  RectF mOval = null;
  Paint mPaint = null;
  float vX = 0.0F;
  float vY = 0.0F;
  float vZ = 0.0F;

  public SensorHost(Context paramContext, AttributeSet paramAttributeSet) {
    this(paramContext, paramAttributeSet, 0);
  }

  public SensorHost(Context paramContext, AttributeSet paramAttributeSet, int paramInt) {
    super(paramContext, paramAttributeSet, paramInt);
    setOnTouchListener(this);
    this.mDisplay = ((WindowManager)paramContext.getSystemService("window")).getDefaultDisplay();
    this.mPaint = new Paint();
    this.mOval = new RectF();
  }

  public void onDraw(Canvas paramCanvas) {
    super.onDraw(paramCanvas);
    int i = getWidth();
    int j = getHeight();
    paramCanvas.translate(i / 2, j / 2);
    this.mPaint.setAntiAlias(true);
    this.mPaint.setColor(-1);
    this.mPaint.setStyle(Paint.Style.STROKE);
    this.mPaint.setAlpha(255);
    int k = Math.min(i, j);
    this.mOval.left = (-k / 2);
    this.mOval.top = (-k / 2);
    this.mOval.right = (k / 2);
    this.mOval.bottom = (k / 2);
    paramCanvas.drawArc(this.mOval, 0.0F, 360.0F, false, this.mPaint);
    paramCanvas.drawLine(-k / 2, 0.0F, k / 2, 0.0F, this.mPaint);
    paramCanvas.drawLine(0.0F, -k / 2, 0.0F, k / 2, this.mPaint);
    int m = 90 * this.mDisplay.getRotation();
    if (m == 90) {
      float f2 = this.vX;
      this.vX = (-this.vY);
      this.vY = f2;
    }
    if (m == 180) {
      this.vX = (-this.vX);
      this.vY = (-this.vY);
    }
    if (m == 270) {
      float f1 = this.vX;
      this.vX = this.vY;
      this.vY = (-f1);
    }
    this.mPaint.setStyle(Paint.Style.FILL);
    paramCanvas.drawCircle(-i / 2 * this.vX / 10.0F, j / 2 * this.vY / 10.0F, 20.0F - this.vZ, this.mPaint);
  }

  protected void onMeasure(int paramInt1, int paramInt2) {
    int i = View.MeasureSpec.getSize(paramInt2);
    int j = 4 * Math.min(View.MeasureSpec.getSize(paramInt1), i) / 5;
    setMeasuredDimension(j, j);
  }

  public void onSensorChanged(SensorEvent paramSensorEvent) {
    if (paramSensorEvent.values.length == 3) {
      this.vX = paramSensorEvent.values[0];
      this.vY = paramSensorEvent.values[1];
      this.vZ = paramSensorEvent.values[2];
      invalidate();
    }
  }

  public boolean onTouch(View paramView, MotionEvent paramMotionEvent) {
    return false;
  }
}
