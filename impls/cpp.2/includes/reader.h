#ifndef READER_H
#define READER_H

#include <string>
#include <memory>
#include <vector>
#include "lineedit.h"
#include "types.h"

extern unsigned int paren_count, square_bracket_count, hm_count, s_index;

class Reader
{
public:
    Reader(PairPtr tokens): m_current_token(0), m_tokens(tokens) {};
    MalPtr peek();
    MalPtr next();

private:
    unsigned int m_current_token;
    PairPtr m_tokens;
};


PairPtr tokenize(std::string input_stream);
Reader read_str(std::string input_stream);

MalPtr read_form(std::string input_stream);

void read_whitespace(std::string input_stream, char leading);
void read_comment(std::string input_stream);
MalPtr read_string(std::string input_stream);
MalPtr read_symbol(std::string input_stream, char leading);


#endif