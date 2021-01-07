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
#include "driverlib/fpu.h"
#include "driverlib/rom.h"
#include "MQ135.h"

#define SERIES_LENGTH  100
float gSeriesData[SERIES_LENGTH];
int32_t i32Datacount;

volatile float dustDensity = 0;
volatile float volMeasured = 0;
volatile float calVoltage = 0;
volatile float vccVol = 5.0;
volatile uint32_t ui32ADCbuf[4];
void Timer0IntHandler(void){};

void SysTickIntHandler(void){};

void ftostr(float f,char *buf)
{
    int pos=0,ix,dp,num;
    if (f<0)
    {
        buf[pos++]='-';
        f = -f;
    }
    dp=0;
    while (f>=10.0)
    {
        f=f/10.0;
        dp++;
    }
    for (ix=1;ix<8;ix++)
    {
            num = (int)f;
            f=f-num;
            if (num>9)
                buf[pos++]='#';
            else
                buf[pos++]='0'+num;
            if (dp==0) buf[pos++]='.';
            f=f*10.0;
            dp--;
    }
}

int main(void){
    uint32_t ui32ADC0Val[4];
    volatile uint32_t ui32Temp;
    float temp = 28., hmid = 47.;
    float  Resistance, CorrectedResistance,pm25;
    uint32_t PPM, CorrectedPPM;

    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    peripheralInit();
    ADCinit();
    UARTinit();
    ADC0init();
    float target;
    UARTprintf("PM2.5 \n");
    UARTprintf("Type Single ended \n");
    UARTprintf("Sample Update : 250ms \n");
    UARTprintf("input: ADC1/AN1/PE2 \n");

//    ROM_FPULazyStackingEnable();
//    ROM_FPUEnable();
//    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    while(i32Datacount < SERIES_LENGTH){
        target = getDustDensity();
        gSeriesData[i32Datacount] = target;
        i32Datacount++;
      //  delayUs(500000);
    }
    while(1){
    char str[8];
    ADCProcessorTrigger(ADC0_BASE, 1);
    while(!ADCIntStatus(ADC0_BASE, 1, false)){}
    ADCIntClear(ADC0_BASE, 1);
    ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Val);
    ui32Temp = (ui32ADC0Val[0]+ui32ADC0Val[1]+ui32ADC0Val[2]+ui32ADC0Val[3])/4;
    Resistance = getResistance(ui32Temp);
    CorrectedResistance = getCorrectedResistance(temp, hmid, ui32Temp);
    PPM = getPPM(ui32Temp, temp, hmid);
    CorrectedPPM = getCorrectedPPM(temp, hmid, ui32Temp);
//    UARTprintf("AIN0 = %4d\n", ui32Temp);
    UARTprintf("PPM = %4d\n", PPM);
    UARTprintf("CorrectedPPM = %4d\n", CorrectedPPM);

    target = getDustDensity();
    ftostr(target*1000, str);
    str[sizeof(str)-1] = 0;
    UARTprintf("Dust density is : %s ug/m3 \n",str);
    target = getADC();
    ftostr(target, str);
    str[sizeof(str)-1] = 0;
//    UARTprintf("ADC value is : %4s  \n",str);
    target = getVoltage();
    ftostr(target, str);
    str[sizeof(str)-1] = 0;
//    UARTprintf("Voltage measured is : %s V \n",str);
    UARTprintf("\n");
    delayUs(1000000);
    }
}
