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

#ifndef FIFO_H
#define FIFO_H

/** @file */

/**
 * @defgroup fifo
 * 
 * Byte oriented fixed size FIFO
 *  
 * @{
 * */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/** FIFO state */
struct fifo {
    
    volatile uint8_t *buffer;
    size_t max;
    size_t size;
    size_t count;
};

/** 
 * initialise a FIFO
 * 
 * @param[in] self
 * @param[in] buffer    FIFO memory
 * @param[in] max       size of buffer in bytes
 * 
 * */
void fifo_init(volatile struct fifo *self, volatile uint8_t *buffer, size_t max);

/**
 * Push byte onto FIFO
 * 
 * @param[in] self
 * @param[in] value
 * 
 * @retval true
 * @retval false FIFO is full
 * 
 * */
bool fifo_push(volatile struct fifo *self, uint8_t value);

/**
 * Pop byte from FIFO
 * 
 * @param[in] self
 * @param[out] value
 * 
 * @retval true
 * @retval false FIFO is empty
 * 
 * */
bool fifo_pop(volatile struct fifo *self, uint8_t *value);

/**
 * Return current size of FIFO in bytes
 * 
 * @param[in] self
 * @return current size in bytes
 * 
 * */
size_t fifo_size(volatile const struct fifo *self);

/**
 * Return maximum size of FIFO in bytes
 * 
 * @param[in] self
 * @return maximum size in bytes
 * 
 * */
size_t fifo_max(volatile const struct fifo *self);

/**
 * Is FIFO empty?
 * 
 * @param[in] self
 * @retval true yes
 * @retval false no
 * 
 * */
bool fifo_empty(volatile const struct fifo *self);

/**
 * Is FIFO full?
 * 
 * @param[in] self
 * @retval true yes
 * @retval false no
 * 
 * */
bool fifo_full(volatile const struct fifo *self);

#ifdef __cplusplus
}
#endif

/** @} */
#endif
