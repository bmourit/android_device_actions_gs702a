.class Lcom/actions/sensor/calib/SensorActivity$2;
.super Ljava/lang/Object;
.source "SensorActivity.java"

# interfaces
.implements Landroid/view/View$OnClickListener;


# annotations
.annotation system Ldalvik/annotation/EnclosingClass;
    value = Lcom/actions/sensor/calib/SensorActivity;
.end annotation

.annotation system Ldalvik/annotation/InnerClass;
    accessFlags = 0x0
    name = null
.end annotation


# instance fields
.field final synthetic this$0:Lcom/actions/sensor/calib/SensorActivity;


# direct methods
.method constructor <init>(Lcom/actions/sensor/calib/SensorActivity;)V
    .locals 0
    .parameter

    .prologue
    .line 65
    iput-object p1, p0, Lcom/actions/sensor/calib/SensorActivity$2;->this$0:Lcom/actions/sensor/calib/SensorActivity;

    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    #p0=(Reference);
    return-void
.end method


# virtual methods
.method public onClick(Landroid/view/View;)V
    .locals 4
    .parameter "v"

    .prologue
    .line 68
    iget-object v0, p0, Lcom/actions/sensor/calib/SensorActivity$2;->this$0:Lcom/actions/sensor/calib/SensorActivity;

    #v0=(Reference);
    invoke-static {v0}, Lcom/actions/sensor/calib/SensorActivity;->access$400(Lcom/actions/sensor/calib/SensorActivity;)Landroid/widget/Button;

    move-result-object v0

    const/4 v1, 0x0

    #v1=(Null);
    invoke-virtual {v0, v1}, Landroid/widget/Button;->setClickable(Z)V

    .line 69
    iget-object v0, p0, Lcom/actions/sensor/calib/SensorActivity$2;->this$0:Lcom/actions/sensor/calib/SensorActivity;

    const/4 v1, 0x1

    #v1=(One);
    invoke-static {v0, v1}, Lcom/actions/sensor/calib/SensorActivity;->access$102(Lcom/actions/sensor/calib/SensorActivity;Z)Z

    .line 72
    iget-object v0, p0, Lcom/actions/sensor/calib/SensorActivity$2;->this$0:Lcom/actions/sensor/calib/SensorActivity;

    invoke-static {v0}, Lcom/actions/sensor/calib/SensorActivity;->access$200(Lcom/actions/sensor/calib/SensorActivity;)Lcom/actions/sensor/calib/SensorControl;

    move-result-object v0

    invoke-virtual {v0}, Lcom/actions/sensor/calib/SensorControl;->resetCalib()V

    .line 74
    iget-object v0, p0, Lcom/actions/sensor/calib/SensorActivity$2;->this$0:Lcom/actions/sensor/calib/SensorActivity;

    invoke-static {v0}, Lcom/actions/sensor/calib/SensorActivity;->access$300(Lcom/actions/sensor/calib/SensorActivity;)Landroid/os/Handler;

    move-result-object v0

    new-instance v1, Lcom/actions/sensor/calib/SensorActivity$2$1;

    #v1=(UninitRef);
    invoke-direct {v1, p0}, Lcom/actions/sensor/calib/SensorActivity$2$1;-><init>(Lcom/actions/sensor/calib/SensorActivity$2;)V

    #v1=(Reference);
    const-wide/16 v2, 0x3e8

    #v2=(LongLo);v3=(LongHi);
    invoke-virtual {v0, v1, v2, v3}, Landroid/os/Handler;->postDelayed(Ljava/lang/Runnable;J)Z

    .line 92
    return-void
.end method
