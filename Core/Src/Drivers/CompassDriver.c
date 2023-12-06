#include "Drivers/CompassDriver.h"

#include "stm32f4xx_hal.h"
#include "TimerHandler.h"

extern I2C_HandleTypeDef hi2c1;
#define I2C_HANDLER hi2c1
#define I2C_ADDRESS 0x3C

#define CONFIGURATION_A_REGISTER 0x00
#define MODE_REGISTER            0x02
#define DATA_START_REGISTER      0x03

#define CALIBRATION_STABILIZING_TIME_MS 1000
#define CALIBRATION_SAMPLING_TIME_MS    5000
#define CALIBRATION_POLL_TIME_MS        1000
#define MESSAGE_TIMEOUT_MS              10

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

static bool CalibrateToNorth(void);

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
static bool Write(uint8_t address,
                  uint8_t* data,
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
static bool Read(uint8_t address,
                 uint8_t* data,
                 uint16_t size);

static bool initDone           = false;
static float calibrationFactor = 0;

bool CompassDriver_Init(bool calibrationNeeded) {
    if (initDone) {
        return true;
    }
    if (!SetConfigurationA()) {
        return false;
    }
    if (!SetContinousMode()) {
        return false;
    }

    initDone = true;

    if (calibrationNeeded) {
        if (!CalibrateToNorth()) {
            initDone = false;
            return false;
        }
    }

    return true;
}

bool CompassDriver_GetAngle(float* angle) {
    if (!initDone) {
        return false;
    }
    int16_t compassX;
    int16_t compassY;
    int16_t compassZ;
    if (!GetHeadings(&compassX, &compassY, &compassZ)) {
        return false;
    }
    *angle = RAD_TO_DEGREES(atan2(compassY, compassX)) +
             calibrationFactor;

    // FIXME (Felipe): this might be wrong
    if (*angle > 180) {
        *angle = *angle - 360;
    } else if (*angle < -180) {
        *angle = *angle + 360;
    }
    return true;
}

bool GetHeadings(int16_t* x, int16_t* y, int16_t* z) {
    uint8_t buffer[6];
    if (!Read(DATA_START_REGISTER,
              buffer,
              sizeof(buffer))) {
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
    uint8_t configurationA = 0b01110000;
    return Write(CONFIGURATION_A_REGISTER,
                 &configurationA,
                 sizeof(configurationA));
}

static bool SetContinousMode(void) {
    // For continous mode: bit 0 and 1 should be 00;
    uint8_t modeRegister = 0b0000000;
    return Write(MODE_REGISTER,
                 &modeRegister,
                 sizeof(modeRegister));
}

static bool CalibrateToNorth(void) {
    calibrationFactor = 0;

    const uint32_t stabilizingTimer = Timer_Update();
    while (
        !Timer_WaitMs(stabilizingTimer,
                       CALIBRATION_STABILIZING_TIME_MS)) {}

    const uint32_t samplingTimer = Timer_Update();
    uint32_t pollTimer           = Timer_Update();
    uint32_t count               = 0;
    float sum                    = 0;
    while (!Timer_WaitMs(samplingTimer,
                          CALIBRATION_SAMPLING_TIME_MS)) {
        if (Timer_WaitMs(pollTimer,
                          CALIBRATION_POLL_TIME_MS)) {
            float detectedAngle;
            if (!CompassDriver_GetAngle(&detectedAngle)) {
                return false;
            }
            count++;
            sum += detectedAngle;
            pollTimer = Timer_Update();
        }
    }

    const float average = sum / count;
    calibrationFactor   = 0 - average;

    return true;
}

static bool Write(uint8_t address,
                  uint8_t* data,
                  uint16_t size) {
    HAL_StatusTypeDef status =
        HAL_I2C_Mem_Write(&I2C_HANDLER,
                          I2C_ADDRESS,
                          address,
                          I2C_MEMADD_SIZE_8BIT,
                          data,
                          size,
                          MESSAGE_TIMEOUT_MS);
    return status == HAL_OK;
}

static bool Read(uint8_t address,
                 uint8_t* data,
                 uint16_t size) {
    HAL_StatusTypeDef status =
        HAL_I2C_Mem_Read(&I2C_HANDLER,
                         I2C_ADDRESS,
                         address,
                         I2C_MEMADD_SIZE_8BIT,
                         data,
                         size,
                         MESSAGE_TIMEOUT_MS);
    return status == HAL_OK;
}
