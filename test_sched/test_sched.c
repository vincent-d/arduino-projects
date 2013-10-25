
#include "Arduino.h"
#include "SPIc.h"
#include "SevenSeg.h"
#include "Sched.h"

int pin_adc = A0;

// ADC / measure
float range_adc = 5.0;
int resolution_adc = 1024;
float step_adc = 0.01; // V/°C

struct Sched scheduler;

//int registered = 0;

void updateTemp();
float readTemp();

void nothing();

void printSync() {
	ss_printValueSync(&Disp);
}

int main() {

	init();

	ss_initialize(&Disp);
	ss_setValueLong(&Disp, 222l);

	initialize(&scheduler, MS_2_US(8));
	registerFunction(&scheduler, updateTemp, 2000/8, 1);
	registerFunction(&scheduler, printSync, 1, 0);

	launchScheduler(&scheduler);
	/*
	 *  delay(5000);
	 *  if (!registered) {
	 *    registered = 1;
	 *    registerFunctionWithOffset(&Scheduler, updateTemp, S_2_US(2), 1875);
}
*/
	return 0;
}


float readTemp() {

	int value_adc = analogRead(pin_adc);
	float temperature = ((( (float) value_adc * range_adc) / (float) resolution_adc) / step_adc);
	return temperature;

}

void updateTemp() {
	float t = readTemp();
	ss_setValueFloat(&Disp, t, 1);
}


void nothing() {
	return;
}
