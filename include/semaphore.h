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

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

/**
 * @defgroup semaphore
 * 
 * Non-blocking semaphores
 * 
 * @{
 * */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/** semaphore state */
struct semaphore {
    
    volatile uint8_t max;
    volatile uint8_t count;
};

/**
 * Initialise a semaphore
 * 
 * @param[in] self
 * @param[in] max semaphore will count to this value
 * 
 * */
void semaphore_init(struct semaphore *self, uint8_t max);

/**
 * Signal (count up) a semaphore
 * 
 * @param[in] self
 * 
 * */
void semaphore_signal(struct semaphore *self);

/**
 * Wait (count down) on a semaphore
 * 
 * @param[in] self
 * @retval true
 * @retval false still waiting for a signal
 * 
 * */
bool semaphore_wait(struct semaphore *self);

/**
 * Check if wait will succeed without affecting the state
 * 
 * @param[in] self
 * @retval true wait will succeed
 * @retval false
 * 
 * */
bool semaphore_peek(struct semaphore *self);

#ifdef __cplusplus
}
#endif

/** @} */
#endif
