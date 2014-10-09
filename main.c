#include <stdint.h>
#include "bsp/bsp.h"

/**
 * @brief Se encarga de prender un led y apagarlo luego de un tiempo
 *
 * @param led    Numero de led a pulsar
 * @param tiempo Numero de ciclos del delay entre prendido y apagado
 */
void ledPulso(uint8_t led, uint32_t tiempo);

/**
 * @brief Aplicacion principal
 */
int main(void) {
	bsp_init();
	int brillo = 0,a;
	int flag = 0;

	while (1) {
		bsp_delayMs(100);

		led_toggle(0);
		led_toggle(1);
		led_toggle(2);
		led_toggle(3);
		led_toggle(4);
		led_toggle(5);
		led_toggle(6);
		led_toggle(7);
		led_toggle(8);
		led_toggle(9);
		led_toggle(10);
		led_toggle(11);

		a=read_pot();

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
	static uint16_t count_1s = 1000;
}


void ledPulso(uint8_t led, uint32_t tiempo){
	led_on(led);
	Delay(tiempo);
	led_off(led);
}
