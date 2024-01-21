#ifndef __FM25040_H 
#define __FM25040_H
 //void sequ_write(unsigned char chipsel,unsigned int *ptr,unsigned int addr,unsigned int length);
 //void sequ_read(unsigned char chipsel,unsigned char *data_restore_addr,unsigned int read_start_addr,unsigned int read_length);
void fm25040_init(void);
void wren_cmd(unsigned char chipsel);/*写使能子程序*/
void wrdi_cmd(unsigned char chipsel);/*写使能复位*/
 void wrsr_cmd1(unsigned char chipsel);/*复位时间位和数据保护位写入状态寄存器*/
 void wrsr_cmd0(unsigned char chipsel);
 unsigned char rdsr_cmd(unsigned char chipsel);/*读状态寄存器*/
 void sequ_write(unsigned char chipsel,char *ptr,unsigned int addr,unsigned int length);
 void sequ_read(unsigned char chipsel,unsigned char *data_restore_addr,unsigned int read_start_addr,unsigned int read_length);
 void outbyt(unsigned char aa);/*输出一个字节到FM25040中，不包括先导字等*/
 unsigned char inputbyt(void);/*由FM25040输入一个字节，不包括先导字等额外的东西*/
 void get_value_from_fm25040(void);//从EEPROM中获取当前存储的值

 void set_baud_to_fm25040(void);
 int get_baud_from_fm25040(void);
 void set_ri_to_fm25040(void);
 int get_ri_from_fm25040(void);
 void set_ru_to_fm25040(void);
 int get_ru_from_fm25040(void);
 void set_iscl_to_fm25040(void);
 int get_iscl_from_fm25040(void);
 void set_vscl_to_fm25040(void);
 int get_vscl_from_fm25040(void);
 int get_net_from_fm25040(void);
 void set_net_to_fm25040(void);
 int get_code_value_from_fm25040(void);
 void set_code_value_to_fm25040(void);
 void set_value_to_fm25040(void);
 void get_related_value_from_fm25040(void);
void write_32_bit_to_fm25040(unsigned int flag, unsigned long int value);
unsigned long int read_32_bit_from_fm25040(unsigned int flag);

 #define FM25040_CODE_VALUE_FLAG	0
 #define FM25040_CODE_VALUE_FLAG_VALUE	0x10
 #define FM25040_CODE_VALUE_LOW	1
 #define FM25040_CODE_VALUE_HIGH	2
 #define FM25040_NET_FLAG 3
 #define FM25040_NET_FLAG_VALUE	0x20
 #define FM_25040_NET	4
 #define FM_25040_VSCL_FLAG 5
 #define FM_25040_VSCL_FLAG_VALUE 0x30
 #define FM_25040_VSCL 6
 #define FM_25040_ISCL_FLAG 7
 #define FM_25040_ISCL_FLAG_VALUE 0x40
 #define FM_25040_ISCL	8
 #define FM_25040_R_U_FLAG 9
 #define FM_25040_R_U_FLAG_VALUE 0x50
 #define FM_25040_R_U_LOW 10
 #define FM_25040_R_U_HIGH 11
 #define FM_25040_R_I_FLAG 12
 #define FM_25040_R_I_FLAG_VALUE 0x60
 #define FM_25040_R_I_HIGH 14
 #define FM_25040_R_I_LOW 13
 #define FM_25040_BAUD_FLAG 15
  #define FM_25040_BAUD_FLAG_VALUE 0x70
 #define FM_25040_BAUD 16
 #define FM_25040_DISP_FLAG 17
 #define FM_25040_DISP_FLAG_VALUE 0x80
 #define FM_25040_DISP 18
 #define FM_25040_VOL_A 19
 #define FM_25040_VOL_B FM_25040_VOL_A + 4
 #define FM_25040_VOL_C FM_25040_VOL_A + 8
 #define FM_25040_CUR_A FM_25040_VOL_A + 12
 #define FM_25040_CUR_B FM_25040_VOL_A + 16
 #define FM_25040_CUR_C FM_25040_VOL_A + 20
#endif