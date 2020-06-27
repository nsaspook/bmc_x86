/*
 * Demo code for remote Daq using Comedi and sockets
 *
 * This file may be freely modified, distributed, and combined with
 * other software, as long as proper attribution is given in the
 * source code.
 */

#include <stdlib.h>
#include <stdio.h> /* for printf() */
#include <unistd.h>
#include <string.h>
#include <comedilib.h>
#include "bmc/bmc.h"
#include "bmc/daq.h"
#include "bmc_x86/bmcnet.h"
#include <time.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/Box.h> 
#include <X11/Xaw/Label.h>        /* Include the Label widget's header file. */
#include <X11/Xaw/Cardinals.h>  /* Definition of ZERO. */
#include <X11/Xaw/Command.h> 

#define MDB true
#define MDB1 true

struct bmcdata bmc;
unsigned char HAVE_DIO = true, HAVE_AI = true;

String fallback_resources[] = {"*Label.Label:    BMC", NULL};

// bmcnet server information
char hostip[32] = "10.1.1.41";
int hostport = 9760;

void quit(w, client, call)
Widget w;
XtPointer client;
XtPointer call;
{

	exit(0);

}

int main(int argc, char *argv[])
{

	Widget toplevel;
	Widget box;
	Widget command;
	Widget label;
	void quit();
	Arg wargs[10];
	int n, update = 0, update_num = 0;
	char	net_message[256];
	time_t rawtime;
	struct tm * timeinfo;

	// Xwindows stuff for later
	toplevel = XtInitialize(argv[0], "simple", NULL, 0,
				&argc, argv);
	box = XtCreateManagedWidget("box", boxWidgetClass,
				toplevel, NULL, 0);
	n = 0;
	XtSetArg(wargs[n], XtNorientation, XtorientVertical);
	n++;
	XtSetArg(wargs[n], XtNvSpace, 10);
	n++;
	XtSetValues(box, wargs, n);
	label = XtCreateManagedWidget("label",
				labelWidgetClass, box, NULL, 0);
	n = 0;
	XtSetArg(wargs[n], XtNlabel, "Hello World");
	n++;
	XtSetValues(label, wargs, n);
	command = XtCreateManagedWidget("command",
					commandWidgetClass, box, NULL, 0);
	n = 0;
	XtSetArg(wargs[n], XtNlabel, "press and die");
	n++;
	XtSetValues(command, wargs, n);
	XtAddCallback(command, XtNcallback, quit, NULL);
	XtRealizeWidget(toplevel);

#ifdef __x86_64__ // for PC with DAQCARD
	if (init_daq() < 0) HAVE_AI = false;
	if (init_dio() < 0) HAVE_DIO = false;
#else // for RPi with USB DAQ card
	if (init_daq() < 0) HAVE_AI = false;
	HAVE_DIO = true;
#endif

	printf("\r\n Remote DAQ Client running  %d %d      \r\n",HAVE_AI, HAVE_DIO);

	while (HAVE_AI && HAVE_DIO) {
		get_data_sample();
		if (++update >= 59) {
			if (MDB) {
				printf("\r\n PV Voltage %2.4fV Sensor Supply Voltage %2.3fV Sensor Voltage %2.3fV Sensor Current %2.2fA Panel Power %3.2fW, Digital %u %u %u %u, Raw data %x, %x, %x, Null %x",
				bmc.pv_voltage, bmc.cm_voltage, bmc.cm_current, bmc.cm_amps, bmc.pv_voltage * bmc.cm_amps,
				bmc.datain.D5, bmc.datain.D4, bmc.datain.D1, bmc.datain.D0,
				bmc.raw[PVV_C], bmc.raw[CMV_C], bmc.raw[CMA_C], bmc.raw[PVV_NULL]);
			}
		}
		usleep(100);
		if (++update >= 60) {
			update = 0;
			if (MDB1) {
				printf("\r\n Update number %d ", update_num++);
				time(&rawtime);
				printf("%ld ", rawtime);
			}
			sprintf(net_message, "%i,%i,%i,%i,X", (int) (bmc.pv_voltage * V_SCALE), (int) (bmc.cm_amps * C_SCALE), update_num, STATION);
			bmc_client(net_message);
		}
		//        XtMainLoop(); // X-windows stuff for later...
	}

	printf("\r\n Remote DAQ Client exiting        \r\n");
	return 0;
}


