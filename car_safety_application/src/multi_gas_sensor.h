#define GAS_SENSOR_I2C_ADDR     0x04

#define ADDR_IS_SET             0           // if this is the first time to run, if 1126, set
#define ADDR_FACTORY_ADC_NH3    2
#define ADDR_FACTORY_ADC_CO     4
#define ADDR_FACTORY_ADC_NO2    6

#define ADDR_USER_ADC_HN3       8
#define ADDR_USER_ADC_CO        10
#define ADDR_USER_ADC_NO2       12
#define ADDR_IF_CALI            14          // IF USER HAD CALI

#define ADDR_I2C_ADDRESS        20

#define CH_VALUE_NH3            1
#define CH_VALUE_CO             2
#define CH_VALUE_NO2            3

#define CMD_ADC_RES0            1           // NH3
#define CMD_ADC_RES1            2           // CO
#define CMD_ADC_RES2            3           // NO2
#define CMD_ADC_RESALL          4           // ALL CHANNEL
#define CMD_CHANGE_I2C          5           // CHANGE I2C
#define CMD_READ_EEPROM         6           // READ EEPROM VALUE, RETURN UNSIGNED INT
#define CMD_SET_R0_ADC          7           // SET R0 ADC VALUE
#define CMD_GET_R0_ADC          8           // GET R0 ADC VALUE
#define CMD_GET_R0_ADC_FACTORY  9           // GET FACTORY R0 ADC VALUE
#define CMD_CONTROL_LED         10
#define CMD_CONTROL_PWR         11

enum {CO, NO2, NH3, C3H8, C4H10, CH4, H2, C2H5OH};

unsigned int get_addr_dta_2(unsigned char addr_reg, unsigned char __dta);
unsigned int get_addr_dta_1(unsigned char addr_reg);
int16_t readData(uint8_t cmd1);
int16_t readR0(void);
int16_t readR(void);
float getR0(unsigned char ch);
float getRs(unsigned char ch);
float measureGas(int gas);
void powerOn(void);
void powerOff(void);
void ledOff(void);
void ledOn(void);
