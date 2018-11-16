AVRBits
=======

A collection of useful things in C to complement existing avr-libc features.

The style will appeal to people that prefer:

- standard types
- enum instead of macros where appropriate
- code that works correctly between mainloop and interrupt
- code that works correctly with system clock prescaler

## Supported Targets

- ATMEGA328p

## Modules

Some modules depend on others (e.g. uart uses fifo).

### pin

- read and write pins using Arduino pin naming conventions
- set/clear pin change interrupt handlers

### fifo

- byte oriented FIFO
- variable buffer size set at initialisation time
- works between interrupt and mainloop

### semaphore

- non-blocking semaphores (i.e. flags)
- works between interrupt and mainloop

### spi

- master mode only
- bit rate and mode settings
- configures pins as required

compile options:

- F_CPU (system clock in Hz)

### uart

- baud rate setting
- buffered tx and rx
- put/get from interrupt and mainloop
- tx_empty/rx_ready handlers
- depends on fifo

compile options:

- F_CPU (system clock in Hz)
- UART_TX_SIZE (tx buffer size)
- UART_RX_SIZE (rx buffer size)

### rccal

- hardware dependent RC oscillator calibration
- uses TC2 (32768Hz async mode) to calibrate RC using TC0 (io clock)

## License

AVRBits has an MIT license.
