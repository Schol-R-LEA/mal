
#ifndef PARSE_READER_MACROS_H
#define PARSE_READER_MACROS_H

#include <iostream>
#include <memory>
#include <vector>
#include <cctype>
#include <cstdlib>
#include "reader.h"
#include "types.h"


template<class RM> PairPtr read_reader_macro(std::string input_stream)
{
    char ch = input_stream[s_index++];
    PairPtr rm_argument;

    if (is_left_balanced(ch))
    {
        switch(ch)
        {
            case '(':
                rm_argument = read_list(input_stream);
                break;
            case '[':
                rm_argument = read_vector(input_stream);
                break;
            case '{':
                rm_argument = read_hashmap(input_stream);
                break;
            case '\"':
                rm_argument = read_string(input_stream);
                break;
        }
    }
    else if (isdigit(ch))
    {
        rm_argument = read_number(input_stream, ch);
    }
    else
    {
        rm_argument = read_symbol(input_stream, ch);
    }

    RM result(rm_argument);

    return result;
}



PairPtr read_unquote(std::string input_stream);
PairPtr read_meta(std::string input_stream);

#endif