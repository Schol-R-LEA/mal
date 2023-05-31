#include <iostream>
#include <memory>
#include <vector>
#include <cctype>
#include <cstdlib>
#include "reader.h"
#include "types.h"
#include "token_types.h"
#include "exceptions.h"


MalPtr Tokenizer::read_list(std::string input_stream)
{
    this->paren_count++;

    return std::make_shared<MalPair>(tokenize(input_stream));
}


void Tokenizer::close_list()
{
    if (this->paren_count > 0)
    {
        this->paren_count--;
    }
    else
    {
        throw UnbalancedParenthesesException();
    }
}


MalPtr Tokenizer::read_vector(std::string input_stream)
{
    this->square_bracket_count++;

    return std::make_shared<MalVector>(tokenize(input_stream));
}


void Tokenizer::close_vector()
{
    if (this->square_bracket_count > 0)
    {
        this->square_bracket_count--;
    }
    else
    {
        throw UnbalancedVectorException();
    }
}


MalPtr Tokenizer::read_hashmap(std::string input_stream)
{
    this->hm_count++;

    return std::make_shared<MalHashmap>(tokenize(input_stream));
}


void Tokenizer::close_hashmap()
{
    if (this->hm_count > 0)
    {
        this->hm_count--;
    }
    else
    {
        throw UnbalancedHashmapException();
    }
}