#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "ds18b20.h"
#include "stm32f1xx.h"
#include "libADC.h"
#include "defines.h"
#include "libUSART.h"
#include <stdio.h>
#include "string.h"
#include "libTIM.h"



void vTaskBlink( void *argument);
void vTaskSendToUART(void *argument);
void vTaskADCConvert (void *argument);


void vTaskServo(void *argument);
void vTaskPowerManegment(void *argument);
void vTaskCmdExe(void *argument);
void vTaskHello( void *argument);




const char* const strsub(char* s, size_t pos, size_t count);

