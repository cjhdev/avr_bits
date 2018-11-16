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

#ifndef RCCAL_H
#define RCCAL_H

/**
 * @defgroup rccal
 * 
 * Hardware dependent RC oscillator calibration using TC0 and asynchronous TC2
 * 
 * This code will calibrate the RC oscillator to 
 * within +-1% of the prescaled nominal frequency.
 * 
 * It is currently very fixed to a particular hardware configuration
 * and takes up an ISR.
 * 
 * Works as follows:
 * 
 * - counts number of TC0 ticks in one TC2 tick
 * - takes up to 8 measurements in OSCCAL binary search
 * - checks 4 nearest neighbours 
 * - if calibration fails the factory calibration byte is restored
 * 
 * Requires:
 * 
 * - TC2 in asynchronous mode clocked from a 32768KHZ source
 * - TC2 1024 divider (overflow every 8 seconds)
 * - TC0 1 divider
 * - system prescaler 2 (i.e. 4MHz io clock)
 * 
 * @{
 * */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

/** calibration result */
enum rccal_result {
    RCCAL_RESULT_NA,    /**< calibration not complete */
    RCCAL_RESULT_PASS,  /**< within +/- 1% */
    RCCAL_RESULT_FAIL   /**< not within +/- 1% */
};

/**
 * start calibration procedure
 * 
 * This procedure will try to calibrate the RC oscillator to 
 * within +-1% of the prescaled nominal frequency. Should
 * the procedure fail, the best calibration will remain set.
 * 
 * The calibration alogrithm works by measuring the number of RC ticks within
 * a given reference window. This measurement is then compared against the acceptible
 * range for +-1% accuracy. 
 * 
 * Up to 8 measurements are made to zero into an appropriate value by
 * way of binary search. Should this fail, the fourth nearest neighbours 
 * to that result are tested.
 * 
 * An implementation quirk means that the nearest neighbour search 
 * will re-test the binary search result. This will increase the worst 
 * case search time by one measurement. This quirk exists to save a 
 * small amount of RAM and complexity.
 * 
 * Another implementation quirk means that to synchronise the measurements
 * correctly, we have to add in additional ticks which cause the calibration
 * to take longer.
 * 
 * Bearing that in mind, each measurement takes:
 * 
 * Tmeasure = (RCCAL_T2_TICKS + 2) * RCCAL_T2_DIV / 32786 seconds
 * 
 * Calibration therefore takes:
 * 
 * Tcalibration = Tmeasure * (8 + 5) seconds
 * 
 * For the default values, calibration will complete in:
 * 
 * (2*512/32768) * (8 + 5) = 0.40625 seconds
 * 
 * */
 
 
/**
 * Start the calibration process
 * 
 * */
void rccal_start(void);

/**
 * is calibration in progress?
 *
 * @retval true     in progress
 * @retval false
 * 
 * */
bool rccal_is_active(void);

/**
 * get the last calibration result
 * 
 * @return calibration result
 * 
 * */
enum rccal_result rccal_get_result(void);

 
#ifdef __cplusplus
}
#endif
 
/** @} */
#endif
