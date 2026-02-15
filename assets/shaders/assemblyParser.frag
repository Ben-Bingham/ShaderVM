#version 330 core

out vec4 outFragColor;

uniform vec3 color;

// Registers:
const int reg_s0 = 0;
const int reg_s1 = 1; 
const int reg_s2 = 2; 
const int reg_s3 = 3; 
const int reg_s4 = 4; 
const int reg_s5 = 5; 
const int reg_v0 = 6; 
const int reg_v1 = 7; 
const int reg_v2 = 8; 
const int reg_v3 = 9; 
const int reg_v4 = 10; 
const int reg_v5 = 11; 
const int reg_pc = 12; 
const int reg_t = 13;
const int reg_s = 14;
const int reg_c = 15;

// Op codes:
const int inst_add = 0;
const int inst_multiply = 1;
const int inst_negate = 2;
const int inst_reciprocal = 3;
const int inst_move = 4;
const int inst_getComponent0 = 5;
const int inst_getComponent1 = 6;
const int inst_getComponent2 = 7;
const int inst_getComponent3 = 8;
const int inst_setComponent0 = 9;
const int inst_setComponent1 = 10;
const int inst_setComponent2 = 11;
const int inst_setComponent3 = 12;
const int inst_load = 13;

struct Instruction {
	int OpCode;
	int r1;
	int r2;
	int r3;
	float constant;
};

const int instructionCount = 10;
Instruction instructions[instructionCount];

void main() {
	/*
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
	*/


	outFragColor = vec4(color.xyz, 1.0);
}
