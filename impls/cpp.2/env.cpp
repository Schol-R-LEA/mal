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



void Env_Symbol::set(MalPtr value)
{
    val = value;
}


PairPtr Env_Primitive::apply(PairPtr args)
{
    PairPtr result;
    size_t effective_arity = abs(arity());

    if ((args->size() == effective_arity) || (arity() < 0 && args->size() >= effective_arity-1))
    {
        return procedure(*std::make_shared<Reader>(args)).list()->as_pair();
    }
    else
    {
        throw new ArityMismatchException();
    }

    return args;
}



Environment::Environment(EnvPtr p, PairPtr binds, PairPtr exprs): parent(p)
{
    bool rests = false;

    PairPtr bind = nullptr;
    PairPtr argument = nullptr;

    for (bind = binds, argument = exprs;
         !(bind->is_null() || argument->is_null());
         bind = bind->cdr()->as_pair(), argument = argument->cdr()->as_pair())
    {
        auto parameter = bind->car();
        if (parameter->type() == MAL_REST_ARG)
        {
            rests = true;
            break;
        }
        this->set(parameter, argument->car());
    }

    if (rests)
    {
        auto rest_sym = bind->cdr()->as_pair()->car();
        auto rest = argument->cdr();
        this->set(rest_sym, rest);
    }
    else if (binds->size() != exprs->size())
    {
        throw new UnequalBindExprListsException(binds->to_str(), exprs->to_str());
    }
}



bool Environment::find(MalPtr p, bool local)
{
    if (p->type() == MAL_SYMBOL)
    {
        for (std::vector<EnvSymbolPtr>::iterator it = env.begin(); it != env.end(); ++it)
        {
            if (it->get()->symbol().to_str() == p->to_str())
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
        if (it->get()->symbol().to_str() == s)
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
            if (it->get()->symbol().to_str() == p->to_str())
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
        if (it->get()->symbol().to_str() == symbol)
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
    auto el_symbol = element->symbol().to_str();
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
    if (find(symbol->to_str(), true))
    {
        EnvSymbolPtr existing_entry = get(symbol->to_str());
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
        result += it->get()->symbol().to_str();
    }

    return result;
}
