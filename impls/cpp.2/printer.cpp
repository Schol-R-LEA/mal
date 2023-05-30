#include <iostream>
#include <string>
#include "types.h"
#include "printer.h"
#include "exceptions.h"


std::string pr_str(Reader tokens, bool print_readably)
{
    std::string s = "";

    for (auto token = tokens.next(); token != nullptr; token = tokens.next())
    {
        std::cout << mal_type_name[token->type()] << ", " << token->to_str() << std::endl;
        s += token->to_str(print_readably);
    }

    return s;
}
