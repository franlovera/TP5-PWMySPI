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
int flag = 0,flag_serie=0,flag_transmit=0;
char string_serie[50];


/**
 * @brief Aplicacion principal
 */
int main(void) {
	int i=0;
	bsp_init();


	while (1) {
		bsp_delayMs(50);
		pote_old_state=a;
		a=read_pot();
		pote_dif=abs(pote_old_state-a);


		for(i=0; i<=11; i++){
			if(a>i*8)
				led_on(i);
			else
				led_off(i);
		}



		led_setBright(0, a);
		led_setBright(1, a);
		led_setBright(2, a);

//		if(flag_transmit){
//			if(!flag_serie){
//			sprintf(string_serie,"potenciometro = %3d º/. \n\r",a);
//			send_char(&string_serie);
//			flag_transmit=0;
//
//		}
//
//
//
//
//	}
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
	if(pote_dif<3){
		count_1s++;

	}

	else
		count_1s=0;
	if(count_1s>=1000){
		count_1s=0;
		flag_transmit=1;

		}
	}


void APP_ISR_usart(char character){
	int k=0,led_serie;
	static struct str_trama {
		uint8_t encab[3];
		uint8_t n_led;
		uint8_t dos_p;
		uint8_t estado;
		uint8_t fin;
	};
	static union u_rx{
		struct str_trama trama;
		uint8_t buffer[7];
	}rx;





	for(k=0;k<7-1;k++){

		rx.buffer[k]=rx.buffer[k+1];

	}
	rx.buffer[6]= character;

	if(rx.trama.encab[0]=='l')
		if(rx.trama.encab[1]=='e')
			if(rx.trama.encab[2]=='d')
				if(rx.trama.dos_p ==':')
					if(rx.trama.fin==0x0d){
						led_serie=rx.trama.n_led-0x30;
						if(rx.trama.estado=='y')
							led_on(12+led_serie);
						if(rx.trama.estado =='n')
							led_off(12+led_serie);
					}




}



void ledPulso(uint8_t led, uint32_t tiempo){
	led_on(led);
	Delay(tiempo);
	led_off(led);
}
