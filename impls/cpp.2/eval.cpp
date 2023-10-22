
/* The following code applies the GNU Readline library and the GNU GMP library,
   which are licensed under the GPL version 3.0. Please refer to the file
   'LICENSE' in the implementation subdirectory.
*/

#include <memory>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
#include <gmpxx.h>
#include "exceptions.h"
#include "types.h"
#include "apply.h"
#include "env.h"
#include "eval.h"


Reader EVAL(Reader input, Env_Frame& parent_env)
{
    Env_Frame env = parent_env;

    while (true)
    {
        if (input.peek() == nullptr)
        {
            throw new NullTokenException();
        }
        if (input.is_empty())
        {
            env.pop();
            return input;
        }

        auto type = input.peek()->type();
        if (type == MAL_SYMBOL || type == MAL_VECTOR || type == MAL_HASHMAP)
        {
            return eval_ast(input, env);
        }
        else if (type == MAL_PAIR)
        {
            auto temp_car = input.peek()->as_pair()->car();
            if (temp_car == nullptr)
            {
                return temp_car;
            }
            auto form = temp_car->to_str();

            if (form == "def!")
            {
                PairPtr temp;
                temp->add(input.next()->as_pair());
                Reader reader(temp);
                auto result = eval_def(reader, env);
                return result;
            }
            else if (form == "let*")
            {
                PairPtr temp;
                temp->add(input.next()->as_pair());
                Reader reader(temp);
                input = eval_let(reader, env);
            }
            else if (form == "do")
            {
                PairPtr temp;
                temp->add(input.next()->as_pair());
                Reader reader(temp);
                input = eval_do(reader, env);
            }
            else if (form == "if")
            {
                PairPtr temp;
                temp->add(input.next()->as_pair());
                Reader reader(temp);
                input = eval_if(reader, env);
            }
            else if (form == "fn*")
            {
                PairPtr source;
                source->add(input.next()->as_pair()->cdr());  // skip the first "fn*" element

                PairPtr parameters;
                parameters->add(source->car());

                PairPtr body;
                body = source->cdr()->as_pair();
                Environment curr_env = env.top();
                auto parent = std::make_shared<Environment>(curr_env);

                PairPtr procedure;
                procedure->add(std::make_shared<MalProcedure>(body, parameters, parent, parameters->size()));
                Reader proc(procedure);
                return proc;
            }
            else
            {
                Reader evaluated = eval_ast(input, env);
                MalPtr result = evaluated.next();
                if (result == nullptr)
                {
                    return result;
                }

                auto result_type = result->type();
                if (result_type == MAL_PAIR)
                {
                    auto result_pair = result->as_pair();
                    if (result_pair->car() == nullptr)
                    {
                        throw new ProcedureNotFoundException("");
                    }
                    else
                    {
                        EnvSymbolPtr fn = nullptr;
                        auto p_type = result->type();

                        if (p_type == MAL_SYMBOL)
                        {
                            fn = env.top()->get(result_pair->car());
                        }
                        else if (p_type == MAL_PRIMITIVE)
                        {
                            auto procedure = result_pair->car()->as_pair()->car();
                            fn = env.top()->get(procedure);
                            return apply_fn(fn, result_pair->cdr()->as_pair());
                        }
                        else if (p_type == MAL_PROCEDURE)
                        {
                            auto procedure = result_pair->car();
                            PairPtr raw_args;
                            raw_args->add(result_pair->cdr());
                            Reader warmed_args(raw_args);
                            auto cooked_args = EVAL(warmed_args, env);
                            PairPtr args;
                            args->add(std::make_shared<MalPair>(cooked_args.next()->as_pair()));

                            ProcPtr proc_frame = procedure->as_procedure();
                            input = Reader(proc_frame->ast());
                            auto new_env = Environment(proc_frame->parent(), proc_frame->params(), args);
                            env.push(std::make_shared<Environment>(new_env));
                        }
                    }
                }
                else
                {
                    env.pop();
                    return result;
                }
            }
        }
        else
        {
            env.pop();
            return input;
        }
    }
}


Reader eval_ast(Reader input, Env_Frame& env)
{
    PairPtr result = std::make_shared<MalPair>();
    MalPtr peek = input.peek();

    if (peek == nullptr)
    {
        return Reader(result);
    }

    MalType type = peek->type();

    switch (type)
    {
        case MAL_SYMBOL:
            {
                MalPtr symbol = input.next();

                if (symbol == nullptr)
                {
                    throw new SymbolNotInitializedException("");
                }

                EnvSymbolPtr p = env.top()->get(symbol);

                if (p == nullptr)
                {
                    throw new SymbolNotInitializedException(symbol->to_str());
                }

                if (p->type() == ENV_PRIMITIVE)
                {
                    MalPtr prim = std::make_shared<MalPrimitive>(p->symbol().to_str(), p->arity());
                    result->add(prim);
                }
                else if (p->type() == ENV_PROCEDURE)
                {
                    auto proc = (dynamic_cast<Env_Procedure*>(&(*p)))->proc();
                    auto procedure_frame = dynamic_cast<MalProcedure*>(&(*proc));
                    auto proc_env = procedure_frame->parent();
                    env.push(proc_env);
                    input = Reader(procedure_frame->ast());
                }
                else if (p->type() == ENV_SYMBOL)
                {
                    result->add(p->value());
                }
                else
                {
                    throw SymbolNotInitializedException("");
                }
                return Reader(result);
            }
            break;
        case MAL_PAIR:
            {
                PairPtr sourcelist, evlist;
                evlist = result = std::make_shared<MalPair>();
                sourcelist = input.next()->as_pair();
                for (auto elem = sourcelist; elem != nullptr; elem = elem->as_pair()->cdr()->as_pair())
                {
                    Reader temp(elem->car());
                    Reader succ = EVAL(temp, env);
                    if (succ.peek() != nullptr)
                    {
                        evlist->add(succ.next());
                    }
                    else
                    {
                        break;
                    }
                    if (!elem->is_pair())
                    {
                        break;
                    }
                    if (elem->as_pair()->cdr() == nullptr)
                    {
                        break;
                    }
                    if (!elem->as_pair()->cdr()->is_pair())
                    {
                        break;
                    }
                }

                return Reader(evlist);
            }
            break;
        case MAL_VECTOR:
            {
                PairPtr veclist = input.next()->as_pair();
                Reader vecreader(veclist);
                return eval_vec(vecreader, env);
            }
            break;
        case MAL_HASHMAP:
            {
                return eval_hashmap(input.next()->as_hashmap(), env);
            }
            break;
        case MAL_QUASIQUOTE:
        {
            input.next()->as_pair();
            PairPtr temp;
            temp->add(input.next()->as_pair());
            Reader qqreader(temp);
            return eval_quasiquoted(qqreader, env);
        }
            break;
        default:
            return input;
    }
}


Reader eval_vec(Reader input, Env_Frame& env)
{
    PairPtr temp, elements;
    for (MalPtr elem = input.next(); elem != nullptr; elem = input.next())
    {
        temp = std::make_shared<MalPair>();
        temp->add(elem);
        Reader tempreader(temp);
        elements->add(EVAL(tempreader, env).next());
    }

    PairPtr result;
    MalPtr vec = std::make_shared<MalVector>(elements);
    result->add(vec);
    return Reader(result);
}


Reader eval_hashmap(MapPtr input, Env_Frame& env)
{
    InternalHashmap source = input->get_internal_hashmap();
    InternalHashmap resultant;

    for (auto element : source)
    {
        PairPtr temp = std::make_shared<MalPair>();
        temp->add(element.second);
        Reader tempreader(temp);
        resultant.emplace(element.first, EVAL(tempreader, env).next());
    }

    PairPtr result;
    MalPtr new_hm = std::make_shared<MalHashmap>(resultant);
    result->add(new_hm);
    return Reader(result);
}


Reader eval_def(Reader input, Env_Frame& env)
{
    if (input.next()->to_str() == "def!")
    {
        auto symbol = input.next();

        if (symbol == nullptr || symbol->type() != MAL_SYMBOL)
        {
            throw new InvalidDefineException(input.next()->to_str());
        }

        if (env.top()->find(symbol, true))
        {
            auto sym_ptr = env.top()->get(symbol);    // get the pointer to the local symbol
            auto val_ptr = input.next();
            PairPtr val_vec;
            val_vec->add(val_ptr);
            if (val_ptr == nullptr)
            {
                throw new ArityMismatchException();
            }
            Reader vec_reader(val_vec);
            auto value = EVAL(vec_reader, env);

            sym_ptr->set(value.next());
            return value;
        }
        else
        {
            auto val_ptr = input.next();
            if (val_ptr == nullptr)
            {
                throw new ArityMismatchException();
            }

            auto placeholder = std::make_shared<MalNil>();
            PairPtr val_vec;
            val_vec->add(val_ptr);
            Reader vec_reader(val_vec);
            auto value = EVAL(vec_reader, env);
            env.top()->set(symbol, value.peek());
            auto sym_ptr = env.top()->get(symbol); // get the pointer to the local symbol
            sym_ptr->set(value.next());
            PairPtr result;
            result->add(env.top()->get(symbol)->value());
            return Reader(result);
        }
    }
    else
    {
        throw new InvalidDefineException(input.next()->to_str());
    }
}


Reader eval_let(Reader input, Env_Frame& env)
{
    if (input.next()->to_str() == "let*")
    {
        EnvPtr current_env(std::make_shared<Environment>(env.top()));
        env.push(current_env);

        auto var_head = input.next();
        if (var_head->type() == MAL_PAIR || var_head->type() == MAL_VECTOR)
        {
            auto var_list = var_head->as_pair();

            // pre-initialize all of the binds
            auto iter_list = var_list;
            auto symbol = iter_list->car();
            while (symbol != nullptr)
            {
                if (symbol->type() != MAL_SYMBOL)
                {
                    throw new InvalidLetException(input.next()->to_str());
                }
                else
                {
                    auto placeholder = std::make_shared<MalNil>();
                    env.top()->set(symbol, placeholder);      // pre-initialize symbol in environment
                    iter_list = iter_list->cdr()->as_pair();  // get and discard the intended value
                    iter_list = iter_list->cdr()->as_pair();
                    symbol = iter_list->car();
                }
            }

            // re-bind with all of the values
            while (var_list->cdr() != nullptr)
            {
                auto symbol = var_list->car();
                if (symbol == nullptr || symbol->type() != MAL_SYMBOL)
                {
                    throw new InvalidLetException(input.next()->to_str());
                }
                else
                {
                    auto val_ptr = var_list->cdr();
                    if (val_ptr == nullptr)
                    {
                        throw new InvalidLetException(input.next()->to_str());
                    }

                    auto sym_ptr = env.top()->get(symbol);    // and retrieve the pointer to the env entry

                    PairPtr val_vec = std::make_shared<MalPair>(val_ptr);
                    Reader val_reader(val_vec);
                    auto value = EVAL(val_reader, env);

                    sym_ptr->set(value.next());
                }
            }

            PairPtr final_value;
            for (auto element = input.next(); element != nullptr; element = input.next())
            {
                final_value = std::make_shared<MalPair>();
                PairPtr elem_val;
                elem_val->add(element);
                Reader elem_reader(elem_val);
                final_value->add(EVAL(elem_reader, env).next());
                if (input.peek() != nullptr)
                {
                    Reader temp_reader(final_value);
                    final_value = EVAL(temp_reader, env).next()->as_pair();
                }
            }

            Reader final_reader(final_value);
            return final_reader;
        }
        else
        {
            throw new InvalidLetException(input.next()->to_str());

        }
    }
    else
    {
        throw new InvalidLetException(input.next()->to_str());
    }
}



Reader eval_quasiquoted(Reader input, Env_Frame& env, bool islist)
{
    PairPtr elements, result;

    for (MalPtr elem = input.next(); elem != nullptr; elem = input.next())
    {
        if (elem->type() == MAL_PAIR)
        {
            Reader temp(elem->as_pair());
            elements->add(eval_quasiquoted(temp, env, true).next()->as_pair());
        }

        else if(elem->type() == MAL_UNQUOTE)
        {
            Reader temp(elem->as_pair());
            elements->add(eval_ast(temp, env).next()->as_pair());
        }
        else
        {
            elements->add(elem);
        }
    }
    if (islist)
    {
        result->add(std::make_shared<MalPair>(elements));
    }
    else
    {
        result->add(std::make_shared<MalQuasiquote>(elements));
    }
    return Reader(result);
}


Reader eval_do(Reader input, Env_Frame& env)
{
    auto discard = input.next();       // discard the 'do' symbol
    PairPtr final_value;
    for (auto element = input.next(); element != nullptr; element = input.next())
    {
        final_value = std::make_shared<MalPair>();
        final_value->add(element);
        if (input.peek() != nullptr)
        {
            Reader temp_reader(final_value);
            final_value = EVAL(temp_reader, env).next()->as_pair();
        }
    }

    return Reader(final_value);
}


Reader eval_if(Reader input, Env_Frame& env)
{
    auto discard = input.next();    // discard the 'if' symbol
    Reader test(input.next());
    auto clause = EVAL(test, env).next();

    if (clause->to_str() != "false" && clause->to_str() != "nil")
    {
        Reader temp(input.next());
        return temp;
    }
    else
    {
        discard = input.next();      // discard the true condition
        if (input.peek() == nullptr)
        {
            Reader temp(std::make_shared<MalNil>());
            return temp;
        }
        else
        {
            Reader temp(input.next());
            return temp;
        }
    }
}


// Reader eval_fn(Reader input, Env_Frame& env)
// {
//     auto discard = input.next();    // discard the 'fn*' symbol
//     PairPtr parameters;
//     parameters->add(input.next());
//     PairPtr arguments, body;
//     body = input.rest();
//     std::shared_ptr<Environment> parent = std::make_shared<Environment>(env);
//     auto current_env = Environment(parent, parameters, arguments);

//     Procedure closure([parameters, body, parent](PairPtr arguments)->PairPtr {
//         PairPtr final_value;
//         PairPtr input = body;
//         auto current_env = Environment(parent, parameters, arguments);
//         for (auto element = input.next(); element != nullptr; element = input.next())
//         {
//             final_value.clear();
//             final_value->add(element);
//             final_value = EVAL(final_value, current_env);
//         }

//         return final_value;
//     });

//     PairPtr procedure;
//     procedure->add(std::make_shared<MalProcedure>(closure, body, parameters, current_env, parameters.size()));

//     return procedure;
// }
