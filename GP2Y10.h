/*
 * GP2Y10.h
 *
 *  Created on: 9 Jun 2020
 *      Author: user
 */

/* Each sampler time 10ms
 * Led 0.32ms with 0.28ms sampling time
 *                  0.04ms off sampling
 * Scheduler:
 * + turn on led delay 0.28ms
 * + read input delay 0.04ms
 * + turn off led, delay 9.68ms
 *
 */

#ifndef GP2Y10_H_
#define GP2Y10_H_

#define SharpGP2Y10_SAMPLINGTIME 280
#define SharpGP2Y10_DELTATIME 40
#define SharpGP2Y10_SLEEPINGTIME 96800

//float getDustDensityField();
float getDustDensity();
//float getVoltageField();
float getVoltage();
int getADC();

void peripheralInit(void);
void delayUs(uint32_t ui32Us);
void UARTinit(void);
void ADCinit(void);
void calc();


#endif /* GP2Y10_H_ */
