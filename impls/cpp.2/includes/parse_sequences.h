#ifndef PARSE_SEQUENCES_H
#define PARSE_SEQUENCES_H

MalPtr read_list(std::string input_stream);
MalPtr read_vector(std::string input_stream);
MalPtr read_hashmap(std::string input_stream);

#endif