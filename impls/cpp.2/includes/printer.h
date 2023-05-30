#ifndef PRINTER_H
#define PRINTER_H

#include <string>
#include "types.h"
#include "reader.h"

std::string pr_str(Reader tokens, bool print_readably = false);

#endif