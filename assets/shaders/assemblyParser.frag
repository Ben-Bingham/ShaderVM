#version 430 core

out vec4 outFragColor;

in vec2 uv;

uniform vec3 screenSize;

uniform float time;

// Registers:
const uint reg_z = 0;
const uint reg_s0 = 1; 
const uint reg_s1 = 2; 
const uint reg_s2 = 3; 
const uint reg_s3 = 4; 
const uint reg_s4 = 5; 
const uint reg_s5 = 6; 
const uint reg_v0 = 7; 
const uint reg_v1 = 8; 
const uint reg_v2 = 9; 
const uint reg_v3 = 10; 
const uint reg_v4 = 11; 
const uint reg_pc = 12; 
const uint reg_t = 13;
const uint reg_s = 14;
const uint reg_c = 15;

// Op codes:
const uint inst_add = 0;
const uint inst_multiply = 1;
const uint inst_negate = 2;
const uint inst_reciprocal = 3;
const uint inst_move = 4;
const uint inst_getComponent = 5;
const uint inst_setComponent = 6;
const uint inst_load = 7;
const uint inst_readMemory = 8;
const uint inst_writeMemory = 9;
const uint inst_sign = 10;

uint GetOpCode(uint instruction) {
	return instruction >> 28;
}

uint GetFirstRegister(uint instruction) {
	uint r1 = instruction << 4;

	return r1 >> 28;
}

uint GetSecondRegister(uint instruction) {
	uint r2 = instruction << 8;

	return r2 >> 28;
}

uint GetThirdRegister(uint instruction) {
	uint r3 = instruction << 12;

	return r3 >> 28;
}

float GetConstant(uint instruction) {
	uint constant = instruction << 8;

	constant = constant >> 8;

	return float(constant);
}

uniform int instructionCount;
layout(std430, binding = 0) readonly buffer InstructionsSSBO {
    uint instructions[];
};

vec4 registers[16];

void ExecuteInstruction(uint instruction, inout vec4 registers[16]);

float memory[64];

void main() {
	// Prepare registers
	registers[reg_z] = vec4(0.0); // Zero out the zero register
	registers[reg_c] = vec4(0.0); // Zero out the color register

	registers[reg_pc] = vec4(uv.x * screenSize.x, uv.y * screenSize.y, 0.0, 0.0);
	registers[reg_s] = vec4(screenSize.x, screenSize.y, 0.0, 0.0);
	registers[reg_t] = vec4(time, 0.0, 0.0, 0.0);

	// Execute the instruction list
	for (int i = 0; i < instructionCount; ++i) {
		ExecuteInstruction(instructions[i], registers);
	}

	outFragColor = registers[reg_c]; // Assign the final color to the color register
}

void ExecuteInstruction(uint instruction, inout vec4 registers[16]) {
	uint opcode = GetOpCode(instruction);

	uint r1 = GetFirstRegister(instruction);
	uint r2 = GetSecondRegister(instruction);
	uint r3 = GetThirdRegister(instruction);
	float constant = GetConstant(instruction);

	switch (opcode) {
		case inst_add:
			registers[r3].x = registers[r1].x + registers[r2].x;
			break;

		case inst_multiply:
			registers[r3].x = registers[r1].x * registers[r2].x;
			break;

		case inst_negate:
			registers[r2].x = -registers[r1].x;
			break;

		case inst_reciprocal:
			registers[r2].x = 1.0 / registers[r1].x;
			break;

		case inst_move:
			registers[r2].x = registers[r1].x;
			break;

		case inst_getComponent:
			float v1 = registers[r3].x;
			uint offset1 = uint(floor(v1));
			registers[r2].x = registers[r1][offset1];
			break;

		case inst_setComponent:
			float v2 = registers[r3].x;
			uint offset2 = uint(floor(v2));
			registers[r1][offset2] = registers[r2].x;
			break;
		
		case inst_load:
			registers[r1].x = uint(constant);
			break;

		case inst_readMemory:
			float v3 = registers[r2].x;
			uint offset3 = uint(floor(v3));
			registers[r1].x = memory[offset3];
			break;
			
		case inst_writeMemory:
			float v4 = registers[r2].x;
			uint offset4 = uint(floor(v4));
			memory[offset4] = registers[r1].x;
			break;

		case inst_sign:
			registers[r2] = sign(registers[r1]);
			break;
	}
}
