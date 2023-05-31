#include <iostream>
#include <memory>
#include <vector>
#include <cctype>
#include <cstdlib>
#include "reader.h"
#include "types.h"
#include "token_types.h"
#include "exceptions.h"



MalPtr Tokenizer::read_number(std::string input_stream, char leading)
{
    std::string s;
    char ch = leading;

    if (ch == '0')
    {
        if (input_stream[s_index]  == '.')
        {
            s_index++;
            return read_fractional(input_stream, "0.");
        }
        else
        {
            return read_based_integer(input_stream);
        }
    }
    else
    {
        return read_decimal(input_stream, ch);
    }
}


MalPtr Tokenizer::read_based_integer(std::string input_stream)
{
    std::string s = "0";
    char ch;

    ch = input_stream[s_index++];
    switch (ch)
    {
        case 'x':
            return read_hex(input_stream);
            break;

        case 'b':
            return read_binary(input_stream);
            break;

        case '0':
            return read_trailing_zeroes(input_stream);
            break;

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
            return read_octal(input_stream, ch);
            break;
        case ' ':
        case ')':
        case ']':
        case '}':
            return std::make_shared<MalInteger>(mpz_class(s));
            s_index--;
            break;
        default:
            if (s_index >= input_stream.length())
            {
                return std::make_shared<MalInteger>(mpz_class(s));
            }
            else
            {
                throw InvalidNumberException(s);
            }
    }

    return nullptr;
}


MalPtr Tokenizer::read_trailing_zeroes(std::string input_stream)
{
    std::string s = "00";
    char ch = '0';
    s += ch;

    while (ch == '0' && s_index < input_stream.length())
    {
        s += ch;
        ch = input_stream[s_index++];
    }
    if (!(ch == '0' || isspace(ch) || is_right_balanced(ch)))
    {
        throw InvalidNumberException(s + ch);
    }
    else if (s_index < input_stream.length())
    {
        s_index--;
    }
    else
    {
        s += ch;
    }

    return std::make_shared<MalInteger>(0);
}



MalPtr Tokenizer::read_binary(std::string input_stream)
{
    std::string s = "0b";
    char ch = input_stream[s_index++];

    while (is_binary(ch) && s_index < input_stream.length())
    {
        s += ch;
        ch = input_stream[s_index++];
    }

    if (!(is_binary(ch) || isspace(ch) || is_right_balanced(ch)))
    {
        throw InvalidBinaryNumberException(s + ch);
    }
    else if (s_index < input_stream.length())
    {
        s_index--;
    }
    else
    {
        s += ch;
    }

    return std::make_shared<MalInteger>(mpz_class(s.substr(2), 2));
}


MalPtr Tokenizer::read_octal(std::string input_stream, char leading)
{
    std::string s = "0";
    char ch = leading;

    while (is_octal(ch) && s_index < input_stream.length())
    {
        s += ch;
        ch = input_stream[s_index++];
    }

    if (!(is_octal(ch) || isspace(ch) || is_right_balanced(ch)))
    {
        throw InvalidOctalNumberException(s + ch);
    }
    else if (s_index < input_stream.length())
    {
        s_index--;
    }
    else
    {
        s += ch;
    }

    return std::make_shared<MalInteger>(mpz_class(s, 8));
}


MalPtr Tokenizer::read_hex(std::string input_stream)
{
    std::string s = "0x";
    char ch = input_stream[s_index++];

    while (is_hex(ch) && s_index < input_stream.length())
    {
        s += ch;
        ch = input_stream[s_index++];
    }

    if (!(is_hex(ch) || isspace(ch) || is_right_balanced(ch)))
    {
        throw InvalidHexNumberException(s + ch);
    }
    else if (s_index < input_stream.length())
    {
        s_index--;
    }
    else
    {
        s += ch;
    }

    return std::make_shared<MalInteger>(mpz_class(s.substr(2), 16));
}


MalPtr Tokenizer::read_decimal(std::string input_stream, char leading)
{
    std::string s = "";
    char ch = leading;

    if (ch == '-')
    {
        s += ch;
        ch = input_stream[s_index++];
    }
    else if (ch == '+')
    {
        ch = input_stream[s_index++];
    }

    while (isdigit(ch) && s_index < input_stream.length())
    {
        s += ch;
        ch = input_stream[s_index++];
        if (!isdigit(ch))
        {
            switch(ch)
            {
                case '.':
                    return read_fractional(input_stream, s);
                case '/':
                    return read_rational(input_stream, s);
                case '-':
                case '+':
                    return read_complex(input_stream, s, ch);
                default:
                    break;
            }
        }
    }

    if (!(isdigit(ch) || isspace(ch) || is_right_balanced(ch)  || ch == ','  || ch == ';'))
    {
        throw InvalidNumberException(s + ch);
    }
    else if (s_index < input_stream.length() || isspace(ch) || is_right_balanced(ch) || ch == ','  || ch == ';')
    {
        s_index--;
    }
    else
    {
        s += ch;
    }

    return std::make_shared<MalInteger>(mpz_class(s));
}


MalPtr Tokenizer::read_fractional(std::string input_stream, std::string leading)
{
    std::string s = leading + '.';
    char ch = input_stream[s_index++];

    while (isdigit(ch) && (s_index < input_stream.length()))
    {
        s += ch;
        ch = input_stream[s_index++];

        if (ch == '+' || ch == '-')
        {
            return read_complex(input_stream, s, ch);
        }
    }

    if (!(isdigit(ch) || isspace(ch) || is_right_balanced(ch)  || ch == ','  || ch == ';'))
    {
        throw InvalidNumberException(s + ch);
    }
    else if (s_index < input_stream.length() || isspace(ch) || is_right_balanced(ch) || ch == ',' || ch == ';')
    {
        s_index--;
    }
    else
    {
        s += ch;
    }

    return std::make_shared<MalFractional>(mpf_class(s));
}


MalPtr Tokenizer::read_rational(std::string input_stream, std::string leading)
{
    std::string s = leading;
    char ch = '/';

    s += ch;
    ch = input_stream[s_index++];
    while (isdigit(ch) && s_index < input_stream.length())
    {
        s += ch;
        ch = input_stream[s_index++];
    }

    if (!(isdigit(ch) || isspace(ch) || is_right_balanced(ch)  || ch == ','  || ch == ';'))
    {
        throw InvalidNumberException(s + ch);
    }
    else if (s_index < input_stream.length() || isspace(ch) || is_right_balanced(ch) || ch == ',')
    {
        s_index--;
    }
    else
    {
        s += ch;
    }

    return std::make_shared<MalRational>(mpq_class (s));
}



MalPtr Tokenizer::read_complex(std::string input_stream, std::string leading, char trailing)
{
    std::string s = "";
    char ch = trailing;

    if (ch == '-')
    {
        s += ch;
    }
    ch = input_stream[s_index++];
    while (isdigit(ch) && s_index < input_stream.length())
    {
        bool decimal_point_found = false;
        s += ch;
        ch = input_stream[s_index++];
        if (ch == '.')
        {
            if (!decimal_point_found)
            {
                decimal_point_found = true;
                s += ch;
                ch = input_stream[s_index++];
            }
            else
            {
                throw InvalidComplexNumberException(s);
            }
            s += ch;
            ch = input_stream[s_index++];
        }
    }
    if (ch == 'i')
    {
        complex_mpf result {mpf_class(leading), mpf_class(s)};
        return std::make_shared<MalComplex>(result);
    }
    else
    {
        throw IncompleteComplexNumberException();
    }
}