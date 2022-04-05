#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include <fstream>
#include <list>
#include <iterator>
#include <regex>
#include "Parser.hpp"
#include "Symbol.hpp"
#include "Section.hpp"


enum Directive {
    GLOBAL = 0, 
    EXTERN,
    SECTION,
    WORD, 
    SKIP,
    EQU,
    END
};

enum Instruction {
    HALT = 0, 
    INT, 
    IRET,
    CALL,
    RET,
    JMP,
    JNE,
    JEQ,
    JGT,
    POP,
    PUSH,
    XCHG,
    ADD,
    SUB,
    MUL,
    DIV,
    CMP,
    NOT,
    AND,
    OR,
    XOR,
    TEST,
    SHL,
    SHR,
    LDR,
    STR,
    ERR
};

class Assembler
{

public:
    Assembler(std::ifstream *inputFile, std::ofstream *outputFile);
    ~Assembler();
    void firtsPass();
    void secondPass();
    void printSymbolTable();

private:

    void resolveLabel(std::string label);
    void resolveDirective(std::string isDirective, std::string label);
    void resolveInstruction (std::string inst, std::string line);
    void resolveLabelSP(std::string label);
    void resolveDirectiveSP(std::string isDirective, std::string label);
    void resolveInstructionSP(std::string inst, std::string line);
    int getNumber (std::string number);
    int getRegisterNumber (std::string line);
    Directive getDirective(std::string direcive);
    Section* getSectionFromSectionList(std::string sectionName);
    Instruction getInstruction(std::string instruction);
    std::list<Symbol*> getSymbolFromSTable (std::string symbolName);
    Symbol* getSymbolFromSTableByID(int id);

    Parser parser;
    std::ifstream *inputFile;
    std::ofstream *outputFile;
    
    int locationCounter;
    Section* currentSection;
    int currentLine;
    bool endFlag;

    std::list<Symbol*> symbolTable;
    std::list<Section*> sectionTable;
    std::list<RelocationSymbol*> relocTable;
     
};

#endif