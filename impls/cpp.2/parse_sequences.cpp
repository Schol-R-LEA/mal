#include <iostream>
#include <memory>
#include <vector>
#include <cctype>
#include <cstdlib>
#include "reader.h"
#include "types.h"
#include "token_types.h"
#include "exceptions.h"
#include "parse_sequences.h"


MalPtr read_list(std::string input_stream)
{
    paren_count++;
    return tokenize(input_stream);
}


void close_list()
{
    
    if (paren_count > 0)
    {
        std::cout << "closing list" << std::endl;
        paren_count--;
    }
    else
    {
        throw UnbalancedParenthesesException();
    }
}


MalPtr read_vector(std::string input_stream)
{
    square_bracket_count++;

    return std::make_shared<MalVector>(tokenize(input_stream));
}


void close_vector()
{
    if (square_bracket_count > 0)
    {
        square_bracket_count--;
    }
    else
    {
        throw UnbalancedVectorException();
    }
}


MalPtr read_hashmap(std::string input_stream)
{
    hm_count++;

    return std::make_shared<MalHashmap>(tokenize(input_stream));
}


void close_hashmap()
{
    if (hm_count > 0)
    {
        hm_count--;
    }
    else
    {
        throw UnbalancedHashmapException();
    }
}