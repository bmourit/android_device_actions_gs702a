.class public Lcom/actions/sensor/calib/SensorHost;
.super Landroid/view/View;
.source "SensorHost.java"

# interfaces
.implements Landroid/view/View$OnTouchListener;


# instance fields
.field public final TAG:Ljava/lang/String;

.field mDisplay:Landroid/view/Display;

.field mOval:Landroid/graphics/RectF;

.field mPaint:Landroid/graphics/Paint;

.field vX:F

.field vY:F

.field vZ:F


# direct methods
.method public constructor <init>(Landroid/content/Context;Landroid/util/AttributeSet;)V
    .locals 1
    .parameter "ctx"
    .parameter "attrs"

    .prologue
    .line 27
    const/4 v0, 0x0

    #v0=(Null);
    invoke-direct {p0, p1, p2, v0}, Lcom/actions/sensor/calib/SensorHost;-><init>(Landroid/content/Context;Landroid/util/AttributeSet;I)V

    .line 28
    #p0=(Reference);
    return-void
.end method

.method public constructor <init>(Landroid/content/Context;Landroid/util/AttributeSet;I)V
    .locals 4
    .parameter "ctx"
    .parameter "attrs"
    .parameter "param"

    .prologue
    const/4 v3, 0x0

    #v3=(Null);
    const/4 v2, 0x0

    .line 31
    #v2=(Null);
    invoke-direct {p0, p1, p2, p3}, Landroid/view/View;-><init>(Landroid/content/Context;Landroid/util/AttributeSet;I)V

    .line 17
    #p0=(Reference);
    const-string v1, "SensorHost"

    #v1=(Reference);
    iput-object v1, p0, Lcom/actions/sensor/calib/SensorHost;->TAG:Ljava/lang/String;

    .line 19
    iput v2, p0, Lcom/actions/sensor/calib/SensorHost;->vX:F

    .line 20
    iput v2, p0, Lcom/actions/sensor/calib/SensorHost;->vY:F

    .line 21
    iput v2, p0, Lcom/actions/sensor/calib/SensorHost;->vZ:F

    .line 22
    iput-object v3, p0, Lcom/actions/sensor/calib/SensorHost;->mDisplay:Landroid/view/Display;

    .line 23
    iput-object v3, p0, Lcom/actions/sensor/calib/SensorHost;->mPaint:Landroid/graphics/Paint;

    .line 24
    iput-object v3, p0, Lcom/actions/sensor/calib/SensorHost;->mOval:Landroid/graphics/RectF;

    .line 32
    invoke-virtual {p0, p0}, Lcom/actions/sensor/calib/SensorHost;->setOnTouchListener(Landroid/view/View$OnTouchListener;)V

    .line 34
    const-string v1, "window"

    invoke-virtual {p1, v1}, Landroid/content/Context;->getSystemService(Ljava/lang/String;)Ljava/lang/Object;

    move-result-object v0

    #v0=(Reference);
    check-cast v0, Landroid/view/WindowManager;

    .line 36
    .local v0, wm:Landroid/view/WindowManager;
    invoke-interface {v0}, Landroid/view/WindowManager;->getDefaultDisplay()Landroid/view/Display;

    move-result-object v1

    iput-object v1, p0, Lcom/actions/sensor/calib/SensorHost;->mDisplay:Landroid/view/Display;

    .line 38
    new-instance v1, Landroid/graphics/Paint;

    #v1=(UninitRef);
    invoke-direct {v1}, Landroid/graphics/Paint;-><init>()V

    #v1=(Reference);
    iput-object v1, p0, Lcom/actions/sensor/calib/SensorHost;->mPaint:Landroid/graphics/Paint;

    .line 39
    new-instance v1, Landroid/graphics/RectF;

    #v1=(UninitRef);
    invoke-direct {v1}, Landroid/graphics/RectF;-><init>()V

    #v1=(Reference);
    iput-object v1, p0, Lcom/actions/sensor/calib/SensorHost;->mOval:Landroid/graphics/RectF;

    .line 40
    return-void
.end method


# virtual methods
.method public onDraw(Landroid/graphics/Canvas;)V
    .locals 13
    .parameter "c"

    .prologue
    const/high16 v12, 0x4120

    #v12=(Integer);
    const/4 v2, 0x0

    .line 50
    #v2=(Null);
    invoke-super {p0, p1}, Landroid/view/View;->onDraw(Landroid/graphics/Canvas;)V

    .line 52
    invoke-virtual {p0}, Lcom/actions/sensor/calib/SensorHost;->getWidth()I

    move-result v11

    .line 53
    .local v11, w:I
    #v11=(Integer);
    invoke-virtual {p0}, Lcom/actions/sensor/calib/SensorHost;->getHeight()I

    move-result v8

    .line 55
    .local v8, h:I
    #v8=(Integer);
    div-int/lit8 v0, v11, 0x2

    #v0=(Integer);
    int-to-float v0, v0

    #v0=(Float);
    div-int/lit8 v1, v8, 0x2

    #v1=(Integer);
    int-to-float v1, v1

    #v1=(Float);
    invoke-virtual {p1, v0, v1}, Landroid/graphics/Canvas;->translate(FF)V

    .line 57
    iget-object v0, p0, Lcom/actions/sensor/calib/SensorHost;->mPaint:Landroid/graphics/Paint;

    #v0=(Reference);
    const/4 v1, 0x1

    #v1=(One);
    invoke-virtual {v0, v1}, Landroid/graphics/Paint;->setAntiAlias(Z)V

    .line 58
    iget-object v0, p0, Lcom/actions/sensor/calib/SensorHost;->mPaint:Landroid/graphics/Paint;

    const/4 v1, -0x1

    #v1=(Byte);
    invoke-virtual {v0, v1}, Landroid/graphics/Paint;->setColor(I)V

    .line 59
    iget-object v0, p0, Lcom/actions/sensor/calib/SensorHost;->mPaint:Landroid/graphics/Paint;

    sget-object v1, Landroid/graphics/Paint$Style;->STROKE:Landroid/graphics/Paint$Style;

    #v1=(Reference);
    invoke-virtual {v0, v1}, Landroid/graphics/Paint;->setStyle(Landroid/graphics/Paint$Style;)V

    .line 60
    iget-object v0, p0, Lcom/actions/sensor/calib/SensorHost;->mPaint:Landroid/graphics/Paint;

    const/16 v1, 0xff

    #v1=(PosShort);
    invoke-virtual {v0, v1}, Landroid/graphics/Paint;->setAlpha(I)V

    .line 62
    invoke-static {v11, v8}, Ljava/lang/Math;->min(II)I

    move-result v10

    .line 63
    .local v10, r:I
    #v10=(Integer);
    iget-object v0, p0, Lcom/actions/sensor/calib/SensorHost;->mOval:Landroid/graphics/RectF;

    neg-int v1, v10

    #v1=(Integer);
    div-int/lit8 v1, v1, 0x2

    int-to-float v1, v1

    #v1=(Float);
    iput v1, v0, Landroid/graphics/RectF;->left:F

    .line 64
    iget-object v0, p0, Lcom/actions/sensor/calib/SensorHost;->mOval:Landroid/graphics/RectF;

    neg-int v1, v10

    #v1=(Integer);
    div-int/lit8 v1, v1, 0x2

    int-to-float v1, v1

    #v1=(Float);
    iput v1, v0, Landroid/graphics/RectF;->top:F

    .line 65
    iget-object v0, p0, Lcom/actions/sensor/calib/SensorHost;->mOval:Landroid/graphics/RectF;

    div-int/lit8 v1, v10, 0x2

    #v1=(Integer);
    int-to-float v1, v1

    #v1=(Float);
    iput v1, v0, Landroid/graphics/RectF;->right:F

    .line 66
    iget-object v0, p0, Lcom/actions/sensor/calib/SensorHost;->mOval:Landroid/graphics/RectF;

    div-int/lit8 v1, v10, 0x2

    #v1=(Integer);
    int-to-float v1, v1

    #v1=(Float);
    iput v1, v0, Landroid/graphics/RectF;->bottom:F

    .line 68
    iget-object v1, p0, Lcom/actions/sensor/calib/SensorHost;->mOval:Landroid/graphics/RectF;

    #v1=(Reference);
    const/high16 v3, 0x43b4

    #v3=(Integer);
    const/4 v4, 0x0

    #v4=(Null);
    iget-object v5, p0, Lcom/actions/sensor/calib/SensorHost;->mPaint:Landroid/graphics/Paint;

    #v5=(Reference);
    move-object v0, p1

    invoke-virtual/range {v0 .. v5}, Landroid/graphics/Canvas;->drawArc(Landroid/graphics/RectF;FFZLandroid/graphics/Paint;)V

    .line 69
    neg-int v0, v10

    #v0=(Integer);
    div-int/lit8 v0, v0, 0x2

    int-to-float v1, v0

    #v1=(Float);
    div-int/lit8 v0, v10, 0x2

    int-to-float v3, v0

    #v3=(Float);
    iget-object v5, p0, Lcom/actions/sensor/calib/SensorHost;->mPaint:Landroid/graphics/Paint;

    move-object v0, p1

    #v0=(Reference);
    move v4, v2

    invoke-virtual/range {v0 .. v5}, Landroid/graphics/Canvas;->drawLine(FFFFLandroid/graphics/Paint;)V

    .line 70
    neg-int v0, v10

    #v0=(Integer);
    div-int/lit8 v0, v0, 0x2

    int-to-float v3, v0

    div-int/lit8 v0, v10, 0x2

    int-to-float v5, v0

    #v5=(Float);
    iget-object v6, p0, Lcom/actions/sensor/calib/SensorHost;->mPaint:Landroid/graphics/Paint;

    #v6=(Reference);
    move-object v1, p1

    #v1=(Reference);
    move v4, v2

    invoke-virtual/range {v1 .. v6}, Landroid/graphics/Canvas;->drawLine(FFFFLandroid/graphics/Paint;)V

    .line 72
    iget-object v0, p0, Lcom/actions/sensor/calib/SensorHost;->mDisplay:Landroid/view/Display;

    #v0=(Reference);
    invoke-virtual {v0}, Landroid/view/Display;->getRotation()I

    move-result v0

    #v0=(Integer);
    mul-int/lit8 v9, v0, 0x5a

    .line 74
    .local v9, k:I
    #v9=(Integer);
    const/16 v0, 0x5a

    #v0=(PosByte);
    if-ne v9, v0, :cond_0

    .line 75
    iget v7, p0, Lcom/actions/sensor/calib/SensorHost;->vX:F

    .line 76
    .local v7, f2:F
    #v7=(Integer);
    iget v0, p0, Lcom/actions/sensor/calib/SensorHost;->vY:F

    #v0=(Integer);
    neg-float v0, v0

    #v0=(Float);
    iput v0, p0, Lcom/actions/sensor/calib/SensorHost;->vX:F

    .line 77
    iput v7, p0, Lcom/actions/sensor/calib/SensorHost;->vY:F

    .line 79
    .end local v7           #f2:F
    :cond_0
    #v7=(Conflicted);
    const/16 v0, 0xb4

    #v0=(PosShort);
    if-ne v9, v0, :cond_1

    .line 80
    iget v0, p0, Lcom/actions/sensor/calib/SensorHost;->vX:F

    #v0=(Integer);
    neg-float v0, v0

    #v0=(Float);
    iput v0, p0, Lcom/actions/sensor/calib/SensorHost;->vX:F

    .line 81
    iget v0, p0, Lcom/actions/sensor/calib/SensorHost;->vY:F

    #v0=(Integer);
    neg-float v0, v0

    #v0=(Float);
    iput v0, p0, Lcom/actions/sensor/calib/SensorHost;->vY:F

    .line 83
    :cond_1
    const/16 v0, 0x10e

    #v0=(PosShort);
    if-ne v9, v0, :cond_2

    .line 84
    iget v7, p0, Lcom/actions/sensor/calib/SensorHost;->vX:F

    .line 85
    .restart local v7       #f2:F
    #v7=(Integer);
    iget v0, p0, Lcom/actions/sensor/calib/SensorHost;->vY:F

    #v0=(Integer);
    iput v0, p0, Lcom/actions/sensor/calib/SensorHost;->vX:F

    .line 86
    neg-float v0, v7

    #v0=(Float);
    iput v0, p0, Lcom/actions/sensor/calib/SensorHost;->vY:F

    .line 89
    .end local v7           #f2:F
    :cond_2
    #v7=(Conflicted);
    iget-object v0, p0, Lcom/actions/sensor/calib/SensorHost;->mPaint:Landroid/graphics/Paint;

    #v0=(Reference);
    sget-object v1, Landroid/graphics/Paint$Style;->FILL:Landroid/graphics/Paint$Style;

    invoke-virtual {v0, v1}, Landroid/graphics/Paint;->setStyle(Landroid/graphics/Paint$Style;)V

    .line 90
    neg-int v0, v11

    #v0=(Integer);
    div-int/lit8 v0, v0, 0x2

    int-to-float v0, v0

    #v0=(Float);
    iget v1, p0, Lcom/actions/sensor/calib/SensorHost;->vX:F

    #v1=(Integer);
    mul-float/2addr v0, v1

    div-float/2addr v0, v12

    div-int/lit8 v1, v8, 0x2

    int-to-float v1, v1

    #v1=(Float);
    iget v2, p0, Lcom/actions/sensor/calib/SensorHost;->vY:F

    #v2=(Integer);
    mul-float/2addr v1, v2

    div-float/2addr v1, v12

    const/high16 v2, 0x41a0

    iget v3, p0, Lcom/actions/sensor/calib/SensorHost;->vZ:F

    #v3=(Integer);
    sub-float/2addr v2, v3

    #v2=(Float);
    iget-object v3, p0, Lcom/actions/sensor/calib/SensorHost;->mPaint:Landroid/graphics/Paint;

    #v3=(Reference);
    invoke-virtual {p1, v0, v1, v2, v3}, Landroid/graphics/Canvas;->drawCircle(FFFLandroid/graphics/Paint;)V

    .line 92
    return-void
.end method

.method protected onMeasure(II)V
    .locals 4
    .parameter "widthMeasureSpec"
    .parameter "heightMeasureSpec"

    .prologue
    .line 43
    invoke-static {p2}, Landroid/view/View$MeasureSpec;->getSize(I)I

    move-result v0

    .line 44
    .local v0, h:I
    #v0=(Integer);
    invoke-static {p1}, Landroid/view/View$MeasureSpec;->getSize(I)I

    move-result v2

    .line 45
    .local v2, w:I
    #v2=(Integer);
    invoke-static {v2, v0}, Ljava/lang/Math;->min(II)I

    move-result v3

    #v3=(Integer);
    mul-int/lit8 v3, v3, 0x4

    div-int/lit8 v1, v3, 0x5

    .line 46
    .local v1, r:I
    #v1=(Integer);
    invoke-virtual {p0, v1, v1}, Lcom/actions/sensor/calib/SensorHost;->setMeasuredDimension(II)V

    .line 47
    return-void
.end method

.method public onSensorChanged(Landroid/hardware/SensorEvent;)V
    .locals 2
    .parameter "e"

    .prologue
    .line 95
    iget-object v0, p1, Landroid/hardware/SensorEvent;->values:[F

    #v0=(Reference);
    array-length v0, v0

    #v0=(Integer);
    const/4 v1, 0x3

    #v1=(PosByte);
    if-ne v0, v1, :cond_0

    .line 96
    iget-object v0, p1, Landroid/hardware/SensorEvent;->values:[F

    #v0=(Reference);
    const/4 v1, 0x0

    #v1=(Null);
    aget v0, v0, v1

    #v0=(Integer);
    iput v0, p0, Lcom/actions/sensor/calib/SensorHost;->vX:F

    .line 97
    iget-object v0, p1, Landroid/hardware/SensorEvent;->values:[F

    #v0=(Reference);
    const/4 v1, 0x1

    #v1=(One);
    aget v0, v0, v1

    #v0=(Integer);
    iput v0, p0, Lcom/actions/sensor/calib/SensorHost;->vY:F

    .line 98
    iget-object v0, p1, Landroid/hardware/SensorEvent;->values:[F

    #v0=(Reference);
    const/4 v1, 0x2

    #v1=(PosByte);
    aget v0, v0, v1

    #v0=(Integer);
    iput v0, p0, Lcom/actions/sensor/calib/SensorHost;->vZ:F

    .line 99
    invoke-virtual {p0}, Lcom/actions/sensor/calib/SensorHost;->invalidate()V

    .line 101
    :cond_0
    return-void
.end method

.method public onTouch(Landroid/view/View;Landroid/view/MotionEvent;)Z
    .locals 1
    .parameter "paramView"
    .parameter "paramMotionEvent"

    .prologue
    .line 104
    const/4 v0, 0x0

    #v0=(Null);
    return v0
.end method
