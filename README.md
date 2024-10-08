# DELTA BASIC

The implementation of the Minimal BASIC ([ECMA-55](https://ecma-international.org/wp-content/uploads/ECMA-55_1st_edition_january_1978.pdf)) for my "old school" terminal web page

* [ΔBASIC](#δbasic)
* [Build](#build)
* [Example code](#example-code)
* [API Example](#api-example)
* [TODO or not TODO](#todo-or-not-todo)

## ΔBASIC

```text
Keywolds:
    DIM, END, FOR, GOSUB, GOTO, IF, INPUT, LET, NEXT, PRINT, RETURN, RUN, STEP, STOP, THEN, TO

Math operations:
    +    add
    -    sub
    *    mul
    /    div
    ^    pow
    %    mod

Comparisons:
    =    Equal
    <>   Not Equal
    <    Less Than
    >    Greater Than
    <=   Less or Equal
    >=   Greater or Equal
```

## Build

Requirements:

* [deltamake](https://github.com/Reklov42/deltamake) >= 2.0.0
* Compiler supporting C99

## Example code

Example with all deltaBASIC features

> You can write in one line using `:` as a separator

```basic
100 REM ------- Variables ------
110 LET Y = 10                  : REM `float` numeric 
120 X = 10                      : REM LET can be omitted
130 STR$ = "Hello"              : REM `char` dynamic string
140 DIM ARR(10)                 : REM Numeric array. Allocated for indexes from 0 to 10 (inclusive)
150 DIM S$(100)                 : REM String array
160 ARR(0) = 5                  : REM Indexing as name(index)
200 REM ---------- IO ----------
210 PRINT STR$ ; " " ; "World!" : REM Prints "Hello World!". ; works as concatenation
220 PRINT "Hello", X, "Worlds!" : REM Prints "Hello 10 Worlds!"
230 INPUT N, STR$               : REM Asks "N? " and then "STR$? "
240 PRINT "N is ", N
250 PRINT "STR$ is ", STR$
300 REM ------- For loop -------
310 FOR I = 0 TO 10             : REM From 0 to 10 (inclusive)
320 PRINT I
330 NEXT                        : REM NEXT does not need to set a variable. It works as the end of a block
340 FOR J = 2 TO 200 STEP 20    : REM You can set the increment value
350 PRINT J : NEXT
400 REM --------- Jumps --------
410 GOTO 420                    : REM Jump to line 420
420 PRINT "Some ignored code..."
430 GOSUB 460                   : REM Save next line number and GOTO 460
440 PRINT "Some fancy code..."
450 GOTO 500
460 PRINT "Cool subcode!"
470 RETURN                      : REM Jump back to line 440
500 REM ---------- If ----------
510 IF N <> 10 THEN GOTO 530    : REM IF statement has only an inline body
520 STR$ = "equal" : GOTO 540
530 STR$ = "not equal"
540 PRINT "N is ";STR$;" to 10"
600 REM ------- END/STOP -------
610 STOP                        : REM stop execution and return DELTA_MACHINE_STOP. Can continue execution
620 END                         : REM end excution, reset state and return DELTA_END
```

## API Example

```cpp
#include <stdio.h>
#include <math.h>

#include "deltabasic.h"

// API function
delta_EStatus Foo(delta_SState* D) { // FOO$(a, b)
    delta_TNumber a;
    delta_GetArgNumeric(D, 1, &a); // Args are in reverse order. Index of `a` is 1

    delta_TNumber b;
    delta_GetArgNumeric(D, 0, &b); // Index of `b` is 0
    
    if (fabs(a - b) < 0.001f)
        delta_ReturnString(D, "OK");
    else
        delta_ReturnString(D, "NO");

    return DELTA_OK;
}

int main() {
    // State with default allocator (uses `malloc`)
    delta_SState* D = delta_CreateState(NULL, NULL);
    // Compile code
    delta_Execute(D, "PRINT \"HELLO WORLD\""); // HELLO WORLD 
    // Interpret all code
    delta_Interpret(D, 0);

    // Numeric variables
    delta_SetNumeric(D, "X", 52);
    delta_Execute(D, "X = X - 10");
    delta_Interpret(D, 0);
    delta_TNumber x;
    delta_GetNumeric(D, "X", &x);
    printf("X: %f\n", x); // X: 42.000000

    // String variables
    delta_Execute(D, "S$ = \"Hello World!\"");
    delta_Interpret(D, 0);
    const char* ptr;
    delta_GetString(D, "S", &ptr);
    printf("%s\n", ptr); // Hello World!

    // Functions
    delta_ECFuncArgType args[2] = {
        DELTA_CFUNC_ARG_NUMERIC, DELTA_CFUNC_ARG_NUMERIC
    };
    delta_RegisterCFunction(D, "FOO", args, 2, DELTA_CFUNC_ARG_STRING, Foo);
    delta_Execute(D, "PRINT FOO$(10, 10), FOO$(3, 2)"); // OK NO
    delta_Interpret(D, 0);

    delta_ReleaseState(D);

    return 0;
}
```

## TODO or not TODO

* Arrays as API arguments
* Arrays as `INPUT` arguments
* Multiline block for `IF`
* Named jump labels?
