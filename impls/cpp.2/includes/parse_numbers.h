#ifndef PARSE_NUMBERS_H
#define PARSE_NUMBERS_H

#include <iostream>
#include <memory>
#include <vector>
#include <cctype>
#include <cstdlib>
#include "reader.h"
#include "types.h"


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

#endif