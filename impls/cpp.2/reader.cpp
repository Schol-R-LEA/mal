#include <iostream>
#include <memory>
#include <vector>
#include <cctype>
#include <cstdlib>
#include "reader.h"
#include "types.h"
#include "token_types.h"
#include "exceptions.h"
#include "parse_numbers.h"
#include "parse_sequences.h"
#include "parse_reader_macros.h"


unsigned int paren_count = 0;
unsigned int square_bracket_count = 0;
unsigned int hm_count = 0;
unsigned int s_index = 0;


Reader read_str(std::string s)
{
    paren_count = 0;
    square_bracket_count = 0;
    hm_count = 0;
    s_index = 0;
    PairPtr tokens = tokenize(s);

    if (paren_count > 0)
    {
        throw UnbalancedParenthesesException();
    }
    if (square_bracket_count > 0)
    {
        throw UnbalancedVectorException();
    }
    if (hm_count > 0)
    {
        throw UnbalancedHashmapException();
    }

    return Reader(tokens);
}



PairPtr tokenize(std::string input_stream)
{
    char ch;

    if (s_index < input_stream.length())
    {
        std::string s = "";

        ch = input_stream[s_index++];

        if (isspace(ch))
        {
            return MalPair(read_whitespace(input_stream, ch), tokenize(input_stream));
        }
        else
        {
            switch (ch)
            {
                case ';':
                    read_comment(input_stream);
                    return tokenize(input_stream);
                    break;
                case '(':
                    return MalPair(read_list(input_stream), tokenize(input_stream));
                    break;
                case ')':
                    close_list();
                    break;
                case '[':
                    return MalPair(std::make_shared<MalVector>(read_vector(input_stream), tokenize(input_stream)));
                    break;
                case ']':
                    close_vector();
                    break;
                case '{':
                    return MalPair(std::make_shared<MalVector>(read_hashmap(input_stream), tokenize(input_stream)));
                    break;
                case '}':
                    close_hashmap();
                    break;
                case '&':
                    return MalPair(std::make_shared<MalRestArg>(), tokenize(input_stream));
                    break;
                case '.':
                    return MalPair(std::make_shared<MalPeriod>(), tokenize(input_stream));
                    break;
                case ',':
                    return MalPair(std::make_shared<MalComma>(), tokenize(input_stream));
                    break;
                case '\'':
                    return MalPair(std::make_shared<MalQuote>(read_reader_macro<MalQuote>(input_stream), tokenize(input_stream)));
                    break;
                case '`':
                    read_reader_macro<MalQuasiquote>(input_stream), tokenize(input_stream)));
                    break;
                case '~':
                    return MalPair(std::make_shared<MalUnquote>(read_unquote(input_stream), tokenize(input_stream)));
                    break;
                case '@':
                    return MalPair(std::make_shared<MalDeref>(read_reader_macro<MalDeref>(input_stream), tokenize(input_stream)));
                    break;
                case '^':
                    return MalPair(std::make_shared<MalDeref>(read_meta(input_stream), tokenize(input_stream)));
                    break;
                case '\"':
                    return MalPair(std::make_shared<MalDeref>(read_string(input_stream), tokenize(input_stream)));
                    break;
                case '-':
                    if (isdigit(input_stream[s_index]))
                    {
                        return MalPair(std::make_shared<MalDeref>(read_number(input_stream, ch), tokenize(input_stream)));
                    }
                    else
                    {
                        return MalPair(std::make_shared<MalDeref>(read_symbol(input_stream, ch), tokenize(input_stream)));
                    }
                    break;
                default:
                    if (isdigit(ch))
                    {
                        return MalPair(std::make_shared<MalNumber>(read_number(input_stream, ch), tokenize(input_stream)));
                    }
                    else
                    {
                        return MalPair(std::make_shared<MalObject>(read_symbol(input_stream, ch), tokenize(input_stream)));
                    }
            }
        }
    }
    return nullptr;
}



PairPtr read_form(std::string input_stream)
{
    return tokenize(input_stream);
}



void read_whitespace(std::string input_stream, char leading)
{
    char ch = leading;
    if (!isspace(ch)) return;

    while (isspace(ch) && s_index < input_stream.length())
    {
        ch = input_stream[s_index++];
    }
    if (s_index < input_stream.length())
    {
        s_index--;
    }
}


void read_comment(std::string input_stream)
{
    for (char ch = input_stream[s_index]; ch != '\n' && s_index < input_stream.length(); s_index++)
    {
        ch = input_stream[s_index];
    }
}


MalString read_string(std::string input_stream)
{
    std::string s = "";
    char ch;

    ch = input_stream[s_index++];

    while ((ch != '\"') && s_index < input_stream.length())
    {
        if ((ch == '\\' ) && s_index < input_stream.length())
        {
            ch = input_stream[s_index++];
            if (s_index == input_stream.length())
            {
                throw IncompleteEscapeException();
            }

            if (ch == '\"' || ch == '\\')
            {
                s += ch;
            }
            else if (ch == 'n')
            {
                s += '\n';
            }
            else if (ch == 't')
            {
                s += '\t';
            }
            else
            {
                throw UnbalancedStringException();
            }
        }

        else
        {
            s += ch;
        }
        ch = input_stream[s_index++];
    }

    if (ch != '\"')
    {
        throw UnbalancedStringException();
    }

    return MalString(s);
}



MalObject read_symbol(std::string input_stream, char leading)
{
    std::string s = "";

    char ch = leading;

    while ((!isspace(ch) && !is_syntax(ch)) && s_index < input_stream.length())
    {
        s += ch;
        ch = input_stream[s_index++];
    }
    if (s_index < input_stream.length() || is_right_balanced(ch))
    {
        s_index--;
    }
    else
    {
        s += ch;
    }

    if (s == "nil")
    {
        return MalPair(nullptr, nullptr);
    }
    else if (s == "true")
    {
        return MalBoolean(true);
    }
        else if (s == "false")
    {
        return MalBoolean(false);
    }
    else if (s[0] == ':')
    {
        return MalKeyword(s.substr(1));
    }
    else
    {
        return MalSymbol(s);
    }
}

