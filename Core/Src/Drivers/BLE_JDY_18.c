#include "Drivers/BLE_JDY_18.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TimerHandler.h"
#include "main.h"

extern UART_HandleTypeDef huart3;
#define UART_HANDLER huart3
extern DMA_HandleTypeDef hdma_usart3_rx;
#define DMA_HANDLER hdma_usart3_rx

#define BEACONS_NUMBER 3

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

#define BEACON1_MAC "E2BB9E4F2A02"
#define BEACON2_MAC "CA92A80D8132"
#define BEACON3_MAC "E2BB9E4F2A04"

typedef struct {
    char Mac[MAC_SIZE];
    int32_t Rssi;
} Slave_t;

struct {
    Slave_t pSlave;
    float distanceCm;
} beacons[BEACONS_NUMBER];

static bool RequestScan();
static bool SendCommand(const char* command,
                        const char* parameter);
static void StoreIfBeacon(Slave_t* slave);

static uint8_t rxDmaBuffer[DMA_BUFFER_SIZE];
static uint8_t rxBuffer[MESSAGE_MAX_SIZE];

static char beaconsMAC[BEACONS_NUMBER][MAC_SIZE] = {
    BEACON1_MAC,
    BEACON2_MAC,
    BEACON3_MAC};
static float distances[BEACONS_NUMBER];

static volatile uint8_t rxBytesLeftToRead = 0;
static volatile bool dataReady            = false;

bool BeaconDistance_Init() {
    HAL_UART_Receive_DMA(&UART_HANDLER,
                         rxDmaBuffer,
                         sizeof(rxDmaBuffer));
    __HAL_UART_ENABLE_IT(&UART_HANDLER, UART_IT_IDLE);

    // Increase baudrate as default is too low (9600)
    if (!SendCommand(COMMAND_SET_BAUD, BAUD_SET_115200)) {
        return false;
    }
    HAL_Delay(10);
    UART_HANDLER.Init.BaudRate = 115200;
    if (HAL_UART_Init(&UART_HANDLER) != HAL_OK) {
        return false;
    }
    if (!SendCommand(COMMAND_SET_ROLE, MODE_SET_MASTER)) {
        return false;
    }

    return true;
}

bool BeaconDistance_Handler() {
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

static bool RequestScan() {
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
                pow(10.f, (-69 - slave->Rssi) / 20.f);

            beacons[i].pSlave = *slave;
            beacons[i].distanceCm =
                (float)((pow(10.f,
                             (float)((-69 - slave->Rssi) /
                                     20.f))) *
                        10.f);
            return;
        }
    }
}

void BeaconDistance_IdleCallback() {
    static volatile uint8_t currentCounter;
    static volatile uint8_t lastCounter;

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
