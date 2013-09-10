package com.actions.hardware;

import com.actions.hardware.ICableStatusListener;

interface IDisplayService
{
         /* number of displayer*/
         int getDisplayerCount();
         /*get config of display */
	String  getDisplayerInfo(int id);
	
	/* select currrent output displayer */
	boolean setOutputDisplayer(int id);
	boolean setOutputDisplayer1(String id);
	/* id of current Displayer */
	int getOutputDisplayer();
	String getOutputDisplayerStr();
	/* enable/disable output  */
	boolean enableOutput(boolean enable);
	boolean enableOutput1(String id, boolean enable);
	/* do we support lcd & tv simultaneously ?*/
	boolean setDualDisplay(boolean dualOutput);
	
	/*config current display: 
	*
	* param:  String like  format=NTSC;audio=5.1
	*/
	boolean setDisplayerParam(String param);
	
	/* read current config 
	*
	* param:  String like  format=NTSC;audio=5.1
	*/
	String getDisplayerParam();
	String getDisplayerParamStr(String displayer);

	
	/* how to scale Video: origion/fullScreen/uniform-scale */
	//boolean setScaleMode(int mode);

	/* current Scale Mode */
	//int getScaleMode();

        /* wheather current tvout/hdmi cable connected 
         * return value is flagged: hdmi/tvout  all with a bit mask
        */
	int getCableState();
	void setDisplayMode(int mode);
	void setDisplaySingleMode(int mode);
	void setHdmiDisconnectFlag(boolean flag);
	void setHdmiVid(int vid);
	int getHdmiVid(); 
	String getHdmiCap();
	
	/* notify cable status changed */
	void addStatusListener(ICableStatusListener listener);
	void testInit();
}