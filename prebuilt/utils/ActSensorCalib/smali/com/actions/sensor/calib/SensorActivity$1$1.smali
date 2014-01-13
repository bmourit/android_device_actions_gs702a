.class Lcom/actions/sensor/calib/SensorActivity$1$1;
.super Ljava/lang/Object;
.source "SensorActivity.java"

# interfaces
.implements Ljava/lang/Runnable;


# annotations
.annotation system Ldalvik/annotation/EnclosingMethod;
    value = Lcom/actions/sensor/calib/SensorActivity$1;->onClick(Landroid/view/View;)V
.end annotation

.annotation system Ldalvik/annotation/InnerClass;
    accessFlags = 0x0
    name = null
.end annotation


# instance fields
.field final synthetic this$1:Lcom/actions/sensor/calib/SensorActivity$1;


# direct methods
.method constructor <init>(Lcom/actions/sensor/calib/SensorActivity$1;)V
    .locals 0
    .parameter

    .prologue
    .line 41
    iput-object p1, p0, Lcom/actions/sensor/calib/SensorActivity$1$1;->this$1:Lcom/actions/sensor/calib/SensorActivity$1;

    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    #p0=(Reference);
    return-void
.end method


# virtual methods
.method public run()V
    .locals 6

    .prologue
    const/4 v5, 0x1

    .line 43
    #v5=(One);
    iget-object v2, p0, Lcom/actions/sensor/calib/SensorActivity$1$1;->this$1:Lcom/actions/sensor/calib/SensorActivity$1;

    #v2=(Reference);
    iget-object v2, v2, Lcom/actions/sensor/calib/SensorActivity$1;->this$0:Lcom/actions/sensor/calib/SensorActivity;

    invoke-static {v2}, Lcom/actions/sensor/calib/SensorActivity;->access$000(Lcom/actions/sensor/calib/SensorActivity;)Landroid/widget/Button;

    move-result-object v2

    invoke-virtual {v2, v5}, Landroid/widget/Button;->setClickable(Z)V

    .line 44
    iget-object v2, p0, Lcom/actions/sensor/calib/SensorActivity$1$1;->this$1:Lcom/actions/sensor/calib/SensorActivity$1;

    iget-object v2, v2, Lcom/actions/sensor/calib/SensorActivity$1;->this$0:Lcom/actions/sensor/calib/SensorActivity;

    const/4 v3, 0x0

    #v3=(Null);
    invoke-static {v2, v3}, Lcom/actions/sensor/calib/SensorActivity;->access$102(Lcom/actions/sensor/calib/SensorActivity;Z)Z

    .line 47
    iget-object v2, p0, Lcom/actions/sensor/calib/SensorActivity$1$1;->this$1:Lcom/actions/sensor/calib/SensorActivity$1;

    iget-object v2, v2, Lcom/actions/sensor/calib/SensorActivity$1;->this$0:Lcom/actions/sensor/calib/SensorActivity;

    invoke-static {v2}, Lcom/actions/sensor/calib/SensorActivity;->access$200(Lcom/actions/sensor/calib/SensorActivity;)Lcom/actions/sensor/calib/SensorControl;

    move-result-object v2

    invoke-virtual {v2}, Lcom/actions/sensor/calib/SensorControl;->runCalib()V

    .line 50
    iget-object v2, p0, Lcom/actions/sensor/calib/SensorActivity$1$1;->this$1:Lcom/actions/sensor/calib/SensorActivity$1;

    iget-object v2, v2, Lcom/actions/sensor/calib/SensorActivity$1;->this$0:Lcom/actions/sensor/calib/SensorActivity;

    invoke-static {v2}, Lcom/actions/sensor/calib/SensorActivity;->access$200(Lcom/actions/sensor/calib/SensorActivity;)Lcom/actions/sensor/calib/SensorControl;

    move-result-object v2

    invoke-virtual {v2}, Lcom/actions/sensor/calib/SensorControl;->getCalibValue()Ljava/lang/String;

    move-result-object v0

    .line 51
    .local v0, calib:Ljava/lang/String;
    #v0=(Reference);
    const-string v2, "SensorActivity"

    new-instance v3, Ljava/lang/StringBuilder;

    #v3=(UninitRef);
    invoke-direct {v3}, Ljava/lang/StringBuilder;-><init>()V

    #v3=(Reference);
    const-string v4, "Calib: "

    #v4=(Reference);
    invoke-virtual {v3, v4}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v3

    invoke-virtual {v3, v0}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v3

    invoke-virtual {v3}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v3

    invoke-static {v2, v3}, Landroid/util/Log;->i(Ljava/lang/String;Ljava/lang/String;)I

    .line 54
    new-instance v2, Ljava/lang/StringBuilder;

    #v2=(UninitRef);
    invoke-direct {v2}, Ljava/lang/StringBuilder;-><init>()V

    #v2=(Reference);
    invoke-virtual {v2, v0}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v2

    const-string v3, "\n"

    invoke-virtual {v2, v3}, Ljava/lang/StringBuilder;->append(Ljava/lang/String;)Ljava/lang/StringBuilder;

    move-result-object v2

    invoke-virtual {v2}, Ljava/lang/StringBuilder;->toString()Ljava/lang/String;

    move-result-object v1

    .line 55
    .local v1, file:Ljava/lang/String;
    #v1=(Reference);
    iget-object v2, p0, Lcom/actions/sensor/calib/SensorActivity$1$1;->this$1:Lcom/actions/sensor/calib/SensorActivity$1;

    iget-object v2, v2, Lcom/actions/sensor/calib/SensorActivity$1;->this$0:Lcom/actions/sensor/calib/SensorActivity;

    invoke-static {v2}, Lcom/actions/sensor/calib/SensorActivity;->access$200(Lcom/actions/sensor/calib/SensorActivity;)Lcom/actions/sensor/calib/SensorControl;

    move-result-object v2

    invoke-virtual {v2, v1}, Lcom/actions/sensor/calib/SensorControl;->writeCalibFile(Ljava/lang/String;)V

    .line 58
    iget-object v2, p0, Lcom/actions/sensor/calib/SensorActivity$1$1;->this$1:Lcom/actions/sensor/calib/SensorActivity$1;

    iget-object v2, v2, Lcom/actions/sensor/calib/SensorActivity$1;->this$0:Lcom/actions/sensor/calib/SensorActivity;

    const v3, 0x7f050004

    #v3=(Integer);
    invoke-static {v2, v3, v5}, Landroid/widget/Toast;->makeText(Landroid/content/Context;II)Landroid/widget/Toast;

    move-result-object v2

    invoke-virtual {v2}, Landroid/widget/Toast;->show()V

    .line 60
    return-void
.end method
