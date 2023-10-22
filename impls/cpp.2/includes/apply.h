#ifndef APPLY_H
#define APPLY_H

/* The following code applies the GNU Readline library and the GNU GMP library,
   which are licensed under the GPL version 3.0. Please refer to the file
   'LICENSE' in the implementation subdirectory.
*/


#include "types.h"
#include "eval.h"
#include "env.h"
#include "exceptions.h"


MalPtr apply_fn(EnvSymbolPtr fn, PairPtr args);

#endif
