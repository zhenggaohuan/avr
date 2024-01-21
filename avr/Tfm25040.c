/*����������Ҫ�������������FM25040�еĴ洢λ��
* address 0 means whether the code has been modified. If vlaue is 0x10, means modified, other value is a invalid value.
* code 0~9999, address 1 and address 2 restore the code. The total value is address 1*100 + address 2

* address 3 means whether the N.3.3/N.3.4 has been modified. If the value is 0x20, it means modified, other value is a invalid value.
* N.3.3/N.3.4 in address 4, 0 means N.3.3, 1 means N.3.4

* address 5 means whether the 100v/220v/380v has been modified. If the value is 0x30, it means modified, other value is a invalid value.
* 100v/220v/380v in address 6, 0 means 100v, 1 means 220v, 2 means 380v

* address 7 means whether the 1A/5A has been modified. If the value is 0x40, it means modified, other value is a invalid value.
* 1A/5A in address 8, 0 means 1A, 1 means 5A

* address 9 means whether the r.u has been modified. If the value is 0x50, it means modified, other value is a invalid value.
* r.u in address 10 and address 6, the value range is within 1~9999

* address 0xa means whether the i.u has been modified. If the value is 0x60, it means modified, other value is a invalid value.
* i.u in address 0xb and address 0xc, the value range is within 1~9999, The total value is address 0xb*100 + address c

* address 0xd means whether the baud has been modified. If the value is 0x70, it means modified, other value is a invalid value.
* baud 4800/9600 in address 9, 0 means 4800, 1 means 9600

* address 17 means whether the disp has been modified. If the value is 0x80, it means modified, other value is a invalid value.
* the value is within 0-6.
*/


#include "Tfm25040.h"
#include "Fm25040.h"
#include <avr/io.h>

 //sbit SO=P4^5;             /*FM25040��� pin 2*/
 //sbit SI=P3^5;             /*FM25040���� pin 5*/
 //sbit SCK=P4^7;            /*FM25040ʱ�� pin 6*/
 unsigned char WREN_INST=0x06; /* Write enable latch instruction (WREN)*/
 unsigned char WRDI_INST=0x04; /* Write disable latch instruction (WRDI)*/
 unsigned char WRSR_INST=0x01; /* Write status register instruction (WRSR)*/
 unsigned char RDSR_INST=0x05; /* Read status register instruction (RDSR)*/
 unsigned char WRITE_INST=0x02; /* Write memory instruction (WRITE)*/
 unsigned char READ_INST=0x03; /* Write memory instruction (WRITE)*/
 unsigned int BYTE_ADDR=0x55; /* Memory address for byte mode operations*/
 unsigned char BYTE_DATA=0x11; /*Data byte for byte write operation*/
 unsigned int  PAGE_ADDR=0x1F; /* Memory address for page mode operations*/
									 /*ҳ��д�����ʼ��ַ*/
 unsigned char PAGE_DATA1=0x22; /* 1st data byte for page write operation*/
 unsigned char PAGE_DATA2=0x33; /* 2nd data byte for page write operation*/
 unsigned char PAGE_DATA3=0x44; /* 3rd data byte for page write operation*/

 unsigned char INIT_STATE=0x09; /* Initialization value for control ports*/
 unsigned int SLIC=0x30; /* Address location of SLIC*/

char dtimer=1;


/*FM25040�����ӳ���*/
void T_delay_us(unsigned char at)
{
 	 unsigned char a, b;
	for (a = 0; a < at; a++) {
		for (int b= 0; b<200;b++)
		{;}
		}
}

void fm25040_init(void)
{
	DDRB |= (1<<DDRB1); //����PB1Ϊ�������Fm25040��WP������Fm25040��Ӳ��д����
	PORTB |= (1<<PORTB1); //����WP�ߵ�ƽ��ȡ��Fm25040��д����
	DDRB &= ~(1<<DDRB2); //����PB2Ϊ���룬��Fm25040��SO��������Fm25040������
	DDRB |= (1<<DDRB3); //����PB3Ϊ�������Fm25040��CS������Fm25040��оƬʹ�ܶ�
	PORTB |= (1<<PORTB3);	//����CS�ߵ�ƽ��Fm25040��ʹ��
	DDRG |= (1<<DDRG3); //����PG3Ϊ�������Fm25040��SI������дFm25040
	PORTG |= (1<<PORTG3); //����SI�ߵ�ƽ����ʹ�õ�ʱ�����Ϊ�͵�ƽ
	DDRG |= (1<<DDRG4); //����PG4Ϊ�������Fm25040��SCK������Fm25040�Ķ�дʱ��
	PORTG &= ~(1<<PORTG4); //����SCKΪ�͵�ƽ��ֻ���������ݶ�д��ʱ��Ÿı�
	fnWREN();
	fnWRSR(0x00);
}


/*дʹ���ӳ���*/
void wren_cmd(unsigned char chipsel)
{
	PORTB &= ~(1<<PORTB3);  //CS�͵�ƽ
	delayus();	
	PORTG &= ~(1<<PORTG4);  //SCK�͵�ƽ
	delayus();
	fnMOSIByte(0x06);  //����дʹ�ܲ�����
	PORTG &= ~(1<<PORTG4); //��ԭSCK�ߵ�ƽ
	PORTB |= (1<<PORTB3);  //��ԭCS�ߵ�ƽ		
	
}


/*дʹ�ܸ�λ�ӳ���*/
void wrdi_cmd(unsigned char chipsel)
{
 unsigned char aa;
 //SCK=0;/* Bring SCK low */
 PORTB &= ~(1<<3);
 T_delay_us(dtimer);
 PORTG &= ~(1<<4);
 T_delay_us(dtimer);
 //CS=chipsel;/* Bring /CS low */
 
 //delayus(dtimer);
 aa=WRDI_INST;
 outbyt(aa);/* Send WRDI instruction */
 //SCK=0;/* Bring SCK low */
 PORTG &= ~(1<<4);
 //CS=0xff;/* Bring /CS high */
PORTB |= (1<<3);
}


/*д״̬�Ĵ����ӳ���*/

void wrsr_cmd0(unsigned char chipsel)
{
	PORTB &= ~(1<<PORTB3); //CS�͵�ƽ
	PORTG &= ~(1<<PORTG4); //SCK�͵�ƽ
	fnMOSIByte(0x05); //��fm25040дд״̬�Ĵ���������
	fnMOSIByte(0x00); //��fm25040д״̬�Ĵ�������
	PORTG &= ~(1<<PORTG4); //SCK�͵�ƽ
	PORTB |= (1<<PORTB3); //CS�ߵ�ƽ
}

/*��״̬�Ĵ���,���������ݷ��뵽aa��*/
unsigned char rdsr_cmd (unsigned char chipsel)
{
 unsigned char aa;
 //SCK=0;
 
 //CS=chipsel;
// PORTB &= ~(1<<3);
PORTB &= ~(1<<3);
 T_delay_us(dtimer);
 PORTG &= ~(1<<4);//sclk
 T_delay_us(dtimer);
 //aa=RDSR_INST;
 aa = 5;
 outbyt(aa);
 aa=inputbyt();
 //SCK=0;
 PORTG &= ~(1<<4);
 //CS=0xff;
 PORTB |= (1<<3);
 return aa;
}

/*����д��,����
chipsel:Ƭѡ��ַ
*ptr��Դ���ݵ�ַ
length: ���ݳ���
addr:	д����׵�ַ
*/
void sequ_write(unsigned char chipsel,char *ptr,unsigned int addr,unsigned int length)
{
	PORTB &= ~(1<<3);
 	T_delay_us(dtimer);
 	PORTG &= ~(1<<4);//sclk
 	T_delay_us(dtimer);
	fnMOSIByte((unsigned char)(addr>>5)&0x8|WRITE_INST);/* Send WRITE instruction inclu
	ding MSB of address */
	/*����λ��ַ����3λ��д���ȵ������,�õ���ȷ���ȵ���д��FM25040*/
	fnMOSIByte((unsigned char)addr);
	for(int i=0;i<length;i++)
	{
	 fnMOSIByte(*(ptr+i));
	 }
	 PORTG &= ~(1<<4);
	PORTB |= (1<<3);
}

void sequ_read(unsigned char chipsel,unsigned char *data_restore_addr,unsigned int read_start_addr,unsigned int read_length)
{
 unsigned int i;
 PORTB &= ~(1<<3);
 T_delay_us(dtimer);
 PORTG &= ~(1<<4);//sclk
 T_delay_us(dtimer);
  
 outbyt(((unsigned char)(read_start_addr>>5)&0x8)|READ_INST);
  
 outbyt((unsigned char)(read_start_addr));
 for (i=0;i<read_length;i++)
 {
  *(data_restore_addr+i)=inputbyt();
 }
 //SCK=0;
 PORTG &= ~(1<<4);
 //CS=0xff;
 PORTB |= (1<<3);
}

/*���һ�����ݵ�FM25040,�����ݿ���Ϊ��ַ,�ȵ���,д������ݵ�*/
void outbyt(unsigned char byData)
{
	for(int i=0; i<8; i++)
	{
		PORTG = (byData&0x80) ? (PORTG | (1<<PORTG3)) : (PORTG & ~(1<<PORTG3));  //ȡbyData���λ
		byData <<= 1;
		T_delay_us(2);
		PORTG |= (1<<PORTG4);  //SCK�����زɼ���ƽ
		T_delay_us(2);
		PORTG &= ~(1<<PORTG4);  //SCK�½��أ�Ϊ�´βɼ���ƽ��׼��
		T_delay_us(2);
	}
}

/*�õ�һ������,�����ݿ���Ϊ״̬�Ĵ�������,�����ĵ�Ԫ���ݵ�*/
unsigned char inputbyt(void)
{

	unsigned char byData = 0;
	for(int i=0; i<8; i++)
	{
		PORTG |= (1<<PORTG4);  //SCK�����أ��ɼ�����
		T_delay_us(2);
		byData<<=1;
		byData |= (PINB&0x04)?1:0;
		T_delay_us(2);
		PORTG &= ~(1<<PORTG4);
		T_delay_us(2);
	}
	return byData;
}

