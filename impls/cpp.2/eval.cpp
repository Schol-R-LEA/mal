
/* The following code applies the GNU Readline library and the GNU GMP library,
   which are licensed under the GPL version 3.0. Please refer to the file
   'LICENSE' in the implementation subdirectory.
*/

#include <memory>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <gmpxx.h>
#include "exceptions.h"
#include "types.h"
#include "apply.h"
#include "env.h"
#include "eval.h"


TokenVector EVAL(TokenVector& input, Environment& env)
{
    Environment global_env = env;
    while (true)
    {
        if (input.peek() == nullptr)
        {
            throw new NullTokenException();
        }
        if (input.empty())
        {
            return input;
        }

        auto type = input.peek()->type();
        if (type == MAL_SYMBOL || type == MAL_VECTOR || type == MAL_HASHMAP)
        {
            return eval_ast(input, env);
        }
        else if (type == MAL_LIST)
        {
            auto form = input.peek()->raw_value().car()->value();

            if (form == "def!")
            {
                TokenVector temp;
                temp.append(input.next()->raw_value());
                auto result = eval_def(temp, env);
                return result;
            }
            else if (form == "let*")
            {
                TokenVector temp;
                temp.append(input.next()->raw_value());
                input = eval_let(temp, env);
            }
            else if (form == "do")
            {
                TokenVector temp;
                temp.append(input.next()->raw_value());
                input = eval_do(temp, env);
            }
            else if (form == "if")
            {
                TokenVector temp;
                temp.append(input.next()->raw_value());
                input = eval_if(temp, env);
            }
            else if (form == "fn*")
            {
                TokenVector source;
                source.append(input.next()->raw_value());

                auto discard = source.next();    // discard the 'fn*' symbol

                TokenVector parameters;
                parameters.append(source.next());

                TokenVector body;
                body = source.rest();
                std::shared_ptr<Environment> parent = std::make_shared<Environment>(env);

                TokenVector procedure;
                procedure.append(std::make_shared<MalProcedure>(body, parameters, parent, parameters.size()));
                return procedure;
            }
            else
            {
                TokenVector result = eval_ast(input, env);
                if (result.empty())
                {
                    return result;
                }
                else
                {
                    if (type == MAL_LIST)
                    {
                        if (result.peek() == nullptr || result.peek()->raw_value().empty())
                        {
                            throw new ProcedureNotFoundException("");
                        }
                        else
                        {
                            EnvSymbolPtr fn = nullptr;
                            auto p_type = result.car()->type();

                            if (p_type == MAL_SYMBOL)
                            {
                                std::cout << "mal symbol" << std::endl;
                                fn = env.get(result.car());
                            }
                            else if (p_type == MAL_PRIMITIVE)
                            {
                                std::cout << "mal Primitive" << std::endl;
                                auto procedure = result.next()->raw_value().car();
                                fn = env.get(procedure);
                                return apply_fn(fn, result.cdr());
                            }
                            else if (p_type == MAL_PROCEDURE)
                            {
                                std::cout << "mal Procedure" << std::endl;
                                auto procedure = result.next();
                                TokenVector raw_args;
                                raw_args.append(result.cdr());
                                auto cooked_args = EVAL(raw_args, env);
                                TokenVector args;
                                args.append(std::make_shared<MalList>(cooked_args));

                                // WARNING: This function uses downcasting of a pointer from it's parent class to 
                                // the actual subclass. This is VERY questionable, and if possible a better 
                                // solution should be found!
                                auto proc_frame = (dynamic_cast<MalProcedure*>(&(*procedure)));
                                input = proc_frame->ast();
                                env = Environment(proc_frame->parent(), proc_frame->params(), args);
                            }
                            if (fn == nullptr)
                            {
                                throw new ProcedureNotFoundException(result.car()->value());
                            }
                        }
                    }
                    else
                    {
                        return result;
                    }
                }
            }
        }
        else
        {
            return input;
        }
    }
}


TokenVector eval_ast(TokenVector& input, Environment& env)
{
    TokenVector result;
    MalPtr peek = input.peek();

    if (peek == nullptr)
    {
        return result;
    }

    MalTypeName type = peek->type();

    switch (type)
    {
        case MAL_SYMBOL:
            {
                std::cout << "Symbol" << std::endl;
                MalPtr symbol = input.next();

                if (symbol == nullptr)
                {
                    throw new SymbolNotInitializedException("");
                }

                EnvSymbolPtr p = env.get(symbol);

                if (p == nullptr)
                {
                    std::cout << "null pointer" << std::endl;
                    throw new SymbolNotInitializedException(symbol->value());
                }

                if (p->type() == ENV_PRIMITIVE)
                {
                    std::cout << "Env Primitive" << std::endl;
                    MalPtr prim = std::make_shared<MalPrimitive>(p->symbol().value(), p->arity());
                    result.append(prim);
                }
                else if (p->type() == ENV_PROCEDURE)
                {
                    std::cout << "Env Procedure" << std::endl;
                    auto proc = (dynamic_cast<Env_Procedure*>(&(*p)))->proc();
                    auto procedure_frame = dynamic_cast<MalProcedure*>(&(*proc));
                    env = procedure_frame->parent();
                    input = procedure_frame->ast();
                }
                else if (p->type() == ENV_SYMBOL)
                {
                    std::cout << "Env Symbol " << p->value()->value() << std::endl;
                    result.append(p->value());
                }
                else
                {
                    throw new SymbolNotInitializedException("");
                }
                return result;
            }
            break;
        case MAL_LIST:
            {
                TokenVector sourcelist, evlist;
                sourcelist = input.next()->raw_value();
                for (auto elem = sourcelist.next(); elem != nullptr; elem = sourcelist.next())
                {
                    TokenVector temp;
                    temp.append(elem);
                    evlist.append(EVAL(temp, env));
                    temp.clear();
                }

                return evlist;
            }
            break;
        case MAL_VECTOR:
            {
                TokenVector veclist = input.next()->raw_value();
                return eval_vec(veclist, env);
            }
            break;
        case MAL_HASHMAP:
            {
                // WARNING: This function uses downcasting of a pointer from it's parent class to the
                // actual subclass. This is VERY questionable, and if possible a better solution should be found!
                HashMapInternal hm((dynamic_cast<MalHashmap*>(&(*input.next())))->internal_map());
                return eval_hashmap(hm, env);
            }
            break;
        case MAL_QUASIQUOTE:
        {
            input.next()->raw_value();
            TokenVector temp;
            temp.append(input.next()->raw_value());
            return eval_quasiquoted(temp, env);
        }
            break;
        default:
            return input;
    }
}


TokenVector eval_vec(TokenVector& input, Environment& env)
{
    TokenVector temp, elements;
    for (MalPtr elem = input.next(); elem != nullptr; elem = input.next())
    {
        temp.append(elem);
        elements.append(EVAL(temp, env));
        temp.clear();
    }

    TokenVector result;
    MalPtr vec = std::make_shared<MalVector>(elements);
    result.append(vec);
    return result;
}

TokenVector eval_hashmap(HashMapInternal& input, Environment& env)
{
    HashMapInternal resultant;

    for (auto element : input)
    {
        TokenVector temp;
        temp.append(element.second);
        resultant.emplace(element.first, EVAL(temp, env).next());
        temp.clear();
    }

    TokenVector result;
    MalPtr new_hm = std::make_shared<MalHashmap>(resultant);
    result.append(new_hm);
    return result;
}


TokenVector eval_def(TokenVector& input, Environment& env)
{
    if (input.next()->value() == "def!")
    {
        auto symbol = input.next();

        if (symbol == nullptr || symbol->type() != MAL_SYMBOL)
        {
            throw new InvalidDefineException(input.values());
        }

        if (env.find(symbol, true))
        {
            auto sym_ptr = env.get(symbol);    // get the pointer to the local symbol
            auto val_ptr = input.next();
            TokenVector val_vec;
            val_vec.append(val_ptr);
            if (val_ptr == nullptr)
            {
                throw new ArityMismatchException();
            }
            auto value = EVAL(val_vec, env);

            sym_ptr->set(value.car());
            return value;
        }
        else
        {
            auto val_ptr = input.next();
            if (val_ptr == nullptr)
            {
                throw new ArityMismatchException();
            }

            auto placeholder = std::make_shared<MalNull>();
            env.set(symbol, placeholder);      // pre-initialize symbol in environment
            auto sym_ptr = env.get(symbol);    // and retrieve the pointer to the env entry
            TokenVector val_vec;
            val_vec.append(val_ptr);
            auto value = EVAL(val_vec, env);
            sym_ptr->set(value.next());
            TokenVector result;
            result.append(env.get(symbol)->value());
            return result;
        }
    }
    else
    {
        throw new InvalidDefineException(input.values());
    }
}

TokenVector eval_let(TokenVector& input, Environment& env)
{
    if (input.next()->value() == "let*")
    {
        Environment current_env(std::make_shared<Environment>(env));

        auto var_head = input.next();
        if (var_head->type() == MAL_LIST || var_head->type() == MAL_VECTOR)
        {
            auto var_list = var_head->raw_value();

            // pre-initialize all of the binds
            auto pre_list = var_list;
            while (pre_list.peek() != nullptr)
            {
                auto symbol = pre_list.next();
                if (symbol == nullptr || symbol->type() != MAL_SYMBOL)
                {
                    throw new InvalidLetException(input.values());
                }
                else
                {
                    auto placeholder = std::make_shared<MalNull>();
                    current_env.set(symbol, placeholder);      // pre-initialize symbol in environment
                    pre_list.next();
                }
            }

            // re-bind with all of the values
            while (var_list.peek() != nullptr)
            {
                auto symbol = var_list.next();
                if (symbol == nullptr || symbol->type() != MAL_SYMBOL)
                {
                    throw new InvalidLetException(input.values());
                }
                else
                {
                    auto val_ptr = var_list.next();
                    if (val_ptr == nullptr)
                    {
                        throw new InvalidLetException(input.values());
                    }

                    auto sym_ptr = current_env.get(symbol);    // and retrieve the pointer to the env entry

                    TokenVector val_vec;
                    val_vec.append(val_ptr);
                    auto value = EVAL(val_vec, current_env);

                    sym_ptr->set(value.next());
                }
            }

            TokenVector final_value;
            for (auto element = input.next(); element != nullptr; element = input.next())
            {
                final_value.clear();
                TokenVector elem_val;
                elem_val.append(element);
                final_value.append(EVAL(elem_val, current_env));
                if (input.peek() != nullptr)
                {
                    final_value = EVAL(final_value, env);
                }
            }
            env = current_env;

            return final_value;
        }
        else
        {
            throw new InvalidLetException(input.values());

        }
    }
    else
    {
        throw new InvalidLetException(input.next()->value());
    }
}



TokenVector eval_quasiquoted(TokenVector& input, Environment& env, bool islist)
{
    TokenVector elements, result;

    for (MalPtr elem = input.next(); elem != nullptr; elem = input.next())
    {
        if (elem->type() == MAL_LIST)
        {
            TokenVector temp;
            temp.append(elem->raw_value());
            elements.append(eval_quasiquoted(temp, env, true));
        }

        else if(elem->type() == MAL_UNQUOTE)
        {
            TokenVector temp;
            temp.append(elem->raw_value());
            elements.append(eval_ast(temp, env));
        }
        else
        {
            elements.append(elem);
        }
    }
    if (islist)
    {
        result.append(std::make_shared<MalList>(elements));
    }
    else
    {
        result.append(std::make_shared<MalQuasiquote>(elements));
    }
    return result;
}


TokenVector eval_do(TokenVector& input, Environment& env)
{
    auto discard = input.next();       // discard the 'do' symbol
    TokenVector final_value;
    for (auto element = input.next(); element != nullptr; element = input.next())
    {
        final_value.clear();
        final_value.append(element);
        if (input.peek() != nullptr)
        {
            final_value = EVAL(final_value, env);
        }
    }

    return final_value;
}


TokenVector eval_if(TokenVector& input, Environment& env)
{
    auto discard = input.next();    // discard the 'if' symbol
    TokenVector test;
    test.append(input.next());
    auto clause = EVAL(test, env).next();

    if (clause->value() != "false" && clause->value() != "nil")
    {
        TokenVector temp;
        temp.append(input.next());
        return temp;
    }
    else
    {
        discard = input.next();      // discard the true condition
        if (input.peek() == nullptr)
        {
            TokenVector temp;
            temp.append(std::make_shared<MalNil>());
            return temp;
        }
        else
        {
            TokenVector temp;
            temp.append(input.next());
            return temp;
        }
    }
}


// TokenVector eval_fn(TokenVector& input, Environment& env)
// {
//     auto discard = input.next();    // discard the 'fn*' symbol
//     TokenVector parameters;
//     parameters.append(input.next());
//     TokenVector arguments, body;
//     body = input.rest();
//     std::shared_ptr<Environment> parent = std::make_shared<Environment>(env);
//     auto current_env = Environment(parent, parameters, arguments);

//     Procedure closure([parameters, body, parent](TokenVector arguments)->TokenVector {
//         TokenVector final_value;
//         TokenVector input = body;
//         auto current_env = Environment(parent, parameters, arguments);
//         for (auto element = input.next(); element != nullptr; element = input.next())
//         {
//             final_value.clear();
//             final_value.append(element);
//             final_value = EVAL(final_value, current_env);
//         }

//         return final_value;
//     });

//     TokenVector procedure;
//     procedure.append(std::make_shared<MalProcedure>(closure, body, parameters, current_env, parameters.size()));

//     return procedure;
// }