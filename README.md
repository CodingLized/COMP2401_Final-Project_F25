# COMP2401 Final Project

## Developed by:
- Name: Kajé Jack
- Student ID: 101359680
- Date: November 30, 2025

## Files
- `main.c`: Provides the main logic of the progran
- `ghosts.c`: Contains the implementation of all ghost functionality
- `hunters.c`: Contains the implementation of all hunter functionality
- `house.c`: Contains implementation of functions to initialize and connect rooms
- `helpers.c`: Contains implementations of additional helper functions. 
- `defs.h` and `helpers.h`: Contain the definitions of constants, structures, function prototypes, etc used in the above files. 
- `makefile`: Allows the use of `make` to automate the building and compiling process. 
- `validate_logs.py`: Used to for testing correct logging from the program. 

## Building and Running

1. Navigate to the folder containing this README in your terminal program. 
1. Run the command `make all`. This should compile all source code and link them together into the executable `finalProject`
1. Type `./finalProject`. This will run the executable. 


## Bonus Items Implements
- Included documentation for functions using the suggested format. 


## Source Acknowledgements and Additional Notes

- Used the course notes to refresh memory on bit manipulation, and dynamic memory
- Used lecture slide 10.2 and Tutorial 8 to refresh memory on threading
- Used part of the code from the `entry_cmp` function from Assignment 2 to do room name comparison. 
- `set_bit function` adapted from my Assignment 3 submisson
- `get_bit function` adapted from my Assignment 3 submisson




- The makefile contains additional labels used for debugging which causes all files to always recompile. These labels should be ignored.