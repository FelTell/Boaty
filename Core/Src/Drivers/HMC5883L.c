#include "Drivers/HMC5883LDriver.h"
#include "stm32f4xx_hal.h"

extern I2C_HandleTypeDef hi2c1;
#define I2C_HANDLER hi2c1
#define I2C_ADDRESS 0x3C

#define MODE_REGISTER       0x02
#define DATA_START_REGISTER 0x03

#define TIMEOUT 10

static bool SetContinousMode(void);

static bool Write(uint8_t address, uint8_t* data,
                  uint16_t size);

static bool Read(uint8_t address, uint8_t* data,
                 uint16_t size);

/** Power on and prepare for general usage.
 * This will prepare the magnetometer with default settings,
 * ready for single- use mode (very low power requirements).
 * Default settings include 8-sample averaging, 15 Hz data
 * output rate, normal measurement bias, a,d 1090 gain (in
 * terms of LSB/Gauss). Be sure to adjust any settings you
 * need specifically after initialization, especially the
 * gain settings if you happen to be seeing a lot of -4096
 * values (see the datasheet for mor information).
 */

bool HMC5883LDriver_Init(void) {
    // The only setting we change from the default is the
    // mode. The default is idle, so change to continous.
    while (!SetContinousMode()) {}
}

static bool SetConfiguration() {
    // For continous mode: bit 0 and 1 should be 00;
    uint8_t modeRegister = 0b0000'0000;
    HAL_StatusTypeDef status =
        HAL_I2C_Mem_Write(&I2C_HANDLER,
                          I2C_ADDRESS,
                          MODE_REGISTER,
                          I2C_MEMADD_SIZE_8BIT,
                          &modeRegister,
                          sizeof(modeRegister),
                          TIMEOUT);
    return status == HAL_OK;
}

static bool SetContinousMode() {
    // For continous mode: bit 0 and 1 should be 00;
    uint8_t modeRegister     = 0b0000'0000;
    HAL_StatusTypeDef status = Write(
        MODE_REGISTER, &modeRegister, sizeof(modeRegister));
    return status == HAL_OK;
}

// DATA* registers

/** Get 3-axis heading measurements.
 * In the event the ADC reading overflows or underflows for
 * the given channel, or if there is a math overflow during
 * the bias measurement, this data register will contain the
 * value -4096. This register value will clear when after
 * the next valid measurement is made. Note that this method
 * automatically clears the appropriate bit in the MODE
 * register if Single mode is active.
 * @param x 16-bit signed integer container for X-axis
 * heading
 * @param y 16-bit signed integer container for Y-axis
 * heading
 * @param z 16-bit signed integer container for Z-axis
 * heading
 * @see HMC5883L_RA_DATAX_H
 */
bool HMC5883LDriver_GetHeading(int16_t* x, int16_t* y,
                               int16_t* z) {
    uint8_t buffer[6];
    HAL_StatusTypeDef status =
        Read(DATA_START_REGISTER, buffer, sizeof(buffer));
    *x = (((int16_t)buffer[0]) << 8) | buffer[1];
    *y = (((int16_t)buffer[4]) << 8) | buffer[5];
    *z = (((int16_t)buffer[2]) << 8) | buffer[3];

    return status == HAL_OK;
}
/** Get X-axis heading measurement.
 * @return 16-bit signed integer with X-axis heading
 * @see HMC5883L_RA_DATAX_H
 */

// function that returns the offset by comparing HMC5883
// data in a default state and a cellphone aproximation
calibration_offset_t HMC5883LDriver_Calibration() {
    int16_t x_cellphone, y_cellphone, z_cellphone;
    int16_t x_default, y_default, z_default;
    calibration_offset_t offsets;

    // Get default readings from your HMC5883L sensor
    HMC5883LDriver_GetHeading(
        &x_default, &y_default, &z_default);

    // Delay to allow time for the cellphone magnetometer to
    // stabilize
    HAL_Delay(10000); // Adjust the delay as needed

    // Get readings from the cellphone magnetometer
    HMC5883LDriver_GetHeading(
        &x_cellphone, &y_cellphone, &z_cellphone);

    // Calculate offsets
    offsets.x_axis = x_default - x_cellphone;
    offsets.y_axis = y_default - y_cellphone;
    offsets.z_axis = z_default - z_cellphone;

    return offsets;
}

static bool Write(uint8_t address, uint8_t* data,
                  uint16_t size) {
    HAL_I2C_Mem_Write(&I2C_HANDLER,
                      I2C_ADDRESS,
                      address,
                      I2C_MEMADD_SIZE_8BIT,
                      data,
                      size,
                      TIMEOUT);
}

static bool Read(uint8_t address, uint8_t* data,
                 uint16_t size) {
    HAL_I2C_Mem_Read(&I2C_HANDLER,
                     I2C_ADDRESS,
                     address,
                     I2C_MEMADD_SIZE_8BIT,
                     data,
                     size,
                     TIMEOUT);
}
