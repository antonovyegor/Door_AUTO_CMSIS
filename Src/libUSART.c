#include  "libUSART.h"


extern char buf;



void USART1_DMA_Init(void){

			RCC->AHBENR |= RCC_AHBENR_DMA1EN;
			DMA1_Channel5->CPAR = (uint32_t) &USART1->DR;
			DMA1_Channel5->CMAR = (uint32_t) &buf;
			DMA1_Channel5->CNDTR = 1;
			DMA1_Channel5->CCR &= ~DMA_CCR_MSIZE;
			DMA1_Channel5->CCR &= ~DMA_CCR_PSIZE;
			DMA1_Channel5->CCR &= ~DMA_CCR_PINC;
			DMA1_Channel5->CCR &= ~DMA_CCR_MINC;
			//DMA1_Channel5->CCR |= DMA_CCR_MINC;

			//DMA1_Channel5->CCR |= DMA_CCR_CIRC;
			DMA1_Channel5->CCR &= ~DMA_CCR_CIRC;

			DMA1_Channel5->CCR &= ~DMA_CCR_DIR;

			//DMA1_Channel5->CCR |= DMA_CCR_TCIE;

			DMA1_Channel5->CCR |= DMA_CCR_EN;

			USART1->CR3 |= USART_CR3_DMAR;






}



void USART1_GPIO_Init(void){
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	// UART ports
	GPIOA->CRH |= GPIO_CRH_MODE9; // PA9    - output      TX
	GPIOA->CRH |= GPIO_CRH_CNF9_1;  //PA9   - GP out PP   TX
	GPIOA->CRH &= ~ GPIO_CRH_MODE10; // PA10  - input                     RX
	GPIOA->CRH |= GPIO_CRH_CNF10_0;  //PA10  - Alternative input float   RX

}


void USART1_Mode_Init(void){

	USART1->BRR =  0x271; // 625   -- 115200 72MHz

	USART1->CR1 |= USART_CR1_UE;
	USART1->CR1 |= USART_CR1_TE;
	USART1->CR1 |= USART_CR1_RE;
	USART1->CR1 |= USART_CR1_RXNEIE;
	NVIC_EnableIRQ(USART1_IRQn);



}


void USART1SendChar(char symb){
	while (! (USART1->SR & USART_SR_TC));
	USART1->DR = symb;

}
void USART1SendStr(char* str){
	uint8_t i=0;
	while (str[i]){
		USART1SendChar(str[i++]);
	}

}





