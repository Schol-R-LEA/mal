#include <iostream>
#include <memory>
#include <vector>
#include <cctype>
#include <cstdlib>
#include "includes/exceptions.h"
#include "includes/types.h"
#include "reader.h"
#include "types.h"
#include "token_types.h"
#include "exceptions.h"



MalPtr Tokenizer::read_list(std::string input_stream)
{
    paren_count++;
    PairPtr result = std::make_shared<MalPair>();

    MalPtr head = tokenize(input_stream);
    if (head == nullptr || head->type() == MAL_RIGHT_PAREN)
    {
        return result;
    }

    if (head->type() == MAL_PERIOD)
    {
        throw ImproperListException(input_stream);
    }


    result->set_car(head);

    auto tail = tokenize(input_stream);
    if (tail == nullptr)
    {
        return result;
    }
    while (true)
    {
        if (tail->type() == MAL_RIGHT_PAREN)
        {
            return result;
        }
        if (tail->type() == MAL_PERIOD)
        {
            auto next = tokenize(input_stream);

            if (next->type() == MAL_RIGHT_PAREN)
                // sanity check - is the next token a right parenthesis?
            {
                throw ImproperListException(input_stream);
            }
            else
            {
                result->set_last(next);
                auto last = tokenize(input_stream);
                if (last->type() != MAL_RIGHT_PAREN)
                {
                    throw ImproperListException(last->to_str());
                }
            }
            return result;
        }
        else
        {
            result->add(tail);
            auto next = tokenize(input_stream);
            if (next == nullptr || (next->type() == MAL_RIGHT_PAREN))
            {
                return result;
            }
            else
            {
                tail = next;
            }
        }
    }

    return result;
}



MalPtr Tokenizer::close_list()
{
    if (this->paren_count > 0)
    {
        this->paren_count--;
        return std::make_shared<MalRightParen>();
    }
    else
    {
        throw UnbalancedVectorException();
    }
}


MalPtr Tokenizer::read_vector(std::string input_stream)
{
    this->square_bracket_count++;

    VecPtr vec = std::make_shared<MalVector>();

    for (auto token = tokenize(input_stream);
         token != nullptr && token->type() != MAL_RIGHT_BRACKET;
         token = tokenize(input_stream))
    {
        vec->add(token);
    }

    return vec;
}


MalPtr Tokenizer::close_vector()
{
    if (this->square_bracket_count > 0)
    {
        this->square_bracket_count--;
        return std::make_shared<MalRightBracket>();
    }
    else
    {
        throw UnbalancedVectorException();
    }
}


MalPtr Tokenizer::read_hashmap(std::string input_stream)
{
    this->hm_count++;

    MapPtr hm = std::make_shared<MalHashmap>();

    for (auto key = tokenize(input_stream), value = tokenize(input_stream);
         key != nullptr && key->type() != MAL_RIGHT_BRACE;
         key = tokenize(input_stream), value = tokenize(input_stream))
    {
        if (key->type() != MAL_STRING && key->type() != MAL_KEYWORD)
        {
            throw InvalidHashmapException();
        }
        if (value == nullptr
            || value->type() == MAL_RIGHT_BRACE
            || value->type() == MAL_RIGHT_PAREN
            || value->type() == MAL_RIGHT_BRACKET)
        {
            throw UnbalancedHashmapException();
        }
        hm->add(key, value);
    }

    return hm;
}


MalPtr Tokenizer::close_hashmap()
{
    if (this->hm_count > 0)
    {
        this->hm_count--;
        return  std::make_shared<MalRightBrace>();
    }
    else
    {
        throw UnbalancedHashmapException();
    }
}
