#version 330 core

out vec4 outFragColor;

in vec2 uv;

uniform vec3 color;

uniform vec3 screenSize;

// Registers:
const int reg_z = 0;
const int reg_s0 = 1; 
const int reg_s1 = 2; 
const int reg_s2 = 3; 
const int reg_s3 = 4; 
const int reg_s4 = 5; 
const int reg_s5 = 6; 
const int reg_v0 = 7; 
const int reg_v1 = 8; 
const int reg_v2 = 9; 
const int reg_v3 = 10; 
const int reg_v4 = 11; 
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
const int inst_getComponent = 5;
const int inst_setComponent = 6;
const int inst_load = 7;
const int inst_readMemory = 8;
const int inst_writeMemory = 9;

struct Instruction {
	int OpCode;
	int r1;
	int r2;
	int r3;
	int constant;
};

uint Build3R(int opcode, int r1, int r2, int r3) {
	uint inst = 0u;

	opcode = clamp(opcode, 0, 15);
	r1 = clamp(r1, 0, 15);
	r2 = clamp(r2, 0, 15);
	r3 = clamp(r3, 0, 15);

	inst = uint(opcode) << 28;

	inst |= (uint(r1) << 24);
	inst |= (uint(r2) << 20);
	inst |= (uint(r3) << 16);

	return inst;
}

uint Build2R(int opcode, int r1, int r2) {
	uint inst = 0u;

	opcode = clamp(opcode, 0, 15);
	r1 = clamp(r1, 0, 15);
	r2 = clamp(r2, 0, 15);

	inst = uint(opcode) << 28;

	inst |= (uint(r1) << 24);
	inst |= (uint(r2) << 20);

	return inst;
}

uint BuildRS(int opcode, int r1, int constant) {
	uint inst = 0u;

	opcode = clamp(opcode, 0, 15);
	r1 = clamp(r1, 0, 15);

	inst = uint(opcode) << 28;

	inst |= (uint(r1) << 24);

	uint adjustedConstant = uint(constant) << 8;
	adjustedConstant = adjustedConstant >> 8;

	inst |= adjustedConstant;

	return inst;
}

int GetOpCode(uint instruction) {
	return int(instruction >> 28);
}

int GetFirstRegister(uint instruction) {
	uint r1 = instruction << 4;

	return int(r1 >> 28);
}

int GetSecondRegister(uint instruction) {
	uint r2 = instruction << 8;

	return int(r2 >> 28);
}

int GetThirdRegister(uint instruction) {
	uint r3 = instruction << 12;

	return int(r3 >> 28);
}

float GetConstant(uint instruction) {
	uint constant = instruction << 8;

	constant = constant >> 8;

	return float(constant);
}

const int instructionCount = 11;
uint instructions[instructionCount];

vec4 registers[16];

void ExecuteInstruction(uint instruction, inout vec4 registers[16]);

float memory[64];

void main() {
	// Build the instructions list:
	instructions[0] = Build3R(inst_getComponent, reg_pc, reg_s0, reg_z);  // getComponent(pc, s0)    # s0 = pc.x
    instructions[1] = BuildRS(inst_load, reg_s5, 1);			            // load(s5, 1)             # s5 = 1
	instructions[2] = Build3R(inst_getComponent, reg_pc, reg_s1, reg_s5); // getComponent(pc, s1)    # s1 = pc.y
	
	instructions[3] = Build3R(inst_getComponent, reg_s, reg_s3, reg_z);   // getComponent(s, s3, z)  # s3 = s.x
	instructions[4] = Build2R(inst_reciprocal, reg_s3, reg_s2);           // reciprocal(s3, s2)      # s2 = 1 / s3
	instructions[5] = Build3R(inst_getComponent, reg_s, reg_s4, reg_s5);  // getComponent(s, s4, s5) # s4 = s.y
	instructions[6] = Build2R(inst_reciprocal, reg_s4, reg_s3);           // reciprocal(s4, s3)      # s3 = 1 / s4
	
	instructions[7] = Build3R(inst_multiply, reg_s0, reg_s2, reg_s0);     // multiply(s0, s2, s0)    # s0 = s0 * s2
	instructions[8] = Build3R(inst_multiply, reg_s1, reg_s3, reg_s1);     // multiply(s1, s3, s1)    # s1 = s1 * s3
	
	instructions[9] = Build3R(inst_setComponent, reg_c, reg_s0, reg_z);   // setComponent(c, s0, z)  # c.x = s0
	instructions[10] = Build3R(inst_setComponent, reg_c, reg_s1, reg_s5); // setComponent(c, s1, s5) # c.y = s1

	// Prepare registers
	registers[reg_z] = vec4(0.0); // Zero out the zero register
	registers[reg_c] = vec4(0.0); // Zero out the color register

	registers[reg_pc] = vec4(uv.x * screenSize.x, uv.y * screenSize.y, 0.0, 0.0);
	registers[reg_s] = vec4(screenSize.x, screenSize.y, 0.0, 0.0);
	// TODO time register

	// Execute the instruction list
	for (int i = 0; i < instructionCount; ++i) {
		ExecuteInstruction(instructions[i], registers);
	}

	outFragColor = registers[reg_c]; // Assign the final color to the color register
}

// TODO modifying read only registers
void ExecuteInstruction(uint instruction, inout vec4 registers[16]) {
	int opcode = GetOpCode(instruction);

	int r1 = GetFirstRegister(instruction);
	int r2 = GetSecondRegister(instruction);
	int r3 = GetThirdRegister(instruction);
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
			int offset1 = int(floor(v1));
			registers[r2].x = registers[r1][offset1];
			break;

		case inst_setComponent:
			float v2 = registers[r3].x;
			int offset2 = int(floor(v2));
			registers[r1][offset2] = registers[r2].x;
			break;
		
		case inst_load:
			registers[r1].x = int(constant);
			break;

		case inst_readMemory:
			float v3 = registers[r2].x;
			int offset3 = int(floor(v3));
			registers[r1].x = memory[offset3];
			break;
			
		case inst_writeMemory:
			float v4 = registers[r2].x;
			int offset4 = int(floor(v4));
			memory[offset4] = registers[r1].x;
			break;
	}
}
