#include <iostream>
#include <memory>
#include <vector>
#include <cctype>
#include <cstdlib>
#include "reader.h"
#include "types.h"
#include "token_types.h"
#include "exceptions.h"


// MalPtr Tokenizer::read_list(std::string input_stream)
// {
//     this->paren_count++;

//     return std::make_shared<MalPair>(tokenize(input_stream));
// }

MalPtr Tokenizer::read_list(std::string input_stream)
{
    MalPtr head = tokenize(input_stream);
    if (head == nullptr)
    {
        return std::make_shared<MalPair>();
    }

    if (head->type() == MAL_RIGHT_PAREN)
    {
        return std::make_shared<MalPair>();
    }
    else if (head->type() == MAL_PERIOD)
    {
        auto last = tokenize(input_stream);
        // sanity check - is the next token a right parenthesis?
        auto next = tokenize(input_stream);
        if (next->type() != MAL_RIGHT_PAREN)
        {
            throw ImproperListException(next->to_str());
        }
        return last;
    }

    MalPtr tail = tokenize(input_stream);

    if (tail == nullptr)
    {
        return std::make_shared<MalPair>(head);
    }

    if (tail->type() == MAL_PERIOD)
    {
        auto last = tokenize(input_stream);
        MalPtr temp = std::make_shared<MalPair>(head, last);
        // sanity check - is the next token a right parenthesis?
        auto next = tokenize(input_stream);
        if (next->type() != MAL_RIGHT_PAREN)
        {
            throw ImproperListException(next->to_str());
        }
        return temp;
    }
    if (tail->type() == MAL_RIGHT_PAREN)
    {
        return std::make_shared<MalPair>(head);
    }
    else if (tail->type() == MAL_PAIR)
    {
        return std::make_shared<MalPair>(head, std::make_shared<MalPair>(tail));
    }
    else
    {
        return std::make_shared<MalPair>(head, std::make_shared<MalPair>(tail, read_list(input_stream)));
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