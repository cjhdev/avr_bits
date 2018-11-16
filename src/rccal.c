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

#include "rccal.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <avr/boot.h>

enum rccal_state {
    RCCAL_STATE_OFF,     
    RCCAL_STATE_START,   
    RCCAL_STATE_SETUP,   
    RCCAL_STATE_HANDLE        
};

static volatile enum rccal_state state = RCCAL_STATE_OFF;
static volatile enum rccal_result result;
static volatile uint32_t ovf_counter;
static volatile uint32_t measure;
static volatile uint8_t step_size;
static volatile uint8_t ncount;

/* prototypes *********************************************************/

static uint32_t min_ticks(void);
static uint32_t max_ticks(void);

/* functions **********************************************************/

uint32_t rccal_measurement(void)
{
    uint32_t retval;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
        
        retval = measure;
    }
    
    return retval;
}

void rccal_start(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    
        result = RCCAL_RESULT_NA;
        state = RCCAL_STATE_START;
     
        while((ASSR & _BV(OCR2BUB)) > 0);
        OCR2B = TCNT2 + 3U;                
        
        TIFR2 = _BV(OCF2B);
        TIMSK2 |= _BV(OCIE2B);   
    }
}

bool rccal_is_active(void)
{
    return (result == RCCAL_RESULT_NA);
}

enum rccal_result rccal_get_result(void)
{
    return result;
}

/* static functions  **************************************************/

static uint32_t min_ticks(void)
{
    return 125000UL - 1250UL;
}

static uint32_t max_ticks(void)
{
    return 125000UL + 1250UL;
}

/* isr ****************************************************************/

ISR(TIMER0_OVF_vect)
{        
    ovf_counter++;
}

ISR(TIMER2_COMPB_vect)
{        
    if(state == RCCAL_STATE_START){
        
        step_size = (UINT8_MAX >> 1U);        
        OSCCAL = step_size;
        state = RCCAL_STATE_SETUP;        
    }
    
    switch(state){  
    case RCCAL_STATE_SETUP:
        
        TCCR0B = _BV(CS00);        
        TCNT0 = 0U;
        TIMSK0 |= _BV(TOIE0);
        
        ovf_counter = 0U;     
        ncount = 0U;       
        
        state = RCCAL_STATE_HANDLE;
        
        while((ASSR & _BV(OCR2BUB)) > 0);
        OCR2B++;        
        break;
        
    case RCCAL_STATE_HANDLE:
    
        /* halt TC0 and don't interrupt */
        TCCR0B &= ~_BV(CS00);
        TIMSK0 &= ~_BV(TOIE0);                
        
        /* capture measurement and compensate for unhandled overflow */
        measure = ovf_counter;
        measure <<= 8U;
        measure += TCNT0;        
        measure += ((TIFR0 & _BV(TOV0)) > 0) ? 0x100U : 0U;
    
        /* halve step size */
        step_size >>= 1U;
        
        /* calibration success */
        if((measure >= min_ticks()) && (measure <= max_ticks())){
            
            result = RCCAL_RESULT_PASS;
            state = RCCAL_STATE_OFF;
            TIMSK2 &= ~_BV(OCIE2B);             
        }   
        /* in progress */     
        else if((step_size > 0U) || (ncount < 5U)){
            
            if(step_size > 0U){
                
                if(measure < min_ticks()){                    
                    
                    OSCCAL += step_size;
                }
                else{                
                    
                    OSCCAL -= step_size;
                }                
            }
            /* check 4 nearest neighbours (lazily re-check what we already have) */
            else{
            
                if(ncount == 0U){
                    
                    if(OSCCAL < 2U){
                    
                        OSCCAL = 0U;
                    }    
                    else if(OSCCAL > 253U){
                               
                        OSCCAL = 250U;
                    }
                    else{
                        
                        OSCCAL -= 2U;
                    } 
                }
                else{
                                    
                    OSCCAL++;                
                }   
                
                ncount++;         
            }
            
            /* measure again */
            while((ASSR & _BV(OCR2BUB)) > 0);
            OCR2B += 2U;            
            state = RCCAL_STATE_SETUP;                        
        }
        /* calibration failure */
        else{
            
            /* restore the factory calibration */
            OSCCAL = boot_signature_byte_get(1U);
            
            result = RCCAL_RESULT_FAIL;
            state = RCCAL_STATE_OFF;
            TIMSK2 &= ~_BV(OCIE2B);
        }
        break;        
        
    default:
    case RCCAL_STATE_OFF:
        break;
    }    
}

