/*
 * PM.c
 *
 *  Created on: 7 Jun 2020
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

extern float dustDensity;
extern float volMeasured;
extern float calVoltage;
extern float vccVol;
extern uint32_t ui32ADCbuf[4];

void delayUs(uint32_t ui32Us){
    SysCtlDelay(ui32Us*(SysCtlClockGet()/ 3/ 1000000));
}

void peripheralInit(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
}

void UARTinit(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0|GPIO_PIN_1);
    UARTStdioConfig(0, 115200, 16000000);
}

void ADCinit(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
    ADCHardwareOversampleConfigure(ADC1_BASE, 64);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC1)){}

    ADCSequenceConfigure(ADC1_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);

    ADCSequenceStepConfigure(ADC1_BASE, 1, 0, ADC_CTL_CH1);
    ADCSequenceStepConfigure(ADC1_BASE, 1, 1, ADC_CTL_CH1);
    ADCSequenceStepConfigure(ADC1_BASE, 1, 2, ADC_CTL_CH1);
    ADCSequenceStepConfigure(ADC1_BASE, 1, 3, ADC_CTL_CH1|ADC_CTL_IE|ADC_CTL_END);
    ADCSequenceEnable(ADC1_BASE, 1);
    ADCIntClear(ADC1_BASE, 1);
}

void calc(){

    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
    delayUs(SharpGP2Y10_SAMPLINGTIME);
    ADCProcessorTrigger(ADC1_BASE, 1);
    while(!ADCIntStatus(ADC1_BASE, 1, false)){}
    ADCIntClear(ADC1_BASE, 1);
    ADCSequenceDataGet(ADC1_BASE, 1, ui32ADCbuf);
    volMeasured = (ui32ADCbuf[0]+ui32ADCbuf[1]+ui32ADCbuf[2]+ui32ADCbuf[3])/4;
    calVoltage = (volMeasured*5.0)/4095.0;
    dustDensity =  0.17 * calVoltage - 0.099;
//
    delayUs(SharpGP2Y10_DELTATIME);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 2);
    delayUs(SharpGP2Y10_SLEEPINGTIME);
//   if (dustDensity < 0) return dustDensity = 0;

}
float getDustDensity(){
    calc();
    return dustDensity;
}

float getVoltage(){
    calc();
    return calVoltage;
}

int getADC(){
    calc();
    return volMeasured;
}
