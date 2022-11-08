#define CommandRegister  0x01
#define ComlEnReg        0x02
#define ComIrqReg        0x04
#define ErrorReg         0x06
#define Status1Reg       0x07
#define Status2Reg       0x08


#define FIFODataReg      0x09
#define FIFOLevelReg     0x0A
#define BitFramingReg    0x0D


#define ModeReg          0x11
#define TxControlReg     0x14
#define TxASKReg         0x15


#define TModeReg         0x2A
#define TPrescalerReg    0x2B
#define TModeRegH        0x2C
#define TModeRegL        0x2D

// RC522 Commands
#define GenRandID    0b00000010
#define Receive      0b00001000
#define SoftReset    0b00001111
#define CalcCRC      0b00000011
#define Idle         0b00000000
#define MFAuthent    0b00001110
#define Transceive   0b00001100
