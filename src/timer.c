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

#include "timer.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>

static volatile struct timer_event *timers;
static volatile uint32_t time;

/* static function prototypes *****************************************/

static void timer_link(volatile struct timer_event **head, uint32_t time, volatile struct timer_event *self, uint32_t interval, timer_handle_fn handler);
static void timer_unlink(volatile struct timer_event **head, volatile struct timer_event *self);
static bool timer_already_linked(volatile struct timer_event *head, volatile struct timer_event *self);
static int32_t timer_process(volatile struct timer_event **head, uint32_t time);
static int32_t delta(uint32_t timeout, uint32_t time);

/* functions **********************************************************/

uint32_t timer_interval(uint32_t t1, uint32_t t2)
{
    (uint32_t)delta(t1, t2);
}

void timer_start(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){            

        timers = NULL;
    
        /* enable tosc */
        ASSR |= _BV(AS2);
        
        /* 8 second overflow */
        TCCR2B |= _BV(CS22) | _BV(CS21) | _BV(CS20);            
        
        /* zero the counter */
        TCNT2 = 0U;
        
        /* ensure next overflow is in one second */
        OCR2A = 0U;        
        
        /* wait for async */
        while((ASSR & _BV(TCN2UB)) > 0);
        while((ASSR & _BV(OCR2AUB)) > 0);
        
        TIMSK2 |= _BV(TOIE2) | _BV(OCIE2A);
    }
}

void timer_set(volatile struct timer_event *self, uint32_t interval, timer_handle_fn handler)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){            

        uint32_t time = timer_get_time();

        assert(!timer_already_linked(timers, self));
        timer_link(&timers, time, self, interval, handler);        
        
        OCR2A = TCNT2 + 2U;
    }
}

void timer_clear(volatile struct timer_event *self)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){            
    
        timer_unlink(&timers, self); 
    }
}

uint32_t timer_get_time(void)
{
    uint32_t retval;
    bool overflow;
    uint8_t timer;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        
        overflow = ((TIFR2 & _BV(TOV2)) > 0);
        timer = TCNT2;

        if(overflow || ((((TIFR2 & _BV(TOV2)) > 0U)) && (timer == 0U))){
            
            retval = time + 1U;            
        }
        else{
            
            retval = time;
        }
    }

    retval <<= 8U;        
    retval |= timer;

    return retval;
}

ISR(TIMER2_OVF_vect)
{    
    time++;
}

ISR(TIMER2_COMPA_vect)
{
    int32_t diff = timer_process(&timers, timer_get_time());
    
    if(diff < 0xffU){
             
        if(diff < 2){
         
            diff = 2;
        }
        
        while((ASSR & _BV(OCR2AUB)) > 0);
        
        OCR2A = TCNT2 + diff;
    }
}
    
/* static functions ***************************************************/
        
static int32_t delta(uint32_t timeout, uint32_t time)
{
    return (int32_t)((timeout >= time) ? (timeout - time) : (UINT32_MAX - time + timeout));
}

static void timer_link(volatile struct timer_event **head, uint32_t time, volatile struct timer_event *self, uint32_t interval, timer_handle_fn handler)
{
    volatile struct timer_event *ptr = *head;
    volatile struct timer_event *prev = *head;
    
    self->timeout = time + interval;
    self->handler = handler;
    self->next = NULL;
    
    if(ptr == NULL){
    
        *head = self;        
    }
    else{
    
        while(ptr != NULL){
            
            int32_t diff = delta(ptr->timeout, time);

            if((diff > 0) && (interval < diff)){
            
                self->next = ptr;
                
                if(prev == ptr){
                
                    *head = self;
                }
                else{
                    
                    prev->next = self;
                }
                
                break;
            }
            
            prev = ptr;
            ptr = ptr->next;
        }
    }
}

static void timer_unlink(volatile struct timer_event **head, volatile struct timer_event *self)
{    
    volatile struct timer_event *ptr = *head;
    volatile struct timer_event *prev = *head;
            
    while(ptr != NULL){
        
        if(ptr == self){
            
            if(prev == ptr){
                
                *head = ptr->next;
            }
            else{
                
                prev->next = ptr->next;
            }
        }
        
        prev = ptr;
        ptr = ptr->next;
    }
}

static int32_t timer_process(volatile struct timer_event **head, uint32_t time)
{    
    int32_t diff = 0xffU;
    
    volatile struct timer_event *ptr = *head;
    
    while(ptr != NULL){
            
        if((diff = delta(ptr->timeout, time)) <= 0){
            
            *head = ptr->next;            
            ptr->handler(ptr);
            ptr = *head;
            diff = 0xffU;
        }
        else{
            
            break;
        }
    }
    
    return diff;
}

static bool timer_already_linked(volatile struct timer_event *head, volatile struct timer_event *self)
{    
    volatile struct timer_event *ptr = head;
    bool retval = false;
    
    while(ptr != NULL){
        
        if(ptr == self){
            
            retval = true;
            break;
        }
        else{
            
            ptr = ptr->next;
        }
    }
    
    return retval;
}
