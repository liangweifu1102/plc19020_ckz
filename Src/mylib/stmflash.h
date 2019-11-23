#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "sys.h"   
 


//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ
//FLASH������ֵ
#define FLASH_KEY1               0X45670123
#define FLASH_KEY2               0XCDEF89AB
//#define FLASH_KEY1               ((uint32_t)0x45670123)
//#define FLASH_KEY2               ((uint32_t)0xCDEF89AB)
//FLASH ��������ʼ��ַ
#define ADDR_FLASH_SECTOR_0     ((u32)0x08000000) 	//����0��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((u32)0x08004000) 	//����1��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((u32)0x08008000) 	//����2��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((u32)0x0800C000) 	//����3��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((u32)0x08010000) 	//����4��ʼ��ַ, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((u32)0x08020000) 	//����5��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((u32)0x08040000) 	//����6��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((u32)0x08060000) 	//����7��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_8     ((u32)0x08080000) 	//����8��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((u32)0x080A0000) 	//����9��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((u32)0x080C0000) 	//����10��ʼ��ַ,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((u32)0x080E0000) 	//����11��ʼ��ַ,128 Kbytes 

#define ADDR_FLASH_SECTOR_12	((u32)0x08100000) 	//����12��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_13	((u32)0x08104000) 	//����13��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_14    ((u32)0x08108000) 	//����14��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_15	((u32)0x0810C000) 	//����15��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_16    ((u32)0x08110000) 	//����16��ʼ��ַ, 64 Kbytes  
#define ADDR_FLASH_SECTOR_17	((u32)0x08120000) 	//����17��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_18	((u32)0x08140000) 	//����18��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_19	((u32)0x08160000) 	//����19��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_20    ((u32)0x08180000) 	//����20��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_21	((u32)0x081A0000) 	//����21��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_22    ((u32)0x081C0000) 	//����22��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_23    ((u32)0x081E0000) 	//����23��ʼ��ַ, 128 Kbytes   


void STMFLASH_Unlock(void);					//FLASH����
void STMFLASH_Lock(void);				 	//FLASH����
u8 STMFLASH_GetStatus(void);				//���״̬
u8 STMFLASH_WaitDone(u32 time);				//�ȴ���������
u8 STMFLASH_ErasePage(u32 sectoraddr);	 	//����ҳ
u8 STMFLASH_WriteWord(u32 faddr, u32 dat);	//д����
u32 STMFLASH_ReadWord(u32 faddr);		  	//������  
void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//ָ����ַ��ʼд��ָ�����ȵ����� 
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����
int STMFLASH_Compare(u32 ReadAddr,u32 *pBuffer,u32 NumToRead);          //�Ƚ���д�����ȡ�Ƿ�����

//����д��
void Test_Write(u32 WriteAddr,u32 WriteData);				

extern u8 STMFLASH_EraseSector(u32 sectoraddr);
extern u8 STMFLASH_GetFlashSector(u32 addr);

#endif
















