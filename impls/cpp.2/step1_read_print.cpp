#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>
#include "lineedit.h"
#include "token.h"
#include "reader.h"
#include "printer.h"
#include "exceptions.h"

TokenVector READ(std::string input)
{
    return read_str(input);
}


TokenVector EVAL(TokenVector input)
{
    return input;
}


std::string PRINT(TokenVector input)
{
    pr_str(input);
    return input.values();
}


std::string rep(std::string input)
{
    return PRINT(EVAL(READ(input)));
}


int main()
{
    LineEdit line;

    while (true)
    {
        std::string input;
        try
        {
            input = line.getline("user> ");
        }
        catch(EndOfInputException* e)
        {
            break;
        }

        try
        {
            rep(input);
        }
        catch(UnbalancedParenthesesException* e)
        {
            std::cout << "(EOF|end of input|unbalanced)." << '\n';
        }
        catch(UnbalancedVectorException* e)
        {
            std::cout << "(EOF|end of input|unbalanced)." << '\n';
        }
        catch(UnbalancedStringException* e)
        {
            std::cout << "(EOF|end of input|unbalanced)." << '\n';
        }
        catch(UnbalancedHashmapException* e)
        {
            std::cout << "(EOF|end of input|unbalanced)."  << '\n';
        }
        catch(IncompleteComplexNumberException* e)
        {
            std::cout << "(EOF|end of input|unbalanced)."  << '\n';
        }
        catch(IncompleteEscapeException* e)
        {
            std::cout << "(EOF|end of input|unbalanced)."  << '\n';
        }
        catch(InvalidNumberException* e)
        {
            std::cout << "(invalid number)." << '\n';
        }
        catch(InvalidHashmapException* e)
        {
            std::cout << "(invalid hash map)." << '\n';
        }
    }
    std::cout << "Exiting.\n";

    return 0;
}