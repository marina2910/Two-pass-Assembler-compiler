#include "../inc/Parser.hpp"

Parser::Parser()
{
}

Parser::~Parser()
{
}

//Returns parameters from line that contains .directive
std::string Parser::getParameters(std::string line)
{
    std::size_t startIndex = line.find_first_not_of(" ");
    std::string toMatch = line.substr(startIndex);
    std::regex reg1("\\.(\\w+)[  ]*");
    std::smatch m1;
    std::string ret = "err";
    if (std::regex_search(toMatch, m1, reg1))
    {
        std::string literal = m1.suffix().str();
        return literal;
    }
    return "err";
}

std::list<std::string> Parser::getBracketParameterList(std::string line) {
    std::regex reg1 ("r[0-9][ ]*\\+[ ]*\\w+");
    std::smatch m1;
    std::list<std::string> paramList = std::list<std::string>();
    if(std::regex_search(line, m1, reg1)) {
        std::string params = m1.str();
        std::string delimiter = "+";
        size_t pos = 0;
        std::string token;

        while ((pos = params.find(delimiter)) != std::string::npos)
        {
            token = params.substr(0, pos);
            paramList.push_back(token);
            params.erase(0, pos + delimiter.length());
        }
        if (!params.empty())
            paramList.push_back(params);
    }
    return paramList;
}

// //Counts the number of lines in inputFile
// int Parser::countFileLines()
// {
//     int numLines = 0;
//     std::string line;

//     while (std::getline(*inputFile, line))
//         numLines++;
//     inputFile->clear();
//     inputFile->seekg(0);
//     return numLines;
// }

// //Returns a string* whose elements have line values from inputFile
// std::string **Parser::parseFile()
// {
//     int numLines = countFileLines();
//     std::cout << numLines << std::endl;
//     std::string **parsedLines = new std::string *[numLines];

//     std::string line;
//     int index = 0;

//     for (int i = 0; i < numLines; std::getline(*inputFile, line))
//     {
//         parsedLines[index++] = new std::string(line);
//         i++;
//     }

//     inputFile->clear();
//     inputFile->seekg(0);
//     return parsedLines;
// }

//Returns true if string passed as arg represents comment
bool Parser::isLineComment(std::string line)
{
    std::size_t startIndex = line.find_first_not_of(" ");
    if (line.std::string::at(startIndex) == *"#")
         return true;
    // startIndex = line->find("#");
    // if(startIndex != std::string::npos)
    //     *line = line->substr(0, line->length()-startIndex + 2);
    return false;
}

//Returns labelName if line contains label
std::string Parser::isLineLabel(std::string line)
{
    std::size_t startIndex = line.find_first_not_of(" ");
    std::string toMatch = line.substr(startIndex);
    std::regex reg1("\\w+:");
    std::smatch match;
    if (std::regex_search(toMatch, match, reg1))
    {
        return match.str();
    }
    return "notLabel";
}

//Returns parameter list 
std::list<std::string> Parser::getParameterList(std::string line)
{

    std::size_t startIndex = line.find_first_not_of(" ");
    std::string toMatch = line.substr(startIndex);
    std::regex reg1("(\\w+)[ ]*(,[ ]*(\\w)+[ ]*)?");
    std::smatch m1;
    std::list<std::string> paramList = std::list<std::string>();

    if (std::regex_search(toMatch, m1, reg1))
    {
        std::string params = m1.suffix().str();

        std::string delimiter = ",";
        size_t pos = 0;
        std::string token;

        while ((pos = params.find(delimiter)) != std::string::npos)
        {
            token = params.substr(0, pos);
            paramList.push_back(token);
            params.erase(0, pos + delimiter.length());
        }
        if (!params.empty())
            paramList.push_back(params);
    }
    return paramList;
}

std::list <std::string> Parser::getRegiserList(std::string inst, std::string line) {
    std::regex reg1("[ ]*r[0-9][ ]*\\,[ ]*r[0-9][ ]*");
    std::smatch m1;
    std::list<std::string> paramsList = std::list<std::string>();
    if (std::regex_search(line, m1, reg1))
    {
        std::string params = m1.str();
        std::string delimiter = ",";
        size_t pos = 0;
        std::string token;
        while ((pos = params.find(delimiter)) != std::string::npos)
        {
            token = params.substr(0, pos);
            token = trimRightSideSpace(token);
            paramsList.push_back(token);
            params.erase(0, pos + delimiter.length());
        }
        if (!params.empty())
            paramsList.push_back(params);
    }
    return paramsList;
}

//Returns true if string passed as argument represents directive
std::string Parser::isLineDirective(std::string line)
{
    std::size_t startIndex = line.find_first_not_of(" ");
    std::string toMatch = line.substr(startIndex);
    std::regex reg1("\\.(\\w+)[ ]*(,[ ]*(\\w)+[ ]*)?");
    std::smatch match;
    if (std::regex_search(toMatch, match, reg1))
    {
        return match.str();
    }
    return "notDirective";
}

//Returns the section name
std::string Parser::getSectionName(std::string line)
{
    std::string sectionName = getParameters(line);
    if (sectionName != "err")
    {
        std::regex reg2("\\.(\\w+)[ ]*(\\.\\w)+");
        std::smatch m2;
        if (std::regex_search(sectionName, m2, reg2))
        {
            sectionName = "err";
        }
        return sectionName;
    }
    return "notFound";
}

//Returns symbol name, first parameter in .EQU
std::string Parser::getEquSymbolName(std::string line)
{
    std::string ret = "err";
    std::string literal = getParameters(line);
    if (literal != "err")
    {
        std::regex reg2("(\\w+)([^\\,])");
        std::smatch m2;
        std::regex reg3("(\\w+)[,]{1}[ ]*(\\w+)");
        std::smatch m3;

        if (std::regex_search(literal, m3, reg3)) //Check if .equ has both symbol and literal
        {
            ret = m3.str();
            //Trim only symbol name from parameters list
            std::regex reg4("(\\w+)[,]{1}");
            std::smatch m4;
            std::regex_search(ret, m4, reg4);
            ret = m4.str();
            ret.resize(ret.size() - 1);
        }
        else if (std::regex_search(literal, m2, reg2))
        {
            ret = "paramerr";
        }
    }
    return ret;
}

//Returns literal, second parameter in .EQU directive
std::string Parser::getLiterals(std::string line)
{
    std::string ret = "err";
    std::regex reg2("\\s*([0-9]+)\\s*");
    std::regex reg3("\\s*0x([0-9abcdefABCDEF]+)\\s*");
    std::smatch m2;
    std::smatch m3;


    if (std::regex_search(line, m3, reg3))
    {
        ret = m3.str();
        fflush(stdout);
    }
    else if (std::regex_search(line, m2, reg2))
    {
        ret = m2.str();

    }

    return ret;
}

std::string Parser::getSymbol(std::string inst, std::string line)
{
    std::regex reg1(inst + "\\s*[\\*%]?\\w+");
    std::smatch m1;
    std::string ret = "err";
    if (std::regex_search(line, m1, reg1))
    {
        ret = m1.str();
        int i = ret.length() - 1; // last character
        while (i != 0 && !isspace(ret[i]))
        {
            --i;
        }
        ret = ret.substr(i + 1); // +1 to skip leading space
    }
    return ret;
}

std::string Parser::isLineInstruction(std::string line)
{
    //std::regex reg0("([A-Za-z]+)[   ]*(([*%$]*\\[?[0-9     \\w,\\.\\+]+\\]?)*)");
    std::string ret = "notInst";
    std::regex reg0("[^0][A-Za-z]+[ ]*");
    std::smatch m;
    if (regex_search(line, m, reg0))
    {
        std::string str = m.str();
        ret = str;
    }
    return ret;
}

bool Parser::resolveInstructionNoParams(std::string line, std::string inst)
{
    std::regex reg("\\s*" + inst + "{1}\\s*");
    std::smatch m;
    if (regex_search(line, m, reg))
    {
        return true;
    }
    return false;
}

std::string Parser::trimRightSideSpace(std::string line)
{
    //Removes any spacing at the end of a isDirective string
    line.erase(std::find_if(line.rbegin(), line.rend(), [](unsigned char ch)
                            { return !std::isspace(ch); })
                   .base(),
               line.end());
    return line;
}
