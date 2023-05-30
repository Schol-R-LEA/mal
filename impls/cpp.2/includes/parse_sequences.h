#ifndef PARSE_SEQUENCES_H
#define PARSE_SEQUENCES_H

MalPtr read_list(std::string input_stream);
void close_list();
MalPtr read_vector(std::string input_stream);
void close_vector();
MalPtr read_hashmap(std::string input_stream);
void close_hashmap();

#endif