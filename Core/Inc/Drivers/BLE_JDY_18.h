#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "main.h"


#define MEDIA_MOVEL_JANELA 10

typedef enum {
	SLAVE = 0,
	MASTER = 1,
	IBEACON = 3
} Funcao_t;

// JDY-18 comandos AT
typedef enum {
	AT_COMMAND_SET_PERMISSIONS,
    AT_COMMAND_RESET,
    AT_COMMAND_DEFINIR_NOME,
    AT_COMMAND_DEFINIR_FUNCAO,
    AT_COMMAND_DEFINIR_BAUD,
    AT_COMMAND_DEFINIR_UUID,
    AT_COMMAND_DEFINIR_POTENCIA,
    AT_COMMAND_SCAN_SLAVES,
    AT_COMMAND_CONECTAR,
    // Adicionar comandos
} AtCommands_t;


typedef struct {
    char mac_address[18]; // Endereço MAC do escravo (por exemplo, "00:11:22:33:44:55")
	int signal_rssi;
} SlaveDevice_t;

typedef enum {
	BAUD_4800 = 3,
	BAUD_9600 = 4,
	BAUD_19200 = 5,
	BAUD_38400 = 6,
	BAUD_57600 = 7,
	BAUD_115200 = 8,
} Baudrate_t;

typedef struct {
    int valores[MEDIA_MOVEL_JANELA];
    int indice;
    int soma;
} MediaMovel_t;

void BLE_setup(UART_HandleTypeDef *huartInt, char *nome, Funcao_t funcao, Baudrate_t baud, char *uuid, int power_pctg);
void BLE_scan_slaves(SlaveDevice_t *slave_list, int max_slaves);
void BLE_connect_Master_to_Slave_MAC (char *mac);
void BLE_send_command(AtCommands_t command, char *parameter);


void init_media_movel(MediaMovel_t *media);
int update_media_movel(MediaMovel_t *media, int new_value);


