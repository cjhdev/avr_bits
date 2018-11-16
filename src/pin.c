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

#include "pin.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdint.h>
#include <stddef.h>

static struct pin_pcint *pcints;

/* static function prototypes *****************************************/

static bool translate_pin(enum pin_id id, volatile uint8_t **state, volatile uint8_t **port, volatile uint8_t **ddr, volatile uint8_t **pcmsk, uint8_t *bit);
static void unmask_pcint(enum pin_id id);
static void mask_pcint(enum pin_id id);

/* functions **********************************************************/

bool pin_get(enum pin_id id)
{
    volatile uint8_t *state;
    volatile uint8_t *port;
    volatile uint8_t *ddr;
    volatile uint8_t *pcmsk;
    uint8_t bit;
    bool retval = false;
    
    if(translate_pin(id, &state, &port, &ddr, &pcmsk, &bit)){
        
        retval = ((*state & _BV(bit)) > 0);
    }
    
    return retval;
}

void pin_set(enum pin_id id, enum pin_direction dir, bool on)
{
    volatile uint8_t *state;
    volatile uint8_t *port;
    volatile uint8_t *ddr;
    volatile uint8_t *pcmsk;    
    uint8_t bit;
    
    if(translate_pin(id, &state, &port, &ddr, &pcmsk, &bit)){
    
        if(dir == PIN_OUTPUT){
        
            *ddr |= _BV(bit);        
        }
        else{
            
            *ddr &= ~_BV(bit);
        }
        
        if(on){
        
            *port |= _BV(bit);        
        }
        else{
            
            *port &= ~_BV(bit);
        }    
    }
}

void pin_set_pcint_handler(struct pin_pcint *self, enum pin_id id, enum pin_pcint_mode mode, pin_pcint_handler_t handler)
{    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){            

       struct pin_pcint *ptr = pcints;
        
        /* enable all the pin change interrupts */
        PCICR |= _BV(PCIE0) |_BV(PCIE1) |_BV(PCIE2);
        
        self->next = NULL;
        
        if(pcints == NULL){
            
            pcints = self;        
        }
        else{
            
            while(ptr->next != NULL){
            
                ptr = ptr->next;
            }
            
            ptr->next = self;
        }
        
        self->id = id;
        self->state = pin_get(id);
        self->handler = handler;    
        unmask_pcint(id);
    }
}

void pin_clear_pcint_handler(const struct pin_pcint *self)
{    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){            
    
        struct pin_pcint *ptr;
        
        if(pcints == self){
            
            pcints = pcints->next;
        }
        else{
            
            ptr = pcints;
            
            while(ptr->next != NULL){
                
                if(ptr->next == self){
                    
                    ptr->next = ptr->next->next;
                    break;
                }            
            }
        }
        
        ptr = pcints;
        
        while(ptr != NULL){
            
            if(ptr->id == self->id){
                
                break;
            }
            
            ptr = ptr->next;
        }
        
        if(ptr == NULL){
            
            mask_pcint(self->id);
        }
    }
}

/* static functions ***************************************************/

static void unmask_pcint(enum pin_id id)
{
    volatile uint8_t *state;
    volatile uint8_t *port;
    volatile uint8_t *ddr;
    volatile uint8_t *pcmsk;    
    uint8_t bit;
    
    if(translate_pin(id, &state, &port, &ddr, &pcmsk, &bit)){
        
       *pcmsk |= _BV(bit);
    }    
}

static void mask_pcint(enum pin_id id)
{
    volatile uint8_t *state;
    volatile uint8_t *port;
    volatile uint8_t *ddr;
    volatile uint8_t *pcmsk;    
    uint8_t bit;
    
    if(translate_pin(id, &state, &port, &ddr, &pcmsk, &bit)){
        
       *pcmsk &= ~_BV(bit);
    }    
}

static bool translate_pin(enum pin_id id, volatile uint8_t **state, volatile uint8_t **port, volatile uint8_t **ddr, volatile uint8_t **pcmsk, uint8_t *bit)
{
    bool retval = false;
    
    if(id != PIN_NA){
        
        if(id <= PIN_D7){            
            
            *state = &(PIND);
            *port = &(PORTD);
            *ddr = &(DDRD);
            *bit = (uint8_t)id;
            *pcmsk = &(PCMSK2);                        
        }
        else if(id <= PIN_D13){
            
            *state = &(PINB);
            *port = &(PORTB);
            *ddr = &(DDRB);
            *bit = ((uint8_t)id) - ((uint8_t)PIN_D8);
            *pcmsk = &(PCMSK0);
        }
        else{            
            
            *state = &(PINC);
            *port = &(PORTC);
            *ddr = &(DDRC);
            *bit = (uint8_t)id - (uint8_t)PIN_A0;
            *pcmsk = &(PCMSK1);
        }
        
        retval = true;
    }
    
    return retval;
}

ISR(PCINT0_vect)
{
    struct pin_pcint *ptr = pcints;
    
    while(ptr != NULL){
    
        bool state = pin_get(ptr->id);        
        
        switch(ptr->mode){
        case PIN_FALLING:
        
            if(!state && ptr->state){
                
                ptr->handler();
            }
            break;
        
        case PIN_RISING:
        
            if(state && !ptr->state){
                
                ptr->handler();
            }
            break;
        
        case PIN_CHANGE:
        
            if(state != ptr->state){
                
                ptr->handler();
            }
            break;
        
        default:
            break;
        }        
    
        ptr->state = state;
        
        ptr = ptr->next;
    }    
}

ISR(PCINT1_vect, ISR_ALIASOF(PCINT0_vect));
ISR(PCINT2_vect, ISR_ALIASOF(PCINT0_vect));
