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

#include "fifo.h"
#include <util/atomic.h>

void fifo_init(volatile struct fifo *self, volatile uint8_t *buffer, size_t max)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){            
 
        self->size = 0U;
        self->count = 0U;
        self->max = max;
        self->buffer = buffer;
    }
}

bool fifo_push(volatile struct fifo *self, uint8_t value)
{
    bool retval = false;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){            
        
        if(!fifo_full(self)){
            
            self->buffer[self->count % fifo_max(self)] = value;
            self->size++;
            self->count++;
            retval = true;
        }
    }
    
    return retval;
}

bool fifo_pop(volatile struct fifo *self, uint8_t *value)
{
    bool retval = false;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){            
            
        if(!fifo_empty(self)){
            
            *value = self->buffer[(self->count - self->size) % fifo_max(self)];
            self->size--;
            retval = true;
        }
    }
    
    return retval;
}

size_t fifo_size(volatile const struct fifo *self)
{
    size_t retval;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){            
     
        retval = self->size;
    }
    
    return retval;
}

size_t fifo_max(volatile const struct fifo *self)
{
    size_t retval;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){            
     
        retval = self->max;
    }
    
    return retval;
}

bool fifo_empty(volatile const struct fifo *self)
{
    bool retval;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){            
     
        retval = (self->size == 0U);
    }
    
    return retval;
}

bool fifo_full(volatile const struct fifo *self)
{
    bool retval;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){            
     
        retval = (self->size == fifo_max(self));
    }
    
    return retval;
}
