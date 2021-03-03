# About
A cooperative scheduler pattern for Time-Triggered Embedded Systems. It's implemented on different embedded targets. There's also a version based on POSIX APIs. 

# Acknowledgment
The pattern is taken from the book <b>Patterns for Time-Triggered Embedded Systems</b> <i>by Michael J. Pont</i>

# Implemented for:
  - `ATmega32A`

# To use the POSIX compatible example:
- You should have a linux distibution that's compatible with POSIX.1 and POSIX.4. The program is tested on raspbian OS with kernel version `5.4.51`.
- Head to the repo and run `make` .
- run `./main.out` and the scheduler works.
