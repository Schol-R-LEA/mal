#include <iostream>
#include <memory>
#include <vector>
#include <cctype>
#include <cstdlib>
#include "reader.h"
#include "types.h"
#include "exceptions.h"


MalPtr Reader::peek()
 {
    if (m_current_token != nullptr && m_current_token->type() == MAL_PAIR)
    {
        return m_current_token->as_pair()->car();
    }
    else
    {
        return m_current_token;
    }
 }


MalPtr Reader::next()
 {
    MalPtr result = peek();
    if (m_current_token != nullptr)
    {
        if (m_current_token->type() == MAL_PAIR)
        {
            m_current_token = m_current_token->as_pair()->cdr();
        }
        else
        {
            m_current_token = nullptr;
        }
    }
    return result;
 }


Reader read_str(std::string s)
{
    Tokenizer tokenizer;
    PairPtr tokens = tokenizer.tokenize(s);

    if (!tokenizer.balanced_lists())
    {
        throw UnbalancedParenthesesException();
    }
    if (!tokenizer.balanced_vectors())
    {
        throw UnbalancedVectorException();
    }
    if (!tokenizer.balanced_hashmaps())
    {
        throw UnbalancedHashmapException();
    }
    if (tokens == nullptr)
    {
        throw InvalidTokenStreamException();
    }

    return Reader(tokens);
}


