/* The following code applies the GNU Readline library, which is
   licensed under the GPL version 3.0. Please refer to the file
   'LICENSE' in the implementation subdirectory.
*/


#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>
#include "lineedit.h"
#include "reader.h"
#include "printer.h"
#include "exceptions.h"


Reader READ(std::string input)
{
    return read_str(input);
}


Reader EVAL(Reader input)
{
    return input;
}


std::string PRINT(Reader input)
{
    return pr_str(input, true);
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
            std::cout << rep(input) << '\n';
        }
        catch (InvalidConversionException& e)
        {
            std::cout << e.what() << '\n';
        }
        catch(UnbalancedParenthesesException& e)
        {
            std::cout << "(EOF|end of input|unbalanced)." << '\n';
        }
        catch (InvalidTokenStreamException& e)
        {
            std::cout << "(no tokens to print.)" << '\n';
        }
        catch(UnbalancedVectorException& e)
        {
            std::cout << "(EOF|end of input|unbalanced)." << '\n';
        }
        catch(UnbalancedStringException& e)
        {
            std::cout << "(EOF|end of input|unbalanced)." << '\n';
        }
        catch(UnbalancedHashmapException& e)
        {
            std::cout << "(EOF|end of input|unbalanced)."  << '\n';
        }
        catch(IncompleteComplexNumberException& e)
        {
            std::cout << "(EOF|end of input|unbalanced)."  << '\n';
        }
        catch(IncompleteEscapeException& e)
        {
            std::cout << "(EOF|end of input|unbalanced)."  << '\n';
        }
        catch(InvalidNumberException& e)
        {
            std::cout << "(invalid number): " << e.what() << "." << '\n';
        }
        catch(InvalidHashmapException& e)
        {
            std::cout << "(invalid hash map)." << '\n';
        }

        catch(InvalidMetaException& e)
        {
            std::cout << "(invalid meta expression)." << '\n';
        }

        catch(IndexOutOfBoundsException& e)
        {
            std::cout << "(index out of bounds): " << e.what() << '\n';
        }

        catch(ImproperListException& e)
        {
            std::cout << "(list function applied to improper list): " << e.what() << '\n';
        }


        // backstop any and all other possible exceptions
        catch(std::exception& e)
        {
            std::cout << e.what() << '\n';
        }
    }
    std::cout << "Exiting.\n";

    return 0;
}