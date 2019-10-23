#include "libEXT.h"

void GPIO_EXT_Init(void){
	RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;
	GPIOB->CRH |= GPIO_CRH_MODE13; //    - output
	GPIOB->CRH &= ~GPIO_CRH_CNF13;  //   - GP out PP



}
void GPIO_Rele_Init(void){
	RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;
	GPIOB->CRH |= GPIO_CRH_MODE12; //    - output
	GPIOB->CRH &= ~GPIO_CRH_CNF12;  //   - GP out PP




}

void Rele(uint8_t mode){
	if (mode){
		GPIOB->BSRR |= GPIO_BSRR_BS12;
	}
	else
		GPIOB->BSRR |= GPIO_BSRR_BR12;
}
void ExtPWR(uint8_t mode){
	if (mode){
		GPIOB->BSRR |= GPIO_BSRR_BS13;
	}
	else
		GPIOB->BSRR |= GPIO_BSRR_BR13;
}
