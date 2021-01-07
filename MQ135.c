/*
 * MQ135.c
 *
 *  Created on: 29 Jun 2020
 *      Author: user
 */

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/pin_map.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/adc.h"
#include "GP2Y10.h"
#include "driverlib/systick.h"
#include "MQ135.h"
#include <math.h>

void ADC0init(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
        GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);

        SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
        ADCHardwareOversampleConfigure(ADC0_BASE, 64);

        while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0)){}

        ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);

        ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH0);
        ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH0);
        ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH0);
        ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_CH0|ADC_CTL_IE|ADC_CTL_END);
        ADCSequenceEnable(ADC0_BASE, 1);
        ADCIntClear(ADC0_BASE, 1);
}

float getCorrectionFactor(float t, float h){
    return CORA * t * t - CORB * t + CORC - (h-33.)*CORD;
}

float getResistance(float ui32Val){
    int val = ui32Val;
    return((4095.0/(float)val) * 5.0 - 1)*RLOAD;
}

float getCorrectedResistance(float t, float h, float ui32Val){
    return getResistance(ui32Val)/getCorrectionFactor(t, h);
}

float getRZero(float ui32Val){
    return getResistance(ui32Val) * pow((ATMOCO2/PARA),(1./PARB));
}

float getCorrectedRZero(float t, float h, float ui32Val){
    return getCorrectedResistance(t, h, ui32Val) * pow((ATMOCO2/PARA), (1./PARB));
}

float getPPM(float ui32Val, float t, float h){
    return PARA * pow((getResistance(ui32Val)/RZERO), -PARB);
}

float getCorrectedPPM(float t, float h, float ui32Val){
    return PARA * pow((getCorrectedResistance(t, h, ui32Val)/RZERO), -PARB);
}
