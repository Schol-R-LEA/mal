#include <iostream>
#include <memory>
#include <vector>
#include <cctype>
#include <cstdlib>
#include "reader.h"
#include "types.h"
#include "token_types.h"
#include "exceptions.h"


PairPtr Tokenizer::tokenize(std::string input_stream)
{
    char ch;
    MalPtr handle = nullptr;

    if (s_index < input_stream.length())
    {
        std::string s = "";

        ch = input_stream[s_index++];

        if (isspace(ch))
        {
            handle = tokenize(input_stream);
        }
        else
        {
            switch (ch)
            {
                case ';':
                    read_comment(input_stream);
                    handle = tokenize(input_stream);
                    break;
                case '(':
                    handle = read_list(input_stream);
                    break;
                case ')':
                    handle = std::make_shared<MalRightParen>();
                    break;
                case '[':
                    handle = read_vector(input_stream);
                    break;
                case ']':
                    handle = std::make_shared<MalRightBracket>();
                    break;
                case '{':
                    handle = read_hashmap(input_stream);
                    break;
                case '}':
                    handle = std::make_shared<MalRightBrace>();
                    break;
                case '&':
                    handle = std::make_shared<MalRestArg>();
                    break;
                case '.':
                    handle = std::make_shared<MalPeriod>();
                    break;
                case ',':
                    handle = std::make_shared<MalComma>();
                    break;
                case '\'':
                    handle = read_reader_macro<MalQuote>(input_stream);
                    break;
                case '`':
                    handle = read_reader_macro<MalQuasiquote>(input_stream);
                    break;
                case '~':
                    handle = read_unquote(input_stream);
                    break;
                case '@':
                    handle = read_reader_macro<MalDeref>(input_stream);
                    break;
                case '^':
                    handle = read_meta(input_stream);
                    break;
                case '\"':
                    handle = read_string(input_stream);
                    break;
                case '-':
                case '+':
                    if (isdigit(input_stream[s_index]))
                    {
                        handle = read_number(input_stream, ch);
                    }
                    else
                    {
                        handle = read_symbol(input_stream, ch);
                    }
                    break;
                default:
                    if (isdigit(ch))
                    {
                        handle = read_number(input_stream, ch);
                    }
                    else
                    {
                        handle = read_symbol(input_stream, ch);
                    }
            }
        }

        if (handle != nullptr)
        {
            MalPtr result;

            if (handle->type() == MAL_PAIR)
            {
                result = handle;
            }
            else
            {
                result = std::make_shared<MalPair>(handle, tokenize(input_stream));
            }

            return result->as_pair();
        }
        else
        {
            return nullptr;
        }
    }
    else
    {
        return nullptr;
    }
}



MalPtr Tokenizer::read_form(std::string input_stream)
{
    return tokenize(input_stream);
}



void Tokenizer::read_comment(std::string input_stream)
{
    for (char ch = input_stream[s_index]; ch != '\n' && s_index < input_stream.length(); s_index++)
    {
        ch = input_stream[s_index];
    }
}


MalPtr Tokenizer::read_string(std::string input_stream)
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

    return std::make_shared<MalString>(s);
}



MalPtr Tokenizer::read_symbol(std::string input_stream, char leading)
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
        return std::make_shared<MalPair>(nullptr, nullptr);
    }
    else if (s == "true")
    {
        return std::make_shared<MalBoolean>(true);
    }
        else if (s == "false")
    {
        return std::make_shared<MalBoolean>(false);
    }
    else if (s[0] == ':')
    {
        return std::make_shared<MalKeyword>(s.substr(1));
    }
    else
    {
        return std::make_shared<MalSymbol>(s);
    }
}

