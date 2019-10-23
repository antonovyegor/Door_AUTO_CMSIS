#include "main.h"


char data;
char commandStrBuf[20];
extern char commandStr[20];
uint8_t indexC;

uint8_t  newCommandFlag;


TaskHandle_t xADC_Handle;
TaskHandle_t xBLINK_Handle;
TaskHandle_t xINFO_Handle;
TaskHandle_t xSERVO_Handle;
TaskHandle_t xPWR_Handle;
TaskHandle_t xEXE_Handle;



int main (void){


	RCC_PLL_Init();
	xTaskCreate(vTaskHello, "Hello", 32, NULL, 3, &xINFO_Handle);
	xTaskCreate(vTaskBlink, "Led Blink 13", 32, NULL, 3, &xBLINK_Handle);

	xTaskCreate(vTaskADCConvert, "ADC", 128, NULL, 2, &xADC_Handle);
	xTaskCreate(vTaskServo, "Servo", 32, NULL, 2, &xSERVO_Handle);



	xTaskCreate(vTaskCmdExe, "USART receive command",256, NULL, 1, &xEXE_Handle);




	vTaskStartScheduler();

	while (1){

	}


}



void USART1_IRQHandler(){

	if (USART1->SR & USART_SR_RXNE)
	{
		data=USART1->DR;
		if (data=='\r' || data=='\n' || data==0 || indexC==sizeof(commandStrBuf)-1)
		{
			if (!newCommandFlag){
				int i=0;
				memset(commandStr,0,sizeof(commandStr));
				while(commandStrBuf[i]){
					commandStr[i]=commandStrBuf[i];
					i++;
				}
				newCommandFlag=1;
			}
			else{
				USART1SendStr("---REPEAT---");
			}
			memset(commandStrBuf,0,sizeof(commandStrBuf));
			indexC=0;
		}
		else {
			commandStrBuf[indexC++]=data;
		}
		USART1->SR &= ~USART_SR_RXNE;
	}
}


