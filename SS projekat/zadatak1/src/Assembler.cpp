#include "../inc/Assembler.hpp"

Assembler::Assembler(std::ifstream *inputFile, std::ofstream *outputFile) : parser()
{
    this->inputFile = inputFile;
    this->outputFile = outputFile;
    this->locationCounter = 0;
    this->currentSection = new Section("-", 0);
    this->symbolTable = std::list<Symbol *>();
    this->sectionTable = std::list<Section *>();
    this->endFlag = false;
}

int Assembler::getRegisterNumber(std::string line)
{
    std::regex reg1("r[0-9]");
    std::smatch m1;
    std::string regS;
    int rS = -1;
    if (std::regex_search(line, m1, reg1))
    {
        regS = m1.str();
        regS = regS.back();
        rS = std::stoul(regS);
    }
    return rS;
}
int Assembler::getNumber(std::string number)
{
    int value;
    if (number.rfind("0x") != std::string::npos)
    {
        value = std::stoul(number, nullptr, 16);
    }
    else
    {
        value = std::stoul(number, nullptr, 10);
    }
    return value;
}
Section *Assembler::getSectionFromSectionList(std::string sectionName)
{

    Section *ret;
    for (auto it = this->sectionTable.begin(); it != this->sectionTable.end(); ++it)
    {
        Section *sym = *it;
        if (sym->getName().compare(sectionName) == 0)
            ret = sym;
    }
    return ret;
}
std::list<Symbol *> Assembler::getSymbolFromSTable(std::string symbolName)
{

    size_t startIndex = symbolName.find_first_not_of(" ");
    symbolName = symbolName.substr(startIndex);
    symbolName = parser.trimRightSideSpace(symbolName);
    std::list<Symbol *> ret = std::list<Symbol *>();
    for (auto it = this->symbolTable.begin(); it != this->symbolTable.end(); ++it)
    {
        Symbol *sym = *it;
        if (sym->getName().compare(symbolName) == 0)
        {
            ret.push_back(sym);
        }
    }
    return ret;
}

Symbol *Assembler::getSymbolFromSTableByID(int id)
{
    Symbol *ret = nullptr;
    for (auto it = this->symbolTable.begin(); it != this->symbolTable.end(); ++it)
    {
        ret = *it;
        if (ret->getSymbolIndex() == id)
            break;
    }
    return ret;
}
void Assembler::printSymbolTable()
{
    std::cout << "============================SYMBOL TABLE=============================================\n";
    std::cout << "|  ID   |     NAME     | SYMB/SECT |   SECTION   |   OFFSET   |   TYPE   |    VALUE | \n";
    std::cout << "=====================================================================================\n";
    *outputFile << "============================SYMBOL TABLE=============================================\n";
    *outputFile << "|  ID   |     NAME     | SYMB/SECT |   SECTION   |   OFFSET   |   TYPE   |    VALUE | \n";
    (*outputFile) << "=====================================================================================\n";
    for (auto it = symbolTable.begin(); it != symbolTable.end(); it++)
    {
        Symbol *symb = *it;
        std::cout << *symb << "\n";
        *outputFile << *symb << "\n";
    };
    std::cout << "=====================================================================================\n";
    *outputFile << "=====================================================================================\n";
}

Directive Assembler::getDirective(std::string directive)
{
    directive = parser.trimRightSideSpace(directive);

    Directive d;
    if (directive.compare(".global") == 0)
        d = Directive::GLOBAL;
    else if (directive.compare(".extern") == 0)
        d = Directive::EXTERN;
    else if (directive.compare(".section") == 0)
        d = Directive::SECTION;
    else if (directive.compare(".word") == 0)
        d = Directive::WORD;
    else if (directive.compare(".skip") == 0)
        d = Directive::SKIP;
    else if (directive.compare(".equ") == 0)
        d = Directive::EQU;
    else if (directive.compare(".end") == 0)
        d = Directive::END;

    return d;
}

Instruction Assembler::getInstruction(std::string inst)
{
    int startIndex = inst.find_first_not_of(" ");
    inst = inst.substr(startIndex);
    inst = parser.trimRightSideSpace(inst);

    Instruction i;
    if (inst.compare("halt") == 0)
        i = Instruction::HALT;
    if (inst.compare("int") == 0)
        i = Instruction::INT;
    if (inst.compare("iret") == 0)
        i = Instruction::IRET;
    if (inst.compare("ret") == 0)
        i = Instruction::RET;
    if (inst.compare("call") == 0)
        i = Instruction::CALL;
    if (inst.compare("pop") == 0)
        i = Instruction::POP;
    if (inst.compare("push") == 0)
        i = Instruction::PUSH;
    if (inst.compare("jmp") == 0)
        i = Instruction::JMP;
    if (inst.compare("jne") == 0)
        i = Instruction::JNE;
    if (inst.compare("jeq") == 0)
        i = Instruction::JEQ;
    if (inst.compare("jgt") == 0)
        i = Instruction::JGT;
    if (inst.compare("xchg") == 0)
        i = Instruction::XCHG;
    if (inst.compare("add") == 0)
        i = Instruction::ADD;
    if (inst.compare("sub") == 0)
        i = Instruction::SUB;
    if (inst.compare("mul") == 0)
        i = Instruction::MUL;
    if (inst.compare("div") == 0)
        i = Instruction::DIV;
    if (inst.compare("cmp") == 0)
        i = Instruction::CMP;
    if (inst.compare("not") == 0)
        i = Instruction::NOT;
    if (inst.compare("and") == 0)
        i = Instruction::AND;
    if (inst.compare("or") == 0)
        i = Instruction::OR;
    if (inst.compare("xor") == 0)
        i = Instruction::XOR;
    if (inst.compare("test") == 0)
        i = Instruction::TEST;
    if (inst.compare("shl") == 0)
        i = Instruction::SHL;
    if (inst.compare("shr") == 0)
        i = Instruction::SHR;
    if (inst.compare("ldr") == 0)
        i = Instruction::LDR;
    if (inst.compare("str") == 0)
        i = Instruction::STR;

    return i;
}

// void Assembler::getInstruction(std::string instruction);
void Assembler::resolveLabel(std::string label)
{
    if (!label.empty())
        label.pop_back(); //Remove ':' from the end of label

    if (this->currentSection->getName() == "-")
    { //Check if label is defined out of section
        std::cerr << "Error: Label " + label + " is defined out of any section! " << std::endl;
        exit(-2);
    }
    std::list<Symbol *> list = getSymbolFromSTable(label);
    bool set = false; // True if symbol is declared as global and updated now, else push it to symbolTable
    while (!list.empty())
    {
        Symbol *declared = list.front();
        if (declared != nullptr && (declared->getSectionName() == this->currentSection->getName()))
        { //Symbol names can be same if deffined in different sections
            std::cerr << "Error: Multiple definition, line: " << this->currentLine << "\n";
            exit(-2);
        }
        else if (declared != nullptr && (declared->getSectionName() == "-") && (declared->getOffset() == 0))
        { //Symbol is declared as global, and deffined as label, update symbol table
            declared->setValue(locationCounter);
            declared->setOffset(locationCounter);
            declared->setSectionName(currentSection->getName());
            declared->setType("global");
            set = true;
        }

        list.pop_front();
    }
    if (!set)
        this->symbolTable.push_back(new Symbol(label, this->currentSection->getName(), "local", this->locationCounter, this->locationCounter, true));
}

void Assembler::resolveDirective(std::string isDirective, std::string label)
{

    Directive d = getDirective(isDirective);

    std::list<std::string> params = std::list<std::string>();
    std::list<Symbol *> list = std::list<Symbol *>();
    std::string sectionName;
    std::string literal;
    std::string symbolName;
    std::string type;
    Symbol *symbolExists = nullptr;
    int value;
    switch (d)
    {
    case Directive::GLOBAL:
        params = parser.getParameterList(label);
        while (!params.empty())
        {
            std::string symbName = params.front();
            std::size_t startIndex = symbName.find_first_not_of(" ");
            symbName = symbName.substr(startIndex);
            std::cout << symbName;
            symbolTable.push_back(new Symbol(symbName, "-", "local", 0, 0, true));
            params.pop_front();
        }
        break;

    case Directive::WORD:
        params = parser.getParameterList(label);
        if (params.empty())
        {
            std::cerr << "Error: Invalid format for .word directve at line: " << currentLine << '\n';
            exit(-2);
        }
        while (!params.empty())
        {
            locationCounter += 2;
            params.pop_front();
        }
        break;

        break;
    case Directive::EXTERN:
        params = parser.getParameterList(label);
        while (!params.empty())
        {
            std::string symbName = params.front();
            std::size_t startIndex = symbName.find_first_not_of(" ");
            symbName = symbName.substr(startIndex);
            symbolTable.push_back(new Symbol(symbName, "-", "global", 0, 0, true));
            params.pop_front();
        }
        break;

    case Directive::SECTION:
        sectionName = parser.getSectionName(label);
        if (sectionName.compare("err") == 0)
        {
            std::cerr << "Error: Multiple sections defined at line: " << this->currentLine << "\n";
            exit(-2);
        }
        else if (sectionName.compare("notFound") == 0)
        {
            std::cerr << "Error: Invalid format for .section directive at line: " << this->currentLine << '\n';
            exit(-2);
        }
        if (this->currentSection->getName() != "-")
        {
            this->currentSection->setSize(locationCounter);
            this->sectionTable.push_back(this->currentSection);
        }
        this->symbolTable.push_back(new Symbol(sectionName, sectionName, "local", 0, 0, false));
        this->currentSection = new Section(sectionName, 0);
        this->locationCounter = 0;
        break;

    case Directive::SKIP:
        literal = parser.getLiterals(label);
        if (literal.compare("err") == 0)
        {
            std::cerr << "Error: Invalid format for .skip directive on line: " << currentLine << "\n";
            exit(-2);
        }
        if (literal.rfind("0x") != std::string::npos)
        {
            value = std::stoul(literal, nullptr, 16);
        }
        else
        {
            value = std::stoul(literal, nullptr, 10);
        }
        this->locationCounter += value;
        break;

    case Directive::EQU:
        symbolName = parser.getEquSymbolName(label);
        type = "local";
        if (symbolName.compare("paramerr") == 0)
        {
            std::cerr << "Error: Not enough parameters passed, directive .equ, at line: " << currentLine << '\n';
            exit(-2);
        }
        else if (symbolName.compare("err") == 0)
        {
            std::cerr << "Error: Invalid format for .equ directive on line: " << currentLine << "\n";
            exit(-2);
        };
        literal = parser.getLiterals(label);
        if (literal.compare("err") == 0)
        {
            std::cerr << "Error: Invalid format for .equ directive on line: " << currentLine << "\n";
            exit(-2);
        };
        if (literal.rfind("0x") != std::string::npos)
        {
            value = std::stoul(literal, nullptr, 16);
        }
        else
        {
            value = std::stoul(literal, nullptr, 10);
        }
        list = getSymbolFromSTable(symbolName);
        while (!list.empty())
        {
            symbolExists = list.front();
            std::cout << *symbolExists << '\n';

            if (symbolExists != nullptr && (symbolExists->getSectionName() == "-") && (symbolExists->getType() == "global"))
            { //Symbol is declared as extern, error in redeffinition
                std::cerr << "Error: Redeffinition of extern symbol " << symbolName << " at line: " << currentLine << '\n';
                exit(-2);
            };
            if (symbolExists != nullptr && (symbolExists->getSectionName() == "-") && (symbolExists->getType() == "local"))
            { //Symbol can already be declared as .global, update symbol table
                symbolExists->setOffset(locationCounter);
                symbolExists->setValue(value);
                symbolExists->setSectionName("__abs_data");
            };
            // if (symbolExists != nullptr && (symbolExists->getSectionName().compare("-")!=0) && symbolExists->getSectionName() == currentSection->getName())
            // {
            //     std::cerr << "Error: Multiple definition of symbol " << symbolName << " at line: " << currentLine << '\n';
            //     exit(-2);
            // }
            list.pop_front();
        }
        symbolTable.push_back(new Symbol(symbolName, "__abs_data", "local", this->locationCounter, value, true));

        break;

    case Directive::END:
        this->endFlag = true;
        if (this->currentSection->getName() != "-")
        {
            this->currentSection->setSize(this->locationCounter);
            this->sectionTable.push_back(this->currentSection);
            this->currentSection = new Section("-", 0);
            this->locationCounter = 0;
            this->currentLine = 0;
        }
        break;

    default:
        break;
    }
}

void Assembler::resolveInstruction(std::string inst, std::string line)
{
    Instruction i = getInstruction(inst);

    std::regex *reg1;
    std::regex *reg2;
    std::regex *reg3;
    std::regex *reg4;
    std::list<std::string> params = std::list<std::string>();
    std::smatch m1;
    switch (i)
    {
    case Instruction::INT:
    case Instruction::NOT:
        reg1 = new std::regex("\\s*" + inst + "[ ]*r[0-9]");
        if (regex_search(line, m1, *reg1))
        {
            this->locationCounter += 2; //Int takes up 2 bytes of memory
        }
        else
        {
            std::cerr << "Invalid format of instruction, at line: " << currentLine << "\n";
            exit(-2);
        }
        delete reg1;
        break;
    case Instruction::POP:
    case Instruction::PUSH:
        reg1 = new std::regex("\\s*" + inst + "[ ]*r[0-9]");
        if (regex_search(line, m1, *reg1))
        {
            this->locationCounter += 3; //Int takes up 2 bytes of memory
        }
        else
        {
            std::cerr << "Invalid format of instruction, at line: " << currentLine << "\n";
            exit(-2);
        }
        delete reg1;
        break;
    case Instruction::HALT:
    case Instruction::RET:
    case Instruction::IRET:
        if (parser.resolveInstructionNoParams(line, inst))
        {
            this->locationCounter += 1; //ret, halt, iret take up 1 byte of memory
        }
        else
        {
            std::cerr << "Invalid format of instruction, at line: " << currentLine << "\n";
            exit(-2);
        }
        break;
    case Instruction::JEQ:
    case Instruction::JMP:
    case Instruction::JGT:
    case Instruction::JNE:
    case Instruction::CALL:

        reg1 = new std::regex("\\s*" + inst + "[ ]*[*%]?\\w+");                                  //call */$ symbol/literal
        reg2 = new std::regex("\\s*" + inst + "[ ]* \\*r[0-9]");                                 //call *rX
        reg3 = new std::regex("\\s*" + inst + "[ ]*\\*\\[r[0-9]\\]");                            //call *[rX]
        reg4 = new std::regex("\\s*" + inst + "[ ]*\\*\\s*\\[[ ]*r[0-9][ ]*\\+[ ]*\\w+[ ]*\\]"); // call *[rX + symbol/literal]

        if (regex_search(line, m1, *reg1))
        {
            // std::cout << m1.str() << '\n';
            this->locationCounter += 5; //call */$ symbol/literal takes up 5 bytes of memory
        }
        else if (regex_search(line, m1, *reg2))
        {

            // std::cout << m1.str() << '\n';
            this->locationCounter += 3; //call *rX takes up 3 bytes of memory
        }
        else if (regex_search(line, m1, *reg3))
        {
            //  std::cout << m1.str() << '\n';
            this->locationCounter += 3; //call *[rX] takes up 3 bytes of memory
        }
        else if (regex_search(line, m1, *reg4))
        {
            // std::cout << m1.str() << '\n';
            this->locationCounter += 5; //call *[rX + symbol/literal] takes up 2 bytes of memory
        };
        delete reg1;
        delete reg2;
        delete reg3;
        delete reg4;
        break;

    case Instruction::XCHG:
    case Instruction::DIV:
    case Instruction::CMP:
    case Instruction::ADD:
    case Instruction::MUL:
    case Instruction::SUB:
    case Instruction::XOR:
    case Instruction::AND:
    case Instruction::TEST:
    case Instruction::OR:
    case Instruction::SHL:
    case Instruction::SHR:

        reg1 = new std::regex("[ ]*" + inst + "\\s*r[0-9]\\s*,\\s*r[0-9]");
        if (regex_search(line, m1, *reg1))
        {
            this->locationCounter += 2; //xchg, div, mul, add, cmp, sub take up 2 bytes of memory
        }
        else
        {
            std::cerr << "Invalid format of instruction, at line: " << currentLine << "\n";
            exit(-2);
        }
        delete reg1;
        break;
    case Instruction::LDR:
    case Instruction::STR:

        reg1 = new std::regex("\\s*" + inst + "[ ]*r[0-9],[ ]*([\\*%$])?\\w+");                     //call */$ symbol/literal
        reg2 = new std::regex("\\s*" + inst + "[ ]*r[0-9][ ]*,[ ]*\\*r[0-9]");                      //call *rX
        reg3 = new std::regex("\\s*" + inst + "[ ]*r[0-9][ ]*,[ ]*\\*\\[r[0-9]\\]");                //call *[rX]
        reg4 = new std::regex("\\s*" + inst + "[ ]*r[0-9][ ]*,[ ]*\\*\\[r[0-9][ ]*\\+[ ]*\\w+\\]"); // call *[rX + symbol/literal]

        if (regex_search(line, m1, *reg1))

        {
            //  std::cout << m1.str() << '\n';
            this->locationCounter += 5; //call */$ symbol/literal takes up 5 bytes of memory
        }
        else if (regex_search(line, m1, *reg2))
        {
            //   std::cout << m1.str() << '\n';
            this->locationCounter += 3; //call *rX takes up 3 bytes of memory
        }
        else if (regex_search(line, m1, *reg3))
        {
            //   std::cout << m1.str() << '\n';
            this->locationCounter += 3; //call *[rX] takes up 2 bytes of memory
        }
        else if (regex_search(line, m1, *reg4))
        {
            //  std::cout << m1.str() << '\n';
            this->locationCounter += 5; //call *[rX + symbol/literal] takes up 2 bytes of memory
        };
        delete reg1;
        delete reg2;
        delete reg3;
        delete reg4;
        //    std::cout << inst << " " << locationCounter << '\n';
        break;
    default:
        break;
    }
}

void Assembler::firtsPass()
{
    std::string line;
    this->locationCounter = 0;
    this->currentSection->setName("-");
    this->currentSection->setSize(0);
    this->currentLine = 0;
    this->endFlag = false;

    while (!inputFile->eof() && !endFlag)
    {
        this->currentLine++;
        getline(*inputFile, line);

        if (line.empty())
            continue; //Check if empty

        if (parser.isLineComment(line))
            continue;                                   //Check if comment
        std::string isLabel = parser.isLineLabel(line); //Check if label
        if (isLabel != "notLabel")
        {
            resolveLabel(isLabel);
            continue;
        }

        std::string isDirective = parser.isLineDirective(line); //Check if directive
        if (isDirective != "notDirective")
        {
            resolveDirective(isDirective, line);
            continue;
        }

        std::string instruction = parser.isLineInstruction(line); //Check if instruction;
        if (instruction != "notInst")
        {
            resolveInstruction(instruction, line);
            continue;
        }
    }
    this->printSymbolTable();

    this->endFlag = false;
}

void Assembler::resolveDirectiveSP(std::string directive, std::string line)
{
    Directive d = getDirective(directive);

    std::list<std::string> params = std::list<std::string>();
    std::string sectionName;
    std::string literal;
    std::string symbolName;
    Symbol *symbolExists = nullptr;
    std::list<Symbol *> symbList = std::list<Symbol *>();
    int value;
    int temp;
    long bytes = 0;
    switch (d)
    {
    case GLOBAL:
        params = parser.getParameterList(line);
        while (!params.empty())
        {
            std::string symbName = params.front();
            std::size_t startIndex = symbName.find_first_not_of(" ");
            symbName = symbName.substr(startIndex);
            std::list<Symbol *> list = getSymbolFromSTable(symbName);
            while (!list.empty())
            {
                Symbol *symbolExists = list.front();
                if (symbolExists->getSectionName() == "-")
                {
                    std::cerr << "Error: Symbol " << symbName << " declared as global at line " << currentLine << " is not deffined in this file! \n";
                    exit(-2);
                }
                list.pop_front();
            }

            params.pop_front();
        }
        break;
    case Directive::WORD:
        params = parser.getParameterList(line);
        while (!params.empty())
        {
            std::string symbName = params.front();
            std::size_t startIndex = symbName.find_first_not_of(" ");
            symbName = symbName.substr(startIndex);

            literal = parser.getLiterals(symbName);
            if (literal != "err")
            {
                value = getNumber(literal);
                temp = value;
                bytes |= (temp << 8) & 0xff00;
                bytes |= (value >> 8) & 0x00ff;
                currentSection->addData(2, bytes);
                locationCounter += 2;
            }
            else
            {
                symbList = getSymbolFromSTable(symbName);
                while (!symbList.empty())
                {
                    symbolExists = symbList.front();
                    symbList.pop_front();
                    std::cout << symbolExists->getName() << " " << symbolExists->getValue() << '\n';
                    value = symbolExists->getValue();
                    temp = value;
                    bytes |= (temp << 8) & 0xff00;
                    bytes |= (value >> 8) & 0x00ff;
                    std::cout << std::hex << bytes << '\n';
                    if (symbolExists->getSectionName() != currentSection->getName())
                    {
                        symbolExists = getSymbolFromSTableByID(symbolExists->getSymbolIndex());
                        currentSection->addToRelocTable(new RelocationSymbol(symbolExists->getSectionName(), "abs_data", symbolExists->getSymbolIndex(), locationCounter));
                    }
                    currentSection->addData(2, bytes);
                    locationCounter += 2;
                }
            }
            params.pop_front();
        }
        break;
    case Directive::EXTERN:
        params = parser.getParameterList(line);
        while (!params.empty())
        {
            std::string symbName = params.front();
            std::size_t startIndex = symbName.find_first_not_of(" ");
            symbName = symbName.substr(startIndex);
            std::list<Symbol *> list = getSymbolFromSTable(symbName);
            while (!list.empty())
            {
                Symbol *symbolExists = list.front();
                if (symbolExists->getSectionName() != "-")
                {
                    std::cerr << "Error: Symbol " << symbName << " declared as extern is redeffined at line " << currentLine << "\n";
                    exit(-2);
                }
                list.pop_front();
            }

            params.pop_front();
        }
        break;
    case Directive::SKIP:
        literal = parser.getLiterals(line);
        value = getNumber(literal);
        //Print value number of bytes in output file
        for (int i = 0; i < value; i++)
        {
            this->currentSection->addData(1, 0);
        }
        //Update locationCounter
        this->locationCounter += value;
        break;
    case Directive::SECTION:
        sectionName = parser.getSectionName(line);
        this->currentSection = getSectionFromSectionList(sectionName);
        this->locationCounter = 0;
        break;
    case Directive::EQU:
        break;
    case Directive::END:
        this->endFlag = true;
        break;
    default:
        break;
    }
}

void Assembler::resolveInstructionSP(std::string inst, std::string line)
{

    Instruction i = getInstruction(inst);
    std::regex *reg1;
    std::regex *reg2;
    std::regex *reg3;
    std::regex *reg4;
    std::regex *reg5;
    int rS;
    int rD;
    long bytes = 0;
    int value = 0;
    int payload = 0;
    std::string regS;
    std::string regD;
    std::string literal;
    std::string type;
    std::string symbName;
    std::list<std::string> params = std::list<std::string>();
    std::string symb;
    std::smatch m1;
    Symbol *symbolExists;
    std::list<Symbol *> symbList = std::list<Symbol *>();
    std::size_t startIndex;
    switch (i)
    {
    case Instruction::INT:
        rS = getRegisterNumber(line);
        bytes |= 1 << 12;
        bytes |= rS << 4;
        bytes |= 0xf;
        this->currentSection->addData(2, bytes); // opCode: 0001 0000 rS 1111
        this->locationCounter += 2;
        break;
    case Instruction::HALT:
        this->locationCounter += 1; //Halt takes up 1 byte of memory
        bytes = getNumber("0x00");  // opCode: 00
        this->currentSection->addData(1, bytes);
        break;
    case Instruction::RET:
        this->locationCounter += 1; //Ret takes up 1 byte of memory
        bytes = getNumber("0x40");  // opCode: 20
        this->currentSection->addData(1, bytes);
        break;
    case Instruction::IRET:
        this->locationCounter += 1; //Iret takes up 1 byte of memory
        bytes = getNumber("0x20");  // opCode: 40
                                    //   std::cout << bytes << std::endl;
        this->currentSection->addData(1, bytes);
        break;
    case Instruction::POP:
    case Instruction::PUSH:
        rD = getRegisterNumber(line);
        rS = 6;
        if (i == Instruction::POP)
            bytes |= (0xa1) << 8;
        else
            bytes |= (0xa2) << 8;
        bytes |= (rD << 4) & 0xf0;
        bytes |= rS & 0x0f;
        bytes = bytes << 8;
        if (i == Instruction::POP)
            bytes |= 0x42;
        else
            bytes |= 0x12;
        locationCounter += 3;
        currentSection->addData(3, bytes);
        break;
    //up - update type
    //am - address mode
    case Instruction::JEQ: //opCode: 0101 0001 1111 rS up am
    case Instruction::JMP: //opCode: 0101 0000 1111 rS up am
    case Instruction::JGT: //opCode: 0101 0011 1111 rS up am
    case Instruction::JNE: //opCode: 0101 0010 1111 rS up am
        bytes = 5 << 20;
        if (i == Instruction::JEQ)
        {
            bytes |= (1 << 16);
        }
        else if (i == Instruction::JNE)
        {
            bytes |= (2 << 16);
        }
        else if (i == Instruction::JGT)
        {
            bytes |= (3 << 16);
        }
        bytes |= (0xF << 12);
        reg1 = new std::regex("\\s*" + inst + "[ ]*[*%]\\w+");                                   //call */$ symbol/literal
        reg2 = new std::regex("\\s*" + inst + "[ ]*\\*r[0-9][ ]*");                              //call *rX
        reg3 = new std::regex("\\s*" + inst + "[ ]*\\*\\[r[0-9][ ]*\\][ ]*");                    //call *[rX]
        reg4 = new std::regex("\\s*" + inst + "[ ]*\\*\\s*\\[[ ]*r[0-9][ ]*\\+[ ]*\\w+[ ]*\\]"); // call *[rX + symbol/literal]
        reg5 = new std::regex("\\s*" + inst + "[ ]*\\w+");                                       //call  symbol/literal

        if (regex_search(line, m1, *reg2)) //rS 0000 0001
        {
            std::cout << m1.str() << '\n';
            value = getRegisterNumber(line);
            bytes |= (value << 8); //rS
            bytes |= 1;            //am
            this->currentSection->addData(3, bytes);
            this->locationCounter += 3; //jmp *rX takes up 3 bytes of memory
        }
        else if (regex_search(line, m1, *reg3))
        {
            std::cout << m1.str() << '\n';
            value = getRegisterNumber(line);
            bytes |= (value << 8); //rS
            bytes |= 2;            //am
            this->currentSection->addData(3, bytes);
            this->locationCounter += 3; //call *[rX] takes up 3 bytes of memory
        }
        else if (regex_search(line, m1, *reg1))
        {
            //   std::cout << m1.str() << '\n';

            this->locationCounter += 5;

            literal = parser.getLiterals(line);
            if (literal != "err" && line.find_first_of("*") != std::string::npos)
            {
                bytes |= 0x04; //1111 rS 0000 0100
                bytes = bytes << 16;
                value = getNumber(literal);
                bytes |= ((value)&0xffff); //little endian for payload in instruction
                this->currentSection->addData(5, bytes);
            }
            else
            {
                symbName = parser.getSymbol(inst, line);
                if (symbName[0] == '*')
                {

                    rS = symbName.find_first_of('*');
                    symbName = symbName.substr(rS + 1);
                    type = "ABS";
                    bytes |= 0x04;
                }
                else if (symbName.find_first_of("%") != std::string::npos)
                {
                    rS = symbName.find_first_of("%");
                    symbName = symbName.substr(rS + 1);
                    type = "PC32_REL";
                    bytes |= 0x700;
                }
                bytes = bytes << 16;
                symbList = getSymbolFromSTable(symbName);
                if (!symbList.empty())
                {
                    symbolExists = symbList.front();
                    symbList.pop_front();
                    value = symbolExists->getValue();

                    if (symbolExists->getSectionName().compare(this->currentSection->getName()) != 0 && symbolExists->getSectionName() != "__abs_data")
                    {
                        if (symbolExists->getType() == "global")
                        {
                            value = 0; //addend
                            this->currentSection->addToRelocTable(new RelocationSymbol(currentSection->getName(), type, symbolExists->getSymbolIndex(), locationCounter - 2));
                        }
                        else
                        {
                            if (symbolExists->getType() == "local")
                            {
                                symbList = getSymbolFromSTable(symbolExists->getSectionName());
                                symbolExists = symbList.front();
                                symbList.pop_front();
                                this->currentSection->addToRelocTable(new RelocationSymbol(currentSection->getName(), type, symbolExists->getSymbolIndex(), locationCounter - 2));
                            }
                            else
                                this->currentSection->addToRelocTable(new RelocationSymbol(currentSection->getName(), type, 0, locationCounter - 2));
                        }
                    }
                    bytes |= (value)&0xffff;
                    currentSection->addData(5, bytes);
                }
            }
        }
        else if (regex_search(line, m1, *reg4))
        {
            this->locationCounter += 5; //call *[rX + symbol/literal] takes up 2 bytes of memory
                                        //        std::cout << m1.str() << '\n';
            params = parser.getBracketParameterList(line);
            bytes |= (0xf << 12);
            regS = params.front();
            params.pop_front();
            rS = getRegisterNumber(regS);
            bytes |= (rS << 8);
            bytes |= (0x03);
            bytes = bytes << 16;
            symbName = params.front();
            params.pop_front();
            literal = parser.getLiterals(symbName);
            if (literal != "err")
            {
                value = getNumber(literal);
                bytes |= ((value)&0xffff);
                currentSection->addData(5, bytes);
            }
            else
            {
                symbList = getSymbolFromSTable(symbName);
                if (!symbList.empty())
                {
                    symbolExists = symbList.back();

                    value = symbolExists->getValue();
                    if (symbolExists->getSectionName() != currentSection->getName() && symbolExists->getSectionName() != "__abs_data")
                    {
                        if (symbolExists->getType() == "global")
                        {
                            value = 0; //addend
                            this->currentSection->addToRelocTable(new RelocationSymbol(currentSection->getName(), "ABS", symbolExists->getSymbolIndex(), locationCounter - 2));
                        }
                        else
                        {
                            if (symbolExists->getSectionName() != "-")
                            {
                                symbList = getSymbolFromSTable(symbolExists->getSectionName());
                                symbolExists = symbList.front();
                                symbList.pop_front();
                                this->currentSection->addToRelocTable(new RelocationSymbol(currentSection->getName(), "ABS", symbolExists->getSymbolIndex(), locationCounter - 2));
                            }
                            else
                                this->currentSection->addToRelocTable(new RelocationSymbol(currentSection->getName(), "ABS", 0, locationCounter - 2));
                        }
                    }

                    bytes |= (value)&0xffff;
                    currentSection->addData(5, bytes);
                }
            }
        }
        else if (regex_search(line, m1, *reg5))
        {
            this->locationCounter += 5;
            literal = parser.getLiterals(line);
            std::cout<<"U REGEXU 5:" <<m1.str() <<  " \n";
            bytes |= (0xf0 << 8); //1111 rS 0000 0000
            bytes = bytes << 16;
            if (literal != "err")
            {
                value = getNumber(literal);
                bytes |= ((value)&0xffff); //little endian for payload in instruction
                this->currentSection->addData(5, bytes);
            }
            else
            {
                symbName = parser.getSymbol(inst, line);
                symbList = getSymbolFromSTable(symbName);
                if (!symbList.empty())
                {
                    symbolExists = symbList.back();

                    value = symbolExists->getValue();
                    if (symbolExists->getSectionName() != currentSection->getName() && symbolExists->getSectionName() != "__abs_data")
                    {
                        if (symbolExists->getType() == "global")
                        {
                            value = 0; //addend
                            this->currentSection->addToRelocTable(new RelocationSymbol(currentSection->getName(), "ABS", symbolExists->getSymbolIndex(), locationCounter - 2));
                        }
                        else
                        {
                            if (symbolExists->getSectionName() != "-" && symbolExists->getSectionName() != "__abs_data")
                            {
                                symbList = getSymbolFromSTable(symbolExists->getSectionName());
                                symbolExists = symbList.front();
                                symbList.pop_front();
                                this->currentSection->addToRelocTable(new RelocationSymbol(currentSection->getName(), "ABS", symbolExists->getSymbolIndex(), locationCounter - 2));
                            }
                            else
                                this->currentSection->addToRelocTable(new RelocationSymbol(currentSection->getName(), "ABS", 0, locationCounter - 2));
                        }
                    }
                    bytes |= ((value)&0xffff); //little endian for payload in instruction
                    this->currentSection->addData(5, bytes);
                }
            }
        }

        delete reg1;
        delete reg2;
        delete reg3;
        delete reg4;
        delete reg5;
        break;
    case Instruction::XCHG:
    case Instruction::ADD:
    case Instruction::SUB:
    case Instruction::MUL:
    case Instruction::DIV:
    case Instruction::CMP:
    case Instruction::NOT:
    case Instruction::AND:
    case Instruction::OR:
    case Instruction::XOR:
    case Instruction::TEST:
    case Instruction::SHL:
    case Instruction::SHR:
        // MMMM==0b0000: /* add */ reg[DDDD]<=(reg[DDDD] + reg[SSSS]);
        // MMMM==0b0001: /* sub */ reg[DDDD]<=(reg[DDDD] - reg[SSSS]);
        // MMMM==0b0010: /* mul */ reg[DDDD]<=(reg[DDDD] * reg[SSSS]);
        // MMMM==0b0011: /* div */ reg[DDDD]<=(reg[DDDD] / reg[SSSS]);
        // MMMM==0b0100: /* cmp */ temp<=(reg[DDDD] - reg[SSSS]); updt psw
        // MMMM==0b0001: /* and */ reg[DDDD]<=(reg[DDDD] & reg[SSSS]);
        // MMMM==0b0010: /* or */ reg[DDDD]<=(reg[DDDD] | reg[SSSS]);
        // MMMM==0b0011: /* xor */ reg[DDDD]<=(reg[DDDD] ^ reg[SSSS]);
        // MMMM==0b0100: /* test */ temp<=(reg[DDDD] & reg[SSSS]); updt psw
        // MMMM==0b0000: /* shl */ reg[DDDD]<=(reg[DDDD] << reg[SSSS]); updt psw;
        // MMMM==0b0001: /* shr */ reg[DDDD]<=(reg[DDDD] >> reg[SSSS]); updt psw;
        std::cout << "I: " << (int)i << '\n';
        if (i == Instruction::XCHG)
            bytes |= (0x60 << 8);
        else if (i == Instruction::ADD)
            bytes |= (0x70 << 8);
        else if (i == Instruction::SUB)
            bytes |= (0x71 << 8);
        else if (i == Instruction::MUL)
            bytes |= (0x72 << 8);
        else if (i == Instruction::DIV)
            bytes |= (0x73 << 8);
        else if (i == Instruction::CMP)
            bytes |= (0x74 << 8);
        else if (i == Instruction::NOT)
            bytes |= (0x80 << 8);
        else if (i == Instruction::AND)
            bytes |= (0x81 << 8);
        else if (i == Instruction::OR)
            bytes |= (0x82 << 8);
        else if (i == Instruction::XOR)
            bytes |= (0x83 << 8);
        else if (i == Instruction::TEST)
            bytes |= (0x84 << 8);
        else if (i == Instruction::SHL)
            bytes |= (0x90 << 8);
        else if (i == Instruction::SHR)
            bytes |= (0x91 << 8);

        if (i == Instruction::NOT)
        {
            rD = getRegisterNumber(line);
            std::cout << rD << std::endl;
            bytes |= rD << 4;
        }
        else
        {
            params = parser.getRegiserList(inst, line);
            while (!params.empty())
            {
                regD = params.front();
                regS = params.back();
                rS = getRegisterNumber(regS);
                rD = getRegisterNumber(regD);

                bytes |= rD << 4;
                bytes |= rS;
                params.pop_back();
                params.pop_front();
            }
        }
        locationCounter += 2;
        currentSection->addData(2, bytes);
        break;

    case Instruction::LDR:
    case Instruction::STR:
        if (i == Instruction::LDR)
        {
            bytes |= (0xA) << 20;
        }
        else if (i == Instruction::STR)
        {
            bytes |= (0xB) << 20;
        }
        params = parser.getParameterList(line);
        if (!params.empty())
        {
            regD = params.front();
            rD = getRegisterNumber(regD);
            bytes |= rD << 12;
            params.pop_front();
            std::cout << "rD: " << rD << std::endl;
        }
        if (!params.empty())
        {
            symbName = params.front();
            startIndex = symbName.find_first_not_of(" ");
            symbName = symbName.substr(startIndex);
            params.pop_front();
            std::cout << "symbolName: " << symbName << '\n';
        }
        reg1 = new std::regex("[ ]*[\\$\\%]?\\w+");                       // %/$ symbol/literal
        reg2 = new std::regex("[ ]*r[0-9][ ]*");                          // ldr/str rY, rX
        reg3 = new std::regex("[ ]*\\[r[0-9]\\]");                        // ldr/str rY, [rX]
        reg4 = new std::regex("[ ]*\\[[ ]*r[0-9][ ]*\\+[ ]*\\w+[ ]*\\]"); //  [rX + symbol/literal]
        reg5 = new std::regex("[ ]*\\w+");

        if (regex_search(line, m1, *reg4))
        {
            this->locationCounter += 5; //[rX + symbol/literal] takes up 2 bytes of memory
            std::cout << "reg4 " << m1.str() << '\n';
            params = parser.getBracketParameterList(symbName);
            if (!params.empty())
            {
                regS = params.front();
                params.pop_front();
            }
            else
            {
                std::cerr << "Error: invalid format of bracket parameters on line " + currentLine + '\n';
            }
            rS = getRegisterNumber(regS);
            bytes |= (rS << 8);
            bytes |= (0x05);
            bytes = bytes << 16;
            symbName = params.front();
            params.pop_front();
            startIndex = symbName.find_first_not_of(" ");
            symbName = symbName.substr(startIndex);
            std::cout << "Symb name: " << symbName << '\n';
            literal = parser.getLiterals(symbName);
            std::cout << "Literal: " << literal << '\n';
            if (literal != "err")
            {
                value = getNumber(literal);
                bytes |= ((value)&0xffff);
                currentSection->addData(5, bytes);
            }
            else
            {
                symbList = getSymbolFromSTable(symbName);
                if (!symbList.empty())
                {
                    symbolExists = symbList.back();

                    value = symbolExists->getValue();

                    if (symbolExists->getSectionName().compare(this->currentSection->getName()) != 0 && symbolExists->getSectionName().compare("__abs_data"))
                    {
                        if (symbolExists->getType() == "global")
                        {
                            value = 0;
                            this->currentSection->addToRelocTable(new RelocationSymbol(currentSection->getName(), "ABS", symbolExists->getSymbolIndex(), locationCounter - 2));
                        }
                        else
                        {
                            symbList = getSymbolFromSTable(symbolExists->getSectionName());
                            symbolExists = symbList.front();
                            std::cout << "Sec name: " << symbolExists->getName() << '\n';
                            symbList.pop_front();
                            this->currentSection->addToRelocTable(new RelocationSymbol(currentSection->getName(), "ABS", symbolExists->getSymbolIndex(), locationCounter - 2));
                        }
                    }
                    bytes |= (value)&0xffff;
                    std::cout << "value: " << value << "bytes: " << bytes << std::endl;
                    currentSection->addData(5, bytes);
                    std::cout << "Line: " << line << '\n'
                              << std::hex << bytes << '\n';
                }
            }
        }
        else if (regex_search(symbName, m1, *reg2)) //rDrS 0000 0001 - register direct
        {
            std::cout << "reg2: " << m1.str() << '\n';
            value = getRegisterNumber(symbName);
            bytes |= (value << 8); //rS
            bytes |= 1;            // 0000 0001 - register direct
            this->currentSection->addData(3, bytes);
            this->locationCounter += 3; //ldr/str rX takes up 3 bytes of memory
        }
        else if (regex_search(symbName, m1, *reg3))
        {
            std::cout << "reg3 " << m1.str() << '\n';
            value = getRegisterNumber(symbName);
            bytes |= (value << 8); //rS
            bytes |= 2;            //am
            this->currentSection->addData(3, bytes);
            this->locationCounter += 3; //call *[rX] takes up 2 bytes of memory
        }
        else if (regex_search(symbName, m1, *reg1))
        {
            std::cout << "reg1 " << m1.str() << '\n';

            this->locationCounter += 5;

            literal = parser.getLiterals(symbName);
            std::cout << literal << std::endl;
            if (literal != "err" && line.find_first_of("$") != std::string::npos)
            {
                bytes |= 0x00; //1111 rS 0000 0000
                bytes = bytes << 16;
                value = getNumber(literal);
                bytes |= ((value)&0xffff); //little endian for payload in instruction
                this->currentSection->addData(5, bytes);
            }
            else if (literal != "err")
            {
                bytes |= 0x04;
                bytes = bytes << 16;
                value = getNumber(literal);
                bytes |= ((value & 0xffff));
                this->currentSection->addData(5, bytes);
            }
            else
            {
                if (symbName.find_first_of("$") != std::string::npos)
                {
                    std::cout << "NADJEN $ \n";
                    rS = symbName.find_first_of('$');
                    symbName = symbName.substr(rS + 1);
                    type = "ABS";
                    bytes |= 0x00;
                }
                else if (symbName.find_first_of("%") != std::string::npos)
                {
                    rS = symbName.find_first_of("%");
                    symbName = symbName.substr(rS + 1);
                    type = "PC32_REL";
                    bytes |= 0x04;
                }
                else
                {
                    type = "ABS";
                    bytes |= 0x04;
                }
                bytes = bytes << 16;
                symbList = getSymbolFromSTable(symbName);
                if (!symbList.empty())
                {
                    symbolExists = symbList.back();
                    symbList.pop_back();
                    value = symbolExists->getValue();
                    std::cout << "symbName: " << symbolExists->getName() << std::endl;
                    bytes |= ((value)&0xffff); //little endian for payload in instruction
                    if (symbolExists->getSectionName() != currentSection->getName() && symbolExists->getSectionName() != "__abs_data")
                    {
                        if (symbolExists->getType() == "global")
                        {
                            bytes &= 0xffffff0000;
                            currentSection->addToRelocTable(new RelocationSymbol(this->currentSection->getName(), type, symbolExists->getSymbolIndex(), locationCounter - 2));
                        }
                        else if (symbolExists->getType() == "local")
                        {
                            if (symbolExists->getSectionName() != "-")
                            {

                                symbList = getSymbolFromSTable(symbolExists->getSectionName());
                                symbolExists = symbList.front();
                                symbList.pop_front();

                                currentSection->addToRelocTable(new RelocationSymbol(this->currentSection->getName(), type, symbolExists->getSymbolIndex(), locationCounter - 2));
                            }
                            else
                                currentSection->addToRelocTable(new RelocationSymbol(this->currentSection->getName(), type, 0, locationCounter - 2));
                        }
                    }

                    std::cout << "Line: " << line << '\n'
                              << std::hex << bytes << '\n';
                    this->currentSection->addData(5, bytes);
                }
            }
        }

        fflush(stdout);

        break;
    default:
        break;
    }
}

void Assembler::secondPass()
{
    std::string line;
    this->locationCounter = 0;
    this->currentSection->setName("-");
    this->currentSection->setSize(0);
    this->currentLine = 0;
    this->endFlag = false;
    inputFile->clear();
    inputFile->seekg(0);

    while (!inputFile->eof() && !endFlag)
    {
        this->currentLine++;
        getline(*inputFile, line);

        if (line.empty())
            continue; //Check if empty

        if (parser.isLineComment(line))
            continue; //Check if comment

        std::string isLabel = parser.isLineLabel(line); //Check if label
        if (isLabel != "notLabel")
        {
            //Second pass does notihg when encounters a label
            continue;
        }

        std::string isDirective = parser.isLineDirective(line); //Check if directive
        if (isDirective != "notDirective")
        {
            resolveDirectiveSP(isDirective, line);
            continue;
        }

        std::string instruction = parser.isLineInstruction(line); //Check if instruction;
        if (instruction != "notInst")
        {
            resolveInstructionSP(instruction, line);
            continue;
        }
    }
    this->printSymbolTable();

    for (auto it : sectionTable)
    {
        Section *sec = it;
        std::cout << sec << "\n\n";
        *outputFile << sec << '\n';
    }
    for (auto it : sectionTable)
    {
        Section *sec = it;
        std::cout << std::endl
                  << std::endl;
        sec->printRelocTable();

        *outputFile << "=========RELOCATION TABLE========\n";
        *outputFile << "|   OFFSET   |   TYPE   |  ID   |\n";
        *outputFile << "=================================\n";

        this->relocTable = sec->getRelocTable();
        for (auto it = relocTable.begin(); it != relocTable.end(); it++)
        {
            RelocationSymbol *symb = *it;
            *outputFile << *symb << "\n";
        };
        *outputFile << "=================================\n";
    }
    this->endFlag = false;
}

Assembler::~Assembler()
{
    for (auto it = symbolTable.begin(); it != symbolTable.end(); ++it)
    {
        Symbol *symb = *it;
        delete symb;
    }

    for (auto it = sectionTable.begin(); it != sectionTable.end(); ++it)
    {
        Section *sect = *it;
        delete sect;
    }

    for (auto it = relocTable.begin(); it != relocTable.end(); ++it)
    {
        RelocationSymbol *symb = *it;
        delete symb;
    }

    currentSection = nullptr;
}