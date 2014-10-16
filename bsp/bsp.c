#include <stdint.h>
#include "stm32f4xx.h"			// Header del micro
#include "stm32f4xx_gpio.h"		// Perifericos de E/S
#include "stm32f4xx_rcc.h"		// Para configurar el (Reset and clock controller)
#include "stm32f4xx_tim.h"		// Modulos Timers
#include "stm32f4xx_exti.h"		// Controlador interrupciones externas
#include "stm32f4xx_adc.h"
#include "stm32f4xx_syscfg.h"	// configuraciones Generales
#include "stm32f4xx_usart.h"
#include "misc.h"				// Vectores de interrupciones (NVIC)
#include "bsp.h"

#define LED_V GPIO_Pin_12
#define LED_N GPIO_Pin_13
#define LED_R GPIO_Pin_14
#define LED_A GPIO_Pin_15
#define LED_11 GPIO_Pin_11
#define LED_10 GPIO_Pin_10
#define LED_9 GPIO_Pin_9
#define LED_8 GPIO_Pin_8
#define LED_7 GPIO_Pin_7
#define LED_6 GPIO_Pin_6
#define LED_5 GPIO_Pin_5
#define LED_4 GPIO_Pin_4
#define LED_3 GPIO_Pin_3
#define LED_2 GPIO_Pin_2
#define LED_1 GPIO_Pin_1
#define LED_0 GPIO_Pin_0


#define BOTON GPIO_Pin_0




/* Puertos de los leds disponibles */
GPIO_TypeDef* leds_port[] = { GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD };
/* Leds disponibles */
const uint16_t leds[] = { LED_0,LED_1,LED_2,LED_3,LED_4,LED_5,LED_6,LED_7,LED_8,LED_9,LED_10,LED_11,LED_V, LED_R, LED_N, LED_A};

uint32_t* const leds_pwm[] = { &TIM3->CCR1, &TIM3->CCR3,
		&TIM3->CCR4, };

extern void APP_ISR_sw(void);
extern void APP_ISR_1ms(void);
extern void APP_ISR_usart( char character);

volatile uint16_t bsp_contMS = 0;

void led_on(uint8_t led) {
	GPIO_SetBits(leds_port[led], leds[led]);
}

void led_off(uint8_t led) {
	GPIO_ResetBits(leds_port[led], leds[led]);
}

void led_toggle(uint8_t led) {
	GPIO_ToggleBits(leds_port[led], leds[led]);
}

uint8_t sw_getState(void) {
	return GPIO_ReadInputDataBit(GPIOA, BOTON);
}

void led_setBright(uint8_t led, uint8_t value) {

	*leds_pwm[led] = 10000 * value / 100;
}

void bsp_delayMs(uint16_t x) {
	bsp_contMS = x;

	while (bsp_contMS)
		;

}

/**
 * @brief Interrupcion llamada cuando se preciona el pulsador
 */
void EXTI0_IRQHandler(void) {

	if (EXTI_GetITStatus(EXTI_Line0) != RESET) //Verificamos si es la del pin configurado
			{
		EXTI_ClearFlag(EXTI_Line0); // Limpiamos la Interrupcion
		// Rutina:
		APP_ISR_sw();
	}
}

/**
 * @brief Interrupcion llamada al pasar 1ms
 */
void TIM2_IRQHandler(void) {

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

		APP_ISR_1ms();

		if (bsp_contMS) {
			bsp_contMS--;
		}
	}
}
void USART3_IRQHandler(void) {
        char data;
        if (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) != RESET) {
                USART_ClearITPendingBit(USART3, USART_IT_RXNE);
                data = USART_ReceiveData(USART3);
                APP_ISR_usart(data);

        }
}


void bsp_led_init();
void bsp_sw_init();
void bsp_timer_config();
void bsp_adc_init();
void bsp_pot_init();
void bsp_uart_init();


uint16_t read_pot();

void send_char(char *character);


void bsp_init() {
	bsp_led_init();
	bsp_pwm_config();
	bsp_pot_init();
//	bsp_sw_init();
	bsp_adc_init();
	bsp_timer_config();
	bsp_uart_init();
}

/**
 * @brief Inicializa Leds
 */
void bsp_led_init() {
	GPIO_InitTypeDef GPIO_InitStruct;

	// Arranco el clock del periferico
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14;
	GPIO_InitStruct.GPIO_Pin |= GPIO_Pin_13 | GPIO_Pin_12 | GPIO_Pin_11 | GPIO_Pin_10
			|GPIO_Pin_9 | GPIO_Pin_8 | GPIO_Pin_7 | GPIO_Pin_6
			|GPIO_Pin_5 | GPIO_Pin_4 | GPIO_Pin_3 | GPIO_Pin_2
			|GPIO_Pin_1 | GPIO_Pin_0;

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP; // (Push/Pull)
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStruct);





}

/**
 * @brief Inicializa SW
 */
void bsp_sw_init() {
	GPIO_InitTypeDef GPIO_InitStruct;

	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	// Arranco el clock del periferico
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	// Configuro interrupcion

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

	/* Configuro EXTI Line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Habilito la EXTI Line Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief Inicializa TIM2
 */
void bsp_timer_config(void) {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Habilito la interrupcion global del  TIM2 */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* TIM2 habilitado */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	/* Configuracion de la base de tiempo */
	TIM_TimeBaseStruct.TIM_Period = 1000; // 1 MHz bajado a 1 KHz (1 ms)
	TIM_TimeBaseStruct.TIM_Prescaler = (2 * 8000000 / 1000000) - 1; // 8 MHz bajado a 1 MHz
	TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStruct);
	/* TIM habilitado */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	/* TIM2 contador habilitado */
	TIM_Cmd(TIM2, ENABLE);

}

void bsp_pwm_config(void) {
	TIM_TimeBaseInitTypeDef TIM_config;
	GPIO_InitTypeDef GPIO_config;
	TIM_OCInitTypeDef TIM_OC_config;

	/* Habilito el clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* Configuro leds como Segunda Funcion */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_config.GPIO_Mode = GPIO_Mode_AF;
	GPIO_config.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 ;
	GPIO_config.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_config.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_config.GPIO_OType = GPIO_OType_PP;

	GPIO_Init(GPIOB, &GPIO_config);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_TIM3);

	TIM_config.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_config.TIM_ClockDivision = 0;
	TIM_config.TIM_Period = 10000;
	TIM_config.TIM_Prescaler = 16 - 1;
	TIM_TimeBaseInit(TIM3, &TIM_config);

	TIM_OC_config.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OC_config.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC_config.TIM_Pulse = 0;
	TIM_OC_config.TIM_OCPolarity = TIM_OCPolarity_Low;

	// CH1 del pwm
	TIM_OC1Init(TIM3, &TIM_OC_config);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

	//CH4 del pwm
	TIM_OC_config.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC_config.TIM_Pulse = 0;

	TIM_OC4Init(TIM3, &TIM_OC_config);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

	//CH3 del pwm corresponde al PB0
	TIM_OC_config.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC_config.TIM_Pulse = 0;

	TIM_OC3Init(TIM3, &TIM_OC_config);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);



	TIM_ARRPreloadConfig(TIM3, ENABLE);

	TIM_Cmd(TIM3, ENABLE);

}

void bsp_adc_init(){
	ADC_CommonInitTypeDef ADC_CommonInitStruct;
	ADC_InitTypeDef ADC1_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	// Configuro el prescaler del ADC
	ADC_CommonStructInit(&ADC_CommonInitStruct);
	ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div4;
	ADC_CommonInit(&ADC_CommonInitStruct);

	/* Configuro el ADC  */
	ADC_StructInit(&ADC1_InitStruct);
	ADC1_InitStruct.ADC_Resolution = ADC_Resolution_12b;
	ADC_Init(ADC1, &ADC1_InitStruct);
	ADC_Cmd(ADC1, ENABLE);


}

void bsp_pot_init(){
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_StructInit(&GPIO_InitStruct);              // Reseteo la estructura
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;      // Modo Analógico
	GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void bsp_uart_init(){


	 USART_InitTypeDef USART_InitStructure;
	 GPIO_InitTypeDef GPIO_InitStructure;
	 NVIC_InitTypeDef NVIC_InitStructure;

	    // Habilito Clocks
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	    // Configuro Pin TX
	 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_Init(GPIOD, &GPIO_InitStructure);

	 GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);

	    //  Configuro Pin RX
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	 GPIO_Init(GPIOD, &GPIO_InitStructure);

	 GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);

		//Configuro UART
	 USART_InitStructure.USART_BaudRate = 115200;
	 USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	 USART_InitStructure.USART_StopBits = USART_StopBits_1;
	 USART_InitStructure.USART_Parity = USART_Parity_No;
	 USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	 USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;



	     // Inicializo la USART
	 USART_Init(USART3, &USART_InitStructure);

	     // Habilito la Usart
	 USART_Cmd(USART3, ENABLE);



	 // Enable the USART RX Interrupt
	 USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	 NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	 NVIC_Init(&NVIC_InitStructure);



}

uint16_t read_pot(){
	uint16_t valor;

	// Selecciono el canal a convertir
	ADC_RegularChannelConfig(ADC1, 12, 1, ADC_SampleTime_15Cycles);
	ADC_SoftwareStartConv(ADC1);

	// Espero a que la conversión termine
	while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != SET)
	        ;

	// Guardo el valor leido
	valor = ADC_GetConversionValue(ADC1);
	valor = valor/41;
	return  valor;
}

void send_char(char *string){
	int8_t character=1;
	int8_t i;
	for(i=0;i<50;i++){
		character=string[i];
		if(character==0x00)
			break;
		while(!USART_GetFlagStatus(USART3, USART_FLAG_TXE)){
			}
		USART_SendData(USART3, (uint16_t) character);
	}




}
