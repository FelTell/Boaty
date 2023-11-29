#include "Drivers/BLE_JDY_18.h"

extern UART_HandleTypeDef huart3;
#define UART_HANDLER huart3

#define MESSAGE_TIMEOUT_MS 10

static SlaveDevice_t slaves[10];

// Prefixos comandos AT
const char* at_commands[] = {
    "AT+PERM",
    "AT+RESET",
    "AT+NAME=",
    "AT+ROLE=",
    "AT+BAUD=",
    "AT+UUID=",
    "AT+POWER=",
    "AT+INQ",
    "AT+CONN",
    // Add mais prefixos
};

void BeaconPosition_Init() {
    // APP permission Settings, all set
    BLE_send_command(AT_COMMAND_SET_PERMISSIONS, "11111");
}

// bool is_MAC_in_list(const const char* mac_to_check) {
//     for (int i = 0; i < MAX_SLAVES; i++) {
//         if (strcmp(slave_list[i].mac_address,
//                    mac_to_check) == 0) {
//             return true;
//         }
//     }
//     return false;
// }

// // escaneia por dispositivos BLE próximos e armazena suas
// // informações em uma lista
// void BLE_scan_slaves_and_save(int max_slaves) {
//     BLE_send_command(AT_COMMAND_SCAN_SLAVES, "");

//     char dados_recebidos[128];
//     char* mac_ptr = strstr(dados_recebidos, "MAC:");

//     if (mac_ptr == NULL) {
//         return;
//     }
//     // Encontrou um pacote válido
//     char mac_address[18];
//     strncpy(mac_address,
//             mac_ptr + 4,
//             17);            // Copia o número MAC
//     mac_address[17] = '\0'; // Adiciona caractere nulo
//     if (is_MAC_in_list(slave_list, mac_address) == false)
//     {
//         strcpy(slave_list[slave_list->topo].mac_address,
//                mac_address); // Salva na lista
//         slave_list->topo++;
//     }
// }

// void BLE_connect_Master_to_Slave_MAC(char* mac) {
//     BLE_send_command(AT_COMMAND_CONECTAR, mac);
// }

// Envia comando AT para o módulo JDY-18
void BLE_send_command(AtCommands_t command,
                      char* parameter) {
    char* command_prefix = (char*)at_commands[command];

    size_t command_length = strlen(command_prefix) +
                            strlen(parameter) +
                            4; // 4 para "\r\n"

    char complete_command[command_length];

    // Comando AT completo
    snprintf(complete_command,
             command_length,
             "%s%s\r\n",
             command_prefix,
             parameter);

    // Envia o comando AT para a UART
    HAL_UART_Transmit(&UART_HANDLER,
                      (uint8_t*)complete_command,
                      strlen(complete_command),
                      MESSAGE_TIMEOUT_MS);
}

// void init_media_movel(MediaMovel_t* media) {
//     for (int i = 0; i < MEDIA_MOVEL_JANELA; i++) {
//         media->valores[i] = 0;
//     }
//     media->indice = 0;
//     media->soma   = 0;
// }

// int update_media_movel(MediaMovel_t* media, int
// new_value) {
//     media->soma -= media->valores[media->indice];
//     media->soma += new_value;
//     media->valores[media->indice] = new_value;
//     media->indice =
//         (media->indice + 1) % MEDIA_MOVEL_JANELA;
//     return media->soma / MEDIA_MOVEL_JANELA;
// }
