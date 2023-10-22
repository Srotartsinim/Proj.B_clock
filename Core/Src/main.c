/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "bmp.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LM75AD_devAddr 0x90 //LM75AD器件地址
#define LM75AD_TempReg 0x00 //温度寄存�?(只读，包�?2个字节的温度数据)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int Num=0,Cnt=0;
int bo_ol=0,percent=0,percentward=0,page=0,jz1=32,click=0,Menu=0,still=0,_f=0;
uint8_t tmp=0;
int thn=0,tmn=0,tsn=0,ts[3],ck[3],sec[3]={0,0,0},timesum=0,temsign=0,tem=0,bltst=0;
void Alarm_Start()
{
	HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_1);
	page =5;
	OLED_Clear();
	//OLED_ShowChinese(jz1,0,0,16,1);
		//	OLED_ShowChinese(jz1+16,0,1,16,1);
			OLED_ShowChinese(jz1+16+16,0,2,16,1);
			OLED_ShowChinese(jz1+16+16+16,0,3,16,1);
			OLED_ShowNum(14,16,thn,2,24,1);
			OLED_ShowString(40,16,":",24,1);
			OLED_ShowNum(56,16,tmn,2,24,1);
			OLED_ShowString(80,16,":",24,1);
			OLED_ShowNum(92,16,tsn,2,24,1);
			OLED_ShowChinese(jz1-16-8,40,29,16,1);
			OLED_ShowChinese(jz1-8,40,30,16,1);
			OLED_ShowChinese(jz1+8,40,31,16,1);
			OLED_ShowChinese(jz1+8+16,40,32,16,1);
		OLED_ShowChinese(jz1+8+16+16,40,34,16,1);
		OLED_ShowChinese(jz1+8+16+32,40,36,16,1);
		OLED_ShowChinese(jz1+8+32+32,40,37,16,1);
		OLED_Refresh();
	while(!click)
	{
	HAL_GPIO_TogglePin(GPIOA,BEEP1_Pin);
	//HAL_GPIO_TogglePin(GPIOA,PIN0);
	HAL_Delay(500/_f);
	}
	click = 0;
	if(HAL_GPIO_ReadPin(GPIOA,BEEP1_Pin) == GPIO_PIN_SET)
		HAL_GPIO_TogglePin(GPIOA,BEEP1_Pin);
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0) == GPIO_PIN_SET)
		HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_0);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
	page = 0;
	OLED_Clear();
	return;
}
void LM75AD_GetTemperature(void)
{
	 /*
    Temp register
    -------------------------------------------------------------
    MSByte                         |LSByte
    7   6   5   4   3   2   1   0  |7   6  5   4   3   2    1   0
    D10 D9 D8  D7  D6  D5  D4  D3  |D2  D1 D0  X   X   X    X   X
    -------------------------------------------------------------
    D10=0温度�?+,D10=1温度�?-
    */
		uint8_t temreg[2];
		HAL_I2C_Mem_Read(&hi2c1,LM75AD_devAddr,LM75AD_TempReg,I2C_MEMADD_SIZE_8BIT,temreg,2,100);
		tem = ((temreg[0] << 8) | temreg[1]);
		if ((temreg[0] & 0x80) != 0)
    {
        tem = ~(tem) + 1;
        temsign =1;
    }
    tem >>= 5;
    tem = tem * 0.125f*10;
	//	tem*=100;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_TIM5_Init();
  MX_I2C1_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
	OLED_Init();
	OLED_Clear();
	OLED_ShowPicture(0,0,128,64,BMP1,1);
	OLED_Refresh();
	HAL_Delay(5000);
	OLED_Clear();
//	OLED_ShowPicture(0,0,128,64,BMP2,1);
	OLED_Refresh();
//	HAL_Delay(5000);
	OLED_ShowPicture(0,0,7,11,BMP3,1);
	HAL_UART_Transmit(&huart2,"A控制当前时间，B控制闹钟，C秒增加，D分钟增加，E小时增加，F秒数减少，G分钟减少，H小时减少，I结束本轮控制，每次控制以A/B开始，以I结束",190,HAL_MAX_DELAY);
	HAL_UART_Receive_IT(&huart2,&tmp,1);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
	HAL_TIM_Base_Start_IT(&htim5);
	HAL_TIM_Base_Start_IT(&htim4);
	//OLED_Clear();
	//OLED_Refresh();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//		OLED_ShowString(1,1,"Num=",16,1);
//		OLED_ShowNum(32,1,Num,5,16,1);
//		OLED_ShowString(1,16,"Cnt=",16,1);
//		OLED_ShowNum(32,16,Cnt,5,16,1);
//		OLED_ShowString(1,32,"Pct=",16,1);
//		OLED_ShowNum(32,32,percent,5,16,1);
//		OLED_ShowString(1,48,"Ptd=",16,1);
//		OLED_ShowNum(32,48,percentward,5,16,1);
//		OLED_Refresh();
		switch(page)
		{
			case 0:
			OLED_ShowChinese(jz1,0,0,16,1);
			OLED_ShowChinese(jz1+16,0,1,16,1);
			OLED_ShowChinese(jz1+16+16,0,2,16,1);
			OLED_ShowChinese(jz1+16+16+16,0,3,16,1);
			OLED_ShowNum(14,16,thn,2,24,1);
			OLED_ShowString(40,16,":",24,1);
			OLED_ShowNum(56,16,tmn,2,24,1);
			OLED_ShowString(80,16,":",24,1);
			OLED_ShowNum(92,16,tsn,2,24,1);
//			OLED_ShowChinese(jz1-16-8,40-8,5,16,1);
//			OLED_ShowChinese(jz1-16-8+16-8,40,6,16,1);
			OLED_ShowChinese(jz1-16+16+16+32,40,4,16,1);
			OLED_ShowNum(jz1+16-8,40,tem/10,2,16,1);
			OLED_ShowString(jz1+16+8,40,".",16,1);
			OLED_ShowNum(jz1+16+16,40,tem%10,1,16,1);
			OLED_ShowChinese(96,48,7,16,0);
			OLED_ShowChinese(96+16,48,8,16,0);
//			OLED_ShowNum(0,0,tem,8,16,1);
			LM75AD_GetTemperature();
			if(temsign)
				OLED_ShowString(jz1-16+16,40,"-",16,1);
			else
				OLED_ShowString(jz1-16+16,40,"+",16,1);
			OLED_Refresh();
			if(click)
			{
			page = 1;
			click = 0;
			OLED_ShowChinese(jz1,0,0,16,1);
			OLED_Clear();
			OLED_Refresh();
			}
			break;
			case 1:
				OLED_ShowChinese(jz1,0,7,16,1);
				OLED_ShowChinese(jz1+48,0,8,16,1);
				switch(Menu)
				{
					case 0:
				OLED_ShowChinese(0,16,9,16,0);
				OLED_ShowChinese(16,16,2,16,0);
				OLED_ShowChinese(32,16,2,16,0);
				OLED_ShowChinese(48,16,3,16,0);
				OLED_ShowChinese(64,16,10,16,0);
				OLED_ShowChinese(80,16,11,16,0);
				OLED_ShowChinese(0,32,12,16,1);
				OLED_ShowChinese(16,32,13,16,1);
				OLED_ShowChinese(32,32,2,16,1);
				OLED_ShowChinese(48,32,3,16,1);
				OLED_ShowChinese(64,32,10,16,1);
				OLED_ShowChinese(80,32,11,16,1);
				OLED_ShowChinese(0,48,17,16,1);
				OLED_ShowChinese(16,48,18,16,1);
				OLED_ShowChinese(96+0,48,14,16,1);
				OLED_ShowChinese(96+16,48,15,16,1);
				OLED_Refresh();
					break;
					case 1:
				OLED_ShowChinese(0,16,9,16,1);
				OLED_ShowChinese(16,16,2,16,1);
				OLED_ShowChinese(32,16,2,16,1);
				OLED_ShowChinese(48,16,3,16,1);
				OLED_ShowChinese(64,16,10,16,1);
				OLED_ShowChinese(80,16,11,16,1);
				OLED_ShowChinese(0,32,12,16,0);
				OLED_ShowChinese(16,32,13,16,0);
				OLED_ShowChinese(32,32,2,16,0);
				OLED_ShowChinese(48,32,3,16,0);
				OLED_ShowChinese(64,32,10,16,0);
				OLED_ShowChinese(80,32,11,16,0);
				OLED_ShowChinese(0,48,17,16,1);
				OLED_ShowChinese(16,48,18,16,1);
				OLED_ShowChinese(96+0,48,14,16,1);
				OLED_ShowChinese(96+16,48,15,16,1);
				OLED_Refresh();
					break;
					case 2:
				OLED_ShowChinese(0,16,9,16,1);
				OLED_ShowChinese(16,16,2,16,1);
				OLED_ShowChinese(32,16,2,16,1);
				OLED_ShowChinese(48,16,3,16,1);
				OLED_ShowChinese(64,16,10,16,1);
				OLED_ShowChinese(80,16,11,16,1);
				OLED_ShowChinese(0,32,12,16,1);
				OLED_ShowChinese(16,32,13,16,1);
				OLED_ShowChinese(32,32,2,16,1);
				OLED_ShowChinese(48,32,3,16,1);
				OLED_ShowChinese(64,32,10,16,1);
				OLED_ShowChinese(80,32,11,16,1);
				OLED_ShowChinese(0,48,17,16,0);
				OLED_ShowChinese(16,48,18,16,0);
				OLED_ShowChinese(96+0,48,14,16,1);
				OLED_ShowChinese(96+16,48,15,16,1);
				OLED_Refresh();
					break;
					case 3:
				OLED_ShowChinese(0,16,9,16,1);
				OLED_ShowChinese(16,16,2,16,1);
				OLED_ShowChinese(32,16,2,16,1);
				OLED_ShowChinese(48,16,3,16,1);
				OLED_ShowChinese(64,16,10,16,1);
				OLED_ShowChinese(80,16,11,16,1);
				OLED_ShowChinese(0,32,12,16,1);
				OLED_ShowChinese(16,32,13,16,1);
				OLED_ShowChinese(32,32,2,16,1);
				OLED_ShowChinese(48,32,3,16,1);
				OLED_ShowChinese(64,32,10,16,1);
				OLED_ShowChinese(80,32,11,16,1);
				OLED_ShowChinese(0,48,17,16,1);
				OLED_ShowChinese(16,48,18,16,1);
				OLED_ShowChinese(96+0,48,14,16,0);
				OLED_ShowChinese(96+16,48,15,16,0);
				OLED_Refresh();
					break;
				}
			if(click)
		{
			click = 0;
			switch(Menu)
			{
				case 0:
					page = 2;
					ts[0] = thn;ts[1] = tmn;ts[2] = tsn;
					break;
				case 1:
					page = 3;
					ts[0] = ck[0];ts[1] = ck[1];ts[2] = ck[2];
					break;
				case 2:
					page = 4;
					break;
				case 3:
					page = 0;
			}
			Menu = 0;
			OLED_Clear();
			OLED_Refresh();
		}
			break;
			case 2://zoushisheding
				OLED_ShowChinese(16,0,9,16,1);
				OLED_ShowChinese(16+16,0,2,16,1);
				OLED_ShowChinese(16+32,0,2,16,1);
				OLED_ShowChinese(16+48,0,3,16,1);
				OLED_ShowChinese(16+64,0,10,16,1);
				OLED_ShowChinese(16+80,0,11,16,1);
				if(click && (Menu != 3))
				{
						still = !still;
						click = 0;
				}
				switch(Menu)
				{
					case 0:
					OLED_ShowNum(14,16,ts[0],2,24,0);
					OLED_ShowString(40,16,":",24,1);
					OLED_ShowNum(56,16,ts[1],2,24,1);
					OLED_ShowString(80,16,":",24,1);
					OLED_ShowNum(92,16,ts[2],2,24,1);
					OLED_ShowChinese(0,48,16,16,1);
					OLED_ShowChinese(16,48,11,16,1);
					OLED_Refresh();
					break;
					case 1:
					OLED_ShowNum(14,16,ts[0],2,24,1);
					OLED_ShowString(40,16,":",24,1);
					OLED_ShowNum(56,16,ts[1],2,24,0);
					OLED_ShowString(80,16,":",24,1);
					OLED_ShowNum(92,16,ts[2],2,24,1);
					OLED_ShowChinese(0,48,16,16,1);
					OLED_ShowChinese(16,48,11,16,1);
					OLED_Refresh();
					break;
					case 2:
					OLED_ShowNum(14,16,ts[0],2,24,1);
					OLED_ShowString(40,16,":",24,1);
					OLED_ShowNum(56,16,ts[1],2,24,1);
					OLED_ShowString(80,16,":",24,1);
					OLED_ShowNum(92,16,ts[2],2,24,0);
					OLED_ShowChinese(0,48,16,16,1);
					OLED_ShowChinese(16,48,11,16,1);
					OLED_Refresh();
					break;
					case 3:
					OLED_ShowNum(14,16,ts[0],2,24,1);
					OLED_ShowString(40,16,":",24,1);
					OLED_ShowNum(56,16,ts[1],2,24,1);
					OLED_ShowString(80,16,":",24,1);
					OLED_ShowNum(92,16,ts[2],2,24,1);
					OLED_ShowChinese(0,48,16,16,0);
					OLED_ShowChinese(16,48,11,16,0);
					OLED_Refresh();
					if(click)
					{
						click = 0;
						thn=ts[0];tmn=ts[1];tsn=ts[2];
						page = 1;
						Menu =0;
						OLED_Clear();
						OLED_Refresh();
					}
					break;
				}
			break;
			case 3://闹钟设定
				OLED_ShowChinese(16,0,12,16,1);
				OLED_ShowChinese(16+16,0,13,16,1);
				OLED_ShowChinese(16+32,0,2,16,1);
				OLED_ShowChinese(16+48,0,3,16,1);
				OLED_ShowChinese(16+64,0,10,16,1);
				OLED_ShowChinese(16+80,0,11,16,1);
				OLED_Refresh();
				if(click && (Menu != 4))
				{
						still = !still;
						click = 0;
				}
				switch(Menu)
				{
					case 0:
					OLED_ShowNum(14,16,ts[0],2,24,0);
					OLED_ShowString(40,16,":",24,1);
					OLED_ShowNum(56,16,ts[1],2,24,1);
					OLED_ShowString(80,16,":",24,1);
					OLED_ShowNum(92,16,ts[2],2,24,1);
					OLED_ShowChinese(96+0,48,16,16,1);
					OLED_ShowChinese(96+16,48,11,16,1);
					if(_f)
					{
					OLED_ShowChinese(0,48,19,16,1);
					OLED_ShowChinese(16,48,20,16,1);
					OLED_ShowNum(32,48,_f,2,16,1);
					OLED_ShowString(48,48,"Hz",16,1);
					}
					else
					{
					OLED_ShowChinese(0,48,12,16,1);
					OLED_ShowChinese(16,48,13,16,1);
					OLED_ShowChinese(32,48,21,16,1);
					OLED_ShowChinese(48,48,22,16,1);
					}
					OLED_Refresh();
					break;
					case 1:
					OLED_ShowNum(14,16,ts[0],2,24,1);
					OLED_ShowString(40,16,":",24,1);
					OLED_ShowNum(56,16,ts[1],2,24,0);
					OLED_ShowString(80,16,":",24,1);
					OLED_ShowNum(92,16,ts[2],2,24,1);
					OLED_ShowChinese(96+0,48,16,16,1);
					OLED_ShowChinese(96+16,48,11,16,1);
					if(_f)
					{
					OLED_ShowChinese(0,48,19,16,1);
					OLED_ShowChinese(16,48,20,16,1);
					OLED_ShowNum(32,48,_f,2,16,1);
					OLED_ShowString(48,48,"Hz",16,1);
					}
					else
					{
					OLED_ShowChinese(0,48,12,16,1);
					OLED_ShowChinese(16,48,13,16,1);
					OLED_ShowChinese(32,48,21,16,1);
					OLED_ShowChinese(48,48,22,16,1);
					}
					OLED_Refresh();
					break;
					case 2:
					OLED_ShowNum(14,16,ts[0],2,24,1);
					OLED_ShowString(40,16,":",24,1);
					OLED_ShowNum(56,16,ts[1],2,24,1);
					OLED_ShowString(80,16,":",24,1);
					OLED_ShowNum(92,16,ts[2],2,24,0);
					OLED_ShowChinese(96+0,48,16,16,1);
					OLED_ShowChinese(96+16,48,11,16,1);
					if(_f)
					{
					OLED_ShowChinese(0,48,19,16,1);
					OLED_ShowChinese(16,48,20,16,1);
					OLED_ShowNum(32,48,_f,2,16,1);
					OLED_ShowString(48,48,"Hz",16,1);
					}
					else
					{
					OLED_ShowChinese(0,48,12,16,1);
					OLED_ShowChinese(16,48,13,16,1);
					OLED_ShowChinese(32,48,21,16,1);
					OLED_ShowChinese(48,48,22,16,1);
					}
					OLED_Refresh();
					break;
					case 3:
					OLED_ShowNum(14,16,ts[0],2,24,1);
					OLED_ShowString(40,16,":",24,1);
					OLED_ShowNum(56,16,ts[1],2,24,1);
					OLED_ShowString(80,16,":",24,1);
					OLED_ShowNum(92,16,ts[2],2,24,1);
					OLED_ShowChinese(96+0,48,16,16,1);
					OLED_ShowChinese(96+16,48,11,16,1);
					if(_f)
					{
					OLED_ShowChinese(0,48,19,16,0);
					OLED_ShowChinese(16,48,20,16,0);
					OLED_ShowNum(32,48,_f,2,16,0);
					OLED_ShowString(48,48,"Hz",16,0);
					}
					else
					{
					OLED_ShowChinese(0,48,12,16,0);
					OLED_ShowChinese(16,48,13,16,0);
					OLED_ShowChinese(32,48,21,16,0);
					OLED_ShowChinese(48,48,22,16,0);
					}
					OLED_Refresh();
					break;
					case 4:
					OLED_ShowNum(14,16,ts[0],2,24,1);
					OLED_ShowString(40,16,":",24,1);
					OLED_ShowNum(56,16,ts[1],2,24,1);
					OLED_ShowString(80,16,":",24,1);
					OLED_ShowNum(92,16,ts[2],2,24,1);
					OLED_ShowChinese(96+0,48,16,16,0);
					OLED_ShowChinese(96+16,48,11,16,0);
					if(_f)
					{
					OLED_ShowChinese(0,48,19,16,1);
					OLED_ShowChinese(16,48,20,16,1);
					OLED_ShowNum(32,48,_f,2,16,1);
					OLED_ShowString(48,48,"Hz",16,1);
					}
					else
					{
					OLED_ShowChinese(0,48,12,16,1);
					OLED_ShowChinese(16,48,13,16,1);
					OLED_ShowChinese(32,48,21,16,1);
					OLED_ShowChinese(48,48,22,16,1);
					}
					OLED_Refresh();
					if(click)
					{
						click = 0;
						ck[0]=ts[0];ck[1]=ts[1];ck[2]=ts[2];
						page = 1;
						Menu =0;
						OLED_Clear();
						OLED_Refresh();
					}
					break;
				}
			break;
			case 4://秒表界面
				OLED_ShowChinese(jz1,0,17,16,1);
				OLED_ShowChinese(jz1+48,0,18,16,1);
				OLED_ShowNum(14,16,sec[0],2,24,1);
				OLED_ShowString(40,16,":",24,1);
				OLED_ShowNum(56,16,sec[1],2,24,1);
				OLED_ShowString(80,16,":",24,1);
				OLED_ShowNum(92,16,sec[2],2,24,1);
				switch(Menu)
				{
					case 0://�?始，暂停
					if(!timesum)
					{
					OLED_ShowChinese(0,48,23,16,0);
					OLED_ShowChinese(16,48,24,16,0);
					OLED_ShowChinese(48,48,27,16,1);
					OLED_ShowChinese(48+16,48,28,16,1);
					OLED_ShowChinese(96+0,48,14,16,1);
					OLED_ShowChinese(96+16,48,15,16,1);
					}
					else{
					OLED_ShowChinese(0,48,25,16,0);
					OLED_ShowChinese(16,48,26,16,0);
					OLED_ShowChinese(48,48,27,16,1);
					OLED_ShowChinese(48+16,48,28,16,1);
					OLED_ShowChinese(96+0,48,14,16,1);
					OLED_ShowChinese(96+16,48,15,16,1);
					}
					if(click)
					{
						click = 0;
						timesum = !timesum;
					}
					break;
					case 1://复位
					if(!timesum)
					{
					OLED_ShowChinese(0,48,23,16,1);
					OLED_ShowChinese(16,48,24,16,1);
					OLED_ShowChinese(48,48,27,16,0);
					OLED_ShowChinese(48+16,48,28,16,0);
					OLED_ShowChinese(96+0,48,14,16,1);
					OLED_ShowChinese(96+16,48,15,16,1);
					}
					else{
					OLED_ShowChinese(0,48,25,16,1);
					OLED_ShowChinese(16,48,26,16,1);
					OLED_ShowChinese(48,48,27,16,0);
					OLED_ShowChinese(48+16,48,28,16,0);
					OLED_ShowChinese(96+0,48,14,16,1);
					OLED_ShowChinese(96+16,48,15,16,1);
					}
					if(click)
					{
						click = 0;
						timesum = 0;
						sec[0] = 0;
						sec[1] = 0;
						sec[2] = 0;
					}
					break;
					case 2://返回
					if(!timesum)
					{
					OLED_ShowChinese(0,48,23,16,1);
					OLED_ShowChinese(16,48,24,16,1);
					OLED_ShowChinese(48,48,27,16,1);
					OLED_ShowChinese(48+16,48,28,16,1);
					OLED_ShowChinese(96+0,48,14,16,0);
					OLED_ShowChinese(96+16,48,15,16,0);
					}
					else{
					OLED_ShowChinese(0,48,25,16,1);
					OLED_ShowChinese(16,48,26,16,1);
					OLED_ShowChinese(48,48,27,16,1);
					OLED_ShowChinese(48+16,48,28,16,1);
					OLED_ShowChinese(96+0,48,14,16,0);
					OLED_ShowChinese(96+16,48,15,16,0);
					}
					if(click)
					{
						click = 0;
						timesum = 0;
						page = 1;
						Menu = 0;
					}
					break;
				}
			OLED_Refresh();
			break;
		}
		if(bo_ol)
		{
			HAL_Delay(35);
			bo_ol=0;
		}
		if(_f && (ck[0] == thn) && (ck[1] == tmn) && (ck[2] == tsn))
			Alarm_Start();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == EC11_D_Pin && !bo_ol)
	{
//		Cnt++;
		bo_ol=1;
		click=1;
	}
	if(GPIO_Pin == EC11_A_Pin && !bo_ol)
	{
		if(HAL_GPIO_ReadPin(GPIOB,EC11_A_Pin) == RESET && HAL_GPIO_ReadPin(GPIOB,EC11_C_Pin) == SET )
			if(!still)
				Menu++;
			else
			{
				if(Menu < 3 &&(page == 2 || page == 3))
				ts[Menu]++;
				else
					if(Menu == 3 && page == 3)
						_f++;
			}
			ts[0] = (ts[0]>=24) ? 0:ts[0];
			ts[1] = (ts[1]>=60) ? 0:ts[1];
			ts[2] = (ts[2]>=60) ? 0:ts[2];
		if((Menu >=4) && (page == 1 || page == 2))
			Menu = 0;
		if(page == 3 && Menu >=5)
			Menu = 0;
		if(Menu == 3 && page == 3 && _f > 10)
			_f=0;
		if(Menu >= 3 && page == 4)
			Menu = 0;
		bo_ol =1;
	}
	if(GPIO_Pin == EC11_C_Pin && !bo_ol)
	{
		if(HAL_GPIO_ReadPin(GPIOB,EC11_C_Pin) == RESET && HAL_GPIO_ReadPin(GPIOB,EC11_A_Pin) == SET )
			if(!still)
				Menu--;
			else
				{
				if(Menu < 3 &&(page == 2 || page == 3))
				ts[Menu]--;
				else
					if(Menu == 3 && page == 3)
						_f--;
			}
			ts[0] = (ts[0]<0) ? 23:ts[0];
			ts[1] = (ts[1]<0) ? 59:ts[1];
			ts[2] = (ts[2]<0) ? 59:ts[2];
		if(Menu <0 && (page == 1 || page == 2))
			Menu = 3;
		if(page == 3 && Menu < 0)
			Menu = 4;
		if(Menu == 3 && page == 3 && _f < 0)
			_f=10;
		if(page ==4 && Menu < 0)
			Menu = 2;
		bo_ol =1;
	}
//	if(GPIO_Pin == BLUETOOTH_STATE_Pin)
//	{
//		HAL_UART_Transmit(&huart2,"OK",8,HAL_MAX_DELAY);
//		OLED_ShowPicture(0,0,7,11,BMP3,1);
//		OLED_Refresh();
//	}
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART2)
	{
		if(tmp =='A' && bltst == 0)
			bltst = 1;
		if(tmp == 'B' && bltst == 0)
		{
			bltst = 2;
			page = 3;
			OLED_Clear();
			for(int i = 0;i<=2;i++)
			ts[i] = ck[i];
		}
		if(tmp == 'I' && bltst != 0)
		{
			bltst = 0;
			ts[0]=0;ts[1]=0;ts[2]=0;
			page = 0;
			OLED_Clear();
		}
		switch(bltst)
		{
			case 0:
				break;
			case 1:
				if(tmp == 'C')
					tsn++;
				if(tmp == 'D')
					tmn++;
				if(tmp == 'E')
					thn++;
				if(tmp =='F')
					tsn--;
				if(tmp =='G')
					tmn--;
				if(tmp =='H')
					thn--;
				break;
			case 2:
				if(tmp == 'C')
				{
					ck[2]++;
					ts[2]++;
				}
				if(tmp == 'D')
					{
					ck[1]++;
					ts[1]++;
				}
				if(tmp == 'E')
					{
					ck[0]++;
					ts[0]++;
				}
					if(tmp == 'F')
				{
					ck[2]--;
					ts[2]--;
				}
				if(tmp == 'G')
					{
					ck[1]--;
					ts[1]--;
				}
				if(tmp == 'H')
					{
					ck[0]--;
					ts[0]--;
				}
		}
		ts[0] = (ts[0]<0) ? 23:ts[0];
			ts[1] = (ts[1]<0) ? 59:ts[1];
			ts[2] = (ts[2]<0) ? 59:ts[2];
		ck[0] = (ck[0]<0) ? 23:ck[0];
			ck[1] = (ck[1]<0) ? 59:ck[1];
			ck[2] = (ck[2]<0) ? 59:ck[2];
		thn = (ts<0) ? 23:thn;
		tmn = (tmn<0) ? 59:tmn;
		tsn = (tsn<0) ? 59:tsn;
		tmp=0;
		HAL_UART_Receive_IT(&huart2,&tmp,1);
	}
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == htim5.Instance) 
	{
		//HAL_UART_Transmit(&huart2,"OK",4,HAL_MAX_DELAY);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,percent);
		percentward = (percent<1000)? percentward : (!percentward);
		percentward = (percent>0)? percentward : (!percentward);
		if(!percentward)
		percent-=20;
		else
		percent+=20;
	}
	if (htim->Instance == htim4.Instance) 
	{
		tsn++;
		LM75AD_GetTemperature();
		if(tsn ==60)
		{
			tsn =0;
			tmn++;
		}
		if(tmn ==60)
		{
			tmn =0;
			thn++;
		}
		if(thn ==24)
		{
			thn =0;
		}
		if(timesum)
		{
			sec[2]++;
		LM75AD_GetTemperature();
		if(sec[2] ==60)
		{
			sec[2] =0;
			sec[1]++;
		}
		if(sec[1] ==60)
		{
			sec[1] =0;
			sec[0]++;
		}
		}
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
