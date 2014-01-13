.class public Lcom/actions/sensor/calib/SensorActivity;
.super Landroid/app/Activity;
.source "SensorActivity.java"

# interfaces
.implements Landroid/hardware/SensorEventListener;


# instance fields
.field private final TAG:Ljava/lang/String;

.field private mBtnCalibListener:Landroid/view/View$OnClickListener;

.field private mBtnResetListener:Landroid/view/View$OnClickListener;

.field private mButtonCalib:Landroid/widget/Button;

.field private final mButtonDelay:I

.field private mButtonReset:Landroid/widget/Button;

.field private mCalibMode:Z

.field private mHandler:Landroid/os/Handler;

.field private mSensorHost:Lcom/actions/sensor/calib/SensorHost;

.field private mTextView:Landroid/widget/TextView;

.field private sc:Lcom/actions/sensor/calib/SensorControl;

.field private sm:Landroid/hardware/SensorManager;


# direct methods
.method public constructor <init>()V
    .locals 2

    .prologue
    const/4 v1, 0x0

    .line 17
    #v1=(Null);
    invoke-direct {p0}, Landroid/app/Activity;-><init>()V

    .line 19
    #p0=(Reference);
    const-string v0, "SensorActivity"

    #v0=(Reference);
    iput-object v0, p0, Lcom/actions/sensor/calib/SensorActivity;->TAG:Ljava/lang/String;

    .line 20
    const/16 v0, 0x3e8

    #v0=(PosShort);
    iput v0, p0, Lcom/actions/sensor/calib/SensorActivity;->mButtonDelay:I

    .line 23
    iput-object v1, p0, Lcom/actions/sensor/calib/SensorActivity;->sm:Landroid/hardware/SensorManager;

    .line 24
    iput-object v1, p0, Lcom/actions/sensor/calib/SensorActivity;->sc:Lcom/actions/sensor/calib/SensorControl;

    .line 25
    const/4 v0, 0x0

    #v0=(Null);
    iput-boolean v0, p0, Lcom/actions/sensor/calib/SensorActivity;->mCalibMode:Z

    .line 32
    new-instance v0, Lcom/actions/sensor/calib/SensorActivity$1;

    #v0=(UninitRef);
    invoke-direct {v0, p0}, Lcom/actions/sensor/calib/SensorActivity$1;-><init>(Lcom/actions/sensor/calib/SensorActivity;)V

    #v0=(Reference);
    iput-object v0, p0, Lcom/actions/sensor/calib/SensorActivity;->mBtnCalibListener:Landroid/view/View$OnClickListener;

    .line 65
    new-instance v0, Lcom/actions/sensor/calib/SensorActivity$2;

    #v0=(UninitRef);
    invoke-direct {v0, p0}, Lcom/actions/sensor/calib/SensorActivity$2;-><init>(Lcom/actions/sensor/calib/SensorActivity;)V

    #v0=(Reference);
    iput-object v0, p0, Lcom/actions/sensor/calib/SensorActivity;->mBtnResetListener:Landroid/view/View$OnClickListener;

    return-void
.end method

.method static synthetic access$000(Lcom/actions/sensor/calib/SensorActivity;)Landroid/widget/Button;
    .locals 1
    .parameter "x0"

    .prologue
    .line 17
    iget-object v0, p0, Lcom/actions/sensor/calib/SensorActivity;->mButtonCalib:Landroid/widget/Button;

    #v0=(Reference);
    return-object v0
.end method

.method static synthetic access$102(Lcom/actions/sensor/calib/SensorActivity;Z)Z
    .locals 0
    .parameter "x0"
    .parameter "x1"

    .prologue
    .line 17
    iput-boolean p1, p0, Lcom/actions/sensor/calib/SensorActivity;->mCalibMode:Z

    return p1
.end method

.method static synthetic access$200(Lcom/actions/sensor/calib/SensorActivity;)Lcom/actions/sensor/calib/SensorControl;
    .locals 1
    .parameter "x0"

    .prologue
    .line 17
    iget-object v0, p0, Lcom/actions/sensor/calib/SensorActivity;->sc:Lcom/actions/sensor/calib/SensorControl;

    #v0=(Reference);
    return-object v0
.end method

.method static synthetic access$300(Lcom/actions/sensor/calib/SensorActivity;)Landroid/os/Handler;
    .locals 1
    .parameter "x0"

    .prologue
    .line 17
    iget-object v0, p0, Lcom/actions/sensor/calib/SensorActivity;->mHandler:Landroid/os/Handler;

    #v0=(Reference);
    return-object v0
.end method

.method static synthetic access$400(Lcom/actions/sensor/calib/SensorActivity;)Landroid/widget/Button;
    .locals 1
    .parameter "x0"

    .prologue
    .line 17
    iget-object v0, p0, Lcom/actions/sensor/calib/SensorActivity;->mButtonReset:Landroid/widget/Button;

    #v0=(Reference);
    return-object v0
.end method

.method private findViews()V
    .locals 1

    .prologue
    .line 96
    const/high16 v0, 0x7f04

    #v0=(Integer);
    invoke-virtual {p0, v0}, Lcom/actions/sensor/calib/SensorActivity;->findViewById(I)Landroid/view/View;

    move-result-object v0

    #v0=(Reference);
    check-cast v0, Landroid/widget/TextView;

    iput-object v0, p0, Lcom/actions/sensor/calib/SensorActivity;->mTextView:Landroid/widget/TextView;

    .line 97
    const v0, 0x7f040002

    #v0=(Integer);
    invoke-virtual {p0, v0}, Lcom/actions/sensor/calib/SensorActivity;->findViewById(I)Landroid/view/View;

    move-result-object v0

    #v0=(Reference);
    check-cast v0, Landroid/widget/Button;

    iput-object v0, p0, Lcom/actions/sensor/calib/SensorActivity;->mButtonCalib:Landroid/widget/Button;

    .line 98
    const v0, 0x7f040003

    #v0=(Integer);
    invoke-virtual {p0, v0}, Lcom/actions/sensor/calib/SensorActivity;->findViewById(I)Landroid/view/View;

    move-result-object v0

    #v0=(Reference);
    check-cast v0, Landroid/widget/Button;

    iput-object v0, p0, Lcom/actions/sensor/calib/SensorActivity;->mButtonReset:Landroid/widget/Button;

    .line 99
    const v0, 0x7f040001

    #v0=(Integer);
    invoke-virtual {p0, v0}, Lcom/actions/sensor/calib/SensorActivity;->findViewById(I)Landroid/view/View;

    move-result-object v0

    #v0=(Reference);
    check-cast v0, Lcom/actions/sensor/calib/SensorHost;

    iput-object v0, p0, Lcom/actions/sensor/calib/SensorActivity;->mSensorHost:Lcom/actions/sensor/calib/SensorHost;

    .line 100
    return-void
.end method

.method private setListensers()V
    .locals 2

    .prologue
    .line 103
    iget-object v0, p0, Lcom/actions/sensor/calib/SensorActivity;->mButtonCalib:Landroid/widget/Button;

    #v0=(Reference);
    iget-object v1, p0, Lcom/actions/sensor/calib/SensorActivity;->mBtnCalibListener:Landroid/view/View$OnClickListener;

    #v1=(Reference);
    invoke-virtual {v0, v1}, Landroid/widget/Button;->setOnClickListener(Landroid/view/View$OnClickListener;)V

    .line 104
    iget-object v0, p0, Lcom/actions/sensor/calib/SensorActivity;->mButtonReset:Landroid/widget/Button;

    iget-object v1, p0, Lcom/actions/sensor/calib/SensorActivity;->mBtnResetListener:Landroid/view/View$OnClickListener;

    invoke-virtual {v0, v1}, Landroid/widget/Button;->setOnClickListener(Landroid/view/View$OnClickListener;)V

    .line 105
    return-void
.end method


# virtual methods
.method public onAccuracyChanged(Landroid/hardware/Sensor;I)V
    .locals 0
    .parameter "paramSensor"
    .parameter "paramInt"

    .prologue
    .line 139
    return-void
.end method

.method public onCreate(Landroid/os/Bundle;)V
    .locals 1
    .parameter "paramBundle"

    .prologue
    .line 108
    invoke-super {p0, p1}, Landroid/app/Activity;->onCreate(Landroid/os/Bundle;)V

    .line 109
    const/high16 v0, 0x7f03

    #v0=(Integer);
    invoke-virtual {p0, v0}, Lcom/actions/sensor/calib/SensorActivity;->setContentView(I)V

    .line 110
    invoke-direct {p0}, Lcom/actions/sensor/calib/SensorActivity;->findViews()V

    .line 112
    new-instance v0, Landroid/os/Handler;

    #v0=(UninitRef);
    invoke-direct {v0}, Landroid/os/Handler;-><init>()V

    #v0=(Reference);
    iput-object v0, p0, Lcom/actions/sensor/calib/SensorActivity;->mHandler:Landroid/os/Handler;

    .line 113
    const-string v0, "sensor"

    invoke-virtual {p0, v0}, Lcom/actions/sensor/calib/SensorActivity;->getSystemService(Ljava/lang/String;)Ljava/lang/Object;

    move-result-object v0

    check-cast v0, Landroid/hardware/SensorManager;

    iput-object v0, p0, Lcom/actions/sensor/calib/SensorActivity;->sm:Landroid/hardware/SensorManager;

    .line 114
    new-instance v0, Lcom/actions/sensor/calib/SensorControl;

    #v0=(UninitRef);
    invoke-direct {v0, p0}, Lcom/actions/sensor/calib/SensorControl;-><init>(Landroid/content/Context;)V

    #v0=(Reference);
    iput-object v0, p0, Lcom/actions/sensor/calib/SensorActivity;->sc:Lcom/actions/sensor/calib/SensorControl;

    .line 116
    invoke-direct {p0}, Lcom/actions/sensor/calib/SensorActivity;->setListensers()V

    .line 117
    return-void
.end method

.method protected onDestroy()V
    .locals 1

    .prologue
    const/4 v0, 0x0

    .line 120
    #v0=(Null);
    invoke-super {p0}, Landroid/app/Activity;->onDestroy()V

    .line 121
    iput-object v0, p0, Lcom/actions/sensor/calib/SensorActivity;->mHandler:Landroid/os/Handler;

    .line 122
    iput-object v0, p0, Lcom/actions/sensor/calib/SensorActivity;->sm:Landroid/hardware/SensorManager;

    .line 123
    iput-object v0, p0, Lcom/actions/sensor/calib/SensorActivity;->sc:Lcom/actions/sensor/calib/SensorControl;

    .line 124
    return-void
.end method

.method protected onPause()V
    .locals 1

    .prologue
    .line 127
    invoke-super {p0}, Landroid/app/Activity;->onPause()V

    .line 128
    iget-object v0, p0, Lcom/actions/sensor/calib/SensorActivity;->sm:Landroid/hardware/SensorManager;

    #v0=(Reference);
    invoke-virtual {v0, p0}, Landroid/hardware/SensorManager;->unregisterListener(Landroid/hardware/SensorEventListener;)V

    .line 129
    return-void
.end method

.method protected onResume()V
    .locals 3

    .prologue
    .line 132
    invoke-super {p0}, Landroid/app/Activity;->onResume()V

    .line 133
    iget-object v0, p0, Lcom/actions/sensor/calib/SensorActivity;->sm:Landroid/hardware/SensorManager;

    #v0=(Reference);
    iget-object v1, p0, Lcom/actions/sensor/calib/SensorActivity;->sm:Landroid/hardware/SensorManager;

    #v1=(Reference);
    const/4 v2, 0x1

    #v2=(One);
    invoke-virtual {v1, v2}, Landroid/hardware/SensorManager;->getDefaultSensor(I)Landroid/hardware/Sensor;

    move-result-object v1

    const/4 v2, 0x3

    #v2=(PosByte);
    invoke-virtual {v0, p0, v1, v2}, Landroid/hardware/SensorManager;->registerListener(Landroid/hardware/SensorEventListener;Landroid/hardware/Sensor;I)Z

    .line 136
    return-void
.end method

.method public onSensorChanged(Landroid/hardware/SensorEvent;)V
    .locals 7
    .parameter "e"

    .prologue
    const/4 v2, 0x3

    #v2=(PosByte);
    const/4 v6, 0x2

    #v6=(PosByte);
    const/4 v5, 0x1

    #v5=(One);
    const/4 v4, 0x0

    .line 142
    #v4=(Null);
    if-eqz p1, :cond_0

    iget-object v1, p1, Landroid/hardware/SensorEvent;->values:[F

    #v1=(Reference);
    array-length v1, v1

    #v1=(Integer);
    if-ne v1, v2, :cond_0

    .line 144
    iget-boolean v1, p0, Lcom/actions/sensor/calib/SensorActivity;->mCalibMode:Z

    #v1=(Boolean);
    if-eqz v1, :cond_1

    .line 156
    :cond_0
    :goto_0
    #v0=(Conflicted);v1=(Conflicted);v2=(Integer);v3=(Conflicted);
    return-void

    .line 147
    :cond_1
    #v0=(Uninit);v1=(Boolean);v2=(PosByte);v3=(Uninit);
    iget-object v1, p0, Lcom/actions/sensor/calib/SensorActivity;->mTextView:Landroid/widget/TextView;

    #v1=(Reference);
    if-eqz v1, :cond_2

    .line 148
    const-string v1, "X: %.3f, Y: %.3f, Z: %.3f"

    new-array v2, v2, [Ljava/lang/Object;

    #v2=(Reference);
    iget-object v3, p1, Landroid/hardware/SensorEvent;->values:[F

    #v3=(Reference);
    aget v3, v3, v4

    #v3=(Integer);
    invoke-static {v3}, Ljava/lang/Float;->valueOf(F)Ljava/lang/Float;

    move-result-object v3

    #v3=(Reference);
    aput-object v3, v2, v4

    iget-object v3, p1, Landroid/hardware/SensorEvent;->values:[F

    aget v3, v3, v5

    #v3=(Integer);
    invoke-static {v3}, Ljava/lang/Float;->valueOf(F)Ljava/lang/Float;

    move-result-object v3

    #v3=(Reference);
    aput-object v3, v2, v5

    iget-object v3, p1, Landroid/hardware/SensorEvent;->values:[F

    aget v3, v3, v6

    #v3=(Integer);
    invoke-static {v3}, Ljava/lang/Float;->valueOf(F)Ljava/lang/Float;

    move-result-object v3

    #v3=(Reference);
    aput-object v3, v2, v6

    invoke-static {v1, v2}, Ljava/lang/String;->format(Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/String;

    move-result-object v0

    .line 150
    .local v0, txt:Ljava/lang/String;
    #v0=(Reference);
    iget-object v1, p0, Lcom/actions/sensor/calib/SensorActivity;->mTextView:Landroid/widget/TextView;

    invoke-virtual {v1, v0}, Landroid/widget/TextView;->setText(Ljava/lang/CharSequence;)V

    .line 151
    iget-object v1, p0, Lcom/actions/sensor/calib/SensorActivity;->mTextView:Landroid/widget/TextView;

    const v2, -0xff0001

    #v2=(Integer);
    invoke-virtual {v1, v2}, Landroid/widget/TextView;->setTextColor(I)V

    .line 153
    .end local v0           #txt:Ljava/lang/String;
    :cond_2
    #v0=(Conflicted);v3=(Conflicted);
    iget-object v1, p0, Lcom/actions/sensor/calib/SensorActivity;->mSensorHost:Lcom/actions/sensor/calib/SensorHost;

    if-eqz v1, :cond_0

    .line 154
    iget-object v1, p0, Lcom/actions/sensor/calib/SensorActivity;->mSensorHost:Lcom/actions/sensor/calib/SensorHost;

    invoke-virtual {v1, p1}, Lcom/actions/sensor/calib/SensorHost;->onSensorChanged(Landroid/hardware/SensorEvent;)V

    goto :goto_0
.end method
