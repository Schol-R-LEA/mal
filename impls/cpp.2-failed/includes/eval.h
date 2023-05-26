#ifndef EVAL_H
#define EVAL_H

/* The following code applies the GNU Readline library and the GNU GMP library,
   which are licensed under the GPL version 3.0. Please refer to the file
   'LICENSE' in the implementation subdirectory.
*/


#include "env.h"
#include "types.h"


TokenVector EVAL(TokenVector& input, Env_Frame& parent_env);
TokenVector eval_ast(TokenVector& input, Env_Frame& env);
TokenVector eval_vec(TokenVector& input, Env_Frame& env);
TokenVector eval_hashmap(HashMapInternal& input, Env_Frame& env);
TokenVector eval_def(TokenVector& input, Env_Frame& env);
TokenVector eval_let(TokenVector& input, Env_Frame& env);
TokenVector eval_do(TokenVector& input, Env_Frame& env);
TokenVector eval_if(TokenVector& input, Env_Frame& env);
TokenVector eval_fn(TokenVector& input, Env_Frame& env);
TokenVector eval_quasiquoted(TokenVector& input, Env_Frame& env, bool islist = false);

#endif