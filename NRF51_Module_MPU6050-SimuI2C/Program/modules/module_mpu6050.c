/*====================================================================================================*/
/*====================================================================================================*/
#include "drivers\nrf51_system.h"
#include "drivers\nrf51_i2cSimu.h"

#include "module_mpu6050.h"
/*====================================================================================================*/
/*====================================================================================================*/
#define I2CxS_SCL_PIN         7
#define I2CxS_SDO_PIN         30
#define I2CxS_FREQUENCY       10
/*====================================================================================================*/
/*====================================================================================================*/
static SimuI2C_InitTypeDef SimuI2C_InitStruct;
/*====================================================================================================*/
/*====================================================================================================*
**函數 : MPU6050_WriteReg
**功能 : Write Reg
**輸入 : writeAddr, writeData
**輸出 : None
**使用 : MPU6050_WriteReg(writeAddr, writeData);
**====================================================================================================*/
/*====================================================================================================*/
void MPU6050_WriteReg( uint8_t writeAddr, uint8_t writeData )
{
  SimuI2C_WriteReg(&SimuI2C_InitStruct, MPU6050_I2C_ADDR, writeAddr, writeData);
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : MPU6050_WriteRegs
**功能 : Write Regs
**輸入 : writeAddr, *writeData, lens
**輸出 : None
**使用 : MPU6050_WriteRegs(writeAddr, writeData, lens);
**====================================================================================================*/
/*====================================================================================================*/
void MPU6050_WriteRegs( uint8_t writeAddr, uint8_t *writeData, uint8_t lens )
{
  SimuI2C_WriteRegs(&SimuI2C_InitStruct, MPU6050_I2C_ADDR, writeAddr, writeData, lens);
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : MPU6050_ReadReg
**功能 : Read Reg
**輸入 : readAddr
**輸出 : readData
**使用 : readData = MPU6050_ReadReg(readAddr);
**====================================================================================================*/
/*====================================================================================================*/
uint8_t MPU6050_ReadReg( uint8_t readAddr )
{
  uint8_t readData = 0;

  SimuI2C_ReadReg(&SimuI2C_InitStruct, MPU6050_I2C_ADDR, readAddr, &readData);

  return readData;
}
/*=====================================================================================================*/
/*=====================================================================================================*
**函數 : MPU6050_ReadRegs
**功能 : Read Regs
**輸入 : readAddr, *readData, lens
**輸出 : None
**使用 : MPU6050_ReadRegs(readAddr, readData, 14);
**=====================================================================================================*/
/*=====================================================================================================*/
void MPU6050_ReadRegs( uint8_t readAddr, uint8_t *readData, uint8_t lens )
{
  SimuI2C_ReadRegs(&SimuI2C_InitStruct, MPU6050_I2C_ADDR, readAddr, readData, lens);
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : MPU6050_Config
**功能 : MPU6050 Config
**輸入 : None
**輸出 : None
**使用 : MPU6050_Config();
**====================================================================================================*/
/*====================================================================================================*/
void MPU6050_Config( void )
{
  SimuI2C_InitStruct.PinSCL    = I2CxS_SCL_PIN;
  SimuI2C_InitStruct.PinSDA    = I2CxS_SDO_PIN;
  SimuI2C_InitStruct.Freguency = I2CxS_FREQUENCY;
  SimuI2C_Config(&SimuI2C_InitStruct);
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : MPU6050_Init
**功能 : MPU6050 Init
**輸入 : None
**輸出 : None
**使用 : MPU6050_Init();
**====================================================================================================*/
/*====================================================================================================*/
#define MPU6050_InitRegNum 6
int8_t MPU6050_Init( MPU_InitTypeDef *MPUx )
{
  uint8_t status = ERROR;
  uint8_t MPU6050_InitData[MPU6050_InitRegNum][2] = {
    {0x01, MPU6050_PWR_MGMT_1},   // Reset device
    {0x03, MPU6050_CONFIG},       // 
    {0x18, MPU6050_GYRO_CONFIG},  // +-2000dps
    {0x08, MPU6050_ACCEL_CONFIG}, // +-4G
    {0x32, MPU6050_INT_PIN_CFG},  // 
    {0x00, MPU6050_USER_CTRL}     // 
  };

//  MPU6050_InitData[6][0] = MPUx->MPU_Gyr_FullScale;       // MPU6500_GYRO_CONFIG
//  MPU6050_InitData[8][0] = MPUx->MPU_Gyr_LowPassFilter;   // MPU6500_CONFIG
//  MPU6050_InitData[7][0] = MPUx->MPU_Acc_FullScale;       // MPU6500_ACCEL_CONFIG
//  MPU6050_InitData[9][0] = MPUx->MPU_Acc_LowPassFilter;   // MPU6500_ACCEL_CONFIG_2

  for(uint8_t i = 0; i < MPU6050_InitRegNum; i++) {
    Delay_1ms(2);
    MPU6050_WriteReg(MPU6050_InitData[i][1], MPU6050_InitData[i][0]);
  }

  /* Check Device ID */
  status = MPU6050_Check();
  if(status != SUCCESS)
    return ERROR;

  return SUCCESS;
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : MPU6050_Check
**功能 : Check Device ID
**輸入 : None
**輸出 : status
**使用 : status = MPU6050_Check();
**====================================================================================================*/
/*====================================================================================================*/
int8_t MPU6050_Check( void )
{
  int8_t deviceID = ERROR;

  deviceID = MPU6050_ReadReg(MPU6050_WHO_AM_I);
  if(deviceID != MPU6050_Device_ID)
    return ERROR;

  return SUCCESS;
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : MPU6050_getData
**功能 : Get IMU Data
**輸入 : *dataIMU
**輸出 : None
**使用 : MPU6050_getData(dataIMU);
**====================================================================================================*/
/*====================================================================================================*/
void MPU6050_getData( int16_t *dataIMU )
{
  uint8_t tmpRead[22] = {0};

  MPU6050_ReadRegs(MPU6050_ACCEL_XOUT_H, tmpRead, 14);

  dataIMU[0] = (Byte16(int16_t, tmpRead[6],  tmpRead[7]));    // Temp
  dataIMU[1] = (Byte16(int16_t, tmpRead[0],  tmpRead[1]));    // Acc.X
  dataIMU[2] = (Byte16(int16_t, tmpRead[2],  tmpRead[3]));    // Acc.Y
  dataIMU[3] = (Byte16(int16_t, tmpRead[4],  tmpRead[5]));    // Acc.Z
  dataIMU[4] = (Byte16(int16_t, tmpRead[8],  tmpRead[9]));    // Gyr.X
  dataIMU[5] = (Byte16(int16_t, tmpRead[10], tmpRead[11]));   // Gyr.Y
  dataIMU[6] = (Byte16(int16_t, tmpRead[12], tmpRead[13]));   // Gyr.Z
}
/*====================================================================================================*/
/*====================================================================================================*/

