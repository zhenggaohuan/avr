/*这里我们需要定义各种数据在FM25040中的存储位置
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

 //sbit SO=P4^5;             /*FM25040输出 pin 2*/
 //sbit SI=P3^5;             /*FM25040输入 pin 5*/
 //sbit SCK=P4^7;            /*FM25040时钟 pin 6*/
 unsigned char WREN_INST=0x06; /* Write enable latch instruction (WREN)*/
 unsigned char WRDI_INST=0x04; /* Write disable latch instruction (WRDI)*/
 unsigned char WRSR_INST=0x01; /* Write status register instruction (WRSR)*/
 unsigned char RDSR_INST=0x05; /* Read status register instruction (RDSR)*/
 unsigned char WRITE_INST=0x02; /* Write memory instruction (WRITE)*/
 unsigned char READ_INST=0x03; /* Write memory instruction (WRITE)*/
 unsigned int BYTE_ADDR=0x55; /* Memory address for byte mode operations*/
 unsigned char BYTE_DATA=0x11; /*Data byte for byte write operation*/
 unsigned int  PAGE_ADDR=0x1F; /* Memory address for page mode operations*/
									 /*页面写入的其始地址*/
 unsigned char PAGE_DATA1=0x22; /* 1st data byte for page write operation*/
 unsigned char PAGE_DATA2=0x33; /* 2nd data byte for page write operation*/
 unsigned char PAGE_DATA3=0x44; /* 3rd data byte for page write operation*/

 unsigned char INIT_STATE=0x09; /* Initialization value for control ports*/
 unsigned int SLIC=0x30; /* Address location of SLIC*/

char dtimer=1;


/*FM25040操作子程序集*/
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
	DDRB |= (1<<DDRB1); //设置PB1为输出，与Fm25040的WP相连，Fm25040的硬件写保护
	PORTB |= (1<<PORTB1); //设置WP高电平，取消Fm25040的写保护
	DDRB &= ~(1<<DDRB2); //设置PB2为输入，与Fm25040的SO相连，读Fm25040的数据
	DDRB |= (1<<DDRB3); //设置PB3为输出，与Fm25040的CS相连，Fm25040的芯片使能端
	PORTB |= (1<<PORTB3);	//设置CS高电平，Fm25040不使能
	DDRG |= (1<<DDRG3); //设置PG3为输出，与Fm25040的SI相连，写Fm25040
	PORTG |= (1<<PORTG3); //设置SI高电平，在使用的时候才置为低电平
	DDRG |= (1<<DDRG4); //设置PG4为输出，与Fm25040的SCK相连，Fm25040的读写时钟
	PORTG &= ~(1<<PORTG4); //设置SCK为低电平，只有在有数据读写的时候才改变
	fnWREN();
	fnWRSR(0x00);
}


/*写使能子程序*/
void wren_cmd(unsigned char chipsel)
{
	PORTB &= ~(1<<PORTB3);  //CS低电平
	delayus();	
	PORTG &= ~(1<<PORTG4);  //SCK低电平
	delayus();
	fnMOSIByte(0x06);  //输入写使能操作码
	PORTG &= ~(1<<PORTG4); //还原SCK高电平
	PORTB |= (1<<PORTB3);  //还原CS高电平		
	
}


/*写使能复位子程序*/
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


/*写状态寄存器子程序*/

void wrsr_cmd0(unsigned char chipsel)
{
	PORTB &= ~(1<<PORTB3); //CS低电平
	PORTG &= ~(1<<PORTG4); //SCK低电平
	fnMOSIByte(0x05); //往fm25040写写状态寄存器操作码
	fnMOSIByte(0x00); //往fm25040写状态寄存器内容
	PORTG &= ~(1<<PORTG4); //SCK低电平
	PORTB |= (1<<PORTB3); //CS高电平
}

/*读状态寄存器,读出的数据放入到aa中*/
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

/*连续写入,其中
chipsel:片选地址
*ptr：源数据地址
length: 数据长度
addr:	写入的首地址
*/
void sequ_write(unsigned char chipsel,char *ptr,unsigned int addr,unsigned int length)
{
	PORTB &= ~(1<<3);
 	T_delay_us(dtimer);
 	PORTG &= ~(1<<4);//sclk
 	T_delay_us(dtimer);
	fnMOSIByte((unsigned char)(addr>>5)&0x8|WRITE_INST);/* Send WRITE instruction inclu
	ding MSB of address */
	/*将高位地址左移3位与写入先导字相或,得到正确的先导字写入FM25040*/
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

/*输出一个数据到FM25040,此数据可能为地址,先导字,写入的数据等*/
void outbyt(unsigned char byData)
{
	for(int i=0; i<8; i++)
	{
		PORTG = (byData&0x80) ? (PORTG | (1<<PORTG3)) : (PORTG & ~(1<<PORTG3));  //取byData最高位
		byData <<= 1;
		T_delay_us(2);
		PORTG |= (1<<PORTG4);  //SCK上升沿采集电平
		T_delay_us(2);
		PORTG &= ~(1<<PORTG4);  //SCK下降沿，为下次采集电平做准备
		T_delay_us(2);
	}
}

/*得到一个数据,此数据可能为状态寄存器数据,读出的单元数据等*/
unsigned char inputbyt(void)
{

	unsigned char byData = 0;
	for(int i=0; i<8; i++)
	{
		PORTG |= (1<<PORTG4);  //SCK上升沿，采集数据
		T_delay_us(2);
		byData<<=1;
		byData |= (PINB&0x04)?1:0;
		T_delay_us(2);
		PORTG &= ~(1<<PORTG4);
		T_delay_us(2);
	}
	return byData;
}

