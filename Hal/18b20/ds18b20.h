#ifndef __DS18B20_H_
#define __DS18B20_H_
#include <stdio.h>
#include <stm32f1xx_hal.h>
#include "main.h"

//IO��������
//#define DS18B20_IO_IN()  {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=8<<12;}
//#define DS18B20_IO_OUT() {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=3<<12;}
////IO��������											   
#define	DS18B20_DQ_OUT_1	 HAL_GPIO_WritePin(temperature_GPIO_Port, temperature_Pin, GPIO_PIN_SET)
#define	DS18B20_DQ_OUT_0 	 HAL_GPIO_WritePin(temperature_GPIO_Port, temperature_Pin, GPIO_PIN_RESET)
#define	DS18B20_DQ_IN  HAL_GPIO_ReadPin(temperature_GPIO_Port, temperature_Pin)  //���ݶ˿�	Pb1
   	
unsigned char DS18B20_Init(void);			//��ʼ��DS18B20
float DS18B20_Get_Temp(void);	//��ȡ�¶�
void DS18B20_Start(void);		//��ʼ�¶�ת��
void DS18B20_Write_Byte(unsigned char dat);//д��һ���ֽ�
unsigned char DS18B20_Read_Byte(void);		//����һ���ֽ�
unsigned char DS18B20_Read_Bit(void);		//����һ��λ
unsigned char DS18B20_Check(void);			//����Ƿ����DS18B20
void DS18B20_Rst(void);			//��λDS18B20    
#endif















