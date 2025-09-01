#include "main.h"
#include <string.h>
#include <stdio.h>


// Buffer for receiving UART data
#define RX_BUFFER_SIZE 64
#define TX_BUFFER_SIZE 128
uint8_t rxBuffer[RX_BUFFER_SIZE];
uint8_t rxIndex = 0;
uint8_t rxData; // Single byte for RX interrupt

// Buffer for transmitting UART data
char txBuffer[TX_BUFFER_SIZE];

// Function prototypes
void UART_ProcessReceivedData(void);
void UART_TransmitMessage(const char *message);

// UART RX Interrupt Callback
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huartX)
    {
        // Echo back the received byte (optional)
        HAL_UART_Transmit(&huartX, &rxData, 1, HAL_MAX_DELAY);

        // Store received byte in buffer
        if (rxData == '\n' || rxData == '\r' || rxIndex >= RX_BUFFER_SIZE - 1)
        {
            rxBuffer[rxIndex] = '\0'; // Null-terminate the string
            UART_ProcessReceivedData(); // Process the received data
            rxIndex = 0; // Reset buffer index
        }
        else
        {
            rxBuffer[rxIndex++] = rxData; // Add byte to buffer
        }

        // Restart UART RX interrupt for the next byte
        HAL_UART_Receive_IT(&huartX, &rxData, 1);
    }
}

// Process received UART data
void UART_ProcessReceivedData(void)
{
    // Example: Parse received data (customize as needed)
    if (strcmp((char *)rxBuffer, "PING\r") == 0)
    {
        UART_TransmitMessage("PONG\r\n");
    }
    else if (strcmp((char *)rxBuffer, "STATUS\r") == 0)
    {
        UART_TransmitMessage("System OK\r\n");
    }
    else
    {
        // Echo back unrecognized command with prefix
        snprintf(txBuffer, TX_BUFFER_SIZE, "ECHO: %s\r\n", rxBuffer);
        UART_TransmitMessage(txBuffer);
    }
}

// Transmit a message over UART
void UART_TransmitMessage(const char *message)
{
    HAL_UART_Transmit(&huartX, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
}

int main(void)
{
    HAL_Init();
    HAL_UART_Receive_IT(&huartX, &rxData, 1);

    while (1)
    {
        UART_TransmitMessage("Howdy!\r\n");
        HAL_Delay(1000); // Delay 1 second (adjust as needed)
    }
}