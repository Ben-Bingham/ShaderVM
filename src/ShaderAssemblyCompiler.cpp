#include "ShaderAssemblyCompiler.h"

#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <cctype>

// Registers:
const unsigned int reg_z = 0;
const unsigned int reg_s0 = 1;
const unsigned int reg_s1 = 2;
const unsigned int reg_s2 = 3;
const unsigned int reg_s3 = 4;
const unsigned int reg_s4 = 5;
const unsigned int reg_s5 = 6;
const unsigned int reg_v0 = 7;
const unsigned int reg_v1 = 8;
const unsigned int reg_v2 = 9;
const unsigned int reg_v3 = 10;
const unsigned int reg_v4 = 11;
const unsigned int reg_pc = 12;
const unsigned int reg_t = 13;
const unsigned int reg_s = 14;
const unsigned int reg_c = 15;

// Op codes:
const unsigned int inst_add = 0;
const unsigned int inst_multiply = 1;
const unsigned int inst_negate = 2;
const unsigned int inst_reciprocal = 3;
const unsigned int inst_move = 4;
const unsigned int inst_getComponent = 5;
const unsigned int inst_setComponent = 6;
const unsigned int inst_load = 7;
const unsigned int inst_readMemory = 8;
const unsigned int inst_writeMemory = 9;

unsigned int Build3R(int opcode, int r1, int r2, int r3) {
    unsigned int inst = 0u;

    opcode = glm::clamp(opcode, 0, 15);
    r1 = glm::clamp(r1, 0, 15);
    r2 = glm::clamp(r2, 0, 15);
    r3 = glm::clamp(r3, 0, 15);

    inst = (unsigned int)opcode << 28;

    inst |= ((unsigned int)r1 << 24);
    inst |= ((unsigned int)r2 << 20);
    inst |= ((unsigned int)r3 << 16);

    return inst;
}

unsigned int Build2R(int opcode, int r1, int r2) {
    unsigned int inst = 0u;

    opcode = glm::clamp(opcode, 0, 15);
    r1 = glm::clamp(r1, 0, 15);
    r2 = glm::clamp(r2, 0, 15);

    inst = (unsigned int)opcode << 28;

    inst |= ((unsigned int)r1 << 24);
    inst |= ((unsigned int)r2 << 20);

    return inst;
}

unsigned int BuildRS(int opcode, int r1, int constant) {
    unsigned int inst = 0u;

    opcode = glm::clamp(opcode, 0, 15);
    r1 = glm::clamp(r1, 0, 15);

    inst = (unsigned int)opcode << 28;

    inst |= ((unsigned int)r1 << 24);

    unsigned int adjustedConstant = (unsigned int)constant << 8;
    adjustedConstant = adjustedConstant >> 8;

    inst |= adjustedConstant;

    return inst;
}

std::vector<unsigned int> Compile(std::string code) {
    std::vector<unsigned int> instructions;

    std::stringstream stream{ code };
    std::string line;

    std::string errorMessage = "";
    size_t lineNumber = 0;
    while (std::getline(stream, line, '\n')) {
        ++lineNumber;

        // Remove comments
        size_t commentStart = line.find_first_of('#');
        line = line.substr(0, commentStart);

        bool foundNonSpace = false;
        for (auto c : line) {
            if (!std::isspace(c) && c != '\n') {
                foundNonSpace = true;
                break;
            }
        }

        if (!foundNonSpace) continue;

        size_t argsStart = line.find_first_of('(');
        size_t argsEnd = line.find_last_of(')');

        if (argsStart == std::string::npos || argsEnd == std::string::npos) {
            errorMessage = "Unable to find opening and closing brackets around arguments, format arguments like: (a1, a2, a3).";
            break;
        }

        std::string args = line.substr(argsStart + 1, argsEnd - argsStart - 1);

        std::cout << line << ", " << args << std::endl;
    }

    if (errorMessage != "") {
        std::cout << "COMPILATION ERROR AT (" << lineNumber << "):\n    " << line << "\n" << errorMessage << std::endl;
    }

    instructions.resize(11);
    instructions[0] = Build3R(inst_getComponent, reg_pc, reg_s0, reg_z);  // getComponent(pc, s0)    # s0 = pc.x
    instructions[1] = BuildRS(inst_load, reg_s5, 1);			          // load(s5, 1)             # s5 = 1
    instructions[2] = Build3R(inst_getComponent, reg_pc, reg_s1, reg_s5); // getComponent(pc, s1)    # s1 = pc.y
    
    instructions[3] = Build3R(inst_getComponent, reg_s, reg_s3, reg_z);   // getComponent(s, s3, z)  # s3 = s.x
    instructions[4] = Build2R(inst_reciprocal, reg_s3, reg_s2);           // reciprocal(s3, s2)      # s2 = 1 / s3
    instructions[5] = Build3R(inst_getComponent, reg_s, reg_s4, reg_s5);  // getComponent(s, s4, s5) # s4 = s.y
    instructions[6] = Build2R(inst_reciprocal, reg_s4, reg_s3);           // reciprocal(s4, s3)      # s3 = 1 / s4
    
    instructions[7] = Build3R(inst_multiply, reg_s0, reg_s2, reg_s0);     // multiply(s0, s2, s0)    # s0 = s0 * s2
    instructions[8] = Build3R(inst_multiply, reg_s1, reg_s3, reg_s1);     // multiply(s1, s3, s1)    # s1 = s1 * s3
    
    instructions[9] = Build3R(inst_setComponent, reg_c, reg_s0, reg_z);   // setComponent(c, s0, z)  # c.x = s0
    instructions[10] = Build3R(inst_setComponent, reg_c, reg_s1, reg_s5); // setComponent(c, s1, s5) # c.y = s1

    return instructions;
}