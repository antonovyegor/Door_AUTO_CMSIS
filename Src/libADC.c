#include "libADC.h"



extern uint16_t valueADC[4];



void ADC_Mode_Init(void){
	ADC1->CR2 |= ADC_CR2_CONT;
	//ADC1->CR2 &= ~ADC_CR2_CONT;
	ADC1->CR2 |= ADC_CR2_EXTSEL;
	ADC1->CR2 |= ADC_CR2_EXTTRIG;

	ADC1->CR1 |= ADC_CR1_SCAN;

	ADC1->SQR1 &= ~ADC_SQR1_L;
	ADC1->SQR1 |= ADC_SQR1_L_1 | ADC_SQR1_L_0  ;  /// 0011 - 4 измерения

	//обнуление
	ADC1->SMPR2 &= ~ADC_SMPR2_SMP0; //  0 канал
	ADC1->SMPR2 &= ~ADC_SMPR2_SMP1; //  1 канал
	ADC1->SMPR2 &= ~ADC_SMPR2_SMP2; //  0 канал
	ADC1->SMPR2 &= ~ADC_SMPR2_SMP3; //  1 канал

	ADC1->SMPR2 |= ADC_SMPR2_SMP0 ; // 111 - 289.5 циклов
	ADC1->SMPR2 |= ADC_SMPR2_SMP1 ;
	ADC1->SMPR2 |= ADC_SMPR2_SMP2 ;
	ADC1->SMPR2 |= ADC_SMPR2_SMP3 ;

	ADC1->SQR3 &= ~ADC_SQR3_SQ1;
	ADC1->SQR3 &= ~ADC_SQR3_SQ2;
	ADC1->SQR3 &= ~ADC_SQR3_SQ3;
	ADC1->SQR3 &= ~ADC_SQR3_SQ4;



	ADC1->SQR3 &= ~ADC_SQR3_SQ1;  //00  //PA0

	ADC1->SQR3 |= ADC_SQR3_SQ2_0; //01

	ADC1->SQR3 |=ADC_SQR3_SQ3_1 ; // 10

	ADC1->SQR3 |= ADC_SQR3_SQ4_0 | ADC_SQR3_SQ4_1 ; // 11




}

void ADC_POWER(uint8_t  pwrmode){
	memset(&valueADC[0],0,sizeof(valueADC));
	if (pwrmode){
		ADC_Calibrate();
	}
	else
		ADC1->CR2 &= ~ADC_CR2_ADON;
}

void ADC_Calibrate(void){
	ADC1->CR2 |= ADC_CR2_ADON;
	ADC1->CR2 |= ADC_CR2_RSTCAL;
	while ((ADC1->CR2 & ADC_CR2_RSTCAL) == ADC_CR2_RSTCAL);
	ADC1->CR2 |= ADC_CR2_CAL;
	while ((ADC1->CR2 & ADC_CR2_RSTCAL) == ADC_CR2_CAL); // пока там 1 идет калибровка по окончанию идет сброс бита

}

void ADC_GPIO_Init(void){
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	// ADC ports
	GPIOA->CRL &= ~GPIO_CRL_MODE0; // PA0   - input
	GPIOA->CRL &= ~GPIO_CRL_CNF0;  //PA0    - analog mode

	GPIOA->CRL &= ~GPIO_CRL_MODE1; // PA1   - input
	GPIOA->CRL &= ~GPIO_CRL_CNF1;  //PA1    - analog mode

	GPIOA->CRL &= ~GPIO_CRL_MODE2; // PA2  - input
	GPIOA->CRL &= ~GPIO_CRL_CNF2;  //PA2    - analog mode

	GPIOA->CRL &= ~GPIO_CRL_MODE3; // PA3   - input
	GPIOA->CRL &= ~GPIO_CRL_CNF3;  //PA3    - analog mode


}

void ADC_DMA_Init(void){
	//--------------------------------------------------------------------------------
		//     DMA   ADC1

		RCC->AHBENR |= RCC_AHBENR_DMA1EN;
		DMA1_Channel1->CPAR = (uint32_t) &ADC1->DR;
		DMA1_Channel1->CMAR = (uint32_t) &valueADC[0];
		DMA1_Channel1->CNDTR = 4;
		DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0;
		DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0;
		DMA1_Channel1->CCR &= ~DMA_CCR_PINC;
		DMA1_Channel1->CCR |= DMA_CCR_MINC;
		DMA1_Channel1->CCR |= DMA_CCR_CIRC;
		//DMA1_Channel1->CCR &= ~DMA_CCR_CIRC;
		DMA1_Channel1->CCR &= ~DMA_CCR_DIR;
		DMA1_Channel1->CCR |= DMA_CCR_EN;
		ADC1->CR2 |= ADC_CR2_DMA;



}

