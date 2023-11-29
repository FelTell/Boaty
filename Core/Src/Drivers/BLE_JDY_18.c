#include "Drivers/BLE_JDY_18.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TimerHandler.h"
#include "usart.h"

#define UART_HANDLER huart3
#define DMA_HANDLER  hdma_usart3_rx

#define MESSAGE_TIMEOUT_MS 10
#define DMA_BUFFER_SIZE    256

#define MESSAGE_MAX_SIZE 100

#define COMMAND_SET_BAUD "AT+BAUD"
#define BAUD_SET_115200  "8"

#define COMMAND_SET_ROLE "AT+ROLE"
#define MODE_SET_MASTER  "1"

#define COMMAND_SCAN "AT+INQ"

#define MAC_SIZE 12

#define ANSWER_SCAN            "+DEV"
#define ANSWER_SCAN_SIZE       4
#define ANSWER_SCAN_MAC_START  7
#define ANSWER_SCAN_RSSI_START (7 + MAC_SIZE + 1)

#define BEACON1_MAC "3CA551936A50"
#define BEACON2_MAC "3CA551936A67"
#define BEACON3_MAC "3CA551936A41"

#define BEACON1_POWER (-69)
#define BEACON2_POWER (-69)
#define BEACON3_POWER (-69)

typedef struct {
    char Mac[MAC_SIZE];
    int32_t Rssi;
} Slave_t;

static bool RequestScan(void);
static bool SendCommand(const char* command,
                        const char* parameter);
static void StoreIfBeacon(Slave_t* slave);
static float GetDistanceFromRssi(float signalStrength,
                                 uint8_t beaconIndex);
static void StartDma(void);

static uint8_t rxDmaBuffer[DMA_BUFFER_SIZE];
static uint8_t rxBuffer[MESSAGE_MAX_SIZE];

static const char beaconsMAC[BEACONS_NUMBER][MAC_SIZE] = {
    BEACON1_MAC,
    BEACON2_MAC,
    BEACON3_MAC};
static const float beaconsPower[BEACONS_NUMBER] = {
    BEACON1_POWER,
    BEACON2_POWER,
    BEACON3_POWER};

static float distances[BEACONS_NUMBER] = {-1, -1, -1};
static Slave_t beacons[BEACONS_NUMBER];

static volatile uint8_t rxBytesLeftToRead = 0;
static volatile bool dataReady            = false;
static volatile uint8_t currentCounter    = 0;
static volatile uint8_t lastCounter       = 0;

bool BeaconDistance_Init(void) {
    // Increase baudrate as default is too low (9600)
    if (!SendCommand(COMMAND_SET_BAUD, BAUD_SET_115200)) {
        return false;
    }
    UART_HANDLER.Init.BaudRate = 115200;
    if (HAL_UART_Init(&UART_HANDLER) != HAL_OK) {
        return false;
    }
    if (!SendCommand(COMMAND_SET_ROLE, MODE_SET_MASTER)) {
        return false;
    }
    StartDma();

    currentCounter = 0;
    lastCounter    = 0;

    return true;
}

bool BeaconDistance_Handler(void) {
    static uint32_t scanBeacontimer;

    if (Timer_WaitMs(scanBeacontimer, 200)) {
        scanBeacontimer = Timer_Update();
        if (!RequestScan()) {
            return false;
        }
    }

    if (rxBytesLeftToRead == 0) {
        return true;
    }

    // Disable interrupt to prevent unwanted changes while
    // manipulation buffer
    __HAL_UART_DISABLE_IT(&UART_HANDLER, UART_IT_IDLE);
    if (strncmp((char*)rxBuffer,
                ANSWER_SCAN,
                ANSWER_SCAN_SIZE) == 0) {
        Slave_t receivedSlave;
        memcpy(receivedSlave.Mac,
               &rxBuffer[ANSWER_SCAN_MAC_START],
               MAC_SIZE);
        receivedSlave.Rssi =
            atoi((char*)&rxBuffer[ANSWER_SCAN_RSSI_START]);
        // Ignore name as MAC is enough
        StoreIfBeacon(&receivedSlave);
    }
    __HAL_UART_ENABLE_IT(&UART_HANDLER, UART_IT_IDLE);

    rxBytesLeftToRead = 0;

    return true;
}

bool BeaconDistance_GetDistances(
    float pDistances[BEACONS_NUMBER]) {
    for (uint8_t i = 0; i < BEACONS_NUMBER; ++i) {
        if (distances[i] < 0) {
            return false;
        }
    }
    memcpy(pDistances, distances, sizeof(distances));

    return 1;
}

static bool RequestScan(void) {
    if (!SendCommand(COMMAND_SCAN, "")) {
        return false;
    }
    return true;
}

static bool SendCommand(const char* command,
                        const char* parameter) {
    char message[MESSAGE_MAX_SIZE];

    // Comando AT completo
    const uint32_t size = snprintf(message,
                                   MESSAGE_MAX_SIZE,
                                   "%s%s\r\n",
                                   command,
                                   parameter);

    if (HAL_UART_Transmit(&UART_HANDLER,
                          (uint8_t*)message,
                          size,
                          MESSAGE_TIMEOUT_MS) != HAL_OK) {
        return false;
    }
    return true;
}

static void StoreIfBeacon(Slave_t* slave) {
    for (uint8_t i = 0; i < BEACONS_NUMBER; ++i) {
        if (memcmp(slave->Mac, beaconsMAC[i], MAC_SIZE) ==
            0) {
            distances[i] =
                GetDistanceFromRssi(slave->Rssi, i);
            beacons[i] = *slave;
            return;
        }
    }
}

static float GetDistanceFromRssi(float signalStrength,
                                 uint8_t beaconIndex) {
    if (!(beaconIndex < BEACONS_NUMBER)) {
        return -1;
    }
    return pow(
        10,
        (beaconsPower[beaconIndex] - signalStrength) /
            (2 * 10));
}

static void StartDma(void) {
    HAL_UART_Receive_DMA(&UART_HANDLER,
                         rxDmaBuffer,
                         sizeof(rxDmaBuffer));
    __HAL_UART_ENABLE_IT(&UART_HANDLER, UART_IT_IDLE);
}

void BeaconDistance_ErrorCallback(void) {
    HAL_UART_DMAStop(&UART_HANDLER);
    HAL_UART_Abort(&UART_HANDLER);
    HAL_UART_DeInit(&UART_HANDLER);

    currentCounter = 0;
    lastCounter    = 0;

    MX_USART3_UART_Init();
    UART_HANDLER.Init.BaudRate = 115200;
    HAL_UART_Init(&UART_HANDLER);
    StartDma();
}

void BeaconDistance_IdleCallback(void) {
    currentCounter = (DMA_BUFFER_SIZE -
                      __HAL_DMA_GET_COUNTER(&DMA_HANDLER));

    const volatile uint8_t delta =
        currentCounter - lastCounter;

    volatile uint8_t i;
    for (i = 0; i < delta; ++i) {
        rxBuffer[i] = rxDmaBuffer[lastCounter];
        lastCounter++;
    }

    rxBytesLeftToRead = i;
}
