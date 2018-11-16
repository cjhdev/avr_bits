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

#include <avr/io.h>
#include <util/atomic.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "fifo.h"

#ifndef F_CPU
#   warning F_CPU defaults to 16000000UL
#   define F_CPU 16000000UL
#endif

static volatile struct fifo tx;
static volatile struct fifo rx;
static volatile uint8_t tx_mem[UART_TX_SIZE];
static volatile uint8_t rx_mem[UART_TX_SIZE];
static volatile uart_handler_t rx_ready_handler;
static volatile uart_handler_t tx_empty_handler;

/* static function prototypes *****************************************/

static uint32_t f_cpu(void);
static uint16_t setting_from_baud(uint32_t baud, bool x2);
static uint32_t baud_from_setting(uint16_t setting, bool x2);
static bool use_2x(uint32_t ideal, uint16_t single_setting, uint16_t double_setting);
static uint32_t delta(uint16_t a, uint16_t b);

/* functions **********************************************************/

void uart_init(uint32_t baud, uart_handler_t rx_ready, uart_handler_t tx_empty)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){            

        uint16_t setting1 = setting_from_baud(baud, false);
        uint16_t setting2 = setting_from_baud(baud, true);
        
        if(use_2x(baud, setting1, setting2)){
            
            UBRR0 = setting2;
            UCSR0A |= _BV(U2X0); 
        }
        else{
            
            UBRR0 = setting1;
            UCSR0A &= ~_BV(U2X0); 
        }
        
        /* enable tx and rx, and rx interrupts */
        UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0);
        
        /* 8 bit character */
        UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);     
        
        fifo_init(&rx, rx_mem, sizeof(rx_mem));
        fifo_init(&tx, tx_mem, sizeof(tx_mem));

        rx_ready_handler = rx_ready;
        tx_empty_handler = tx_handler;
    }
}

bool uart_write(uint8_t c)
{
    bool retval;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){            
        
        if(fifo_empty(&tx) && ((UCSR0A & _BV(UDRE0)) > 0U)){
            
            UDR0 = c;
            UCSR0B |= _BV(UDRIE0);
            retval = true;
        }
        else{
        
            retval = fifo_push(&tx, c);
        }                
    }
    
    return retval;
}

bool uart_read(uint8_t *c)
{
    bool retval;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){            
        
        retval = fifo_pop(&rx, c);        
    }
    
    return retval;
}

bool uart_tx_full(void)
{
    return fifo_full(&tx);
}

ISR(USART_RX_vect)
{    
    (void)fifo_push(&rx, UDR0);
    rx_ready_handler();
}

ISR(USART_UDRE_vect)
{
    uint8_t c;
    
    if(fifo_pop(&tx, &c)){
        
        UDR0 = c;
    }
    else{
     
        UCSR0B &= ~_BV(UDRIE0);
    }
    
    if(fifo_empty(&tx)){
        
        tx_empty_handler();
    }    
}

/* static functions ***************************************************/

static uint32_t f_cpu(void)
{
    return (F_CPU >> (CLKPR & 0xfU));
}

static uint16_t setting_from_baud(uint32_t baud, bool x2)
{
    return (f_cpu() / ((x2 ? 8UL : 16UL) * baud)) - 1U;
}

static uint32_t baud_from_setting(uint16_t setting, bool x2)
{
    return f_cpu() / ((x2 ? 8UL : 16UL) * (setting + 1U));
}

static uint32_t delta(uint16_t a, uint16_t b)
{
    return (a > b) ? (a - b) : (b - a);    
}

static bool use_2x(uint32_t ideal, uint16_t single_setting, uint16_t double_setting)
{
    return delta(ideal, baud_from_setting(double_setting, true)) < delta(ideal, baud_from_setting(single_setting, false));
}
