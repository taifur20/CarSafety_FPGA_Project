#include <stdbool.h>

#define VERTICAL_MODE                       01
#define HORIZONTAL_MODE                     02

#define SeeedGrayOLED_Address               0x3C
#define SeeedGrayOLED_Command_Mode          0x80
#define SeeedGrayOLED_Data_Mode             0x40

#define SeeedGrayOLED_Display_Off_Cmd       0xAE
#define SeeedGrayOLED_Display_On_Cmd        0xAF

#define SeeedGrayOLED_Normal_Display_Cmd    0xA4
#define SeeedGrayOLED_Normal_Display_Cmd_SH1107G    0xA6
#define SeeedGrayOLED_Inverse_Display_Cmd   0xA7
#define SeeedGrayOLED_Activate_Scroll_Cmd   0x2F
#define SeeedGrayOLED_Dectivate_Scroll_Cmd  0x2E
#define SeeedGrayOLED_Set_ContrastLevel_Cmd 0x81

#define Scroll_Left             0x00
#define Scroll_Right            0x01

#define Scroll_2Frames          0x7
#define Scroll_3Frames          0x4
#define Scroll_4Frames          0x5
#define Scroll_5Frames          0x0
#define Scroll_25Frames         0x6
#define Scroll_64Frames         0x1
#define Scroll_128Frames        0x2
#define Scroll_256Frames        0x3

//unsigned char grayH;
//unsigned char grayL;

void init();
void sendCommand(unsigned char command);
void setContrastLevel(unsigned char ContrastLevel) ;
void setHorizontalMode() ;
void setVerticalMode() ;
void setTextXY(unsigned char Row, unsigned char Column) ;
void clearDisplay() ;
void sendData(unsigned char Data);
void setGrayLevel(unsigned char grayLevel);
void putChar(unsigned char C);
void putString(const char* String);
unsigned char putNumber(long long_num);
void drawBitmap(const unsigned char* bitmaparray, int bytes) ;
void setHorizontalScrollProperties(bool direction, unsigned char startRow, unsigned char endRow,
        unsigned char startColumn, unsigned char endColumn, unsigned char scrollSpeed);
void activateScroll();
void deactivateScroll();
void setNormalDisplay();
void setInverseDisplay() ;
