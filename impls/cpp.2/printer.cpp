#include <iostream>
#include <string>
#include "types.h"
#include "printer.h"
#include "exceptions.h"


std::string pr_str(Reader tokens, bool print_readably)
{
    std::string s = "";

    auto token = tokens.next();
    if (token != nullptr)
    {
        if (token->type() != MAL_COMMA)
        {
            s += token->to_str(print_readably);
        }
    }
    else
    {
        s += "()";
    }

    return s;
}
