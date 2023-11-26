#include "Drivers/BLE_JDY_18.h"

#include "global_instances.h"

UART_HandleTypeDef* huart;

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

void Ble_Handler() {
    for (;;) {
        bleMessage_t message;
        SlaveDevice_t slaves[3];
        BLE_scan_slaves_and_save(message.slaves, 3);

        osMessageQueuePutOverwrite(
            q_bleMessageHandle, &message, 0);

        osDelay(200);
    }
}

// Função para configurar o JDY-18 com configurações
// iniciais
void BLE_setup(UART_HandleTypeDef* huartInt,
               SlaveDevice_t* slave_list,
               char* nome,
               Funcao_t funcao,
               Baudrate_t baud,
               char* uuid,
               int power_pctg) {
    huart            = huartInt;
    slave_list->topo = 0;

    // APP permission Settings, todos habilitados
    BLE_send_command(AT_COMMAND_SET_PERMISSIONS, "11111");

    // Define o nome do dispositivo
    BLE_send_command(AT_COMMAND_DEFINIR_NOME, nome);

    // Define a função do dispositivo
    char funcao_str[2];
    itoa(funcao, funcao_str, 10);
    BLE_send_command(AT_COMMAND_DEFINIR_FUNCAO, funcao_str);

    // Define a taxa de transmissão (baud rate)
    char baud_str[2];
    itoa(baud, baud_str, 10);
    BLE_send_command(AT_COMMAND_DEFINIR_BAUD, baud_str);

    // Define o UUID do Beacon
    BLE_send_command(AT_COMMAND_DEFINIR_UUID, uuid);

    // Define a potência de transmissão
    float power = power_pctg / 100.0;
    char power_str[5];
    gcvt(power, 2, power_str);
    BLE_send_command(AT_COMMAND_DEFINIR_POTENCIA,
                     power_str);
}

bool is_MAC_in_list(const SlaveDevice_t* slave_list,
                    const char* mac_to_check) {
    for (int i = 0; i < MAX_SLAVES; i++) {
        if (strcmp(slave_list[i].mac_address,
                   mac_to_check) == 0) {
            return true;
        }
    }
    return false;
}

// escaneia por dispositivos BLE próximos e armazena suas
// informações em uma lista
void BLE_scan_slaves_and_save(SlaveDevice_t* slave_list,
                              int max_slaves) {
    BLE_send_command(AT_COMMAND_SCAN_SLAVES, "");

    char dados_recebidos[128];
    char* mac_ptr = strstr(dados_recebidos, "MAC:");

    if (mac_ptr == NULL) {
        return;
    }
    // Encontrou um pacote válido
    char mac_address[18];
    strncpy(
        mac_address, mac_ptr + 4, 17); // Copia o número MAC
    mac_address[17] = '\0'; // Adiciona caractere nulo
    if (is_MAC_in_list(slave_list, mac_address) == false) {
        strcpy(slave_list[slave_list->topo].mac_address,
               mac_address); // Salva na lista
        slave_list->topo++;
    }
}

void BLE_connect_Master_to_Slave_MAC(char* mac) {
    BLE_send_command(AT_COMMAND_CONECTAR, mac);
}

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
    HAL_UART_Transmit(&huart,
                      (uint8_t*)complete_command,
                      strlen(complete_command),
                      HAL_MAX_DELAY);
}

void init_media_movel(MediaMovel_t* media) {
    for (int i = 0; i < MEDIA_MOVEL_JANELA; i++) {
        media->valores[i] = 0;
    }
    media->indice = 0;
    media->soma   = 0;
}

int update_media_movel(MediaMovel_t* media, int new_value) {
    media->soma -= media->valores[media->indice];
    media->soma += new_value;
    media->valores[media->indice] = new_value;
    media->indice =
        (media->indice + 1) % MEDIA_MOVEL_JANELA;
    return media->soma / MEDIA_MOVEL_JANELA;
}
