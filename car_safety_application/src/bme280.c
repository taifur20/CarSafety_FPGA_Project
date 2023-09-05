#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xiicps.h"

#include "bme280.h"

extern XIicPs Iic;		/**< Instance of the IIC Device */

uint16_t dig_T1;
int16_t dig_T2;
int16_t dig_T3;
uint16_t dig_P1;
int16_t dig_P2;
int16_t dig_P3;
int16_t dig_P4;
int16_t dig_P5;
int16_t dig_P6;
int16_t dig_P7;
int16_t dig_P8;
int16_t dig_P9;
int32_t t_fine;


uint8_t bmp280Read8(uint8_t reg) {

	u8 cmd[1] = {0};
	cmd[0] = reg;
	u8 data = 0;
	s32 Status;

	while (XIicPs_BusIsBusy(&Iic));

	Status = XIicPs_MasterSendPolled(&Iic, cmd, 1, BMP280_ADDRESS);

	if (Status != XST_SUCCESS) {
		printf("Failure of iic send\n\r");
		return XST_FAILURE;
	}

	while (XIicPs_BusIsBusy(&Iic));

	Status = XIicPs_MasterRecvPolled(&Iic, &data, 1, BMP280_ADDRESS);

	if (Status != XST_SUCCESS) {
		printf("Failure of iic receive\n\r");
		return XST_FAILURE;
    }

	while (XIicPs_BusIsBusy(&Iic));

    return data;
}

uint16_t bmp280Read16(uint8_t reg) {

  uint8_t msb, lsb;
  u8 cmd[1] = {0};
  cmd[0] = reg;
  uint8_t raw[2];

  //XIicPs_SetOptions(&Iic, XIICPS_REP_START_OPTION);
  while (XIicPs_BusIsBusy(&Iic));

  XIicPs_MasterSendPolled(&Iic, cmd, 1, BMP280_ADDRESS);
  while (XIicPs_BusIsBusy(&Iic));

  XIicPs_MasterRecvPolled(&Iic, raw, 2, BMP280_ADDRESS);

  while (XIicPs_BusIsBusy(&Iic));

  //XIicPs_ClearOptions(&Iic, XIICPS_REP_START_OPTION);

  msb = raw[0];
  lsb = raw[1];

  return (uint16_t) msb << 8 | lsb;

}

uint16_t bmp280Read16LE(uint8_t reg) {
  uint16_t data = bmp280Read16(reg);
  return (data >> 8) | (data << 8);
}

int16_t bmp280ReadS16(uint8_t reg) {
  return (int16_t)bmp280Read16(reg);
}

int16_t bmp280ReadS16LE(uint8_t reg) {
  return (int16_t)bmp280Read16LE(reg);
}

uint32_t bmp280Read24(uint8_t reg) {

  uint32_t data;
  u8 cmd[1] = {0};
  cmd[0] = reg;
  uint8_t raw[3] = {0,0,0};

  //XIicPs_SetOptions(&Iic, XIICPS_REP_START_OPTION);
  XIicPs_MasterSendPolled(&Iic, cmd, 1, BMP280_ADDRESS);
  while (XIicPs_BusIsBusy(&Iic));

  XIicPs_MasterRecvPolled(&Iic, raw, 3, BMP280_ADDRESS);

  while (XIicPs_BusIsBusy(&Iic));

  //XIicPs_ClearOptions(&Iic, XIICPS_REP_START_OPTION);

  data = raw[0];
  data <<= 8;
  data |= raw[1];
  data <<= 8;
  data |= raw[2];
  //printf("Raw data = %d", data);
  return data;

}

void writeRegister(uint8_t reg, uint8_t val) {

  u8 cmd[2] = {0, 0};
  cmd[0] = reg;
  cmd[1] = val;

  XIicPs_MasterSendPolled(&Iic, cmd, 2, BMP280_ADDRESS);
  while (XIicPs_BusIsBusy(&Iic));
}

bool init_bme280() {
  uint8_t chip_id = 0;
  uint8_t retry = 0;

  while ((retry++ < 5) && (chip_id != 0x58)) {
    chip_id = bmp280Read8(BMP280_REG_CHIPID);
    printf("Read chip ID: ");
    printf("%d\n\r", chip_id);
    usleep(100000);
  }

  dig_T1 = bmp280Read16LE(BMP280_REG_DIG_T1);
  dig_T2 = bmp280ReadS16LE(BMP280_REG_DIG_T2);
  dig_T3 = bmp280ReadS16LE(BMP280_REG_DIG_T3);
  dig_P1 = bmp280Read16LE(BMP280_REG_DIG_P1);
  dig_P2 = bmp280ReadS16LE(BMP280_REG_DIG_P2);
  dig_P3 = bmp280ReadS16LE(BMP280_REG_DIG_P3);
  dig_P4 = bmp280ReadS16LE(BMP280_REG_DIG_P4);
  dig_P5 = bmp280ReadS16LE(BMP280_REG_DIG_P5);
  dig_P6 = bmp280ReadS16LE(BMP280_REG_DIG_P6);
  dig_P7 = bmp280ReadS16LE(BMP280_REG_DIG_P7);
  dig_P8 = bmp280ReadS16LE(BMP280_REG_DIG_P8);
  dig_P9 = bmp280ReadS16LE(BMP280_REG_DIG_P9);
  writeRegister(BMP280_REG_CONTROL, 0x3F);
  return true;
}

float getTemperature(void) {
  int32_t var1, var2;
  int32_t adc_T = bmp280Read24(BMP280_REG_TEMPDATA);

  adc_T >>= 4;
  var1 = (((adc_T >> 3) - ((int32_t)(dig_T1 << 1))) *
          ((int32_t)dig_T2)) >> 11;
  var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) *
            ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) *
          ((int32_t)dig_T3)) >> 14;
  t_fine = var1 + var2;
  float T = (t_fine * 5 + 128) >> 8;
  //printf("Temp = %f\n\r", T/100);
  return T / 100;
}

uint32_t getPressure(void) {
  int64_t var1, var2, p;
  // Call getTemperature to get t_fine
  getTemperature();

  int32_t adc_P = bmp280Read24(BMP280_REG_PRESSUREDATA);
  //printf("Pressure adc= %d\n\r", adc_P);

  adc_P >>= 4;

  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)dig_P6;
  var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
  var2 = var2 + (((int64_t)dig_P4) << 35);
  var1 = ((var1 * var1 * (int64_t)dig_P3) >> 8) + ((var1 * (int64_t)dig_P2) << 12);
  var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dig_P1) >> 33;
  if (var1 == 0) {
    return 0; // avoid exception caused by division by zero
  }

  p = 1048576 - adc_P;
  p = (((p << 31) - var2) * 3125) / var1;
  var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
  var2 = (((int64_t)dig_P8) * p) >> 19;
  p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);
  //printf("Pressure = %ld\n\t", (long)p/256);
  return (uint32_t)p / 256;
}

float calcAltitude_1(float p0, float p1, float t) {
  float C;
  C = (p0 / p1);
  //C = pow(C, (1 / 5.25588)) - 1.0;
  C = (C * (t + 273.15)) / 0.0065;
  return C;
}

float calcAltitude(float p0) {
  float t = getTemperature();
  float p1 = getPressure();
  return calcAltitude_1(p0, p1, t);
}

//example read
/*
init_bme280();
float temperature = getTemperature();
printf("Temperature = %.2f\n\n", temperature);
uint32_t pressure = getPressure() ;
printf("Atmospheric pressure = %d HPa\n\r", pressure/100);
float altitude = calcAltitude(1020.09); //mean see level 1020.09
printf("Altitude = %.2f m\n\r", altitude);
*/

