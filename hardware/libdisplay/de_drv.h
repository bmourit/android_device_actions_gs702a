#ifndef __ACT_DE_DRIVER__H__
#define __ACT_DE_DRIVER__H__
//����\psp\fwsp\include\de.h ����ͬ������
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

/* DE ����ʾģʽ
 ��ģʽ�����0--3��λ����ʾ
 MODE_DISP_SYNC_DEFAULT���ڶ����������£�Ĭ��ʹ�� sync��ʾģʽ��
 MODE_DISP_DOUBLE_DEFAULT_NO_SYNC���ڶ����������£�double dispaly��ģʽ����֧���������ϵ�ͬʱ��ʾ
 MODE_DISP_DOUBLE_DEFAULT_SYNC���ڶ����������£���ʾ�豸������������ʱ�����double ģʽ��
 ��ʾ�豸�������ߵ�����²���syncģʽ��
 MODE_DISP_DOUBLE_DEFAULT_SYNC_EXT���ڶ����������£�double dispaly��ģʽ��
 �������������ʾ�豸��ʱ�����sync����չ��ʽ��������������sync display����cvbs ��hdmi ͬ�ԣ�����lcd˫��
 */
#define MODE_DISP_SHIFT      0x00
#define MODE_DISP_MASK       0x07
#define MODE_DISP_SYNC_DEFAULT             0
#define MODE_DISP_DOUBLE_DEFAULT_NO_SYNC   1
#define MODE_DISP_DOUBLE_DEFAULT_SYNC      2
#define MODE_DISP_DOUBLE_DEFAULT_SYNC_EXT  3

/* DE �����ģʽ
 ��ģʽ��4--7��λ����ʾvideo���graphic�����������
 MODE_OUT_TV_GV_LCD_GV: ������tv�ˣ�������lcd�ˣ�����graphic��video����Ϣ
 MODE_OUT_TV_GV_LCD_G��tv����graphic��video����Ϣ������lcd��ֻ��graphic����Ϣ��û����Ƶ��Ϣ
 MODE_OUT_TV_V_LCD_G��tv��ֻ��ʾ��Ƶ��Ϣ��lcd������ʾgraphic����Ϣ��
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
 �����������ʾ�豸
 ioctl FBIOGET_DISPLAYER
 int ioctl(int fd, int request, int * disp_id);

 ��ȡ��ǰ��������ʾ�豸,
 ��ʾ�豸�����������¼��֣�
 LCD_DISPLAYER = x01
 HDMI_DISPLAYER = 0x02
 TV_CVBS_DISPLAYER = 0x04
 TV_YPbPr_DISPLAYER = 0x08
 ���϶��壬ΪӦ�Զ������ʱ���ж�����ʾ�豸ͬʱ�����ÿһbitλ����һ����ʾ�豸����������ʾ�豸�����ʱ�򣬲�����ϵķ�ʽ�����磺TV_YPbPr_DISPLAYER | LCD_DISPLAYER,��Ϊ�����������lcd��ypbpr�������ʾ��
 �ڻ�ȡ��ǰ��ʾ�豸��ʱ��Ҳ�ǲ���������ϡ�
 */
#define DEIOSET_DISPLAYER        0x5702
#define DEIOGET_DISPLAYER					0x5703

/*
 ioctl DEIOSET_OVERLAY_STATUS
 int ioctl(int fd, int request, int * enable);

 ����FBʹ�ܻ�ر�OVERLAY�Ĺ��ܣ��˲���ֻ�����ػ�����ʾVIDEO������ݣ���û�йر�VIDEO�����ʾ���̡�
 enable:���� 1��ʾEBANL, 0��ʾDISABLE

 */
#define DEIOSET_OVERLAY_STATUS    0x5704

/*
 ioctl DEIO_SELECT_AS_MAIN_FB
 int ioctl(int fd, int request, void* );
 ��˫��˫�Ե�����£�����������FB��Ӧ������Ļ,ѡ������һ����Ϊ��������TVOUT��ʱ�򣬻Ὣ������Ϣ��TVOUT�����
 �������������fdȷ�����ĸ�FB��������Ϣ
 ioctl(fd, FBIO_SELECT_AS_MAIN_FB, NULL);
 */
#define DEIO_SELECT_AS_MAIN_FB    0x5705

/*
 ioctl DEIO_SET_SCALE_RATE_FULL_SCREEN
 ioctl DEIO_GET_SCALE_RATE_FULL_SCREEN
 int ioctl(int fd, int request, int * rate);
 ��TVOUT�����ʱ�򣬵�����ȫ��Ļ�����ʱ����ЩUIû����ʾ��������Ҫ��������Ĵ�С��������Ӧ������ʾ
 rate :����ȫ�����ŵı���������ֵΪHrate << 8 | Vrate Hrate�� Vrate�ķ�Χ�� 25-100֮�䡣
 ioctl(fd, DEIO_SET_SCALE_RATE_FULL_SCREEN,rate);
 */
#define DEIO_SET_SCALE_RATE_FULL_SCREEN 0x5706
#define DEIO_GET_SCALE_RATE_FULL_SCREEN 0x5707
/*
 ioctl DEIO_GET_MDSB_INFO
 int ioctl(int fd, int request, int * changed_status);
 �ڲ���߼���ʱ��,�û���ֻ��Ҫʹ��POLL DE,	���б䶯֪ͨ��ʱ��,�û�����øĽӿ�
 ����ȡ�䶯״̬,changed_status �ֱ�
 bit 0: TV_CVBS_DISPLAYER
 bit 1: TV_YPbPr_DISPLAYER
 bit 2: HDMI_DISPLAYER
 ����1��ʾ����,0��ʾ�γ�.
 ioctl(fd, DEIO_SET_SCALE_RATE_FULL_SCREEN,changed_status);
 */

#define DEIO_GET_DISP_CHANGED 0x5708
/*
 ioctl DEIO_GET_DISP_CHANGED
 int ioctl(int fd, int request, int * mdsb_info);
 ����MDSB��ͬ�Ե�ʱ������LCD�ϵ������TPʵ����ʾ��������Ҫ��������Ϊ����ʱ�����¼���������Ҫ�Ĳ���
 mdsb_info�д洢��ǰ��MDSB��ͬ�Ե�ʱ��,��LCD��HDMI��YPBPRͬ�Ե�ʱ��ǰ��ᶪ��
 int drop_line_front;��ͬ�Ե�ʱ��LCDǰ�涪�������������������������ǰ��ͬ��ʱ��ķֱ��ʡ�
 int drop_line_back:��ͬ�Ե�ʱ��LCD���涪�������������������������ǰ��ͬ��ʱ��ķֱ��ʡ�
 int xres;ͬ�Եķֱ���X����
 int yres;ͬ�Եķֱ���Y����
 ioctl(fd, DEIO_SET_SCALE_RATE_FULL_SCREEN,changed_status);
 */
#define DEIO_GET_MDSB_INFO 0x5709

/*
 ioctl DEIO_GET_2X_MODE and DEIO_SET_2X_MODE
 int ioctl(int fd, int request, int * enable);
 GL5201��GRAPHIC��ֻ֧��2Xģʽ���õ����������úͻ�ȡGRAPHIC���2xģʽ
 int * enable; ʹ�ܻ�ر� 1��ʹ�ܣ�0�رգ�
 ��DEIO_GET_2X_MODE��ʱ�򷵻ص�enable��ֵҲ�����������塣
 ioctl(fd, DEIO_SET_2X_MODE,enable);
 */
#define DEIO_GET_2X_MODE 0x5710
#define DEIO_SET_2X_MODE 0x5711

/*
 ioctl DEIO_GET_VIDEO_CHANGED
 int ioctl(int fd, int request, int * video_status);
 �õ������ڻ�ȡ��ǰVIDEO�Ĳ���״̬����VIDEO��ʼ���ŵ�ʱ�򣬻ỽ��POLL�����øõ��û�ȡVIDEO��״̬
 video_status = 1 ������
 video_status = 0 ������
 ����ֵ��1 ��������POLL�ĳ�video_status�仯�⻹��������Դ
 0 ��������POLL��ֻ��video_status�仯����ִ�гɹ�
 -1 ��������POLL�Ĳ���video_status�仯
 ioctl(fd, DEIO_GET_VIDEO_CHANGED,video_status);
 */
#define DEIO_GET_VIDEO_CHANGED 0x5712

/*
 ioctl DEIO_SET_DISP_MODE
 int ioctl(int fd, int request, int * disp_mode);
 �õ�����������de�����ģʽ������5202����˫�Ժ�ͬ�Ե�����ͬʱ����ѡ��video���graphic�����·����
 ���Ըú��������þ������ò��ú���ģʽ��ʾ��������������ѡ��video��graphic�����·����

 ģʽ��˵��������������
 ����
 ��Ҫ����ͬ��ģʽ������video��graphic��tv�˺�lcd��ͬʱ����ʾ��
 disp_mode = (MODE_DISP_SYNC_DEFAULT << MODE_DISP_SHIFT) | (MODE_OUT_TV_GV_LCD_GV << MODE_OUT_SHIFT)

 ��Ҫ����˫��ģʽ��ֻ֧��������ʾ�豸��������video��graphic��tv�˺�lcd��ͬʱ����ʾ��
 disp_mode = (MODE_DISP_DOUBLE_DEFAULT_NO_SYNC << MODE_DISP_SHIFT) | (MODE_OUT_TV_GV_LCD_GV << MODE_OUT_SHIFT)

 ��Ҫ������ʾ�豸ʱ������˫��ģʽ ���������豸ʱ�����ͬ�ԣ�������video��graphic��tv�˺�lcd��ͬʱ����ʾ��
 disp_mode = (MODE_DISP_DOUBLE_DEFAULT_SYNC << MODE_DISP_SHIFT) | (MODE_OUT_TV_GV_LCD_GV << MODE_OUT_SHIFT)

 ��Ҫ������ʾ�豸ʱ������˫��ģʽ ���������豸ʱ��lcd ��tv �˲���˫�ԣ�tv�˵�hdmi ��cvbs ����ͬ�ԣ�������video��tv�ˣ�graphic ��lcd����ʾ��
 disp_mode = (MODE_DISP_DOUBLE_DEFAULT_SYNC_EXT << MODE_DISP_SHIFT) | (MODE_OUT_TV_V_LCD_G << MODE_OUT_SHIFT)

 ������ơ�

 */
#define DEIO_SET_DISP_MODE 0x5713

#define SCALE_ORIGINAL_SIZE 1
#define SCALE_FIT_SIZE      2
#define SCALE_FULL_SIZE     3
#define SCALE_CUSTOM_SCALE  4
#define SCALE_2X_MODE       5

#endif

