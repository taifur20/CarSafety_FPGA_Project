#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include "xil_printf.h"
#include "xgpiops.h"
#include "sleep.h"
#include "xil_exception.h"
#include "xparameters.h"
#include "xiicps.h"

#include "multi_gas_sensor.h"
#include "grove_oled.h"
#include "bme280.h"


#define GPIO_DEVICE_ID XPAR_XGPIOPS_0_DEVICE_ID
#define IIC_DEVICE_ID		XPAR_XIICPS_0_DEVICE_ID
#define IIC_SCLK_RATE		400000


XGpioPs Gpio;
XGpioPs_Config *ConfigPtr;

XIicPs_Config  *IICConfig;
XIicPs Iic;

void warning_bell();
void read_and_display_bme280();
void read_and_display_gas();

void init_iic(){
    IICConfig = XIicPs_LookupConfig(IIC_DEVICE_ID);
    XIicPs_CfgInitialize(&Iic, IICConfig, IICConfig->BaseAddress);
    XIicPs_SetSClk(&Iic, IIC_SCLK_RATE);
}

void init_gpio(){
	ConfigPtr = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
	XGpioPs_CfgInitialize(&Gpio, ConfigPtr, ConfigPtr->BaseAddr);
	XGpioPs_SetDirectionPin(&Gpio, 54, 1);
    XGpioPs_SetOutputEnablePin(&Gpio, 54, 1);
}

float temperature, altitude;
uint32_t pressure;

float co, no2, nh3, c3h8, c4h10, ch4, h2, c2h5oh;

int main()
{

    init_platform();
    init_iic();
    init_gpio();

    init_bme280();

    init_groveOLED();
    clearDisplay();
    setNormalDisplay();
    setVerticalMode();

    setTextXY(0, 0); //set Cursor to ith line, 0th column
    setGrayLevel(5); //Set Grayscale level. Any number between 0 - 15.
    putString("CarSafety Device"); //Print Hello World

    setTextXY(1, 0); //set Cursor to ith line, 0th column
    setGrayLevel(7); //Set Grayscale level. Any number between 0 - 15.
    putString("Measured Values:"); //Print Hello World

    //warning_bell();

    while (1)
    {

    	read_and_display_bme280();
    	usleep(1000000);
    	read_and_display_gas();
    	usleep(1000000);

    	if(temperature>35 || pressure>1200 || co>100 || nh3>2 || ch4>5100000){
    		warning_bell();
    	}

    }
    cleanup_platform();
    return 0;
}

void warning_bell(){
	XGpioPs_WritePin(&Gpio, 54, 0x1);
	usleep(500000);
	XGpioPs_WritePin(&Gpio, 54, 0x0);
	usleep(500000);
	XGpioPs_WritePin(&Gpio, 54, 0x1);
	usleep(500000);
	XGpioPs_WritePin(&Gpio, 54, 0x0);
	usleep(500000);
	XGpioPs_WritePin(&Gpio, 54, 0x1);
	usleep(500000);
	XGpioPs_WritePin(&Gpio, 54, 0x0);
}

void read_and_display_bme280(){
	//init_bme280();
	usleep(100000);
	temperature = getTemperature();
	printf("Temperature = %.2f C\n\r", temperature);
	usleep(100000);

	pressure = getPressure();
	printf("Atmospheric pressure = %d HPa\n\r", pressure/100);
	usleep(100000);

	altitude = calcAltitude(1020.09); //mean see level 1020.09
	printf("Altitude = %.2f m\n\r", altitude);

	//convert float value to string for showing in oled display.
	int tempInt1 = temperature;
	float tempF = temperature - tempInt1;
	int tempInt2 = tempF * 100;

	char temperature_string[30];
	snprintf(temperature_string, sizeof(temperature_string), "Temp. = %d.%d C", tempInt1, tempInt2);

	int presInt1 = pressure/100;

	char pressure_string[30];
	snprintf(pressure_string, sizeof(pressure_string), "Pres. = %d HPa", presInt1);

	int altiInt1 = altitude;
	float altiF = altitude - altiInt1;
	int altiInt2 = altiF * 100;

	char altitude_string[30];
	snprintf(altitude_string, sizeof(altitude_string), "Alti. = %d.%d m", altiInt1, altiInt2);

	setTextXY(3, 0); //set Cursor to ith line, 0th column
	putString(temperature_string);

    setTextXY(4, 0);
	putString(pressure_string);

	setTextXY(5, 0);
	putString(altitude_string);
}

void read_and_display_gas(){
	powerOn();
	getVersion();

	co = measureGAS(CO);
    printf("The concentration of CO is %.2f ppm\n\r", co);

	no2 = measureGAS(NO2);
	printf("The concentration of NO2 is %.2f ppm\n\r", no2);

	nh3 = measureGAS(NH3);
	printf("The concentration of NH3 is %.2f ppm\n\r", nh3);

	c3h8 = measureGAS(C3H8);
	printf("The concentration of C3H8 is %.2f ppm\n\r", c3h8);

	c4h10 = measureGAS(C4H10);
	printf("The concentration of C4H10 is %.2f ppm\n\r", c4h10);

	ch4 = measureGAS(CH4);
	printf("The concentration of CH4 is %.2f ppm\n\r", ch4);

	h2 = measureGAS(H2);
	printf("The concentration of H2 is %.2f ppm\n\r", h2);

	c2h5oh = measureGAS(C2H5OH);
	printf("The concentration of C2H5OH is %.2f ppm\n\r", c2h5oh);

	powerOff();

	int co1 = co;
    float cof = co - co1;
    int co2 = cof * 100;
    char co_string[20];
    snprintf(co_string, sizeof(co_string), "CO = %d.%d", co1, co2);

    int no21 = no2;
    float no2f = no2 - no21;
    int no22 = no2f * 100;
    char no2_string[20];
    snprintf(no2_string, sizeof(no2_string), "NO = %d.%d", no21, no22);

    int nh31 = nh3;
    float nh3f = nh3 - nh31;
    int nh32 = nh3f * 100;
    char nh3_string[20];
    snprintf(nh3_string, sizeof(nh3_string), "NH3 = %d.%d", nh31, nh32);

    int c3h81 = c3h8;
    float c3h8f = c3h8 - c3h81;
    int c3h82 = c3h8f * 100;
    char c3h8_string[20];
    snprintf(c3h8_string, sizeof(c3h8_string), "C3H8 = %d.%d", c3h81, c3h82);

    int c4h101 = c4h10;
    float c4h10f = c4h10 - c4h101;
    int c4h102 = c4h10f * 100;
    char c4h10_string[20];
    snprintf(c4h10_string, sizeof(c4h10_string), "C4H10 = %d.%d", c4h101, c4h102);

    int ch41 = ch4;
    float ch4f = ch4 - ch41;
    int ch42 = ch4f * 100;
    char ch4_string[20];
    snprintf(ch4_string, sizeof(ch4_string), "CH4 = %d.%d", ch41, ch42);

    int h21 = h2;
    float h2f = h2 - h21;
    int h22 = h2f * 100;
    char h2_string[20];
    snprintf(h2_string, sizeof(h2_string), "H2 = %d.%d", h21, h22);

    int c2h5oh1 = c2h5oh;
    float c2h5ohf = c2h5oh - c2h5oh1;
    int c2h5oh2 = c2h5ohf * 100;
    char c2h5oh_string[20];
    snprintf(c2h5oh_string, sizeof(c2h5oh_string), "C2H5OH = %d.%d", c2h5oh1, c2h5oh2);

    setTextXY(7, 0);
    putString(co_string);
    setTextXY(8, 0);
    putString(no2_string);
    setTextXY(9, 0);
    putString(nh3_string);
    setTextXY(10, 0);
    putString(c3h8_string);
    setTextXY(11, 0);
    putString(c4h10_string);
    setTextXY(12, 0);
    putString(ch4_string);
    setTextXY(13, 0);
    putString(h2_string);
    setTextXY(14, 0);
    putString(c2h5oh_string);
    setTextXY(15, 0);
    putString("All units ppm");
}
