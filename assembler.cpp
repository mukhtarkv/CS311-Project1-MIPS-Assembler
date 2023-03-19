#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <bitset>
#include <ctype.h>
#include "assembler.h"
using namespace std;

const unsigned int startPC = 0x400000;
const unsigned int startData = 0x10000000;
const unsigned int immediate_length = 16;

const map<string, string> opcodes = {{"addiu", "001001"}, {"addu", "000000"}, {"and", "000000"}, {"andi", "001100"},
                                    {"beq", "000100"}, {"bne", "000101"}, {"j", "000010"}, {"jal", "000011"}, {"jr", "000000"},
                                    {"lui", "001111"}, {"lw", "100011"}, {"nor", "000000"}, {"or", "000000"}, 
                                    {"ori", "001101"}, {"sltiu", "001011"}, {"sltu", "000000"}, {"sll", "000000"},
                                    {"srl", "000000"}, {"sw", "101011"}, {"subu", "000000"}};
const map<string, string> functs = {{"addu", "100001"}, {"and", "100100"}, {"nor", "100111"}, {"or", "100101"}, 
                                    {"sltu", "101011"}, {"sll", "000000"}, {"srl", "000010"}, {"subu", "100011"}};

string getNumbers(string s) {
    string res = "";
    for (auto &ch : s) {
        if (isdigit(ch)) {
            res += ch;
        }
    }
    return res;
}

int valueToBytesString(string val) {
    unsigned int length = val.length();
    int intVal = stoi(val, nullptr, 0); // auto base
    return intVal;
}

vector<tuple<string, vector<string>>> getDataValues(vector<string> assembly_lines) {
    vector<tuple<string, vector<string>>> data;
    for(unsigned int i = 1; i < assembly_lines.size(); i++) {
        string line = assembly_lines[i];
        istringstream iss (line);
        string fst;
        iss >> fst;
        string val;
        if (fst == ".text") break;
        if (fst == ".word") {
            iss >> val;
            get<1>(data.back()).push_back(bitset<32>(valueToBytesString(val)).to_string());
        }
        else {
            iss >> val; // val now contains .word
            iss >> val;
            // data1: .word val
            data.push_back(tuple<string, vector<string>>{fst.substr(0, fst.length() - 1), {bitset<32>(valueToBytesString(val)).to_string()}});
        }
    }
    return data;
}

unsigned int getTextIdx(vector<string> assembly_lines) {
    unsigned int textIdx = 0;
    for(unsigned int i = 1; i < assembly_lines.size(); i++) {
        string line = assembly_lines[i];
        istringstream iss (line);
        string fst;
        iss >> fst;
        if (fst == ".text") {
            textIdx = i;
            break;
        }
    }
    return textIdx;
}

string getDataAddress(string label, vector<tuple<string, vector<string>>> data) {
    unsigned int offset = 0;
    for (unsigned int labelIdx = 0; labelIdx < data.size(); labelIdx++) {
        string curLabel = get<0>(data[labelIdx]);
        if (curLabel == label) break;
        offset += get<1>(data[labelIdx]).size();
    }
    unsigned int labelAddress = startData + 4 * offset;
    return bitset<32>(labelAddress).to_string();
}

void decodeInstr(string line, vector<tuple<string, vector<string>>> data, vector<string> &instructions) {
    istringstream iss (line);
    string operation;
    iss >> operation;

    string opcode;
    string rs;
    string rt;
    string rd;
    string shamt = "00000";
    string funct;
    string instr;
    string imm;
    size_t position_open_bracket;

    if (operation == "la") {
        // la $2, VAR1
        rs = "00000";
        iss >> rt;
        rt = bitset<5>(valueToBytesString(getNumbers(rt))).to_string();
        iss >> imm;
        imm = getDataAddress(imm, data);
        string upper = imm.substr(0, immediate_length);
        string lower = imm.substr(immediate_length);
        opcode = opcodes.at("lui");
        instr = opcode + rs + rt + upper;
        instructions.push_back(instr);
        if (lower.find('1') != string::npos) {
            opcode = opcodes.at("ori");
            instr = opcode + rt + rt + lower;
            instructions.push_back(instr);
        }
        return;
    }

    opcode = opcodes.at(operation);

    if (operation == "lw" || operation == "sw") {
        // lw $rt, imm($rs)
        iss >> rt;
        rt = bitset<5>(valueToBytesString(getNumbers(rt))).to_string();
        iss >> rs;
        position_open_bracket = rs.find_first_of("(");
        imm = rs.substr(0, position_open_bracket);
        imm = bitset<16>(valueToBytesString(imm)).to_string();
        rs = bitset<5>(valueToBytesString(getNumbers(rs.substr(position_open_bracket)))).to_string();
        instr = opcode + rs + rt + imm;
    }
    else if (operation == "lui") {
        // lui $2, ADDR
        rs = "00000";
        iss >> rt;
        rt = bitset<5>(valueToBytesString(getNumbers(rt))).to_string();
        iss >> imm;
        imm = bitset<16>(valueToBytesString(imm)).to_string();
        instr = opcode + rs + rt + imm;
    }
    else if (operation == "addiu" || operation == "andi" || operation == "ori" || operation == "lw" || operation == "sltiu" || operation == "sw") {
        // ori $2, $2, IMM
        iss >> rt;
        rt = bitset<5>(valueToBytesString(getNumbers(rt))).to_string();
        iss >> rs;
        rs = bitset<5>(valueToBytesString(getNumbers(rs))).to_string();
        iss >> imm;
        imm = bitset<16>(valueToBytesString(imm)).to_string();
        instr = opcode + rs + rt + imm;
    }
    else if (operation == "sll" || operation == "srl") {
        // sll $1, $2, 1
        rs = "00000";
        iss >> rd;
        rd = bitset<5>(valueToBytesString(getNumbers(rd))).to_string();
        iss >> rt;
        rt = bitset<5>(valueToBytesString(getNumbers(rt))).to_string();
        iss >> shamt;
        shamt = bitset<5>(valueToBytesString(getNumbers(shamt))).to_string();
        funct = functs.at(operation);
        instr = opcode + rs + rt + rd + shamt + funct;
    }
    else {
        // subu $1, $2, $3
        iss >> rd;
        rd = bitset<5>(valueToBytesString(getNumbers(rd))).to_string();
        iss >> rs;
        rs = bitset<5>(valueToBytesString(getNumbers(rs))).to_string();
        iss >> rt;
        rt = bitset<5>(valueToBytesString(getNumbers(rt))).to_string();
        funct = functs.at(operation);
        instr = opcode + rs + rt + rd + shamt + funct;
    }
    instructions.push_back(instr);
}

string decodeJump(string jumpInstr, unsigned int curInstrCount, map<string, string> textAddresses) {
    istringstream iss (jumpInstr);
    string operation;
    iss >> operation;
    string destination;
    string rs;
    string rt;

    if (operation[0] == 'b') {
        // beq $1, $2, VAR
        iss >> rs;
        rs = bitset<5>(valueToBytesString(getNumbers(rs))).to_string();
        iss >> rt;
        rt = bitset<5>(valueToBytesString(getNumbers(rt))).to_string();
        iss >> destination;
        bitset<26> branchAbsoluteAddress(textAddresses.at(destination));
        int branchAddrInt = branchAbsoluteAddress.to_ulong();
        int branchAddrRel = (branchAddrInt * 4 - (int) startPC) / 4 - (curInstrCount + 1);
        string branchAddress = bitset<16>(branchAddrRel).to_string();
        return opcodes.at(operation) + rs + rt + branchAddress;
    }

    iss >> destination;
    if (operation == "jr") {
        // jr $n
        rs = bitset<5>(stoi(getNumbers(destination), nullptr)).to_string();
        rt = "00000";
        string rd = "00000";
        string shamt = "00000";
        string funct = "001000";
        return opcodes.at(operation) + rs + rt + rd + shamt + funct;
    }
    
    return opcodes.at(operation) + textAddresses.at(destination);
}

vector<string> assemble(vector<string> assembly_lines) {
    vector<tuple<string, vector<string>>> data = getDataValues(assembly_lines);
    unsigned int textIdx = getTextIdx(assembly_lines);
    unsigned int dataSizeInBytes = (textIdx - 1) * 4;
    vector<string> instructions;
    map<string, string> textAddresses;
    for (unsigned int i = textIdx + 1; i < assembly_lines.size(); i++) {
        string line = assembly_lines[i];
        istringstream iss (line);
        string fst;
        iss >> fst;
        if (fst.back() == ':') {
            unsigned int locationAddress = (startPC + 4 * instructions.size())/4;
            textAddresses.insert(pair<string, string>(fst.substr(0, fst.length() - 1), bitset<26>(locationAddress).to_string()));
        }
        else if (fst[0] == 'j' || fst[0] == 'b') {
            // come back to j instructions with addresses specified after one pass
            instructions.push_back(line);
        }
        else {
            decodeInstr(line, data, instructions);
        }
    }

    unsigned int textSizeInBytes = instructions.size() * 4;

    vector<string> outputBinaryLines;
    outputBinaryLines.push_back(bitset<32>(textSizeInBytes).to_string());
    outputBinaryLines.push_back(bitset<32>(dataSizeInBytes).to_string());

    for (unsigned int i = 0; i < instructions.size(); i++) {
        string instr = instructions[i];
        string binaryLine = isdigit(instr[0]) ? instr : decodeJump(instr, i, textAddresses);
        outputBinaryLines.push_back(binaryLine);
    }

    for (unsigned int i = 0; i < data.size(); i++) {
        vector<string> dataValues = get<1>(data[i]);
        for (string const& value: dataValues) {
            outputBinaryLines.push_back(value);
        }
    }

    return outputBinaryLines;
}
