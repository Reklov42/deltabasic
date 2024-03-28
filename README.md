# DELTA BASIC

The implementation of the Minimal BASIC for my  "old school" terminal web page

## ΔBASIC Keywolds

[`DIM`](#variables) [`END`](#execution-control) [`FOR`](#for-next-loop) [`GOSUB`](#subroutine) [`GOTO`](#execution-control) [`IF`](#branch) [`LET`](#variables) [`NEXT`](#for-next-loop) [`PRINT`](#io) [`RETURN`](#subroutine) [`STEP`](#for-next-loop) [`STOP`](#execution-control) [`THEN`](#branch) [`TO`](#for-next-loop)

## `LET/DIM` Variables {#variables}

```basic
LET name = numeric-exp
LET name$ = string-exp

DIM name(size)
DIM name$(size)
```

## `END/STOP/GOTO` Execution control {#execution-control}

```basic
END
STOP
GOTO line
```

## `PRINT` I/O {#io}

## `IF` Branch {#branch}

```basic
IF numeric-exp THEN inline-body
```

## `FOR` Loop {#for-next-loop}

```basic
FOR variable = numeric-exp TO numeric-exp [STEP numeric-exp]
REM Loop body
NEXT
```

## `GOSUB` Subroutine {#subroutine}

```basic
GOSUB line
...
REM Subroutine body
RETURN
```
