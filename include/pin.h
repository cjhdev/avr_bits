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

#ifndef PIN_H
#define PIN_H

/** @file */

/**
 * @defgroup pin
 * 
 * Pin/Port Control Interfaces with Arduino naming scheme
 * 
 * @{
 * */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

/** direction */
enum pin_direction {PIN_INPUT, PIN_OUTPUT};

/** Arduino pin names */
enum pin_id {
  
    PIN_D0,
    PIN_D1,
    PIN_D2,
    PIN_D3,
    PIN_D4,
    PIN_D5,
    PIN_D6,
    PIN_D7,
    PIN_D8,
    PIN_D9,
    PIN_D10,
    PIN_D11,
    PIN_D12,
    PIN_D13,
    
    PIN_A0,
    PIN_A1,
    PIN_A2,
    PIN_A3,
    PIN_A4,
    PIN_A5,
    
    PIN_NA      /**< not connected */
};

/** pin change interrupt handler */
typedef void (*pin_pcint_handler_t)(void);

/** pin change interrupt state */
struct pin_pcint {
    
    bool state;
    enum pin_id id;
    enum pin_pcint_mode {
        PIN_RISING,
        PIN_FALLING,
        PIN_CHANGE
    } mode;
    pin_pcint_handler_t handler;
    volatile struct pin_pcint *next;
};

/**
 * get logical state of a pin
 * 
 * @param[in] id
 * 
 * @retval true pin is high
 * @retval false pin is low
 * 
 * */
bool pin_get(enum pin_id id);

/**
 * set logical state of a pin
 * 
 * @param[in] id 
 * @param[in] dir   direction of pin
 * @param[in] on    true if high
 * 
 * /bexamples:
 * 
 * Drive a pin low:
 * 
 * @code
 * pin_set(PIN_D0, PIN_OUTPUT, false);
 * @endcode
 *
 * Drive a pin high:
 * 
 * @code
 * pin_set(PIN_D0, PIN_OUTPUT, true);
 * @endcode
 * 
 * Set a pin as HIZ input:
 * 
 * @code
 * pin_set(PIN_D0, PIN_INPUT, false);
 * @endcode
 * 
 * Set a pin as input with pullup enabled
 * 
 * @code
 * pin_set(PIN_D0, PIN_INPUT, true);
 * @endcode
 *  * 
 * */
void pin_set(enum pin_id id, enum pin_direction dir, bool on);

/**
 * Associate a handle function with a pin change interrupt
 * 
 * @note it is posible to have more than one handler on a single PCINT
 * 
 * @param[in] self linked into a queue of PCINTs
 * @param[in] id 
 * @param[in] mode rising/falling/any
 * @param[in] handler 
 * 
 * */
void pin_set_pcint_handler(volatile struct pin_pcint *self, enum pin_id id, enum pin_pcint_mode mode, pin_pcint_handler_t handler);

/**
 * Disassociate handler from pin change interrupt
 * 
 * @param[in] self
 * 
 * */
void pin_clear_pcint_handler(const struct pin_pcint *self);

#ifdef __cplusplus
}
#endif

/** @} */
#endif
