/* Copyright (c) 2018 Cameron Harper
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * */

#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @defgroup uart
 * 
 * Interrupt driven UART with fixed size RX and TX buffers
 * 
 * @{
 * */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef UART_TX_SIZE
/** size of TX FIFO */
#   define UART_TX_SIZE 10U
#endif

#ifndef UART_RX_SIZE
/** size of RX FIFO */
#   define UART_RX_SIZE 10U
#endif

typedef void (*uart_handler_t)(void *user);

/**
 * Initialise UART
 * 
 * @param[in] baud
 * 
 * */
void uart_init(uint32_t baud, uart_handler_t rx_ready, uart_handler_t tx_empty);

/**
 * Write a byte
 * 
 * @param[in] c
 * 
 * @retval true 
 * @retval false TX FIFO is full
 * 
 * */
bool uart_write(uint8_t c);

/**
 * Read a byte
 * 
 * @param[out] c
 * 
 * @retval true
 * @retval false RX FIFO is empty
 * 
 * */
bool uart_read(uint8_t *c);

/**
 * Is the TX FIFO full?
 * 
 * @retval true TX FIFO is full
 * @retval false
 * 
 * */
bool uart_tx_full(void);

/**
 * Is the RX FIFO empty?
 * 
 * @retval true RX FIFO empty
 * @retval false
 * 
 * */
bool uart_rx_empty(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif
