#include "libRTOS.h"

xQueueHandle TransmitDataADC1;
xQueueHandle TransmitDataADC2;
xQueueHandle TransmitDataADC3;
xQueueHandle TransmitDataADC4;


extern TaskHandle_t xADC_Handle;
extern TaskHandle_t xBLINK_Handle;
extern TaskHandle_t xINFO_Handle;
extern TaskHandle_t xSERVO_Handle;
extern TaskHandle_t xPWR_Handle;
extern TaskHandle_t xEXE_Handle;

SemaphoreHandle_t   xSemaphorMOVE;
SemaphoreHandle_t   xSemaphorTAMPER;
SemaphoreHandle_t   xSemaphorGERCON;

uint16_t valueADC[5];

uint8_t degree=0;
uint8_t  CurrentStateZero = 1;

extern uint8_t newCommandFlag;

char commandStr[20];

const char *OK = "OK";
const char *MOVEON = "MOVE";
const char *TAMPER = "TAMPER";
const char *VOLTAGE = "VOLTAGE=";
const char *GERCON = "GERCON";
const char *STOP = "STOP";
const char *RUN = "RUN";
const char *POWER = "POWER=";
const char *EXTCONTACT = "EXT=";
//====================================================================================

void vTaskSendToUART(void *argument){
	uint16_t uBuffer;
	char strbuffer[6];

	USART1_GPIO_Init();
	USART1_Mode_Init();


	while (1){

		if (uxQueueMessagesWaiting(TransmitDataADC1)!= 0){
			xQueueReceive(TransmitDataADC1, &uBuffer, 10);
			sprintf(strbuffer, "%u",uBuffer);
			USART1SendStr("BatLevel=     ");USART1SendStr(strbuffer);USART1SendStr("\r\n");
		}
		if (uxQueueMessagesWaiting(TransmitDataADC2)!= 0){
			xQueueReceive(TransmitDataADC2, &uBuffer, 10);
			sprintf(strbuffer, "%u",uBuffer);
			USART1SendStr("CurrentLevel=   ");USART1SendStr(strbuffer);USART1SendStr("\r\n");
		}
		if (uxQueueMessagesWaiting(TransmitDataADC3)!= 0){
			xQueueReceive(TransmitDataADC3, &uBuffer, 10);
			sprintf(strbuffer, "%u",uBuffer);
			USART1SendStr("LED1=     ");USART1SendStr(strbuffer);USART1SendStr("\r\n");
		}
		if (uxQueueMessagesWaiting(TransmitDataADC4)!= 0){
			USART1SendStr("!Очередь не пустая !\r\n");
			xQueueReceive(TransmitDataADC4, &uBuffer, 10);
			sprintf(strbuffer, "%u",uBuffer);
			USART1SendStr("LED2=   ");USART1SendStr(strbuffer);USART1SendStr("\r\n");
		}


		vTaskDelay(100);

	}
}
void vTaskBlink( void *argument){

	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	GPIOC->CRH |= GPIO_CRH_MODE13; // PC13   - output
	GPIOC->CRH &= ~GPIO_CRH_CNF13;  //PC13   - GP out PP

	while(1){
	GPIOC->BSRR |= GPIO_BSRR_BS13;
	vTaskDelay(900);
	GPIOC->BSRR |= GPIO_BSRR_BR13;
	vTaskDelay(100);
	}
}
void vTaskHello( void *argument){
	USART1_GPIO_Init();
	USART1_Mode_Init();

	while(1){
	USART1SendStr("Hello\r\n");
	vTaskDelay(2000);
	}
}

void vTaskADCConvert (void *argument){
	USART1_GPIO_Init();
	USART1_Mode_Init();
	ADC_GPIO_Init();
	ADC_Mode_Init();
	char strbuffer[6];
	ADC_DMA_Init();
	ADC_POWER(ON);
	while (1){
		ADC1->CR2 |= ADC_CR2_SWSTART; // start
		while (  (DMA1->ISR & DMA_ISR_TCIF1) == 0  );

		sprintf(strbuffer, "%u",valueADC[0]);
		USART1SendStr("BatLevel      =");USART1SendStr(strbuffer);USART1SendStr("\r\n");
		sprintf(strbuffer, "%u",valueADC[1]);
		USART1SendStr("CurrentLevel  =");USART1SendStr(strbuffer);USART1SendStr("\r\n");
		sprintf(strbuffer, "%u",valueADC[2]);
		USART1SendStr("LED1          =");USART1SendStr(strbuffer);USART1SendStr("\r\n");
		sprintf(strbuffer, "%u",valueADC[3]);
		USART1SendStr("LED2          =");USART1SendStr(strbuffer);USART1SendStr("\r\n");

		vTaskDelay(500);
	}
}

const char* const strsub(char* s, size_t pos, size_t count)
{
   static char buf[BUFSIZ];
   buf[sizeof buf - 1] = '\0';
   if ( count >= BUFSIZ )
      return NULL;
   else
      return strncpy(buf, s + pos, count);
}


void vTaskServo(void *argument){
	PWM_Servo_Init_TIM3();
	PWM_Level_TIM3_Ch1(100);
	vTaskDelay(1000);
	//char buffer[20];


	while(1){
		if( xSemaphoreTake( xSemaphorMOVE, 0 ) == pdPASS )
		{

			PWM_Level_TIM3_Ch1(75+degree);
			if (degree==0)
				CurrentStateZero=1;
			else
				CurrentStateZero=0;
		}
		vTaskDelay(100);
	}
}

void vTaskPowerManegment(void *argument){
	PWM_Power_Init_TIM3();
	PWM_Level_TIM3_Ch2(1000);

	while(1){
		PWM_Level_TIM3_Ch2(1000);
	}

}


void vTaskCmdExe(void *argument){
	char buffer[20];
	USART1_GPIO_Init();
	USART1_Mode_Init();
	newCommandFlag=0;

	xSemaphorMOVE = xSemaphoreCreateBinary();
	xSemaphorTAMPER = xSemaphoreCreateBinary();
	xSemaphorGERCON = xSemaphoreCreateBinary();

	PWM_Power_Init_TIM3();

	while(1)
	{


		if (newCommandFlag)
		{
			while(1)
			{
				if (strncmp(commandStr, OK , 2)==0){
					USART1SendStr("---OK---\r\n");
					newCommandFlag=0;
					break;
				}
				if (strncmp(commandStr, POWER , 6)==0){
					strncpy(buffer,commandStr+6,2);
					if(strncmp(buffer,"ON",3)==0){
						//включение
						USART1SendStr("---OK---\r\n");
						newCommandFlag=0;break;
					}
					if(strncmp(buffer,"OFF",3)==0){
						//выключение
						USART1SendStr("---OK---\r\n");
						newCommandFlag=0;break;
					}
				}
				if (strncmp(commandStr, STOP , 4)==0){

					vTaskSuspend(xSERVO_Handle);
					vTaskSuspend(xINFO_Handle);
					vTaskSuspend(xBLINK_Handle);
					vTaskSuspend(xADC_Handle);

					USART1SendStr("---OK---\r\n");
					newCommandFlag=0;
					break;
				}
				if (strncmp(commandStr, RUN , 3)==0){


					vTaskResume(xSERVO_Handle);
					vTaskResume(xINFO_Handle);
					vTaskResume(xBLINK_Handle);
					vTaskResume(xADC_Handle);

					USART1SendStr("---OK---\r\n");
					newCommandFlag=0;
					break;
				}
				if (strncmp(commandStr, EXTCONTACT , 4)==0){
					if(strncmp(buffer,"ON",3)==0){
						//включение

						USART1SendStr("---OK---\r\n");
						newCommandFlag=0;break;
					}
					if(strncmp(buffer,"OFF",3)==0){
						//выключение
						USART1SendStr("---OK---\r\n");
						newCommandFlag=0;break;
					}
				}

				if (strncmp(commandStr, MOVEON , 4)==0){
					if (commandStr[4]!='=')
					{
						if (CurrentStateZero)
							degree=30;
						else
							degree=0;
					}
					else
					{
						uint8_t  len= 0;
						while(commandStr[len++]!=0);
						len-=6;
						strncpy(buffer,commandStr+5,len);
						degree=0;
						for (uint8_t i=0;i<len;i++)
							if (buffer[i]>=0x30 &&  buffer[i]<=0x39){
								uint8_t bufi=(buffer[i]-0x30);
								uint8_t j = len-i-1;
								while (j--) bufi*=10;
								degree+=bufi;
							}
					}
					xSemaphoreGive(xSemaphorMOVE);
					USART1SendStr("---OK---\r\n");
					newCommandFlag=0;break;
				}
				if (strncmp(commandStr, TAMPER , 6)==0){
					xSemaphoreGive(xSemaphorTAMPER);
					USART1SendStr("---OK---\r\n");
					newCommandFlag=0;break;
				}
				if (strncmp(commandStr, GERCON , 6)==0){
					xSemaphoreGive(xSemaphorGERCON);
					USART1SendStr("---OK---\r\n");
					newCommandFlag=0;break;
				}

				if (strncmp(commandStr, VOLTAGE , 8)==0)
				{
					strncpy(buffer,commandStr+8,2);
					if(strncmp(buffer,"0",2)==0){
						PWM_Level_TIM3_Ch2(0);
						USART1SendStr("---OK---\r\n");
						newCommandFlag=0;break;
					}
					if(strncmp(buffer,"20",2)==0){
						PWM_Level_TIM3_Ch2(1212);
						USART1SendStr("---OK---\r\n");
						newCommandFlag=0;break;
					}
					if(strncmp(buffer,"21",2)==0){
						PWM_Level_TIM3_Ch2(1273);
						USART1SendStr("---OK---\r\n");
						newCommandFlag=0;break;
					}
					if(strncmp(buffer,"22",2)==0){
						PWM_Level_TIM3_Ch2(1333);
						USART1SendStr("---OK---\r\n");
						newCommandFlag=0;break;
					}
					if(strncmp(buffer,"23",2)==0){
						PWM_Level_TIM3_Ch2(1394);
						USART1SendStr("---OK---\r\n");
						newCommandFlag=0;break;
					}
					if(strncmp(buffer,"24",2)==0){
						PWM_Level_TIM3_Ch2(1455);
						USART1SendStr("---OK---\r\n");
						newCommandFlag=0;break;
					}
					if(strncmp(buffer,"25",2)==0){
						PWM_Level_TIM3_Ch2(1515);
						USART1SendStr("---OK---\r\n");
						newCommandFlag=0;break;
					}
					if(strncmp(buffer,"26",2)==0){
						PWM_Level_TIM3_Ch2(1576);
						USART1SendStr("---OK---\r\n");
						newCommandFlag=0;break;
					}
					if(strncmp(buffer,"27",2)==0){
						PWM_Level_TIM3_Ch2(1636);
						USART1SendStr("---OK---\r\n");
						newCommandFlag=0;break;
					}
					if(strncmp(buffer,"28",2)==0){
						PWM_Level_TIM3_Ch2(1697);
						USART1SendStr("---OK---\r\n");
						newCommandFlag=0;break;
					}
					if(strncmp(buffer,"29",2)==0){
						PWM_Level_TIM3_Ch2(1758);
						USART1SendStr("---OK---\r\n");
						newCommandFlag=0;break;
					}
					if(strncmp(buffer,"30",2)==0){
						PWM_Level_TIM3_Ch2(1818);
						USART1SendStr("---OK---\r\n");
						newCommandFlag=0;break;
					}
				}
				if (newCommandFlag)
				{
					USART1SendStr("---UKNOWN COMMAND---\r\n");
					newCommandFlag=0;break;
				}
				break;
			}

		}
		vTaskDelay(10);
	}
}

