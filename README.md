# Math Language
---
Language of mathematical analysis. You can write your own program on math language and translate it to [CPU emulator](https://github.com/Varnike/calculator/tree/update) assembler. After compiling you can run your programm on cpu emulator.
## Syntax
---
###  Scope
All variables, initialized within function body are global. Any variables initialized in the body of the function are local. Functions don't have access to variables outside the function body and there is no access to local variables from outside. 
### Basic syntax rules
- ####  Any program must end with a '$' character
- #### Any line of code must end with ';' 
---
### Variables initialization
The variable is initialized at the time of its first declaration:
```sh
size = 1; # new variable 'size' is initialized and has a value of 1.
```
Accessing a variable that has not been initialized will result in an error!
### Basic constructions
Here are some examples:
#### #1 if else construcion
```sh
# if else construction
suppose (<condition>) perfomed #any expression can be used in place of <condition>
therefore {
        # do smth if condition is true
        ...
} however {
        # else
        ...
}
```
#### #2 while
```sh
Consider {
        # do smth
        ...
} assuming expression (<condition>) perfomed;
```
#### #3 Functions
```sh
# function body
Theorem (func_name)
Given (arg1 arg2) # arg1 and arg2 - function arguments names
                  # arg1 and arg2 should be separated by any space symbol
Proof
    ...
    # do smth
    proven <expr>; # return value
QED

# function call
func_name(arg1 arg2); # any expression can be used in place of arg1 and arg2
                      # arg1 and arg2 should be separated by any space symbol
```
#### #4 input
```sh
Consider(x); # set x to input value
```
#### #5 Output
```sh
Introduce(<expr>); # print value of <expr>
```

## Program example
```sh
# calculating the sine value expanded along the Taylor series

Theorem (Factorial)
Given (x)
Proof
        suppose (x <= 1) perfomed 
        therefore proven 1;
        proven x * Factorial(x - 1); 
QED

Theorem (pow)
Given (x y)
Proof
        suppose (y == 0) perfomed
        therefore proven 1;

        mulx = x;
        Consider {
                x = x * mulx; 
                y = y - 1;
        } assuming expression (y >= 2) perfomed;
        proven x;
QED

Theorem (sin)
Given (x)
Proof
        result   = 0;
        currTerm = 0;
        minus    = 0;

        Consider {
                currVal = pow(x currTerm * 2 + 1);
                Conclusion(currVal);
                currVal = currVal / Factorial(2 * currTerm + 1);
                suppose (minus == 1) perfomed
                therefore {
                        result = result - currVal;
                        minus = 0;
                } however {
                        result = result + currVal;      
                        minus = 1;
                }
                currTerm = currTerm + 1;
        } assuming expression (currTerm <= 10) perfomed;
        proven result;
QED

y = 0;
Introduce(y);
y = sin(y);   
              
Conclusion(y);
$             
```

# X86-64 compilation.
Now you can compile programs in our language under the X86-64 architecture under the Linux operating system. In tests, a program compiled under X86-64 runs 200 times faster than a program running on our processor.

### Usage
After compilation, you need to make the compiled program executable:
```sh
$ chmod +x out
```

Where "out" is the name of the compiled program. After that you can run program by running following command in console:
```sh
$ ./out
```
