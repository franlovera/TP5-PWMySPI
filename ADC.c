// Estructuras de configuración
GPIO_InitTypeDef GPIO_InitStruct;
ADC_CommonInitTypeDef ADC_CommonInitStruct;
ADC_InitTypeDef ADC1_InitStruct;

// Habilito los clock a los periféricos
RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

// Configuro el pin en modo analógico
GPIO_StructInit(&GPIO_InitStruct);              // Reseteo la estructura
GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;      // Modo Analógico
GPIO_Init(GPIOC, &GPIO_InitStruct);

// Configuro el prescaler del ADC
ADC_CommonStructInit(&ADC_CommonInitStruct);
ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div4;
ADC_CommonInit(&ADC_CommonInitStruct);

/* Configuro el ADC  */
ADC_StructInit(&ADC1_InitStruct);
ADC1_InitStruct.ADC_Resolution = ADC_Resolution_12b;
ADC_Init(ADC1, &ADC1_InitStruct);
ADC_Cmd(ADC1, ENABLE);



// Selecciono el canal a convertir
ADC_RegularChannelConfig(ADC1, 12, 1, ADC_SampleTime_15Cycles);
ADC_SoftwareStartConv(ADC1);

// Espero a que la conversión termine
while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != SET)
        ;

// Guardo el valor leido
valor = ADC_GetConversionValue(ADC1);

while(1){
    
}