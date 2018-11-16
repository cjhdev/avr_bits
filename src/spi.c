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

#include "spi.h"
#include "pin.h"

#include <avr/io.h>
#include <avr/power.h>
#include <util/atomic.h>

void spi_init(enum spi_mode mode, enum spi_order order, uint32_t rate)
{
    uint32_t clock_setting;
    uint8_t clock_div;
    
    /* atmega328: SCK (output) */
    pin_setPin(PIN_D13, PIN_OUTPUT, false);
    
    /* atmega328: MISO (input, pullup) */
    pin_setPin(PIN_D12, PIN_INPUT, true);
    
    /* atmega328: MOSI (output) */
    pin_setPin(PIN_D11, PIN_OUTPUT, false);
     
    clock_setting = (F_CPU >> (CLKPR & 0xfU)) >> 1U;
    clock_div = 0U;
            
    while((clock_div < 6U) && (rate < clock_setting)){
        
        clock_setting =  clock_setting >> 1U;
        clock_div++;
    }
    
    if(clock_div == 6U){
        
        clock_div = 7U;
    }
    
    clock_div ^= 1U;
    
    SPCR = _BV(MSTR) | _BV(SPE) | 
        ((order == SPI_ORDER_LSB) ? _BV(DORD) : 0U) | 
        (uint8_t)mode |         
        ((clock_div >> 1U) & 3U);
    
    SPSR = (clock_div & 1U);    
}

uint8_t spi_write(uint8_t data)
{
    SPDR = data;
    while(!(SPSR & _BV(SPIF)));
    return SPDR;
}
