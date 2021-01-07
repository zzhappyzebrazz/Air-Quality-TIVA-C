/*
 * MQ135.h
 *
 *  Created on: 29 Jun 2020
 *      Author: user
 */

#ifndef MQ135_H_
#define MQ135_H_


//the load resistance on the board
#define RLOAD 10.0
//calibration resistance at the atmospheric C02 level
#define RZERO 76.63
//parameters for calculating ppm of C02 from sensor resistance
#define PARA 116.6020682
#define PARB 2.769034857

//paramenters to model temperature and humidity dependence
#define CORA 0.00035
#define CORB 0.02718
#define CORC 1.39538
#define CORD 0.0018

//atmospheric C02 level for calibration purposes
#define ATMOCO2 397.13
    float ui32Val;
    float getCorrectionFactor(float t, float h);
    float getResistance(float ui32Val);
    float getCorrectedResistance(float t, float h, float ui32Val);

    float getRZero(float ui32Val);
    float getCorrectedRzero(float t, float h, float ui32Val);

    float getPPM(float ui32Val, float t, float h);
    float getCorrectedPPM(float t, float h, float ui32Val);
    void ADC0init(void);
#endif /* MQ135_H_ */
