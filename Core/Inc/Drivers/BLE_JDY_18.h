#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "main.h"


#define MEDIA_MOVEL_JANELA 10
#define MAX_SLAVES 3

typedef enum {
    SLAVE,
    MASTER,
    IBEACON,
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
    char mac_address[18];
    int signal_rssi;
    int topo;
} SlaveDevice_t;

typedef enum {
    BAUD_4800,
    BAUD_9600,
    BAUD_19200,
    BAUD_38400,
    BAUD_57600,
    BAUD_115200,
} Baudrate_t;

typedef struct {
    int valores[MEDIA_MOVEL_JANELA];
    int indice;
    int soma;
} MediaMovel_t;

void BLE_setup(UART_HandleTypeDef *huartInt, SlaveDevice_t *slave_list, char *nome, Funcao_t funcao, Baudrate_t baud, char *uuid, int power_pctg);
void BLE_scan_slaves_and_save(SlaveDevice_t *slave_list, int max_slaves);
void BLE_connect_Master_to_Slave_MAC (char *mac);
void BLE_send_command(AtCommands_t command, char *parameter);


void init_media_movel(MediaMovel_t *media);
int update_media_movel(MediaMovel_t *media, int new_value);
bool is_MAC_in_list(const SlaveDevice_t *slave_list, const char *mac_to_check);

