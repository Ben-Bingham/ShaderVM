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

vec4 registers[16];

void ExecuteInstruction(Instruction instruction, inout vec4 registers[16]);

void main() {
	// Build the instructions list:
	instructions[0] = Instruction(inst_getComponent0, reg_pc, reg_s0, 0, 0.0); // getComponent0(pc, s0) # s0 = pc.x
	instructions[1] = Instruction(inst_getComponent1, reg_pc, reg_s1, 0, 0.0); // getComponent1(pc, s1) # s1 = pc.y

	instructions[2] = Instruction(inst_getComponent0, reg_s, reg_s3, 0, 0.0);  // getComponent0(s, s3)  # s3 = s.x
	instructions[3] = Instruction(inst_reciprocal, reg_s3, reg_s2, 0, 0.0);    // reciprocal(s3, s2)    # s2 = 1 / s3
	instructions[4] = Instruction(inst_getComponent1, reg_s, reg_s4, 0, 0.0);  // getComponent1(s, s4)  # s4 = s.y
	instructions[5] = Instruction(inst_reciprocal, reg_s4, reg_s3, 0, 0.0);    // reciprocal(s4, s3)    # s3 = 1 / s4

	instructions[6] = Instruction(inst_multiply, reg_s0, reg_s2, reg_s0, 0.0); // multiply(s0, s2, s0)  # s0 = s0 * s2
	instructions[7] = Instruction(inst_multiply, reg_s1, reg_s3, reg_s1, 0.0); // multiply(s1, s3, s1)  # s1 = s1 * s3

	instructions[8] = Instruction(inst_setComponent0, reg_c, reg_s0, 0, 0.0);  // setComponent0(c, s0)  # c.x = s0
	instructions[9] = Instruction(inst_setComponent1, reg_c, reg_s1, 0, 0.0);  // setComponent1(c, s1)  # c.y = s1

	registers[reg_c] = vec4(0.0); // Zero out the color register

	// Execute the instruciton list
	for (int i = 0; i < instructionCount; ++i) {
		ExecuteInstruction(instructions[i], registers);
	}

	outFragColor = registers[reg_c]; // Assign the final color to the color register
}

void ExecuteInstruction(Instruction instruction, inout vec4 registers[16]) {
	switch (instruction.OpCode) {
		case inst_getComponent0:
		break;
		case inst_getComponent1:
		break;
		case inst_reciprocal:
		break;
		case inst_multiply:
		break;
		case inst_setComponent0:
		break;
		case inst_setComponent1:
		break;
	}
}
