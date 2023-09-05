#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xiicps.h"

#include "multi_gas_sensor.h"


extern XIicPs Iic;		/**< Instance of the IIC Device */

int __version;
unsigned int adcValueR0_NH3_Buf;
unsigned int adcValueR0_CO_Buf;
unsigned int adcValueR0_NO2_Buf;
uint16_t res0[3];       //sensors res0
uint16_t res[3];        //sensors res
bool r0_inited;
unsigned char dta_test[20];


unsigned char getVersion() {
    if (get_addr_dta_2(CMD_READ_EEPROM, ADDR_IS_SET) == 1126) {     // get version
        __version = 2;
        printf("Version = 2\n\r");
        return 2;
    }

    __version = 1;
    printf("Version = 1\n\r");
    return 1;
}

unsigned int get_addr_dta_1(unsigned char addr_reg) {

    u8 cmd[1] = {0};
    cmd[0] = addr_reg;
    unsigned char raw[10];

    XIicPs_SetOptions(&Iic,XIICPS_REP_START_OPTION);
    XIicPs_MasterSendPolled(&Iic, cmd, 1, GAS_SENSOR_I2C_ADDR);

    XIicPs_MasterRecvPolled(&Iic, raw, 2, GAS_SENSOR_I2C_ADDR);
    while (XIicPs_BusIsBusy(&Iic)) {
    	/* NOP */
    }
    XIicPs_ClearOptions(&Iic,XIICPS_REP_START_OPTION);

    unsigned int dta = 0;

    dta = raw[0];
    dta <<= 8;
    dta += raw[1];

    switch (addr_reg) {
        case CH_VALUE_NH3:

            if (dta > 0) {
                adcValueR0_NH3_Buf = dta;
            } else {
                dta = adcValueR0_NH3_Buf;
            }

            break;

        case CH_VALUE_CO:

            if (dta > 0) {
                adcValueR0_CO_Buf = dta;
            } else {
                dta = adcValueR0_CO_Buf;
            }

            break;

        case CH_VALUE_NO2:

            if (dta > 0) {
                adcValueR0_NO2_Buf = dta;
            } else {
                dta = adcValueR0_NO2_Buf;
            }

            break;

        default:;
    }
    return dta;
}


unsigned int get_addr_dta_2(unsigned char addr_reg, unsigned char __dta) {

    u8 cmd[2] = {0, 0};
    cmd[0] = addr_reg;
    cmd[1] = __dta;
    unsigned char raw[10];

    XIicPs_SetOptions(&Iic,XIICPS_REP_START_OPTION);
    XIicPs_MasterSendPolled(&Iic, cmd, 2, GAS_SENSOR_I2C_ADDR);

    XIicPs_MasterRecvPolled(&Iic, raw, 2, GAS_SENSOR_I2C_ADDR);
    while (XIicPs_BusIsBusy(&Iic)) {
	/* NOP */
    }
    XIicPs_ClearOptions(&Iic,XIICPS_REP_START_OPTION);

    unsigned int dta = 0;

    dta = raw[0];
    dta <<= 8;
    dta += raw[1];

    return dta;
}


int16_t readData(uint8_t cmd1) {

    uint8_t buffer[4];
    uint8_t checksum = 0;
    int16_t rtnData = 0;

    u8 cmd[1] = {0};
    cmd[0] = cmd1;

    XIicPs_SetOptions(&Iic,XIICPS_REP_START_OPTION);
    XIicPs_MasterSendPolled(&Iic, cmd, 1, GAS_SENSOR_I2C_ADDR);
    usleep(2000);

    XIicPs_MasterRecvPolled(&Iic, buffer, 4, GAS_SENSOR_I2C_ADDR);
    while (XIicPs_BusIsBusy(&Iic)) {
        /* NOP */
    }
    XIicPs_ClearOptions(&Iic,XIICPS_REP_START_OPTION);

    checksum = (uint8_t)(buffer[0] + buffer[1] + buffer[2]);
    if (checksum != buffer[3]) {
        return -4;    //checksum wrong
    }
    rtnData = ((buffer[1] << 8) + buffer[2]);

    return rtnData;//successful
}


int16_t readR0(void) {
    int16_t rtnData = 0;

    rtnData = readData(0x11);

    if (rtnData > 0) {
        res0[0] = rtnData;
    } else {
        return rtnData;    //unsuccessful
    }

    rtnData = readData(0x12);
    if (rtnData > 0) {
        res0[1] = rtnData;
    } else {
        return rtnData;    //unsuccessful
    }

    rtnData = readData(0x13);
    if (rtnData > 0) {
        res0[2] = rtnData;
    } else {
        return rtnData;    //unsuccessful
    }

    return 1;//successful
}

int16_t readR(void) {
    int16_t rtnData = 0;

    rtnData = readData(0x01);
    if (rtnData >= 0) {
        res[0] = rtnData;
    } else {
        return rtnData;    //unsuccessful
    }

    rtnData = readData(0x02);
    if (rtnData >= 0) {
        res[1] = rtnData;
    } else {
        return rtnData;    //unsuccessful
    }

    rtnData = readData(0x03);
    if (rtnData >= 0) {
        res[2] = rtnData;
    } else {
        return rtnData;    //unsuccessful
    }

    return 0;//successful
}


float getR0(unsigned char ch) {       // 0:CH3, 1:CO, 2:NO2
    if (__version == 1) {
        printf("ERROR: getR0() is NOT support by V1 firmware.\n\r");
        return -1;
    }

    int a = 0;
    switch (ch) {
        case 0:         // CH3
            a = get_addr_dta_2(CMD_READ_EEPROM, ADDR_USER_ADC_HN3);
            printf("a_ch3 = ");
            printf("%d\n\r",a);
            break;

        case 1:         // CO
            a = get_addr_dta_2(CMD_READ_EEPROM, ADDR_USER_ADC_CO);
            printf("a_co = ");
            printf("%d\n\r",a);
            break;

        case 2:         // NO2
            a = get_addr_dta_2(CMD_READ_EEPROM, ADDR_USER_ADC_NO2);
            printf("a_no2 = ");
            printf("%d\n\r",a);
            break;

        default:;
    }

    float r = 56.0 * (float)a / (1023.0 - (float)a);
    return r;
}

float getRs(unsigned char ch) {       // 0:CH3, 1:CO, 2:NO2

    if (__version == 1) {
        printf("ERROR: getRs() is NOT support by V1 firmware.\n\r");
        return -1;
    }

    int a = 0;
    switch (ch) {
        case 0:         // NH3
            a = get_addr_dta_1(1);
            break;

        case 1:         // CO
            a = get_addr_dta_1(2);
            break;

        case 2:         // NO2
            a = get_addr_dta_1(3);
            break;

        default:;
    }

    float r = 56.0 * (float)a / (1023.0 - (float)a);
    return r;
}


float measureGAS(int gas) {

    float ratio0, ratio1, ratio2;
    if (1 == __version) {
        if (!r0_inited) {
            if (readR0() >= 0) {
                r0_inited = true;
            } else {
                return -1.0f;
            }
        }

        if (readR() < 0) {
            return -2.0f;
        }

        ratio0 = (float)res[0] / res0[0];
        ratio1 = (float)res[1] / res0[1];
        ratio2 = (float)res[2] / res0[2];
        //printf("%f %f %f", ratio0, ratio1, ratio2);
    } else if (2 == __version) {
        // how to calc ratio/123
        ledOn();
        int A0_0 = get_addr_dta_2(6, ADDR_USER_ADC_HN3);
        int A0_1 = get_addr_dta_2(6, ADDR_USER_ADC_CO);
        int A0_2 = get_addr_dta_2(6, ADDR_USER_ADC_NO2);

        int An_0 = get_addr_dta_1(CH_VALUE_NH3);
        int An_1 = get_addr_dta_1(CH_VALUE_CO);
        int An_2 = get_addr_dta_1(CH_VALUE_NO2);

        ratio0 = (float)An_0 / (float)A0_0 * (1023.0 - A0_0) / (1023.0 - An_0);
        ratio1 = (float)An_1 / (float)A0_1 * (1023.0 - A0_1) / (1023.0 - An_1);
        ratio2 = (float)An_2 / (float)A0_2 * (1023.0 - A0_2) / (1023.0 - An_2);
        //printf("Ratio0 = %f \tRatio1 = %f \tRation2 = %f", ratio0, ratio1, ratio2);
    }

    float c = 0.0;

    switch (gas) {
        case CO: {
            c = pow(ratio1, -1.179) * 4.385; //mod by jack
        	return c;
            break;
            }
        case NO2: {
            c = pow(ratio2, 1.007) / 6.855; //mod by jack
        	return c;
            break;
            }
        case NH3: {
            c = pow(ratio0, -1.67) / 1.47; //modi by jack
        	return c;
            break;
            }
        case C3H8: { //add by jack
            c = pow(ratio0, -2.518) * 570.164;
        	return c;
            break;
            }
        case C4H10: { //add by jack
            c = pow(ratio0, -2.138) * 398.107;
        	return c;
            break;
            }
        case CH4: { //add by jack
            c = pow(ratio1, -4.363) * 630.957;
        	return c;
            break;
            }
        case H2: { //add by jack
            c = pow(ratio1, -1.8) * 0.73;
        	return c;
            break;
            }
        case C2H5OH: { //add by jack
            c = pow(ratio1, -1.552) * 1.622;
        	return c;
            break;
            }
        default:
            break;
    }

    if (2 == __version) {
        ledOff();
    }
    return c;
}

//power on sensor heater
void powerOn(void) {
    if (__version == 1) {
    	u8 cmd[1] = {0};
    	cmd[0] = 0x21;
    	XIicPs_MasterSendPolled(&Iic, cmd, 1, GAS_SENSOR_I2C_ADDR);
    } else if (__version == 2) {
        dta_test[0] = CMD_CONTROL_PWR;
        dta_test[1] = 1;
        XIicPs_MasterSendPolled(&Iic, dta_test, 2, GAS_SENSOR_I2C_ADDR);
    }
}

//power off sensor heater
void powerOff(void) {
    if (__version == 1) {
    	u8 cmd[1] = {0};
    	cmd[0] = 0x20;
    	XIicPs_MasterSendPolled(&Iic, cmd, 1, GAS_SENSOR_I2C_ADDR);
    } else if (__version == 2) {
        dta_test[0] = CMD_CONTROL_PWR;
        dta_test[1] = 0;
        XIicPs_MasterSendPolled(&Iic, dta_test, 2, GAS_SENSOR_I2C_ADDR);
    }
}

void ledOn() {
    dta_test[0] = CMD_CONTROL_LED;
    dta_test[1] = 1;
    XIicPs_MasterSendPolled(&Iic, dta_test, 2, GAS_SENSOR_I2C_ADDR);
}

void ledOff() {
    dta_test[0] = CMD_CONTROL_LED;
    dta_test[1] = 0;
    XIicPs_MasterSendPolled(&Iic, dta_test, 2, GAS_SENSOR_I2C_ADDR);
}


/*
//get raw data
powerOn();
getVersion();

float R0_NH3, R0_CO, R0_NO2;
float Rs_NH3, Rs_CO, Rs_NO2;
float ratio_NH3, ratio_CO, ratio_NO2;

R0_NH3 = getR0(0);
R0_CO  = getR0(1);
R0_NO2 = getR0(2);

Rs_NH3 = getRs(0);
Rs_CO  = getRs(1);
Rs_NO2 = getRs(2);

ratio_NH3 = Rs_NH3 / R0_NH3;
ratio_CO  = Rs_CO / R0_CO;
ratio_NO2 = Rs_NH3 / R0_NO2;

printf("R0_NH3 = %.2f\tR0_CO = %.2f\tR0_NO2 = %.2f\n\r", R0_NH3, R0_CO, R0_NO2);
printf("Rs_NH3 = %.2f\tRs_CO = %.2f\tRs_NO2 = %.2f\n\r", Rs_NH3, Rs_CO, Rs_NO2);

//Measure gas concentration
float c;

c = measureGAS(NH3);
printf("The concentration of NH3 is ");
if (c >= 0) {
   printf("%.2f", c);
   printf(" ppm\n\r");
} else {
   printf("invalid\n\r");
}

c = measureGAS(CO);
printf("The concentration of CO2 is ");
if (c >= 0) {
   printf("%.2f", c);
   printf(" ppm\n\r");
} else {
   printf("invalid\n\r");
}

c = calcGAS(NO2);
printf("The concentration of NO2 is ");
if (c >= 0) {
   printf("%.2f", c);
   printf(" ppm\n\r");
} else {
   printf("invalid\n\r");
}

*/
