## Shader Assembly Language

A simple assembly language that is executed inside of a shader, programs created with this assembly language are executed as fragment shaders running once per pixel across an entire screen, allows for modification of shaders without recompiling them.

## Specifications:
### Basics
Every program written in Shader Assembly will be executed for each and every pixel of the image, it will be made up of instructions that operate on the provided inputs and update the needed outputs.

### Types
* Scaler: A floating point number
* Vector: Always has four components

### Registers
* s0 -> s7: Scaler registers
* v0 -> v7: Vector registers

### Instructions
#### Three Register
Take in three registers, typically applies an operation using the first two and returns the output in the third register
* add(sA, sB, sC): Adds sA and sB placing the result in sC
* multiply(sA, sB, sC): Multiples sA and sB placing the result in sC

#### Two Register
Take in two registers, typically applies an operation to the first and stores the result in the second
* negate(sA, sB): Negates the value in sA and stores the result in sB
* reciprocal(sA, sB): Takes the reciprocal of the value in sA and stores the result in sB
* move(sA, sB): Copies the value stored in sA into sB
* getComponent0(vA, sA): Gets the 0th component of vA and places it inside of sA
* getComponent1(vA, sA): Gets the 1st component of vA and places it inside of sA
* getComponent2(vA, sA): Gets the 2nd component of vA and places it inside of sA
* getComponent3(vA, sA): Gets the 3rd component of vA and places it inside of sA
* setComponent0(vA, sA): Sets the 0th component of vA to the value of sA
* setComponent1(vA, sA): Sets the 1st component of vA to the value of sA
* setComponent2(vA, sA): Sets the 2nd component of vA to the value of sA
* setComponent3(vA, sA): Sets the 3rd component of vA to the value of sA

#### Register Scaler
Takes in one register and one hardcoded scaler
* load(sA, scaler): Assigns the value inside sA to become the value "scaler"

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

### Example Programs
#### Pixel UV Color
Assign each pixels red component to be based on its x coordinate where pixels on the far left are black and on the far right are red, and assign the green component based on the pixels y coordinate so that pixels on the bottom are white and pixels at the top are green.

getComponent0(pc, s0) # s0 = pc.x
getComponent1(pc, s1) # s1 = pc.y

getComponent0(s, s3)  # s3 = s.x
reciprocal(s3, s2)    # s2 = 1 / s3
getComponent1(s, s4)  # s4 = s.y
reciprocal(s4, s3)    # s3 = 1 / s4

multiply(s0, s2, s0)  # s0 = s0 * s2
multiply(s1, s3, s1)  # s1 = s1 * s3

setComponent0(c, s0)  # c.x = s0
setComponent1(c, s1)  # c.y = s1
