test_tvout_halʹ��˵��
    1.test_tvout_hal��Ҫ�Ƕ�tvout��������hal����в��ԣ���Ҫ�漰��libcvbs,libhdmi,libdisplay����������hal�㡣
    2.test_tvout_hal��ʹ�÷������£�
        (1)ʹ��ʱ�����������������ɵĶ������ļ�test_tvout_hal(����λ�òο�Android.mk)ͨ��adb push
        ��С����
        (2)���뵽С����test_tvout_hal�ļ�����Ŀ¼��ִ��./test_tvout_hal ����1 ����2 ����3 ����4,���и��������ÿո�����
        (3)������˵��
            ����1��ʾCVBS����ʽ��ֵΪ "pal" �� "ntsc";
            ����2��ʾHDMI��video id��ֵ�ο���¼��
            ����3��ʾdisplay mode,ֵ�ο���¼��
            ����4��ʾ��ʾ�豸��ֵΪcvbs_lcd0 �� hdmi_lcd0 �� lcd0
            ע����ĳ�������������á���������
        (4)����
           a.����Ҫ����CVBS��pal��ʽ��lcdͬ�ԣ���display modeΪ0,��./test_tvout_hal pal �� 0 cvbs_lcd0;
           b.����Ҫ����HDMI��vidΪ86�ķֱ�����lcdͬ�ԣ���display modeΪ0,��./test_tvout_hal ��86 0 hdmi_lcd0;
           c.���Ҫ����lcd���ԣ���:./test_tvout_hal ! ! ! lcd0.
    3.��¼
        (1)display modeֵ
         MODE_DISP_SYNC_DEFAULT_TV_GV_LCD_GV:0
 
        MODE_DISP_DOUBLE_DEFAULT_NO_SYNC_TV_GV_LCD_GV:1
        MODE_DISP_DOUBLE_DEFAULT_NO_SYNC_TV_GV_LCD_G:17
        MODE_DISP_DOUBLE_DEFAULT_NO_SYNC_TV_V_LCD_G:33

       MODE_DISP_DOUBLE_DEFAULT_SYNC_EXT_TV_GV_LCD_GV:3
       MODE_DISP_DOUBLE_DEFAULT_SYNC_EXT_TV_GV_LCD_G:19
       MODE_DISP_DOUBLE_DEFAULT_SYNC_EXT_TV_V_LCD_G:35
       
       (2)hdmi vidֵ
       VID640x480P_60_4VS3 : 1	
       VID720x480P_60_4VS3 : 2	
       VID720x480P_60_16VS9  : 3	
       VID1280x720P_60_16VS9 ��4	
       VID1920x1080I_60_16VS9 ��5	
       VID720x480I_60_4VS3 ��6
       VID720x480I_60_16VS9 ��7
       VID1440x480P_60_4VS3 : 14	
       VID1440x480P_60_16VS9 : 15	
       VID1920x1080P_60_16VS9 : 16	
       VID720x576P_50_4VS3:17
       VID720x576P_50_16VS9 : 18
       VID1280x720P_50_16VS9  : 19	
       VID1920x1080I_50_16VS9 : 20	
       VID720x576I_50_4VS3 : 21
       VID720x576I_50_16VS9 : 22
       VID1440x576P_50_4VS3 : 29	
       VID1440x576P_50_16VS9 : 30	
       VID1920x1080P_50_16VS9 : 31	
       VID1920x1080P_24_16VS9 : 32	
       VID1920x1080P_25_16VS9 : 33	
       VID1920x1080P_30_16VS9 : 34	
       VID720x480P_59P94_4VS3 : 72	
       VID1280X720P_59P94_16VS9 : 74	
           