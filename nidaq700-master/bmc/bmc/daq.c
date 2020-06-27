
#include <stdio.h> /* for printf() */
#include <unistd.h>
#include <stdint.h>
#include "daq.h"

#define OVER_SAMPLE_ADC  8 // adc bit resolution
#define OVER_SAMPLE_NUM  7 // 128-bit oversample
#define OVER_SAMPLE_BITS 2 // extract 2 extra bits of resolution
#define OVER_SAMPLE  (1<<OVER_SAMPLE_NUM)
#define OVER_SAMPLE_SHIFTS OVER_SAMPLE_NUM-OVER_SAMPLE_BITS // two extra bits 16384 total resolution

int subdev_ai = 0; /* change this to your input subdevice */
int chan_ai = 0; /* change this to your channel */
int range_ai = 0; /* more on this later */
int aref_ai = AREF_GROUND; /* more on this later */
int maxdata_ai, ranges_ai, channels_ai;

int subdev_dio = 0; /* change this to your input subdevice */
int chan_dio = 0; /* change this to your channel */
int range_dio = 0; /* more on this later */
int maxdata_dio, ranges_dio, channels_dio, datain_dio;

comedi_t *it;
int8_t ADC_OPEN = false, DIO_OPEN = false, ADC_ERROR = false, DEV_OPEN = false, DIO_ERROR = false;


#ifdef __x86_64__ // for PC with DAQCARD
int8_t comedi_dev[] = "/dev/comedi4";
#else // for RPi with USB DAQ card
int8_t comedi_dev[] = "/dev/comedi0";
#endif

#ifdef __x86_64__ // for PC with DAQCARD

#else // for RPi with USB DAQ card

#endif

int init_daq(void)
{
	int i = 0, range_index = 0;
	comedi_range *ad_range;

	if (!DEV_OPEN) {
		it = comedi_open(comedi_dev);
		if (it == NULL) {
			comedi_perror("comedi_open");
			ADC_OPEN = false;
			DEV_OPEN = false;
			HAVE_DIO = false;
			HAVE_AI = false;
			return -1;
		}
		DEV_OPEN = true;
	}

	subdev_ai = comedi_find_subdevice_by_type(it, COMEDI_SUBD_AI, subdev_ai);
	if (subdev_ai < 0) {
		return -1;
		ADC_OPEN = false;
	}

	printf("Subdev  %i ", subdev_ai);
	channels_ai = comedi_get_n_channels(it, subdev_ai);
	printf("Analog  Channels %i ", channels_ai);
	maxdata_ai = comedi_get_maxdata(it, subdev_ai, i);
	printf("Maxdata %i ", maxdata_ai);
	ranges_ai = comedi_get_n_ranges(it, subdev_ai, i);
	printf("Ranges %i ", ranges_ai);

	for (range_index = 0; range_index < ranges_ai; range_index++) {
		ad_range = comedi_get_range(it, subdev_ai, i, range_index);
		printf(": range %i, min = %.1f, max = %.1f ", range_index,
			ad_range->min, ad_range->max);
	}
	printf("\n");

	ADC_OPEN = true;
	comedi_set_global_oor_behavior(COMEDI_OOR_NUMBER);
	return 0;
}

int adc_range(double min_range, double max_range)
{
	if (ADC_OPEN) {
		ad_range->min = min_range;
		ad_range->max = max_range;
		return 0;
	} else {
		return -1;
	}
}

double get_adc_volts(int chan, int diff, int range)
{
	lsampl_t data[OVER_SAMPLE]; // adc burst data buffer
	int retval, intg, i;
	comedi_range *ad_range;
	comedi_insn daq_insn;

	ADC_ERROR = false; // global fault flag

	if (diff == true) {
		aref_ai = AREF_DIFF;
	} else {
		aref_ai = AREF_GROUND;
	}

	intg = 0; // clear the integration counter
	// configure the AI channel for burst reads of OVER_SAMPLE times with the passed params
	daq_insn.subdev = subdev_ai;
	daq_insn.chanspec = CR_PACK(chan, range, aref_ai);
	daq_insn.insn = INSN_READ;
	daq_insn.n = OVER_SAMPLE;
	daq_insn.data = data;

	retval = comedi_do_insn(it, &daq_insn); // send one instruction to the driver
	if (retval < 0) {
		comedi_perror("comedi_do_insn in get_adc_volts");
		ADC_ERROR = true;
		bmc.raw[chan] = 0;
		return 0.0;
	}
	for (i = 0; i < OVER_SAMPLE; i++) {
		intg += data[i];
	}
	data[0] = intg >> OVER_SAMPLE_SHIFTS;

	ad_range = comedi_get_range(it, subdev_ai, chan, range);
	bmc.raw[chan] = data[0];
	return comedi_to_phys(data[0], ad_range, ((1 << (OVER_SAMPLE_ADC + OVER_SAMPLE_BITS)) - 1)); // return the double result
}

int get_dio_bit(int chan)
{
	lsampl_t data;
	int retval;

	DIO_ERROR = false;
	retval = comedi_data_read(it, subdev_dio, chan, range_dio, aref_dio, &data);
	if (retval < 0) {
		comedi_perror("comedi_data_read in get_dio_bits");
		DIO_ERROR = true;
		return 0;
	}
	if (data != 0) data = 1;
	return data;
}

int put_dio_bit(int chan, int bit_data)
{
	lsampl_t data = bit_data;
	int retval;

	DIO_ERROR = false;
	retval = comedi_data_write(it, subdev_dio, chan, range_dio, aref_dio, data);
	if (retval < 0) {
		comedi_perror("comedi_data_read in put_dio_bits");
		DIO_ERROR = true;
		return -1;
	}
	return 0;
}

int init_dio(void)
{
	int i = 0;

	if (!DEV_OPEN) {
		it = comedi_open(comedi_dev);
		if (it == NULL) {
			comedi_perror("comedi_open");
			DIO_OPEN = false;
			DEV_OPEN = false;
			return -1;
		}
		DEV_OPEN = true;
	}

	subdev_dio = comedi_find_subdevice_by_type(it, COMEDI_SUBD_DIO, subdev_dio);
	if (subdev_dio < 0) {
		return -1;
		DIO_OPEN = false;
	}

	printf("Subdev  %i ", subdev_dio);
	channels_dio = comedi_get_n_channels(it, subdev_dio);
	printf("Digital Channels %i ", channels_dio);
	maxdata_dio = comedi_get_maxdata(it, subdev_dio, i);
	printf("Maxdata %i ", maxdata_dio);
	ranges_dio = comedi_get_n_ranges(it, subdev_dio, i);
	printf("Ranges %i \n", ranges_dio);
	DIO_OPEN = true;
	return 0;
}

int get_data_sample(void)
{
	int i;
	static int pv_stable = 0, current_null_reset = false, first_run = true;

	if (HAVE_AI) {
#ifdef __x86_64__ // for PC with DAQCARD
		bmc.pv_voltage = lp_filter(get_adc_volts(PVV_C, true, RANGE_10) * PVV_GAIN, PVV_C, true); // read PV voltage on DIFF channels
		bmc.cm_voltage = lp_filter(get_adc_volts(CMV_C, false, RANGE_5), CMV_C, true); // back to SE channels
		bmc.cm_current = lp_filter(get_adc_volts(CMA_C, false, RANGE_5), CMA_C, true);
		if (bmc.pv_voltage < 0.0) bmc.pv_voltage = 0.0;
		if (bmc.cm_voltage < 0.0) bmc.cm_voltage = 0.0;
		if (bmc.cm_current < 0.0) bmc.cm_current = 0.0;

		if (first_run) {
			first_run = false;
			bmc.cm_null = CMA_DEFAULT;
			get_adc_volts(PVV_NULL, true, RANGE_10);
		}
#else // for RPi with USB DAQ card
		bmc.pv_voltage = lp_filter(get_adc_volts(PVV_C, false, 0) * PVV_GAIN, PVV_C, true); // read PV voltage on voltage divider
		bmc.cm_current = lp_filter(get_adc_volts(CMV_C, false, 0), CMV_C, true); // read current sensor
		if (bmc.pv_voltage < 0.0) bmc.pv_voltage = 0.0;
		if (bmc.cm_voltage < 0.0) bmc.cm_voltage = 0.0;
		if (bmc.cm_current < 0.0) bmc.cm_current = 0.0;

		if (first_run) {
			first_run = false;
			bmc.cm_null = CMA_DEFAULT;
		}
#endif

		if (bmc.pv_voltage > 3.0) { // use the default current null if we have not reset the null
			if (!current_null_reset) bmc.cm_null = bmc.cm_voltage * CMA_NULL;
			pv_stable = 0; // when voltage drops wait a bit until the null update
		} else {
			if (pv_stable++ > 64) current_null_reset = true; // wait for readings to be stable
			if (current_null_reset) {
				if (bmc.cm_current > 2.0) bmc.cm_null = bmc.cm_current; // make sure we have something real
				current_null_reset = false;
			}
		}
		bmc.cm_amps = lp_filter((bmc.cm_current - bmc.cm_null) / CMA_GAIN, CMC_C, true);
		if (bmc.cm_amps < 0.0) bmc.cm_amps = 0.0;
	} else {
		return -1;
	}

	// FIXME
	if (HAVE_DIO) {
#ifdef __x86_64__ // for PC with DAQCARD
		bmc.datain.D0 = get_dio_bit(8);
		bmc.datain.D1 = get_dio_bit(9);
		bmc.datain.D2 = get_dio_bit(10);
		bmc.datain.D3 = get_dio_bit(11);
		bmc.datain.D4 = get_dio_bit(12);
		bmc.datain.D5 = get_dio_bit(13);
		bmc.datain.D6 = get_dio_bit(14);
		bmc.datain.D7 = get_dio_bit(15);
		put_dio_bit(0, bmc.dataout.D0);
		put_dio_bit(1, bmc.dataout.D1);
		put_dio_bit(2, bmc.dataout.D2);
		put_dio_bit(3, bmc.dataout.D3);
		put_dio_bit(4, bmc.dataout.D4);
		put_dio_bit(5, bmc.dataout.D5);
		put_dio_bit(6, bmc.dataout.D6);
		put_dio_bit(7, bmc.dataout.D7);
#else // for RPi with USB DAQ card
#endif
	} else {
		return -2;
	}
	return 0;
}

double lp_filter(double new, int bn, int slow) // low pass filter, slow rate of change for new, LPCHANC channels, slow/fast select (-1) to zero channel
{
	static double smooth[LPCHANC] = {0};
	double lp_speed, lp_x;

	if ((bn >= LPCHANC) || (bn < 0)) // check for proper array position
		return new;
	if (slow) {
		lp_speed = 0.033;
	} else {
		lp_speed = 0.125;
	}
	lp_x = ((smooth[bn]*100.0) + (((new * 100.0)-(smooth[bn]*100.0)) * lp_speed)) / 100.0;
	smooth[bn] = lp_x;
	if (slow == (-1)) { // reset and return zero
		lp_x = 0.0;
		smooth[bn] = 0.0;
	}
	return lp_x;
}