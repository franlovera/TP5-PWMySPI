#include <stdint.h>
#include <stdio.h>
#include "bsp/bsp.h"

/**
 * @brief Se encarga de prender un led y apagarlo luego de un tiempo
 *
 * @param led    Numero de led a pulsar
 * @param tiempo Numero de ciclos del delay entre prendido y apagado
 */
void ledPulso(uint8_t led, uint32_t tiempo);

int brillo = 0,a,pote_old_state,pote_dif;
int flag = 0,flag_serie=0;
char string_serie[50];
/**
 * @brief Aplicacion principal
 */
int main(void) {
	bsp_init();


	while (1) {
		bsp_delayMs(50);
		pote_old_state=a;
		a=read_pot();
		pote_dif=abs(pote_old_state-a);


		if(a>8)
			led_on(0);
		else {
			led_off(0);
		}
		if(a>16)
			led_on(1);
		else
			led_off(1);
		if(a>24)
			led_on(2);
		else
			led_off(2);
		if(a>32)
			led_on(3);
		else
			led_off(3);
		if(a>40)
			led_on(4);
		else
			led_off(4);
		if(a>48)
			led_on(5);
		else
			led_off(5);
		if(a>56)
			led_on(6);
		else
			led_off(6);
		if(a>64)
			led_on(7);
		else
			led_off(7);
		if(a>72)
			led_on(8);
		else
			led_off(8);
		if(a>80)
			led_on(9);
		else
			led_off(9);
		if(a>88)
			led_on(10);
		else
			led_off(10);
		if(a>95)
			led_on(11);
		else
			led_off(11);

		led_setBright(0, a);
		led_setBright(1, a);
		led_setBright(2, a);

//		if(pote_dif<3){
//			if(!flag_serie){
//			sprintf(string_serie,"potenciometro = %3d% \n\r",a);
//			send_char(&string_serie);
//			flag_serie=1;
//		}
//		}
//		else{
//			flag_serie=0;
//		}


	}
}

/**
 * @brief Se preciono el pulsador
 *
 */
void APP_ISR_sw(void){

}

/**
 * @brief Interrupcion cada 1ms
 *
 */
void APP_ISR_1ms(void){
	static uint16_t count_1s=0,new_dif_pote =0;
	if(pote_dif<3)
	count_1s++;
	else
		count_1s=0;
	if(count_1s>=1000){
		count_1s=0;
			if(!flag_serie){
			sprintf(string_serie,"potenciometro = %3d% \n\r",a);
			send_char(&string_serie);
			flag_serie=1;

		}
		else{
			flag_serie=0;
		}

		new_dif_pote=pote_dif;
	}
}


void ledPulso(uint8_t led, uint32_t tiempo){
	led_on(led);
	Delay(tiempo);
	led_off(led);
}
