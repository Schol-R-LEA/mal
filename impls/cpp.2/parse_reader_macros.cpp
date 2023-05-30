
#include <iostream>
#include <memory>
#include "reader.h"
#include "types.h"
#include "token_types.h"
#include "exceptions.h"



MalPtr Tokenizer::read_unquote(std::string input_stream)
{
    char ch = input_stream[s_index++];
    if (ch == '@')
    {
        return read_reader_macro<MalSpliceUnquote>(input_stream);
    }
    else
    {
        s_index--;
        return read_reader_macro<MalUnquote>(input_stream);
    }
}


MalPtr Tokenizer::read_meta(std::string input_stream)
{
    char ch = input_stream[s_index++];
    MalPtr seq_argument, main_argument;

    if (is_left_balanced(ch))
    {
        switch(ch)
        {
            case '(':
                seq_argument = read_list(input_stream);
                break;
            case '[':
                seq_argument = read_vector(input_stream);
                break;
            case '{':
                seq_argument = read_hashmap(input_stream);
                break;
            case '\"':
                seq_argument = read_string(input_stream);
                break;
        }

        read_whitespace(input_stream, ch);
        if (s_index >= input_stream.length())
        {
            throw new InvalidMetaException();
        }

        s_index++;
        ch = input_stream[s_index++];

        if (is_left_balanced(ch))
        {
            switch(ch)
            {
                case '(':
                    main_argument = read_list(input_stream);
                    break;
                case '[':
                    main_argument = read_vector(input_stream);
                    break;
                case '{':
                    main_argument = read_hashmap(input_stream);
                    break;
                case '\"':
                    main_argument = read_string(input_stream);
                    break;
            }
        }
        else if (isdigit(ch))
        {
            main_argument = read_number(input_stream, ch);
        }
        else if (!is_syntax(ch))
        {
            main_argument = read_symbol(input_stream, ch);
        }
        else
        {
            throw new InvalidMetaException();
        }
    }
    else
    {
        throw new InvalidMetaException();
    }

    return std::make_shared<MalMeta>(main_argument, seq_argument);
}
