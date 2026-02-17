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

unsigned int GetOpcode(std::string operation) {
    if (operation == "add") return inst_add;
    if (operation == "multiply") return inst_multiply;
    if (operation == "negate") return inst_negate;
    if (operation == "reciprocal") return inst_reciprocal;
    if (operation == "move") return inst_move;
    if (operation == "getComponent") return inst_getComponent;
    if (operation == "setComponent") return inst_setComponent;
    if (operation == "load") return inst_load;
    if (operation == "readMemory") return inst_readMemory;
    if (operation == "writeMemory") return inst_writeMemory;
    
    return 404;
}

unsigned int GetRegister(std::string reg) {
    if (reg == "z") return reg_z;
    if (reg == "s0") return reg_s0;
    if (reg == "s1") return reg_s1;
    if (reg == "s2") return reg_s2;
    if (reg == "s3") return reg_s3;
    if (reg == "s4") return reg_s4;
    if (reg == "s5") return reg_s5;
    if (reg == "v0") return reg_v0;
    if (reg == "v1") return reg_v1;
    if (reg == "v2") return reg_v2;
    if (reg == "v3") return reg_v3;
    if (reg == "v4") return reg_v4;
    if (reg == "pc") return reg_pc;
    if (reg == "t") return reg_t;
    if (reg == "s") return reg_s;
    if (reg == "c") return reg_c;
    
    return 404;
}

// Taken from: https://stackoverflow.com/questions/216823/how-can-i-trim-a-stdstring
// Trim from the start (in place)
inline void ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    } ));
}

// Trim from the end (in place)
inline void rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    } ).base(), s.end());
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

        // Clean up empty lines
        bool foundNonSpace = false;
        for (auto c : line) {
            if (!std::isspace(c) && c != '\n') {
                foundNonSpace = true;
                break;
            }
        }

        if (!foundNonSpace) continue;

        // Find operation and args
        size_t argsStart = line.find_first_of('(');
        size_t argsEnd = line.find_last_of(')');

        if (argsStart == std::string::npos || argsEnd == std::string::npos) {
            errorMessage = "Unable to find opening and closing brackets around arguments, format arguments like: (a1, a2, a3).";
            break;
        }

        std::string args = line.substr(argsStart + 1, argsEnd - argsStart - 1);

        std::string operation = line.substr(0, argsStart);

        // Parse Instruction
        unsigned int opcode = GetOpcode(operation);

        if (opcode == 404) {
            errorMessage = "Unrecognized operation, please consult the language spec to ensure correct spelling.";
            break;
        }

        std::vector<std::string> cleanArguments{ };

        std::stringstream argStringStream{ args };
        std::string arg;
        while (std::getline(argStringStream, arg, ',')) {
            ltrim(arg);
            rtrim(arg);

            cleanArguments.push_back(arg);
        }

        std::cout << line << ", " << opcode << std::endl;
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