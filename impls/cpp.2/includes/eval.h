#ifndef EVAL_H
#define EVAL_H

/* The following code applies the GNU Readline library and the GNU GMP library,
   which are licensed under the GPL version 3.0. Please refer to the file
   'LICENSE' in the implementation subdirectory.
*/

#include "reader.h"
#include "env.h"
#include "types.h"


Reader EVAL(Reader input, Env_Frame& parent_env);
Reader eval_ast(Reader input, Env_Frame& env);
Reader eval_vec(Reader input, Env_Frame& env);
Reader eval_hashmap(MapPtr input, Env_Frame& env);
Reader eval_def(Reader input, Env_Frame& env);
Reader eval_let(Reader input, Env_Frame& env);
Reader eval_do(Reader input, Env_Frame& env);
Reader eval_if(Reader input, Env_Frame& env);
Reader eval_fn(Reader input, Env_Frame& env);
Reader eval_quasiquoted(Reader input, Env_Frame& env, bool islist = false);

#endif
