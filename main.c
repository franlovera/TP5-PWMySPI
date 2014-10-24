#include <stdint.h>
#include <stdio.h>
#include "bsp/bsp.h"
#include <math.h>

/**
 * @brief Se encarga de prender un led y apagarlo luego de un tiempo
 *
 * @param led    Numero de led a pulsar
 * @param tiempo Numero de ciclos del delay entre prendido y apagado
 */
void ledPulso(uint8_t led, uint32_t tiempo);

int a, pote_old_state, pote_dif;
int flag = 0, flag_serie = 0, flag_transmit = 0;
char string_serie[100];
float brillo = 0;
int red, green, blue;


/**
 * @brief Aplicacion principal
 */
typedef enum {
	Inicio, Test_adc, Test_leds, Test_sw
} estados_e;
estados_e estado = Inicio;

int main(void) {


	int i = 0;
	bsp_init();

	while (1) {
		bsp_delayMs(50);
		pote_old_state = a;
		a = read_pot();
		pote_dif = abs(pote_old_state - a);

		for (i = 0; i <= 11; i++) {
			if (a > i * 8)
				led_on(i);
			else
				led_off(i);
		}



		if (a < 12) {
			green = 8 * a;
			blue = 0;
			red = 100;
		}

		if (12 <= a && a< 26) {
			// Code
			green = 100;
			blue = 0;
			red = 100 - 8 * a;
		}
		if (26 <= a && a < 42) {
			// Code
			blue = 8 * a;
			red = 0;
			green = 100;
		}
		if (42 <= a && a < 59) {
			// Code
			blue = 100;
			red = 0;
			green = 100 - 8 * a;

		}
		if (59<=a && a<78) {
			red = 8 * a;
			green = 0;
			blue = 100;
		}
		if (78<=a && a<101) {
			// Code
			red=100;
			green = 0;
			blue = 100 - 8 * a;

		}
		led_setBright(0, (uint8_t) green);
		led_setBright(1, (uint8_t) red);
		led_setBright(2, (uint8_t) blue);


		switch (estado) {
				case Inicio:
					// Proceso estado
			sprintf(string_serie, "1:Test ADC \n\r 2:Test LEDS\n\r 3: Test Switch\n\r");
			send_char(&string_serie);
			while(estado==Inicio);
					break;
//				case estado2:
//					// Proceso estado
//					if (sw_getState(BT_DERECHA) == 0) {
//						pulsoLed(0, 1000);
//						estado = estado3;
//					}
//					break;
//				case estado3:
//					// Proceso estado
//					if (sw_getState(BT_ABAJO) == 0) {
//						pulsoLed(0, 1000);
//						estado = estado4;
//					}
//					break;
//				case estado4:
//					// Proceso estado
//					if (sw_getState(BT_ARRIBA) == 0) {
//						pulsoLed(0, 1000);
//						estado = estado1;
//					}
//					break;
				}



//		if (flag_transmit) {
//			if (!flag_serie) {
//				sprintf(string_serie, "potenciometro = %3d �/. \n\r", a);
//				send_char(&string_serie);
//				flag_transmit = 0;
//
//			}
//
//		}
	}
}

/**
 * @brief Se preciono el pulsador
 *
 */
void APP_ISR_sw(void) {

}

/**
 * @brief Interrupcion cada 1ms
 *
 */
void APP_ISR_1ms(void) {
	static uint16_t count_1s = 0, new_dif_pote = 0;
	if (pote_dif < 3) {
		count_1s++;

	}

	else
		count_1s = 0;
	if (count_1s >= 1000) {
		count_1s = 0;
		flag_transmit = 1;

	}
}

void APP_ISR_usart(char character) {
	if(estado==Inicio){
		if(character=='1')
			estado = Test_adc;
		if(character=='2')
			estado = Test_leds;
		if(character=='3')
			estado = Test_sw;
		else
			estado=Inicio;
	}


//	int k = 0, led_serie;
//	static struct str_trama {
//		uint8_t encab[3];
//		uint8_t n_led;
//		uint8_t dos_p;
//		uint8_t estado;
//		uint8_t fin;
//	};
//	static union u_rx {
//		struct str_trama trama;
//		uint8_t buffer[7];
//	} rx;
//
//	for (k = 0; k < 7 - 1; k++) {
//
//		rx.buffer[k] = rx.buffer[k + 1];
//
//	}
//	rx.buffer[6] = character;
//
//	if (rx.trama.encab[0] == 'l')
//		if (rx.trama.encab[1] == 'e')
//			if (rx.trama.encab[2] == 'd')
//				if (rx.trama.dos_p == ':')
//					if (rx.trama.fin == 0x0d) {
//						led_serie = rx.trama.n_led - 0x30;
//						if (rx.trama.estado == 'y')
//							led_on(12 + led_serie);
//						if (rx.trama.estado == 'n')
//							led_off(12 + led_serie);
//					}

}

void ledPulso(uint8_t led, uint32_t tiempo) {
	led_on(led);
	Delay(tiempo);
	led_off(led);
}
