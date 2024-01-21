

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


//������ݺ���ʾ��
static const unsigned char g_byaDisplayCharCode[17][4] = {
	   		 	 	  					   	  		  	 {0xff,0xff,0xff,0xff},   //ȫ������IDM_NULL 0
		 	  					   	  					 {0xf0,0x9a,0x1a,0xb0},   //��ʾCode��IDM_Code 1
									  					 {0x91,0xb0,0xb2,0xff},   //��ʾSet��IDM_Set 2 
									  					 {0x1a,0x5f,0x91,0xff},   //��ʾDis��IDM_Dis 3
									  					 {0xf0,0x9a,0x9e,0x9e},   //��ʾConn��IDM_Conn 4 
														 {0x34,0xda,0xb2,0xff},	  //��ʾPut��IDM_CodePut 5
														 {0x91,0xb0,0xb2,0xff},   //��ʾSet��IDM_CodeSet 6 
														 {0x9e,0xb0,0xb2,0xff},	  //��ʾNet�� IDM_SetNet 7
														 {0x34,0xb2,0xef,0x52},	  //��ʾPT.U��IDM_SetPT_U 8 
														 {0xf0,0xb2,0xef,0xfe},	  //��ʾCT.I�� IDM_SetCT_I 9
														 {0xa0,0xf0,0xf2,0xb0},   //��ʾE.CLE�� IDM_SetE_CLE 10
														 {0x1a,0x5f,0x81,0xb0},	  //��ʾDISP.E�� IDM_DisDIS_E 11
														 {0x1a,0x5f,0x81,0x34},	  //��ʾDIS.P��IDM_DisDIS_P 12
														 {0x82,0xf2,0xb0,0x1a},	  //��ʾB.LED��IDM_DisB_LED 13
														 {0x14,0x1a,0x1a,0xff},   //��ʾAdd��IDM_ConnAdd   14
														 {0x1a,0x14,0xb2,0x14},   //��ʾData��IDM_ConnData  15
														 {0x92,0xda,0x1a,0xff},   //��ʾBud��IDM_ConnBud   16
													    };
														
//��ʾ���ֵ�����
static const unsigned char g_byaDisplayNumCode[10] = {0x50, 0x5f, 0x38, 0x19, 0x17, 0x91, 0x90, 0x5d, 0x10, 0x11 };


//��ʾ���ȣ���Ϊ6���ȼ������120����С0��
static volatile unsigned char g_byDisplayLightness = 120;
//��ʾ��־��Ϊ1ʱ�������һֱʵ�֣�Ϊ0ʱ�������60S����ʾ
static volatile unsigned char g_byDisplayEnable = 1;
//��ʾʱ��
static volatile unsigned char g_byDisplayTime = 60;
//��˸��־λ��Ϊ1ʱ���������Ӧλ��˸��ʾ��Ϊ0ʱ������ܲ���˸
static volatile unsigned char g_byFlashEnable = 0;
//��˸λ
static volatile unsigned char g_byFlashDigit = 0;
//��ʾ����
static volatile unsigned char g_byaDisplayContents[3][4] = {{0xff,0xff,0xff,0xff},
		 	  							    	   {0xff,0xff,0xff,0xff},
												   {0xff,0xff,0xff,0xff}};

//**********************��ʼ��**************************************************************************************** 
//��ʾ��ʼ��
void fnDisplayInit();
//********************************************************************************************************************


//***********************��ʾ*****************************************************************************************
//�����ɨ����ʾ					   
void fnScanLED();
//�������˸��ʾ
void fnDisplay();
//********************************************************************************************************************


//************************��ʾ����************************************************************************************
//������ʾ�ַ����ݣ�Ҫ������ʾ������ܶ������������ϵ���һ��Ϊ0,1��2������Ҫ��ʾ�ַ�
void fnSetDisplayContentChar(unsigned char byDisplayRow, unsigned char byDisplayChar);
//������ʾ�������ݣ�Ҫ������ʾ������ܶ������������ϵ���һ��Ϊ0,1��2������Ҫ��ʾ����
void fnSetDisplayContentNum(unsigned char byDisplayRow, unsigned int byDisplayNum);
//��ո����е�����
void fnCleanDisplayContent(unsigned char byDisplayRow);
//********************************************************************************************************************


//****************************��ʾ����********************************************************************************
//������ʾ����
void fnSetDisplayLightness(unsigned char byDisplayLightness);
//��������ܵ���ʾ���Ⱥͷ�������ܵ���ʾʱ���й⣬��ʾʱ��Խ������������ܾ�Խ���������ʾ��ʱʱ��ĳ��Ⱦ����˷�������ܵ�ʵ������
void fnDelayDisplay(unsigned char byDelayTime);
//********************************************************************************************************************


//*****************************��ʾʱ��********************************************************************************
//��λ��ǰ��ʾʱ�䣬����ǰ��ʾʱ���趨Ϊ���ֵ
void fnResetDisplayTime();
//��ȡ��ǰ��ʾʱ��
unsigned char fnGetDisplayTime();
//����һֱ��ʾ���ܣ�ʹ����ʾ��־λ����ʾ��־λ��1�������һֱ��ʾ
void fnEnableDisplay();
//�ر�һֱ��ʾ���ܣ�ʧ����ʾ��־λ����ʾ��־λ��0���������ʾ60S
void fnDisableDisplay();
//*********************************************************************************************************************


//****************************��˸*************************************************************************************
//�ر���˸���ܣ���˸��־λ��0
void fnDisableFlash();
//������˸���ܣ���˸��־λ��1
void fnSetFlash(unsigned char byFlashDigit);
//*********************************************************************************************************************
