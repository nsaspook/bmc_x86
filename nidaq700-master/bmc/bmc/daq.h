/* 
 * File:   daq.h
 * Author: root
 *
 * Created on September 21, 2012, 6:49 PM
 */

/*
 Current sensor pins to ribbon cable:
 *      wire 1  pin 3 output
 *      wire 3  pin 2 ground
 *      wire 5  pin 1 power VS 5vdc
 *      wire 9  pv voltage from cal pot
 *      wire 10 PV ground
 */


#ifndef DAQ_H
#define	DAQ_H

#ifdef	__cplusplus
extern "C" {
#endif

#define PVV_C   0x0
#define CMV_C   0x1
#define CMA_C   0x2
#define CMC_C   0x3
#define PVV_NULL    8

#define LPCHANC        16

#define RANGE_10    0
#define RANGE_5     1

#define PVV_GAIN    2.50
#define CMA_NULL    0.50
#define CMA_GAIN    0.0151  // increase to reduce current readout.
#define CMA_DEFAULT 2.40

#include <comedilib.h>
#include "bmc.h"

    int subdev_ai; /* change this to your input subdevice */
    int chan_ai; /* change this to your channel */
    int range_ai; /* more on this later */
    int aref_ai; /* more on this later */
    int maxdata_ai, ranges_ai, channels_ai;

    int subdev_dio; /* change this to your input subdevice */
    int chan_dio; /* change this to your channel */
    int range_dio; /* more on this later */
    int aref_dio; /* more on this later */
    int maxdata_dio, ranges_dio, channels_dio, datain_dio;

    comedi_t *it;
    comedi_range *ad_range;
    int8_t ADC_OPEN, DIO_OPEN, ADC_ERROR, DEV_OPEN, DIO_ERROR;

    extern struct bmcdata bmc;
    extern struct didata datain;
    extern struct dodata dataout;
    extern unsigned char HAVE_DIO, HAVE_AI;

    int init_daq(void);
    int init_dio(void);
    int adc_range(double, double);
    double get_adc_volts(int,int,int);
    int get_dio_bit(int);
    int put_dio_bit(int, int);
    int get_data_sample(void);
    double lp_filter(double, int, int);
#ifdef	__cplusplus
}
#endif

#endif	/* DAQ_H */

