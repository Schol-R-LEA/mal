/* The following code applies the GNU Readline library and the GNU GMP library,
   which are licensed under the GPL version 3.0. Please refer to the file
   'LICENSE' in the implementation subdirectory.
*/

#include <complex>
#include <functional>
#include <iostream>
#include <typeinfo>
#include <cstdlib>
#include <cstdarg>
#include <gmpxx.h>
#include "exceptions.h"
#include "types.h"
#include "env.h"
#include "apply.h"


Environment repl_env;


Env_Symbol::Env_Symbol(MalPtr s, MalPtr v): val(v), n_ary(0)
{
    if (s != nullptr && s->type() == MAL_SYMBOL)
    {
        sym = s->value();
    }
    else
    {
        throw new InvalidEnvironmentSymbolException(s->value());
    }
}


void Env_Symbol::set(MalPtr value)
{
    val = value;
}


TokenVector Env_Primitive::apply(TokenVector& args)
{
    TokenVector result;
    size_t effective_arity = abs(arity());

    if ((args.size() == effective_arity) || (arity() < 0 && args.size() >= effective_arity-1))
    {
        return procedure(args);
    }
    else
    {
        throw new ArityMismatchException();
    }


    return args;
}



Environment::Environment(EnvPtr p, TokenVector binds, TokenVector exprs): parent(p)
{
    if (!is_mal_container(binds.peek()->type()) || !is_mal_container(exprs.peek()->type()))
    {
        throw new InvalidBindExprListsException(binds.values(), exprs.values());
    }

    auto parameters = binds.next()->raw_value();
    auto arguments = exprs.next()->raw_value();

    bool rests = false;

    for (auto parameter = parameters.next(); parameter != nullptr; parameter = parameters.next())
    {
        if (parameter->type() == MAL_REST_ARG)
        {
            rests = true;
            break;
        }
        this->set(parameter, arguments.next());
    }

    if (rests)
    {
        auto rest = std::make_shared<MalList>(arguments.rest());
        this->set(parameters.peek(), rest);
    }
    else if (parameters.size() != arguments.size())
    {
        throw new UnequalBindExprListsException(parameters.values(), arguments.values());
    }
}



bool Environment::find(MalPtr p, bool local)
{
    if (p->type() == MAL_SYMBOL)
    {
        for (std::vector<EnvSymbolPtr>::iterator it = env.begin(); it != env.end(); ++it)
        {
            if (it->get()->symbol().value() == p->value())
            {
                return true;
            }
        }

        if (parent != nullptr && !local)
        {
            return parent->find(p);
        }
    }

    return false;
}


bool Environment::find(std::string s, bool local)
{
    for (std::vector<EnvSymbolPtr>::iterator it = env.begin(); it != env.end(); ++it)
    {
        if (it->get()->symbol().value() == s)
        {
            return true;
        }
    }

    if (parent != nullptr && !local)
    {
        return parent->find(s);
    }

    return false;
}


EnvSymbolPtr Environment::get(MalPtr p)
{
    if (p->type() == MAL_SYMBOL)
    {
        for (std::vector<EnvSymbolPtr>::iterator it = env.begin(); it != env.end(); ++it)
        {
            if (it->get()->symbol().value() == p->value())
            {
                return *it;
            }
        }

        if (parent != nullptr)
        {
            return parent->get(p);
        }
    }

    return nullptr;
}


EnvSymbolPtr Environment::get(std::string symbol)
{
    for (std::vector<EnvSymbolPtr>::iterator it = env.begin(); it != env.end(); ++it)
    {
        if (it->get()->symbol().value() == symbol)
        {
            return *it;
        }
    }

    if (parent != nullptr)
    {
        return parent->get(symbol);
    }

    return nullptr;
}


void Environment::set(EnvSymbolPtr element)
{
    auto el_symbol = element->symbol().value();
    if (find(el_symbol, true))
    {
        EnvSymbolPtr existing_entry = get(el_symbol);
        existing_entry->set(element->value());
    }
    else
    {
        env.push_back(element);
    }
}


void Environment::set(MalPtr symbol, MalPtr value)
{
    if (find(symbol->value(), true))
    {
        EnvSymbolPtr existing_entry = get(symbol->value());
        existing_entry->set(value);
    }
    env.push_back(std::make_shared<Env_Symbol>(symbol, value));
}


void Environment::set(std::string symbol, MalPtr value)
{
    if (find(symbol, true))
    {
        EnvSymbolPtr existing_entry = get(symbol);
        existing_entry->set(value);
    }
    env.push_back(std::make_shared<Env_Symbol>(std::make_shared<MalSymbol>(symbol), value));
}


std::string Environment::element_names()
{
    std::string result;

    for (std::vector<EnvSymbolPtr>::iterator it = env.begin(); it != env.end(); ++it)
    {
        if (it != env.begin())
        {
            result += ", ";
        }
        result += it->get()->symbol().value();
    }

    return result;
}
