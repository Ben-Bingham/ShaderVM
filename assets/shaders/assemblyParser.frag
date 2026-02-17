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

int Build3R(int opcode, int r1, int r2, int r3) {
	int inst = 0;

	opcode = clamp(opcode, 0, 15);
	r1 = clamp(r1, 0, 15);
	r2 = clamp(r2, 0, 15);
	r3 = clamp(r3, 0, 15);

	inst = opcode << 28;

	inst |= (r1 << 24);
	inst |= (r2 << 20);
	inst |= (r3 << 16);

	return inst;
}

int Build2R(int opcode, int r1, int r2) {
	int inst = 0;

	opcode = clamp(opcode, 0, 15);
	r1 = clamp(r1, 0, 15);
	r2 = clamp(r2, 0, 15);

	inst = opcode << 28;

	inst |= (r1 << 24);
	inst |= (r2 << 20);

	return inst;
}

int BuildRS(int opcode, int r1, int constant) {
	int inst = 0;

	opcode = clamp(opcode, 0, 15);
	r1 = clamp(r1, 0, 15);

	inst = opcode << 28;

	inst |= (r1 << 24);
	uint floatAsInt = floatBitsToUint(constant);
	floatAsInt >> 8;

	int adjustedConstant = constant << 8;
	adjustedConstant = adjustedConstant >> 8;

	inst |= adjustedConstant;

	return inst;
}

const int instructionCount = 11;
Instruction instructions[instructionCount];

vec4 registers[16];

void ExecuteInstruction(Instruction instruction, inout vec4 registers[16]);

float memory[64];

void main() {
	// Build the instructions list:
	instructions[0] = Instruction(inst_getComponent, reg_pc, reg_s0, reg_z, 0);  // getComponent(pc, s0)    # s0 = pc.x
    instructions[1] = Instruction(inst_load, reg_s5, 0, 0, 1);			         // load(s5, 1)             # s5 = 1
	instructions[2] = Instruction(inst_getComponent, reg_pc, reg_s1, reg_s5, 0); // getComponent(pc, s1)    # s1 = pc.y

	instructions[3] = Instruction(inst_getComponent, reg_s, reg_s3, reg_z, 0);   // getComponent(s, s3, z)  # s3 = s.x
	instructions[4] = Instruction(inst_reciprocal, reg_s3, reg_s2, 0, 0);        // reciprocal(s3, s2)      # s2 = 1 / s3
	instructions[5] = Instruction(inst_getComponent, reg_s, reg_s4, reg_s5, 0);  // getComponent(s, s4, s5) # s4 = s.y
	instructions[6] = Instruction(inst_reciprocal, reg_s4, reg_s3, 0, 0);        // reciprocal(s4, s3)      # s3 = 1 / s4

	instructions[7] = Instruction(inst_multiply, reg_s0, reg_s2, reg_s0, 0);     // multiply(s0, s2, s0)    # s0 = s0 * s2
	instructions[8] = Instruction(inst_multiply, reg_s1, reg_s3, reg_s1, 0);     // multiply(s1, s3, s1)    # s1 = s1 * s3

	instructions[9] = Instruction(inst_setComponent, reg_c, reg_s0, reg_z, 0);   // setComponent(c, s0, z)  # c.x = s0
	instructions[10] = Instruction(inst_setComponent, reg_c, reg_s1, reg_s5, 0); // setComponent(c, s1, s5) # c.y = s1

	// Prepare registers
	registers[reg_z] = vec4(0.0); // Zero out the zero register
	registers[reg_c] = vec4(0.0); // Zero out the color register

	registers[reg_pc] = vec4(uv.x * screenSize.x, uv.y * screenSize.y, 0.0, 0.0);
	registers[reg_s] = vec4(screenSize.x, screenSize.y, 0.0, 0.0);
	// TODO time register

	// Execute the instruciton list
	for (int i = 0; i < instructionCount; ++i) {
		ExecuteInstruction(instructions[i], registers);
	}

	outFragColor = registers[reg_c]; // Assign the final color to the color register
}

// TODO modifying read only registers
void ExecuteInstruction(Instruction instruction, inout vec4 registers[16]) {
	switch (instruction.OpCode) {
		case inst_add:
			registers[instruction.r3].x = registers[instruction.r1].x + registers[instruction.r2].x;
			break;

		case inst_multiply:
			registers[instruction.r3].x = registers[instruction.r1].x * registers[instruction.r2].x;
			break;

		case inst_negate:
			registers[instruction.r2].x = -registers[instruction.r1].x;
			break;

		case inst_reciprocal:
			registers[instruction.r2].x = 1.0 / registers[instruction.r1].x;
			break;

		case inst_move:
			registers[instruction.r2].x = registers[instruction.r1].x;
			break;

		case inst_getComponent:
			float v1 = registers[instruction.r3].x;
			int offset1 = int(floor(v1));
			registers[instruction.r2].x = registers[instruction.r1][offset1];
			break;

		case inst_setComponent:
			float v2 = registers[instruction.r3].x;
			int offset2 = int(floor(v2));
			registers[instruction.r1][offset2] = registers[instruction.r2].x;
			break;
		
		case inst_load:
			registers[instruction.r1].x = int(instruction.constant);
			break;

		case inst_readMemory:
			float v3 = registers[instruction.r2].x;
			int offset3 = int(floor(v3));
			registers[instruction.r1].x = memory[offset3];
			break;
			
		case inst_writeMemory:
			float v4 = registers[instruction.r2].x;
			int offset4 = int(floor(v4));
			memory[offset4] = registers[instruction.r1].x;
			break;
	}
}
