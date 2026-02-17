## Shader Assembly Language

A simple assembly language that is executed inside of a shader, programs created with this assembly language are executed as fragment shaders running once per pixel across an entire screen, allows for modification of shaders without recompiling them.

## Specifications:
### Basics
Every program written in Shader Assembly will be executed for each and every pixel of the image, it will be made up of instructions that operate on the provided inputs and update the needed outputs.

### Types
* Scaler: A 32 bit IEEE.754 floating point number
* Vector: Always has four components
* Integer: An integer

### Registers
* s0 -> s5: Scaler registers
* v0 -> v4: Vector registers

| Number | Name | Type | Read/Write | Use |
|:---:|:---:|:---:|:---:|:---:|
| 00 | z | Scaler | r | Returns zero |
| 01 | s0 | Scaler | rw | - |
| 02 | s1 | Scaler | rw | - |
| 03 | s2 | Scaler | rw | - |
| 04 | s3 | Scaler | rw | - |
| 05 | s4 | Scaler | rw | - |
| 06 | s5 | Vector | rw | - |
| 07 | v0 | Vector | rw | - |
| 08 | v1 | Vector | rw | - |
| 09 | v2 | Vector | rw | - |
| 10 | v3 | Vector | rw | - |
| 11 | v4 | Vector | rw | - |
| 12 | pc | Vector | r | Pixel coordinate |
| 13 | t | Scaler | r | Time |
| 14 | s | Vector | r | Size of screen in pixels |
| 15 | c | Vector | w | Output color |

### Memory
In addition to registers, each program has access to 64 scalers of memory accessible with the readMemory and writeMemory instructions

### Instructions
There must be at most one instruction per line
#### Types:
### Three Register (3R)
Take in three registers, typically applies an operation using the first two and returns the output in the third register

| | OpCode | R1 | R2 | R3 | - |
|---|---|---|---|---|---|
| Bits | 4 | 4 | 4 | 4 | 16 |

### Two Register (2R)
Take in two registers, typically applies an operation to the first and stores the result in the second

| | OpCode | R1 | R2 | - |
|---|---|---|---|---|
| Bits | 4 | 4 | 4 | 20 |

### Register Scaler (RS)
Takes in one register and one hardcoded scaler

| | OpCode | R1 | Scaler
|---|---|---|---|
| Bits | 4 | 4 | 24 | 

| Opcode | Type | Instruction | Action |
|:---:|:---:|---|---|
| 00 | 3R | add(sA, sB, sC) | Adds sA and sB placing the result in sC |
| 01 | 3R | multiply(sA, sB, sC) | Multiples sA and sB placing the result in sC |
| 02 | 2R | negate(sA, sB) | Negates the value in sA and stores the result in sB |
| 03 | 2R | reciprocal(sA, sB) | Takes the reciprocal of the value in sA and stores the result in sB |
| 04 | 2R | move(sA, sB) | Copies the value stored in sA into sB |
| 05 | 3R | getComponent(vA, sA, sB) | Gets the floor(sB)'th component of vA and places it inside of sA |
| 06 | 3R | setComponent(vA, sA, sB) | Sets the floor(sB)'th of vA to the value of sA |
| 07 | RS | load(sA, Integer) | Converts Integer to a floating point number (ie: 42 -> 42.0f or 24 -> 24.0f), and assigns sA to that value. |
| 08 | RS | readMemory(sA, scaler) | Reads the floor(scaler)'th value in memory into sA |
| 09 | RS | writeMemory(sA, scaler) | Writes the value in sA into the floor(scaler)'th value in memory |

### Inputs
Values stored in special read only registers that can be used throughout shader execution
* pc (Vector): The 0th and 1st components store the pixel coordinate of the running program
* t (Scaler): The time since the start of the program in seconds
* s (Vector): The 0th and 1st components store the maximum size in pixels of the current screen

### Outputs
Special registers that are read from by the backing program in order to show the shader output
* c (Vector): The desired final pixel color, is 0.0 in all components by default

### Comments
Whenever a hashtag (#) is found, all characters including it to the end of the line will be ignored

## Example Programs
### Pixel UV Color
Assign each pixels red component to be based on its x coordinate where pixels on the far left are black and on the far right are red, and assign the green component based on the pixels y coordinate so that pixels on the bottom are white and pixels at the top are green.

```
getComponent(pc, s0, z)   # s0 = pc.x
load(s5, 1)               # s5 = 1
getComponent(pc, s1, s5) # s1 = pc.y

getComponent(s, s3, z)    # s3 = s.x
reciprocal(s3, s2)        # s2 = 1 / s3
getComponent(s, s4, s5)   # s4 = s.y
reciprocal(s4, s3)        # s3 = 1 / s4

multiply(s0, s2, s0)      # s0 = s0 * s2
multiply(s1, s3, s1)      # s1 = s1 * s3

setComponent(c, s0, z)    # c.x = s0
setComponent(c, s1, s5)   # c.y = s1
```