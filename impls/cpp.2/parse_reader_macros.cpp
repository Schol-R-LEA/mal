
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
    MalPtr seq_argument, main_argument;

    seq_argument = tokenize(input_stream);

    if (s_index >= input_stream.length())
    {
        throw InvalidMetaException();
    }

    main_argument = tokenize(input_stream);

    return std::make_shared<MalMeta>(main_argument, seq_argument);
}
