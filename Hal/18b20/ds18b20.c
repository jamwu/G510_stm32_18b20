#include "ds18b20.h"
#include "delay.h"	
  
void DS18B20_Mode_Out_PP(void)
{
    GPIO_InitTypeDef myGPIO_InitStruct;
    myGPIO_InitStruct.Pin = temperature_Pin;
    myGPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    myGPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(temperature_GPIO_Port, &myGPIO_InitStruct);
}

void DS18B20_Mode_IPU(void)
{
    GPIO_InitTypeDef myGPIO_InitStruct;
    myGPIO_InitStruct.Pin = temperature_Pin;
    myGPIO_InitStruct.Pull = GPIO_PULLUP;
    myGPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    myGPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(temperature_GPIO_Port, &myGPIO_InitStruct);
}
void DS18B20_Rst(void)
{
	/* ��������Ϊ������� */
	DS18B20_Mode_Out_PP();
	
	DS18B20_DQ_OUT_0;
	/* �������ٲ���480us�ĵ͵�ƽ��λ�ź� */
	//Delay_us(750);
	delay_us(750);
	/* �����ڲ�����λ�źź��轫�������� */
	//DS18B20_DATA_OUT(HIGH);
	DS18B20_DQ_OUT_1;
	/*�ӻ����յ������ĸ�λ�źź󣬻���15~60us���������һ����������*/
	delay_us(15);
}
/*
 * ���ӻ����������صĴ�������
 * 0���ɹ�
 * 1��ʧ��
 */
static uint8_t DS18B20_Presence(void)
{
	uint8_t pulse_time = 0;
	
	/* ��������Ϊ�������� */
	DS18B20_Mode_IPU();
	
	/* �ȴ���������ĵ�������������Ϊһ��60~240us�ĵ͵�ƽ�ź� 
	 * �����������û����������ʱ�����ӻ����յ������ĸ�λ�źź󣬻���15~60us���������һ����������
	 */
	while( DS18B20_DQ_IN&& pulse_time<100 )
	{
		pulse_time++;
		delay_us(1);
	}	
	/* ����100us�󣬴������嶼��û�е���*/
	if( pulse_time >=100 )
		return 1;
	else
		pulse_time = 0;
	
	/* �������嵽�����Ҵ��ڵ�ʱ�䲻�ܳ���240us */
	while( !DS18B20_DQ_IN&& pulse_time<240 )
	{
		pulse_time++;
		delay_us(1);
	}	
	if( pulse_time >=240 )
		return 1;
	else
		return 0;
}


/*
 * ��DS18B20��ȡһ��bit
 */
uint8_t DS18B20_Read_Bit(void)
{
	uint8_t dat;
	
	/* ��0�Ͷ�1��ʱ������Ҫ����60us */	
	DS18B20_Mode_Out_PP();
	/* ��ʱ�����ʼ���������������� >1us <15us �ĵ͵�ƽ�ź� */
	DS18B20_DQ_OUT_0;
	delay_us(10);
	
	/* ���ó����룬�ͷ����ߣ����ⲿ�������轫�������� */
	DS18B20_Mode_IPU();
	//Delay_us(2);
	
	if( DS18B20_DQ_IN == SET )
		dat = 1;
	else
		dat = 0;
	
	/* �����ʱ������ο�ʱ��ͼ */
	delay_us(45);
	
	return dat;
}

/*
 * ��DS18B20��һ���ֽڣ���λ����
 */
uint8_t DS18B20_Read_Byte(void)
{
	uint8_t i, j, dat = 0;	
	
	for(i=0; i<8; i++) 
	{
		j = DS18B20_Read_Bit();		
		dat = (dat) | (j<<i);
	}
	
	return dat;
}

/*
 * дһ���ֽڵ�DS18B20����λ����
 */
void DS18B20_Write_Byte(uint8_t dat)
{
	uint8_t i, testb;
	DS18B20_Mode_Out_PP();
	
	for( i=0; i<8; i++ )
	{
		testb = dat&0x01;
		dat = dat>>1;		
		/* д0��д1��ʱ������Ҫ����60us */
		if (testb)
		{			
			DS18B20_DQ_OUT_0;
			/* 1us < �����ʱ < 15us */
			delay_us(8);
			
			DS18B20_DQ_OUT_1;
			delay_us(58);
		}		
		else
		{			
			DS18B20_DQ_OUT_0;
			/* 60us < Tx 0 < 120us */
			delay_us(70);
			
			DS18B20_DQ_OUT_1;			
			/* 1us < Trec(�ָ�ʱ��) < �����*/
			delay_us(2);
		}
	}
}

void DS18B20_Start(void)
{
	DS18B20_Rst();	   
	DS18B20_Presence();	 
	DS18B20_Write_Byte(0XCC);		/* ���� ROM */
	DS18B20_Write_Byte(0X44);		/* ��ʼת�� */
}

uint8_t DS18B20_Init(void)
{
	//DS18B20_GPIO_Config();
	void DS18B20_Mode_Out_PP();
	DS18B20_DQ_OUT_1;
	DS18B20_Rst();
	
	return DS18B20_Presence();
}

/*
 * �洢���¶���16 λ�Ĵ�������չ�Ķ����Ʋ�����ʽ
 * ��������12λ�ֱ���ʱ������5������λ��7������λ��4��С��λ
 *
 *         |---------����----------|-----С�� �ֱ��� 1/(2^4)=0.0625----|
 * ���ֽ�  | 2^3 | 2^2 | 2^1 | 2^0 | 2^(-1) | 2^(-2) | 2^(-3) | 2^(-4) |
 *
 *
 *         |-----����λ��0->��  1->��-------|-----------����-----------|
 * ���ֽ�  |  s  |  s  |  s  |  s  |    s   |   2^6  |   2^5  |   2^4  |
 *
 * 
 * �¶� = ����λ + ���� + С��*0.0625
 */
float DS18B20_Get_Temp(void)
{
	uint8_t tpmsb, tplsb;
	short s_tem;
	float f_tem;
	
	DS18B20_Rst();	   
	DS18B20_Presence();	 
	DS18B20_Write_Byte(0XCC);				/* ���� ROM */
	DS18B20_Write_Byte(0X44);				/* ��ʼת�� */
	
	DS18B20_Rst();
  DS18B20_Presence();
	DS18B20_Write_Byte(0XCC);				/* ���� ROM */
  DS18B20_Write_Byte(0XBE);				/* ���¶�ֵ */
	
	tplsb = DS18B20_Read_Byte();		 
	tpmsb = DS18B20_Read_Byte(); 
	
	s_tem = tpmsb<<8;
	s_tem = s_tem | tplsb;
	
	if( s_tem < 0 )		/* ���¶� */
		f_tem = (~s_tem+1) * 0.0625;	
	else
		f_tem = s_tem * 0.0625;
	
	return f_tem; 	
}


