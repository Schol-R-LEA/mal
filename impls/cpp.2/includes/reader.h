#ifndef READER_H
#define READER_H


#include <string>
#include <memory>
#include <vector>
#include "lineedit.h"
#include "types.h"
#include "token_types.h"


class Reader
{
public:
    Reader(PairPtr tokens): m_current_token(tokens), m_tokens(tokens) {};
    MalPtr peek();
    MalPtr next();

private:
    MalPtr m_current_token;
    PairPtr m_tokens;
};


Reader read_str(std::string input_stream);


class Tokenizer
{
public:
    Tokenizer(): paren_count(0), square_bracket_count(0), hm_count(0), s_index(0) {};
    PairPtr tokenize(std::string input_stream);
    bool balanced_lists() {return (paren_count == 0);};
    bool balanced_vectors() {return (square_bracket_count == 0);};
    bool balanced_hashmaps() {return (hm_count == 0);};

    MalPtr read_form(std::string input_stream);

    void read_whitespace(std::string input_stream, char leading);
    void read_comment(std::string input_stream);

    MalPtr read_string(std::string input_stream);
    MalPtr read_symbol(std::string input_stream, char leading);

    MalPtr read_number(std::string input_stream, char leading);
    MalPtr read_based_integer(std::string input_stream);
    MalPtr read_binary(std::string input_stream);
    MalPtr read_octal(std::string input_stream, char leading);
    MalPtr read_hex(std::string input_stream);
    MalPtr read_trailing_zeroes(std::string input_stream);
    MalPtr read_decimal(std::string input_stream, char leading);
    MalPtr read_fractional(std::string input_stream, std::string leading);
    MalPtr read_rational(std::string input_stream, std::string leading);
    MalPtr read_complex(std::string input_stream, std::string leading, char trailing);

    MalPtr read_list(std::string input_stream);
    void close_list();
    MalPtr read_vector(std::string input_stream);
    void close_vector();
    MalPtr read_hashmap(std::string input_stream);
    void close_hashmap();

    MalPtr read_unquote(std::string input_stream);
    MalPtr read_meta(std::string input_stream);

    template<class RM> MalPtr read_reader_macro(std::string input_stream)
    {
        char ch = input_stream[s_index++];
        MalPtr rm_argument;

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

        return std::make_shared<MalReaderMacro>(result);
    };

private:
    unsigned int paren_count;
    unsigned int square_bracket_count;
    unsigned int hm_count;
    unsigned int s_index;
};




#endif