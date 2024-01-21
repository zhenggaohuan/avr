

#define FlashTime 400
#define MaxDisplayLightness 120
#define MaxDisplayTime 60


#define IDM_NULL 0
#define IDM_Code 1
#define IDM_Set  2
#define IDM_Dis  3
#define IDM_Conn 4
#define IDM_CodePut  5
#define IDM_CodeSet  6
#define IDM_SetNet  7
#define IDM_SetPT_U 8
#define IDM_SetCT_I 9
#define IDM_SetE_CLE 10
#define IDM_DisDIS_E 11
#define IDM_DisDIS_P 12
#define IDM_DisB_LED 13
#define IDM_ConnAdd   14
#define IDM_ConnData  15
#define IDM_ConnBud   16


//输出内容和显示码
static const unsigned char g_byaDisplayCharCode[17][4] = {
	   		 	 	  					   	  		  	 {0xff,0xff,0xff,0xff},   //全不亮，IDM_NULL 0
		 	  					   	  					 {0xf0,0x9a,0x1a,0xb0},   //显示Code，IDM_Code 1
									  					 {0x91,0xb0,0xb2,0xff},   //显示Set，IDM_Set 2 
									  					 {0x1a,0x5f,0x91,0xff},   //显示Dis，IDM_Dis 3
									  					 {0xf0,0x9a,0x9e,0x9e},   //显示Conn，IDM_Conn 4 
														 {0x34,0xda,0xb2,0xff},	  //显示Put，IDM_CodePut 5
														 {0x91,0xb0,0xb2,0xff},   //显示Set，IDM_CodeSet 6 
														 {0x9e,0xb0,0xb2,0xff},	  //显示Net， IDM_SetNet 7
														 {0x34,0xb2,0xef,0x52},	  //显示PT.U，IDM_SetPT_U 8 
														 {0xf0,0xb2,0xef,0xfe},	  //显示CT.I， IDM_SetCT_I 9
														 {0xa0,0xf0,0xf2,0xb0},   //显示E.CLE， IDM_SetE_CLE 10
														 {0x1a,0x5f,0x81,0xb0},	  //显示DISP.E， IDM_DisDIS_E 11
														 {0x1a,0x5f,0x81,0x34},	  //显示DIS.P，IDM_DisDIS_P 12
														 {0x82,0xf2,0xb0,0x1a},	  //显示B.LED，IDM_DisB_LED 13
														 {0x14,0x1a,0x1a,0xff},   //显示Add，IDM_ConnAdd   14
														 {0x1a,0x14,0xb2,0x14},   //显示Data，IDM_ConnData  15
														 {0x92,0xda,0x1a,0xff},   //显示Bud，IDM_ConnBud   16
													    };
														
//显示数字的字码
static const unsigned char g_byaDisplayNumCode[10] = {0x50, 0x5f, 0x38, 0x19, 0x17, 0x91, 0x90, 0x5d, 0x10, 0x11 };


//显示亮度，分为6个等级，最大120，最小0。
static volatile unsigned char g_byDisplayLightness = 120;
//显示标志：为1时，数码管一直实现，为0时，数码管60S后不显示
static volatile unsigned char g_byDisplayEnable = 1;
//显示时间
static volatile unsigned char g_byDisplayTime = 60;
//闪烁标志位，为1时，数码管相应位闪烁显示，为0时，数码管不闪烁
static volatile unsigned char g_byFlashEnable = 0;
//闪烁位
static volatile unsigned char g_byFlashDigit = 0;
//显示内容
static volatile unsigned char g_byaDisplayContents[3][4] = {{0xff,0xff,0xff,0xff},
		 	  							    	   {0xff,0xff,0xff,0xff},
												   {0xff,0xff,0xff,0xff}};

//**********************初始化**************************************************************************************** 
//显示初始化
void fnDisplayInit();
//********************************************************************************************************************


//***********************显示*****************************************************************************************
//数码管扫描显示					   
void fnScanLED();
//数码管闪烁显示
void fnDisplay();
//********************************************************************************************************************


//************************显示内容************************************************************************************
//设置显示字符内容，要给定显示的数码管段数（段数从上到下一次为0,1，2），和要显示字符
void fnSetDisplayContentChar(unsigned char byDisplayRow, unsigned char byDisplayChar);
//设置显示数字内容，要给定显示的数码管段数（段数从上到下一次为0,1，2），和要显示数字
void fnSetDisplayContentNum(unsigned char byDisplayRow, unsigned int byDisplayNum);
//清空给定行的内容
void fnCleanDisplayContent(unsigned char byDisplayRow);
//********************************************************************************************************************


//****************************显示亮度********************************************************************************
//设置显示亮度
void fnSetDisplayLightness(unsigned char byDisplayLightness);
//发光二极管的显示亮度和发光二极管的显示时间有光，显示时间越长，发光二极管就越亮，因此显示延时时间的长度决定了发光二极管的实现亮度
void fnDelayDisplay(unsigned char byDelayTime);
//********************************************************************************************************************


//*****************************显示时间********************************************************************************
//复位当前显示时间，将当前显示时间设定为最大值
void fnResetDisplayTime();
//获取当前显示时间
unsigned char fnGetDisplayTime();
//开启一直显示功能，使能显示标志位，显示标志位置1，数码管一直显示
void fnEnableDisplay();
//关闭一直显示功能，失能显示标志位，显示标志位置0，数码管显示60S
void fnDisableDisplay();
//*********************************************************************************************************************


//****************************闪烁*************************************************************************************
//关闭闪烁功能，闪烁标志位置0
void fnDisableFlash();
//开启闪烁功能，闪烁标志位置1
void fnSetFlash(unsigned char byFlashDigit);
//*********************************************************************************************************************
