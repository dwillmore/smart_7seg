#include "ch32v003fun.h"
#include <stdio.h>

#include "font.h"

#define digit0 PD4
#define digit1 PD7
#define digit2 PD2
#define digit3 PD3
#define num_digits 4

uint8_t digits[] = {	digit0,
											digit1,
											digit2,
											digit3 };

#define segmentA PC7
#define segmentB PC6
#define segmentC PC5
#define segmentD PC4
#define segmentE PC3
#define segmentF PC2
#define segmentG PC1
#define segmentDP PC0
#define num_segments 8

uint8_t segments[] = {  segmentA,
												segmentB,
												segmentC,
												segmentD,
												segmentE,
												segmentF,
												segmentG,
												segmentDP };

uint power[] = {1000, 100, 10, 1};
volatile uint current_digit = 0;
volatile uint8_t display_buf[num_digits];

#define SYSTICK_DELAY ((uint32_t)FUNCONF_SYSTEM_CORE_CLOCK / 1000)

void setDigit(uint8_t character){
	for(int i=0; i < num_segments; i++)
		if(font[character] & 1<<i)
			funDigitalWrite( segments[i], FUN_HIGH )
		else
			funDigitalWrite( segments[i], FUN_LOW );
}

void SysTick_Handler(void) __attribute__((interrupt));
void SysTick_Handler(void)
{
	SysTick->CMP += SYSTICK_DELAY;
	SysTick->SR = 0x00000000;
	for(int i = 0; i < num_digits; i++)
		funDigitalWrite( digits[i], FUN_LOW);
	setDigit(display_buf[current_digit]);
	funDigitalWrite( digits[current_digit], FUN_HIGH);
	if(current_digit++ == num_digits)
		current_digit = 0;
}

int main()
{
	uint count = 0;
	uint loops = 0;
	SystemInit();
	funGpioInitAll();
	enum State_t {  First_char,
									Second_char,
									Third_char,
									Fourth_char,
									Passthrough,
									EOL_ignore };
	enum State_t State = First_char;
	uint8_t in_char;
	
	// Setup digit select pins
	for(int i=0; i < num_digits; i++){
		funPinMode( digits[i], GPIO_Speed_2MHz | GPIO_CNF_OUT_PP );
		funDigitalWrite( digits[i], FUN_LOW );
	}

  // Setup segment pins
	for(int i=0; i < num_segments; i++){
		funPinMode( segments[i], GPIO_Speed_2MHz | GPIO_CNF_OUT_PP );
	  funDigitalWrite( segments[i], FUN_LOW );
	}

  // Setup serial pins and function
	SetupUART( UART_BRR );
	funPinMode( PD6, GPIO_CNF_IN_FLOATING );
	USART1->CTLR1 |= USART_CTLR1_RE;

  // Setup SysTicK based IRQ routine to run once a ms
	SysTick->CTLR = 0x0000;
	SysTick->CMP = SYSTICK_DELAY - 1;
	SysTick->CNT = 0x00000000;
	SysTick->CTLR |= SYSTICK_CTLR_STE | SYSTICK_CTLR_STIE | SYSTICK_CTLR_STCLK;
	NVIC_EnableIRQ(SysTicK_IRQn);

	for(int i = 0; i < num_digits; i++)
		display_buf[i] = 0;

	while(1)
	{
	  if(USART1->STATR & USART_STATR_RXNE){
			in_char = USART1->DATAR;
			switch(State){
				case First_char:
					display_buf[0] = in_char;
					State = Second_char;
					break;
				case Second_char:
					display_buf[1] = in_char;
					State = Third_char;
					break;
				case Third_char:
					display_buf[2] = in_char;
					State = Fourth_char;
					break;
				case Fourth_char:
					display_buf[3] = in_char;
					State = Passthrough;
					break;
				case Passthrough:
					if((in_char == 13) || (in_char == 10))
						State = EOL_ignore;
					printf("%c", in_char);
					break;
				case EOL_ignore:
					if((in_char == 13) || (in_char == 10)){
					  printf("%c", in_char);
					}else{
					  display_buf[0] = in_char;
						State = Second_char;
					}
					break;
			}
		}
	}
	while(1)
	{
		for(int dig=0; dig<num_digits; dig++){
			display_buf[dig] = ((count/power[dig])%10 + 48);
		}
		Delay_Ms(1);
		if(loops++ > 9){
			loops = 0;
			if(count++ > 9999) count = 0;
		}
	}
}
