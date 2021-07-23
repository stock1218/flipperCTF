#include <api-hal-console.h>

#include <stdbool.h>
#include <stm32wbxx_ll_gpio.h>
#include <stm32wbxx_ll_usart.h>

volatile bool api_hal_console_alive = false;

void api_hal_console_init() {
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LL_GPIO_PIN_6|LL_GPIO_PIN_7;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    LL_USART_InitTypeDef USART_InitStruct = {0};
    USART_InitStruct.PrescalerValue = LL_USART_PRESCALER_DIV1;
    USART_InitStruct.BaudRate = 230400;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(USART1, &USART_InitStruct);
    LL_USART_SetTXFIFOThreshold(USART1, LL_USART_FIFOTHRESHOLD_1_2);
    LL_USART_EnableFIFO(USART1);
    LL_USART_ConfigAsyncMode(USART1);

    LL_USART_Enable(USART1);

    while(!LL_USART_IsActiveFlag_TEACK(USART1)) ;
    api_hal_console_alive = true;
}

void api_hal_console_tx(const uint8_t* buffer, size_t buffer_size) {
    if (!api_hal_console_alive)
        return;

    while(buffer_size > 0) {
        while (!LL_USART_IsActiveFlag_TXE(USART1));

        LL_USART_TransmitData8(USART1, *buffer);

        buffer++;
        buffer_size--;
    }

    /* Wait for TC flag to be raised for last char */
    while (!LL_USART_IsActiveFlag_TC(USART1));
}