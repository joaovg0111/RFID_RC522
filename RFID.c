
#include "stm32f1xx_hal.h"
#include <stdbool.h>

SPI_HandleTypeDef hspi1;
#include "RFID.h"


const uint8_t ReceiveCommand_RC522[] =    {CommandRegister,Receive};
const uint8_t SoftResetCommand_RC522[] =  {CommandRegister,SoftReset};
const uint8_t CalcCRCCommand_RC522[] =  {CommandRegister,CalcCRC};
const uint8_t IdleCommand_RC522[] =  {CommandRegister,Idle};
const uint8_t AuthCommand_RC522[] =  {CommandRegister,MFAuthent};
const uint8_t TransceiveCommand_RC522[] =  {CommandRegister,Transceive};

void SET_HSPI(SPI_HandleTypeDef hspi){
    //Usado para definir a conexão SPI usada
    hspi1=hspi;
}

void RC522_writeReg(uint8_t reg, uint8_t data){
  //Escreve data no registrador reg
  reg = ( (reg<<1) &(0b01111110) );
  uint8_t pData[2]={reg,data};
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, (uint8_t *)pData, 2, 100);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
}

void RC522_writeRegMult(uint8_t reg, uint8_t* data,uint8_t size){
  //Escreve data no registrador reg
  reg = ( (reg<<1) &(0b01111110) );
  uint8_t pData[size+1];
  pData[0]=reg;
  for (int i=0;i<size;i++){
      pData[i+1]=data[i];
  }
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, (uint8_t *)pData, size, 100);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
}

uint8_t RC522_readReg(uint8_t reg){
  //lê registrador reg para
  uint8_t output=0xFF;
  reg = ((reg<<1)|(0x80));
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, (uint8_t *)&reg, 1, 100);
  HAL_SPI_Receive(&hspi1, (uint8_t *)&output, 1, 100);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
  return output;
}

void RC522_ANTENNA_ON(void){
    //Ativa a antena
    uint8_t data = RC522_readReg(TxControlReg);//read
    data=data | 0x03;
    RC522_writeReg(TxControlReg,data);//write
}

void RC522_Run_Command(uint8_t* command){
    RC522_writeReg(command[0],command[1]);
}

void RC522_INIT(void){
    //SOFT RESET
    RC522_Run_Command(ReceiveCommand_RC522);
    RC522_Run_Command(SoftResetCommand_RC522);

    RC522_writeReg(TModeReg,0x80);//write

    RC522_writeReg(TPrescalerReg,0xA9);//write

    RC522_writeReg(TModeRegH,0xE8);//write

    RC522_writeReg(TModeRegL,0x03);//write


    //MFRC522_REG_TX_AUTO
    RC522_writeReg(TxASKReg,0x40);//write

    RC522_writeReg(ModeReg,0x3D);//write

    RC522_ANTENNA_ON();
}

void PICC_TransmitData(uint8_t* piccData,uint8_t dataSize,uint8_t* dataOut,uint8_t* outputLen){
    //piccData são valores a serem enviados para a tag RFID
    //dataSize é o tamanho de piccData
    //dataOut é o vetor que recebe os resultados de said
    //ouputLen o tamanho da resposta da tag
    uint8_t irqEn = 0x77;
    //SET RC522 CONFIG
    RC522_writeReg(BitFramingReg,0x07);//write

    //rc522_regWrite8(MFRC522_REG_COMM_IE_N, irqEn | 0x80);
    RC522_writeReg(ComlEnReg,irqEn |0x80);//write


    // rc522_clearBit(MFRC522_REG_COMM_IRQ, 0x80);
    uint8_t data = (RC522_readReg(ComIrqReg)&(~0x80));
    RC522_writeReg(ComIrqReg,data);

    //rc522_setBit(MFRC522_REG_FIFO_LEVEL, 0x80);
    RC522_writeReg(FIFOLevelReg,0x80);//reset FIFO

    //rc522_regWrite8(MFRC522_REG_COMMAND, PCD_IDLE);
    RC522_Run_Command(IdleCommand_RC522);//reset RC522

    //Write PICC_DATA to fifo
    RC522_writeRegMult(FIFODataReg,piccData,dataSize);//reset FIFO
    
    //Transceive
    RC522_Run_Command(TransceiveCommand_RC522);//Transceive

    uint8_t framingReg = (RC522_readReg(BitFramingReg)|0x80 );
    RC522_writeReg(BitFramingReg,framingReg);//set BitFramingReg to start transmission

    HAL_Delay(25);//Esperando comunicação

    
    framingReg = ( RC522_readReg(BitFramingReg)&(~0x80) );
    RC522_writeReg(BitFramingReg,framingReg);//reset BitFramingReg to stop transmission

    //Le resutados
    outputLen[0] = RC522_readReg(FIFOLevelReg);
    if(outputLen[0]!=0x00){
        for(uint8_t i=0x00;i<outputLen[0];i++){
            dataOut[i]=RC522_readReg(FIFODataReg);
        }
    }
}

bool IsCardPresent(void){
    uint8_t piccData[1]={0x26};
    uint8_t dataOut[2]={0x00,0x00};
    uint8_t outputLen[1]={0x00};
    PICC_TransmitData(piccData,1,dataOut,outputLen);
    if(outputLen!=0x00){
        return true;
    }else{
        return false;
    }
}

bool getCardUID(uint8_t* uid){
    uint8_t piccData[2]={0x93,0x20};
    uint8_t dataOut[4]={0x00,0x00,0x00,0x00};
    uint8_t outputLen[1]={0x00};

    //Coloca o PICC em estado READY
    if(IsCardPresent()){
        uint8_t i=0;
        while((i<10)&(outputLen[0]==0x00)){
            i++;
            PICC_TransmitData(piccData,2,dataOut,outputLen);
        };
    }
    //Le uid
    if(outputLen[0]!=0x00){
        for(uint8_t i=0x00;i<outputLen[0];i++){
            uid[i]=dataOut[i];
        }
        return true;
    }else{
        return false;
    }
}

bool verifyUID(uint8_t*uid){
    uint8_t newUID[4];
    getCardUID(newUID);
    for(int i;i<4;i++){
        if(newUID[i]!=uid[i]){
            return false;
        }
    }
    return true;
}

bool readMIFARE(uint8_t* blockValue,uint8_t blockAddr){
    //block é onde ficam os dados de saida, deve ser ter ao menos 18 endereços
    uint8_t piccData[2]={0x30,blockAddr};
    uint8_t dataOut[32];
    uint8_t outputLen[1]={0x00};

    //Coloca o PICC em estado READY
    if(IsCardPresent()){
        uint8_t i=0;
        while((i<10)&(outputLen[0]==0x00)){
            i++;
            PICC_TransmitData(piccData,2,dataOut,outputLen);
        };
    }
    //Le block
    if(outputLen[0]!=0x00){
        for(uint8_t i=0x00;i<outputLen[0];i++){
            blockValue[i]=dataOut[i];
        }
        return true;
    }else{
        return false;
    }
}

void writeMIFARE(uint8_t* data,uint8_t blockAddr){
    //data deve ter 16 bytes
    uint8_t piccData[2]={0xA0,blockAddr};
    uint8_t dataOut[32];
    uint8_t outputLen[1]={0x00};

    //Coloca o PICC em estado READY
    if(IsCardPresent()){
        uint8_t i=0;
        while((i<10)&(outputLen[0]==0x00)){
            i++;
            PICC_TransmitData(piccData,2,dataOut,outputLen);
            PICC_TransmitData(data,16,dataOut,outputLen);
        };
    }
    //Le block
    // if(outputLen[0]!=0x00){
    //     for(uint8_t i=0x00;i<outputLen[0];i++){
    //         block   [i]=dataOut[i];
    //     }
    //     return true;
    // }else{
    //     return false;
    // }
}

bool TEST(void){
    //testa se todas as funções estao funcionando corretamente
    uint8_t uid[4]={0xFF,0xFF,0xFF,0xFF};
    getCardUID(uid);
    if(!verifyUID(uid)){
        return false;
    }
    
    uint8_t blockAddr=0xFF;
    uint8_t blockData1[16];
    uint8_t blockData2[16];
    readMIFARE(blockData1,blockAddr);
    writeMIFARE(blockData1,blockAddr);
    readMIFARE(blockData2,blockAddr);
    for(int i=0;i<16;i++){
        if(blockData1!=blockData2){
            return false;
        }
    }
    return true;
}