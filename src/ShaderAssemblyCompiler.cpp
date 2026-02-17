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
const unsigned int inst_sign = 10;

unsigned int Build3R(unsigned int opcode, unsigned int r1, unsigned int r2, unsigned int r3) {
    unsigned int inst = 0u;

    opcode = glm::clamp(opcode, 0u, 15u);
    r1 = glm::clamp(r1, 0u, 15u);
    r2 = glm::clamp(r2, 0u, 15u);
    r3 = glm::clamp(r3, 0u, 15u);

    inst = opcode << 28;

    inst |= (r1 << 24);
    inst |= (r2 << 20);
    inst |= (r3 << 16);

    return inst;
}

unsigned int Build2R(unsigned int opcode, unsigned int r1, unsigned int r2) {
    unsigned int inst = 0u;

    opcode = glm::clamp(opcode, 0u, 15u);
    r1 = glm::clamp(r1, 0u, 15u);
    r2 = glm::clamp(r2, 0u, 15u);

    inst = opcode << 28;

    inst |= (r1 << 24);
    inst |= (r2 << 20);

    return inst;
}

unsigned int BuildRS(unsigned int opcode, unsigned int r1, unsigned int constant) {
    unsigned int inst = 0u;

    opcode = glm::clamp(opcode, 0u, 15u);
    r1 = glm::clamp(r1, 0u, 15u);

    inst = opcode << 28;

    inst |= (r1 << 24);

    unsigned int adjustedConstant = constant << 8;
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
    if (operation == "sign") return inst_sign;

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

enum class InstructionType {
    R3,
    R2,
    RS
};

InstructionType GetInstructionType(unsigned int opcode){
    if (opcode == inst_add ||
        opcode == inst_multiply ||
        opcode == inst_getComponent ||
        opcode == inst_setComponent) {
        return InstructionType::R3;
    }
    if (opcode == inst_negate ||
        opcode == inst_reciprocal ||
        opcode == inst_move ||
        opcode == inst_readMemory || 
        opcode == inst_writeMemory ||
        opcode == inst_sign) {
        return InstructionType::R2;
    }
    if (opcode == inst_load) {
        return InstructionType::RS;
    }

    std::cout << "ERROR, COMPILER SHOULD NOT EVER REACH THIS POINT, IF YOU ARE READING THIS YOUR CODE IS BADLY FORMATTED, ASSUME ALL COMPILER OUTPUT IS INVALID." << std::endl;
    return InstructionType::R3;
}

std::pair<std::string, unsigned int> ParseInstruction(unsigned int opcode, std::vector<std::string> arguments) {
    std::string error{ "" };
    unsigned int instruction{ 0 };

    InstructionType type = GetInstructionType(opcode);

    switch (type) {
        case InstructionType::R3: {
            if (arguments.size() != 3) {
                error = "Incorrect number of arguments passed, this operation expects three different arugments.";
                break;
            }

            unsigned int r1 = GetRegister(arguments[0]);
            if (r1 > 15) {
                error = "Invalid register given as first argument, please check the spec and ensure that spelling is correct.";
                break;
            }
            unsigned int r2 = GetRegister(arguments[1]);
            if (r2 > 15) {
                error = "Invalid register given as second argument, please check the spec and ensure that spelling is correct.";
                break;
            }
            unsigned int r3 = GetRegister(arguments[2]);
            if (r3 > 15) {
                error = "Invalid register given as third argument, please check the spec and ensure that spelling is correct.";
                break;
            }

            instruction = Build3R(opcode, r1, r2, r3);
            break;
        }
        case InstructionType::R2: {
            if (arguments.size() != 2) {
                error = "Incorrect number of arguments passed, this operation expects two different arugments.";
                break;
            }

            unsigned int r1 = GetRegister(arguments[0]);
            if (r1 > 15) {
                error = "Invalid register given as first argument, please check the spec and ensure that spelling is correct.";
                break;
            }
            unsigned int r2 = GetRegister(arguments[1]);
            if (r2 > 15) {
                error = "Invalid register given as second argument, please check the spec and ensure that spelling is correct.";
                break;
            }

            instruction = Build2R(opcode, r1, r2);
            break;
        }
        case InstructionType::RS: {
            if (arguments.size() != 2) {
                error = "Incorrect number of arguments passed, this operation expects two different arugments.";
                break;
            }

            unsigned int r1 = GetRegister(arguments[0]);
            if (r1 > 15) {
                error = "Invalid register given as first argument, please check the spec and ensure that spelling is correct.";
                break;
            }

            unsigned int constant = std::atoi(arguments[1].c_str());

            if (constant > 0xFFFFFF) {
                error = "The constant provided is too large, please keep the constant to 24 bits long or smaller.";
                break;
            }

            instruction = BuildRS(opcode, r1, constant);
            break;
        }
    }

    return std::make_pair(error, instruction);
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

        auto [e, i] = ParseInstruction(opcode, cleanArguments);

        if (e != "") { 
            errorMessage = e;
            break;
        }

        instructions.push_back(i);
    }

    if (errorMessage != "") {
        std::cout << "COMPILATION ERROR AT (" << lineNumber << "):\n    " << line << "\n" << errorMessage << std::endl;
    }

    return instructions;
}