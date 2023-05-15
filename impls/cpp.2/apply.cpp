
/* The following code applies the GNU Readline library and the GNU GMP library,
   which are licensed under the GPL version 3.0. Please refer to the file
   'LICENSE' in the implementation subdirectory.
*/

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include "types.h"
#include "apply.h"
#include "eval.h"
#include "env.h"


TokenVector apply_fn(EnvPtr fn, TokenVector args, Environment env)
{
    if (fn->type() != ENV_PRIMITIVE)
    {
        throw new InvalidPrimitiveException();
    }
    else
    {
        return fn->apply(args, env);
    }
}


TokenVector apply_fn(TokenVector fn, TokenVector args, Environment env)
{
    env.size();
    fn.append(args);
    return fn;
}