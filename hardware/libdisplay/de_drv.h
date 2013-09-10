#ifndef __ACT_DE_DRIVER__H__
#define __ACT_DE_DRIVER__H__
//请与\psp\fwsp\include\de.h 保持同步更新
struct mdsb_info {
	int drop_line_front;
	int drop_line_back;
	int xres;
	int yres;
};
//DISP DEVICE TYPE
enum disp_dev_id {
	LCD_DISPLAYER = 0x1,
	HDMI_DISPLAYER = 0x2,
	TV_CVBS_DISPLAYER = 0x4,
	TV_YPbPr_DISPLAYER = 0x8,
	LCD_DISPLAYER_SECOND = 0x10,
};

/* DE 的显示模式
 用模式的最低0--3两位来表示
 MODE_DISP_SYNC_DEFAULT：在多输出的情况下，默认使用 sync显示模式。
 MODE_DISP_DOUBLE_DEFAULT_NO_SYNC：在多输出的情况下，double dispaly的模式，不支持三者以上的同时显示
 MODE_DISP_DOUBLE_DEFAULT_SYNC：在多输出的情况下，显示设备个数少于三的时候采用double 模式，
 显示设备超过三者的情况下采用sync模式。
 MODE_DISP_DOUBLE_DEFAULT_SYNC_EXT：在多输出的情况下，double dispaly的模式，
 如果超过三种显示设备的时候才用sync的扩展方式，就是另外两者sync display，如cvbs 和hdmi 同显，在与lcd双显
 */
#define MODE_DISP_SHIFT      0x00
#define MODE_DISP_MASK       0x07
#define MODE_DISP_SYNC_DEFAULT             0
#define MODE_DISP_DOUBLE_DEFAULT_NO_SYNC   1
#define MODE_DISP_DOUBLE_DEFAULT_SYNC      2
#define MODE_DISP_DOUBLE_DEFAULT_SYNC_EXT  3

/* DE 的输出模式
 用模式的4--7两位来表示video层和graphic层的输出情况，
 MODE_OUT_TV_GV_LCD_GV: 无论在tv端，还是在lcd端，都有graphic和video的信息
 MODE_OUT_TV_GV_LCD_G：tv端有graphic和video的信息，但在lcd端只有graphic的信息，没有视频信息
 MODE_OUT_TV_V_LCD_G：tv端只显示视频信息，lcd端是显示graphic的信息。
 */
#define MODE_OUT_MASK       0x0
#define MODE_OUT_SHIFT      0x04
#define MODE_OUT_TV_GV_LCD_GV         0
#define MODE_OUT_TV_GV_LCD_G          1
#define MODE_OUT_TV_V_LCD_G           2

#define MODE_DISP_SYNC_DEFAULT_TV_GV_LCD_GV        (MODE_DISP_SYNC_DEFAULT | (MODE_OUT_TV_GV_LCD_GV << MODE_OUT_SHIFT))

#define MODE_DISP_DOUBLE_DEFAULT_NO_SYNC_TV_GV_LCD_GV   (MODE_DISP_DOUBLE_DEFAULT_NO_SYNC | (MODE_OUT_TV_GV_LCD_GV << MODE_OUT_SHIFT))
#define MODE_DISP_DOUBLE_DEFAULT_NO_SYNC_TV_GV_LCD_G   (MODE_DISP_DOUBLE_DEFAULT_NO_SYNC | (MODE_OUT_TV_GV_LCD_G << MODE_OUT_SHIFT))
#define MODE_DISP_DOUBLE_DEFAULT_NO_SYNC_TV_V_LCD_G  (MODE_DISP_DOUBLE_DEFAULT_NO_SYNC | (MODE_OUT_TV_V_LCD_G << MODE_OUT_SHIFT))

#define MODE_DISP_DOUBLE_DEFAULT_SYNC_TV_GV_LCD_GV   (MODE_DISP_DOUBLE_DEFAULT_SYNC | (MODE_OUT_TV_GV_LCD_GV << MODE_OUT_SHIFT))
#define MODE_DISP_DOUBLE_DEFAULT_SYNC_TV_GV_LCD_G   (MODE_DISP_DOUBLE_DEFAULT_SYNC | (MODE_OUT_TV_GV_LCD_G << MODE_OUT_SHIFT))
#define MODE_DISP_DOUBLE_DEFAULT_SYNC_TV_V_LCD_G  (MODE_DISP_DOUBLE_DEFAULT_SYNC | (MODE_OUT_TV_V_LCD_G << MODE_OUT_SHIFT))

#define MODE_DISP_DOUBLE_DEFAULT_SYNC_EXT_TV_GV_LCD_GV   (MODE_DISP_DOUBLE_DEFAULT_SYNC_EXT | (MODE_OUT_TV_GV_LCD_GV << MODE_OUT_SHIFT))
#define MODE_DISP_DOUBLE_DEFAULT_SYNC_EXT_TV_GV_LCD_G   (MODE_DISP_DOUBLE_DEFAULT_SYNC_EXT | (MODE_OUT_TV_GV_LCD_G << MODE_OUT_SHIFT))
#define MODE_DISP_DOUBLE_DEFAULT_SYNC_EXT_TV_V_LCD_G  (MODE_DISP_DOUBLE_DEFAULT_SYNC_EXT | (MODE_OUT_TV_V_LCD_G << MODE_OUT_SHIFT))

// IOCTRL COMMAND CODE
/*
 ioctl DEIOCSET_DISPLAYER
 int ioctl(int fd, int request, int * disp_id);
 设置输出的显示设备
 ioctl FBIOGET_DISPLAYER
 int ioctl(int fd, int request, int * disp_id);

 获取当前是哪种显示设备,
 显示设备的种类有以下几种：
 LCD_DISPLAYER = x01
 HDMI_DISPLAYER = 0x02
 TV_CVBS_DISPLAYER = 0x04
 TV_YPbPr_DISPLAYER = 0x08
 以上定义，为应对多输出的时候有多种显示设备同时激活，故每一bit位代表一种显示设备，当多种显示设备输出的时候，采用组合的方式，例如：TV_YPbPr_DISPLAYER | LCD_DISPLAYER,作为参数输入就是lcd和ypbpr多输出显示。
 在获取当前显示设备的时候也是采用类似组合。
 */
#define DEIOSET_DISPLAYER        0x5702
#define DEIOGET_DISPLAYER					0x5703

/*
 ioctl DEIOSET_OVERLAY_STATUS
 int ioctl(int fd, int request, int * enable);

 用于FB使能或关闭OVERLAY的功能，此操作只是隐藏或是显示VIDEO层的内容，并没有关闭VIDEO层的显示流程。
 enable:参数 1表示EBANL, 0表示DISABLE

 */
#define DEIOSET_OVERLAY_STATUS    0x5704

/*
 ioctl DEIO_SELECT_AS_MAIN_FB
 int ioctl(int fd, int request, void* );
 在双屏双显的情况下，由于有两个FB对应两个屏幕,选择其中一个作为主屏，在TVOUT的时候，会将该屏信息从TVOUT输出。
 无须输入参数，fd确定是哪个FB是主屏信息
 ioctl(fd, FBIO_SELECT_AS_MAIN_FB, NULL);
 */
#define DEIO_SELECT_AS_MAIN_FB    0x5705

/*
 ioctl DEIO_SET_SCALE_RATE_FULL_SCREEN
 ioctl DEIO_GET_SCALE_RATE_FULL_SCREEN
 int ioctl(int fd, int request, int * rate);
 在TVOUT输出的时候，电视在全屏幕输出的时候，有些UI没有显示出来，需要调整输出的大小比率以适应电视显示
 rate :表明全屏缩放的比例，其中值为Hrate << 8 | Vrate Hrate和 Vrate的范围在 25-100之间。
 ioctl(fd, DEIO_SET_SCALE_RATE_FULL_SCREEN,rate);
 */
#define DEIO_SET_SCALE_RATE_FULL_SCREEN 0x5706
#define DEIO_GET_SCALE_RATE_FULL_SCREEN 0x5707
/*
 ioctl DEIO_GET_MDSB_INFO
 int ioctl(int fd, int request, int * changed_status);
 在插拔线检测的时候,用户层只需要使用POLL DE,	在有变动通知的时候,用户层调用改接口
 来获取变动状态,changed_status 分别
 bit 0: TV_CVBS_DISPLAYER
 bit 1: TV_YPbPr_DISPLAYER
 bit 2: HDMI_DISPLAYER
 其中1表示插入,0表示拔出.
 ioctl(fd, DEIO_SET_SCALE_RATE_FULL_SCREEN,changed_status);
 */

#define DEIO_GET_DISP_CHANGED 0x5708
/*
 ioctl DEIO_GET_DISP_CHANGED
 int ioctl(int fd, int request, int * mdsb_info);
 在用MDSB做同显的时候，由于LCD上的坐标和TP实际显示的坐标需要调整，作为调整时候重新计算坐标需要的参数
 mdsb_info中存储当前用MDSB做同显的时候,当LCD和HDMI或YPBPR同显的时候，前后会丢行
 int drop_line_front;在同显的时候LCD前面丢的行数，该行数是相对与缩放前的同显时候的分辨率。
 int drop_line_back:在同显的时候LCD后面丢的行数，该行数是相对与缩放前的同显时候的分辨率。
 int xres;同显的分辨率X方向
 int yres;同显的分辨率Y方向
 ioctl(fd, DEIO_SET_SCALE_RATE_FULL_SCREEN,changed_status);
 */
#define DEIO_GET_MDSB_INFO 0x5709

/*
 ioctl DEIO_GET_2X_MODE and DEIO_SET_2X_MODE
 int ioctl(int fd, int request, int * enable);
 GL5201中GRAPHIC层只支持2X模式，该调用用于设置和获取GRAPHIC层的2x模式
 int * enable; 使能或关闭 1：使能，0关闭：
 在DEIO_GET_2X_MODE的时候返回的enable的值也表明上述含义。
 ioctl(fd, DEIO_SET_2X_MODE,enable);
 */
#define DEIO_GET_2X_MODE 0x5710
#define DEIO_SET_2X_MODE 0x5711

/*
 ioctl DEIO_GET_VIDEO_CHANGED
 int ioctl(int fd, int request, int * video_status);
 该调用用于获取当前VIDEO的播放状态，当VIDEO开始播放的时候，会唤醒POLL，再用该调用获取VIDEO的状态
 video_status = 1 表明开
 video_status = 0 表明关
 返回值：1 表明唤醒POLL的除video_status变化外还有其他的源
 0 表明唤醒POLL的只有video_status变化，且执行成功
 -1 表明唤醒POLL的不是video_status变化
 ioctl(fd, DEIO_GET_VIDEO_CHANGED,video_status);
 */
#define DEIO_GET_VIDEO_CHANGED 0x5712

/*
 ioctl DEIO_SET_DISP_MODE
 int ioctl(int fd, int request, int * disp_mode);
 该调用用于设置de输出的模式，由于5202存在双显和同显的区别，同时可以选择video层和graphic层输出路径，
 所以该函数的作用就是设置采用何种模式显示，并根据需求来选择video和graphic的输出路径。

 模式的说明如上面描述，
 例如
 需要采用同显模式并采用video和graphic在tv端和lcd端同时都显示则
 disp_mode = (MODE_DISP_SYNC_DEFAULT << MODE_DISP_SHIFT) | (MODE_OUT_TV_GV_LCD_GV << MODE_OUT_SHIFT)

 需要采用双显模式且只支持两种显示设备，并采用video和graphic在tv端和lcd端同时都显示则
 disp_mode = (MODE_DISP_DOUBLE_DEFAULT_NO_SYNC << MODE_DISP_SHIFT) | (MODE_OUT_TV_GV_LCD_GV << MODE_OUT_SHIFT)

 需要两种显示设备时，采用双显模式 三种以上设备时候采用同显，并采用video和graphic在tv端和lcd端同时都显示则
 disp_mode = (MODE_DISP_DOUBLE_DEFAULT_SYNC << MODE_DISP_SHIFT) | (MODE_OUT_TV_GV_LCD_GV << MODE_OUT_SHIFT)

 需要两种显示设备时，采用双显模式 三种以上设备时候lcd 和tv 端采用双显，tv端的hdmi 和cvbs 采用同显，并采用video在tv端，graphic 在lcd端显示则
 disp_mode = (MODE_DISP_DOUBLE_DEFAULT_SYNC_EXT << MODE_DISP_SHIFT) | (MODE_OUT_TV_V_LCD_G << MODE_OUT_SHIFT)

 如此类推。

 */
#define DEIO_SET_DISP_MODE 0x5713

#define SCALE_ORIGINAL_SIZE 1
#define SCALE_FIT_SIZE      2
#define SCALE_FULL_SIZE     3
#define SCALE_CUSTOM_SCALE  4
#define SCALE_2X_MODE       5

#endif

