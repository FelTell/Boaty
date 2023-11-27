#include "Drivers/CompassDriver.h"

#include "stm32f4xx_hal.h"

extern I2C_HandleTypeDef hi2c1;
#define I2C_HANDLER hi2c1
#define I2C_ADDRESS 0x3C

#define CONFIGURATION_A_REGISTER 0x00
#define MODE_REGISTER            0x02
#define DATA_START_REGISTER      0x03

#define TIMEOUT 10

/**
 * @brief Configures the configurationA register
 *
 * @return true ok
 * @return false not ok
 */
static bool SetConfigurationA(void);

/**
 * @brief Configures the mode register to continous mode
 *
 * @return true ok
 * @return false not ok
 */
static bool SetContinousMode(void);

/**
 * @brief Get magnetometer values
 *
 * @param x
 * @param y
 * @param z
 * @return true ok
 * @return false not ok
 */
static bool GetHeadings(int16_t* x, int16_t* y, int16_t* z);

/**
 * @brief Write to i2c
 *
 * @param address memory address
 * @param data pointer to the data buffer
 * @param size size of the data buffer and the size to
 * written
 * @return true ok
 * @return false not ok
 */
static bool Write(uint8_t address, uint8_t* data,
                  uint16_t size);

/**
 * @brief Read i2c
 *
 * @param address memory address
 * @param data pointer to the data buffer
 * @param size size of the data buffer and the size to read
 * @return true ok
 * @return false not ok
 */
static bool Read(uint8_t address, uint8_t* data,
                 uint16_t size);

bool CompassDriver_Init(void) {
    if (!SetConfigurationA()) {
        return false;
    }
    if (!SetContinousMode()) {
        return false;
    }
    return true;
}

bool CompassDriver_GetAngle(float* angle) {
    int16_t compassX;
    int16_t compassY;
    int16_t compassZ;
    if (!GetHeadings(&compassX, &compassY, &compassZ)) {
        return false;
    }
    *angle = RAD_TO_DEGREES(atan2(compassY, compassX));
    return true;
}

bool GetHeadings(int16_t* x, int16_t* y, int16_t* z) {
    uint8_t buffer[6];
    if (!Read(
            DATA_START_REGISTER, buffer, sizeof(buffer))) {
        return false;
    }
    *x = (((int16_t)buffer[0]) << 8) | buffer[1];
    *y = (((int16_t)buffer[4]) << 8) | buffer[5];
    *z = (((int16_t)buffer[2]) << 8) | buffer[3];

    return true;
}

static bool SetConfigurationA(void) {
    // Set output as 15 Hz and set the numbers of samples
    // averaged to 8
    uint8_t configurationA = 0b0111'0000;
    return Write(CONFIGURATION_A_REGISTER,
                 &configurationA,
                 sizeof(configurationA));
}

static bool SetContinousMode(void) {
    // For continous mode: bit 0 and 1 should be 00;
    uint8_t modeRegister = 0b000'0000;
    return Write(
        MODE_REGISTER, &modeRegister, sizeof(modeRegister));
}

static bool Write(uint8_t address, uint8_t* data,
                  uint16_t size) {
    HAL_StatusTypeDef status =
        HAL_I2C_Mem_Write(&I2C_HANDLER,
                          I2C_ADDRESS,
                          address,
                          I2C_MEMADD_SIZE_8BIT,
                          data,
                          size,
                          TIMEOUT);
    return status == HAL_OK;
}

static bool Read(uint8_t address, uint8_t* data,
                 uint16_t size) {
    HAL_StatusTypeDef status =
        HAL_I2C_Mem_Read(&I2C_HANDLER,
                         I2C_ADDRESS,
                         address,
                         I2C_MEMADD_SIZE_8BIT,
                         data,
                         size,
                         TIMEOUT);
    return status == HAL_OK;
}
