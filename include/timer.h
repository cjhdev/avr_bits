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

#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <stdbool.h>

#define TIMER_TICKS_PER_SECOND (256UL/8UL)

struct timer_event;

typedef void (*timer_handle_fn)(volatile struct timer_event *ev);

struct timer_event {
    volatile struct timer_event *next;    
    uint32_t timeout;
    uint32_t interval;
    bool once;
    timer_handle_fn handler;
};

/**
 * Initialise the underlying timer/counter to start the timer ticking
 * 
 * */
void timer_start(void);

/** 
 * Current value of timer ticks
 * 
 * @return timer ticks
 * 
 * */
uint32_t timer_get_time(void);

/**
 * Set a timer
 * 
 * @warning do not accidentally link the same timer state more than once
 * 
 * @param[in] self      pointer to app managed state
 * @param[in] interval
 * @param[in] handler
 * 
 * */
void timer_set(volatile struct timer_event *self, uint32_t interval, timer_handle_fn handler);

/**
 * Clear a timer
 * 
 * @param[in] self      pointer to app managed state
 * 
 * 
 * */
void timer_clear(volatile struct timer_event *self);


/**
 * Difference between two timer_get_time() readings
 * 
 * @code
 * uint32_t timestamp = timer_get_time();
 * // time passes, system wakes up
 * uint32_t interval_since_timestamp = timer_interval(timestamp, timer_get_time());
 * @endcode
 * 
 * @param[in] t1
 * @param[in] t2
 * @return difference between t1 and t2
 * 
 * */
uint32_t timer_interval(uint32_t t1, uint32_t t2);

#endif
