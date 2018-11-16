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

#ifndef SPI_H
#define SPI_H

/** @file */

/**
 * @defgroup spi
 * 
 * Blocking SPI interface
 * 
 * @warning do not use from an interrupt context
 * 
 * @{
 * */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/** SPI mode */
enum spi_mode {
    SPI_MODE_0 = 0x0U,
    SPI_MODE_1 = 0x4U,
    SPI_MODE_2 = 0x8U,
    SPI_MODE_3 = 0xCU
};

/** SPI bit order */
enum spi_order {
    SPI_ORDER_MSB,
    SPI_ORDER_LSB,    
};

/** 
 * 
 * Initialise SPI 
 * 
 * @param[in] mode
 * @param[in] order
 * @param[in] rate clock rate in Hz
 * 
 * */
void spi_init(enum spi_mode mode, enum spi_order order, uint32_t rate);

/**
 * Write (and read) SPI
 * 
 * @param[in] data
 * @return byte read
 * 
 * */
uint8_t spi_write(uint8_t data);

#ifdef __cplusplus
}
#endif

/** @} */
#endif
