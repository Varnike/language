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
````sh
size = 1; # new variable 'size' is initialized and has a value of 1.
````
Accessing a variable that has not been initialized will result in an error!
### Basic constructions
Here are some examples:
#### #1 if else construcion
````sh
# if else construction
suppose (<condition>) perfomed #any expression can be used in place of <condition>
therefore {
        # do smth if condition is true
        ...
} however {
        # else
        ...
}
````
#### #2 while
````sh
Consider {
        # do smth
        ...
} assuming expression (<condition>) perfomed;
````
#### #3 Functions
````sh
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
````
#### #4 input
````sh
Consider(x); # set x to input value
````
#### #5 Output
````sh
Introduce(<expr>); # print value of <expr>
````

## Program example
````sh
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
````

# X86-64 compilation.
Now you can compile programs in our language under the X86-64 architecture and Linux operating system. In tests, a program compiled under X86-64 runs 200 times faster than a program running on our processor.

## Usage
You can run program by running following command in console:
````sh
$ ./out
````

## Compilation notes
The following are a few features of how the program is compiled. The assembly code examples below were obtained by decompiling the compiled program. For convenience, instead of absolute addresses, labels are made.

### Arithmetics
So far, the compiled program can only work with integer values. To simulate floating point numbers, the last n digits can be treated as n digits after the decimal point. To enable this feature, specify constant PRESISION in backend64.h to 10^n and define INT_AS_FLOAT.

Each expression with binary operator calculates its value and returns it to `rax`. If the expression is complex, then when passing through the tree, the right branch will be calculated first, then the `rax` will be saved, after that the right branch and the expression itself will be calculated. 

````asm
;------------------------------------------------
; y = 0;
; x = y + 5;
;------------------------------------------------

	; y - [var_8h]
	; x - [var_10h]
	movabs rax, 0
	mov qword [var_8h], rax
	mov rax, qword [var_8h]
	push rax
	movabs rax, 5
	mov rbx, rax
	pop rax
	add rax, rbx
	mov qword [var_10h], rax
````
### Relation operators
As for arithmetic operators, conditional operators evaluate their condition and, depending on the result, assign `rax` 0 or 1.
````asm
;------------------------------------------------
; x = (2 >= 3);
;------------------------------------------------

	; x - [var_8h]
	movabs rax, 2
	push rax
	movabs rax, 3
	mov rbx, rax
	pop rax
	cmp rax, rbx
	movabs rax, 0
	setge al
	mov qword [var_8h], rax
````

### While example
````asm

;------------------------------------------------
; Consider {
;	<stmnts>
; } assuming expression (<expression>) perfomed;
;------------------------------------------------

	jmp L1
L2:	; ...
	; stmnts block
	; ...

L1:	; ...
	; calculate expression
	; ...

	test rax, rax
	jne L2
````
### Standart functions
There are two standart functions: print and read. They are automatically used when the Introduce() and Conclusion() functions are called. After the program code is compiled, exit(0) and these two functions are added to the end of the executable file and linked to the rest of the program.

### Function calls
All arguments are passed through stack and have to be poped after function call. Function returns its value in `rax` register.
