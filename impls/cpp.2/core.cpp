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
#include "printer.h"


Procedure mal_plus([](Reader& tokens)->Reader
{
    MalPtr x_peek = tokens.peek();

    if (x_peek != nullptr)
    {
        PairPtr x_tokens = std::make_shared<MalPair>(tokens.next());

        MalPtr y_peek = tokens.peek();
        if (y_peek != nullptr)
        {
            PairPtr y_tokens = std::make_shared<MalPair>(tokens.next()->as_pair()->car());

            MalPtr x = x_tokens->car();
            MalPtr y = y_tokens->car();

            MalPtr result;

            switch (x->type())
            {
                    case MAL_INTEGER:
                    {
                        switch (y->type())
                        {
                            case MAL_INTEGER:
                            {
                                return Reader(std::make_shared<MalInteger>(x->as_integer() + y->as_integer()));
                            }
                                break;
                            case MAL_FRACTIONAL:
                            {
                                return Reader(std::make_shared<MalFractional>(x->as_integer() + y->as_fractional()));
                            }
                                break;
                            case MAL_RATIONAL:
                            {
                                return Reader(std::make_shared<MalRational>(x->as_integer() + y->as_rational()));
                            }
                                break;
                            case MAL_COMPLEX:
                            {
                                return Reader(std::make_shared<MalComplex>(complex_mpf(x->as_integer()) + y->as_complex()));
                            }
                                break;
                            default:
                                throw new InvalidFunctionArgumentException(y->to_str());
                        }
                    }
                        break;

                    case MAL_FRACTIONAL:
                    {
                        switch (y->type())
                        {
                            case MAL_INTEGER:
                            {
                                return Reader(std::make_shared<MalFractional>(x->as_fractional() + y->as_integer()));
                            }
                                break;
                            case MAL_FRACTIONAL:
                            {
                                return Reader(std::make_shared<MalFractional>(x->as_fractional() + y->as_fractional()));
                            }
                                break;
                            case MAL_RATIONAL:
                            {
                                return Reader(std::make_shared<MalFractional>(x->as_fractional() + y->as_rational()));
                            }
                                break;
                            case MAL_COMPLEX:
                            {
                                return Reader(std::make_shared<MalComplex>(complex_mpf(x->as_fractional()) + y->as_complex()));
                            }
                                break;
                            default:
                                throw new InvalidFunctionArgumentException(y_peek->to_str());
                        }
                    }
                        break;
                    case MAL_RATIONAL:
                    {
                        switch (y->type())
                        {
                            case MAL_INTEGER:
                            {
                                return Reader(std::make_shared<MalRational>(x->as_rational() + y->as_integer()));
                            }
                                break;
                            case MAL_FRACTIONAL:
                            {
                                return Reader(std::make_shared<MalFractional>(x->as_rational() + y->as_fractional()));
                            }
                                break;
                            case MAL_RATIONAL:
                            {
                                return Reader(std::make_shared<MalRational>(x->as_rational() + y->as_rational()));
                            }
                                break;
                            case MAL_COMPLEX:
                            {
                                return Reader(std::make_shared<MalComplex>(complex_mpf(x->as_rational()) + y->as_complex()));
                            }
                                break;
                            default:
                                throw new InvalidFunctionArgumentException(y->to_str());
                        }
                    }
                        break;
                case MAL_COMPLEX:
                    {
                        switch (y->type())
                        {
                            case MAL_INTEGER:
                            {
                                return Reader(std::make_shared<MalComplex>(x->as_complex() + complex_mpf(y->as_integer())));
                            }
                                break;
                            case MAL_FRACTIONAL:
                            {
                                return Reader(std::make_shared<MalComplex>(x->as_complex() + complex_mpf(y->as_fractional())));
                            }
                                break;
                            case MAL_RATIONAL:
                            {
                                return Reader(std::make_shared<MalComplex>(x->as_complex() + complex_mpf(y->as_rational())));
                            }
                                break;
                            case MAL_COMPLEX:
                            {
                                return Reader(std::make_shared<MalComplex>(x->as_complex() + y->as_complex()));
                            }
                                break;
                            default:
                                throw new InvalidFunctionArgumentException(y->to_str());
                        }
                    }
                        break;
                    default:
                        throw new InvalidFunctionArgumentException(x->to_str());
            }
        }
        else
        {
            throw new MissingFunctionArgumentException();
        }
    }
    else
    {
        throw new MissingFunctionArgumentException();
    }
});



// comparisons
/*
Procedure mal_equal([](Reader& tokens)->Reader
{
    PairPtr car;
    car.append(tokens.car());
    PairPtr cdr;
    cdr.append(tokens.cdr());

    PairPtr mal_true, mal_false;
    mal_true.append(std::make_shared<MalBoolean>("true"));
    mal_false.append(std::make_shared<MalBoolean>("false"));

    // first, check that the two elements to compare are of the same size
    // this handles the case where there are dangling elements
    if (car.size() != cdr.size())
    {
        return mal_false;
    }

    if (car.peek()->type() != cdr.peek()->type())
    {
        if (!(is_mal_container(car.peek()->type()) && is_mal_container(cdr.peek()->type())))
        {
            return mal_false;
        }
    }

    if (is_mal_numeric(car.peek()->type()))
    {
        switch (car.peek()->type())
        {
            case MAL_INTEGER:
                {
                    auto comp1 = dynamic_cast<MalInteger*>(&(*car.next()))->numeric_to_str();
                    auto comp2 = dynamic_cast<MalInteger*>(&(*cdr.next()))->numeric_to_str();
                    if (comp1 == comp2)
                    {
                        return mal_true;
                    }
                    else
                    {
                        return mal_false;
                    }
                }
                break;
            case MAL_RATIONAL:
                {
                    auto comp1 = dynamic_cast<MalRational*>(&(*car.next()))->numeric_to_str();
                    auto comp2 = dynamic_cast<MalRational*>(&(*cdr.next()))->numeric_to_str();
                    if (comp1 == comp2)
                    {
                        return mal_true;
                    }
                    else
                    {
                        return mal_false;
                    }
                }
                break;
            case MAL_FRACTIONAL:
                {
                    auto comp1 = dynamic_cast<MalFractional*>(&(*car.next()))->numeric_to_str();
                    auto comp2 = dynamic_cast<MalFractional*>(&(*cdr.next()))->numeric_to_str();
                    if (comp1 == comp2)
                    {
                        return mal_true;
                    }
                    else
                    {
                        return mal_false;
                    }
                }
                break;
            case MAL_COMPLEX:
                {
                    auto comp1 = dynamic_cast<MalComplex*>(&(*car.next()))->numeric_to_str();
                    auto comp2 = dynamic_cast<MalComplex*>(&(*cdr.next()))->numeric_to_str();
                    if (comp1 == comp2)
                    {
                        return mal_true;
                    }
                    else
                    {
                        return mal_false;
                    }
                }
                break;
            default:
                throw new NonNumericComparisonException(car.next()->to_str(), cdr.next()->to_str());
        }
    }
    else if (is_mal_container(car.peek()->type()) || is_mal_reader_macro(car.peek()->type()))
    {
        auto car_list = car.peek()->raw_to_str();
        auto cdr_list = cdr.peek()->raw_to_str();

        if (car_list.size() != cdr_list.size())
        {
            return mal_false;
        }

        for (size_t i = 0; i < cdr_list.size(); ++i)
        {
            PairPtr comp;
            comp.append(car_list.next());
            comp.append(cdr_list.next());

            if (mal_equal(comp).peek()->to_str() == "false")
            {
                return mal_false;
            }
            comp.clear();
        }
        return mal_true;
    }

    else if (car.next()->to_str() != cdr.next()->to_str())
    {
        return mal_false;
    }
    else
    {
        return mal_true;
    }
});

Procedure mal_not_equal([](Reader& tokens)->Reader
{
    PairPtr mal_true, mal_false;
    mal_true.append(std::make_shared<MalBoolean>("true"));
    mal_false.append(std::make_shared<MalBoolean>("false"));

    if (mal_equal(tokens).next()->to_str() != "true")
    {
        return mal_false;
    }
    else
    {
        return mal_true;
    }

});


Procedure mal_greater_than([](Reader& tokens)->Reader
{
    PairPtr car, cdr;
    car.append(tokens.car());
    cdr.append(tokens.cdr());

    if (!is_mal_numeric(car.peek()->type()))
    {
        throw new NonNumericComparisonException(car.next()->to_str(), cdr.next()->to_str());
    }


    PairPtr mal_true, mal_false;
    mal_true.append(std::make_shared<MalBoolean>("true"));
    mal_false.append(std::make_shared<MalBoolean>("false"));

    if (car.peek()->type() != cdr.peek()->type())
    {
        return mal_false;
    }


    switch (car.peek()->type())
    {
        case MAL_INTEGER:
            {
                auto comp1 = dynamic_cast<MalInteger*>(&(*car.next()))->numeric_to_str();
                auto comp2 = dynamic_cast<MalInteger*>(&(*cdr.next()))->numeric_to_str();
                if (comp1 > comp2)
                {
                    return mal_true;
                }
                else
                {
                    return mal_false;
                }
            }
            break;
        case MAL_RATIONAL:
            {
                auto comp1 = dynamic_cast<MalRational*>(&(*car.next()))->numeric_to_str();
                auto comp2 = dynamic_cast<MalRational*>(&(*cdr.next()))->numeric_to_str();
                if (comp1 > comp2)
                {
                    return mal_true;
                }
                else
                {
                    return mal_false;
                }
            }
            break;
        case MAL_FRACTIONAL:
            {
                auto comp1 = dynamic_cast<MalFractional*>(&(*car.next()))->numeric_to_str();
                auto comp2 = dynamic_cast<MalFractional*>(&(*cdr.next()))->numeric_to_str();
                if (comp1 > comp2)
                {
                    return mal_true;
                }
                else
                {
                    return mal_false;
                }
            }
            break;

        default:
            throw new NonNumericComparisonException(car.next()->to_str(), cdr.next()->to_str());
    }
});

Procedure mal_less_than([](Reader& tokens)->Reader
{
    PairPtr car, cdr;
    car.append(tokens.car());
    cdr.append(tokens.cdr());

    if (!is_mal_numeric(car.peek()->type()))
    {
        throw new NonNumericComparisonException(car.next()->to_str(), cdr.next()->to_str());
    }


    PairPtr mal_true, mal_false;
    mal_true.append(std::make_shared<MalBoolean>("true"));
    mal_false.append(std::make_shared<MalBoolean>("false"));

    if (car.peek()->type() != cdr.peek()->type())
    {
        return mal_false;
    }


    switch (car.peek()->type())
    {
        case MAL_INTEGER:
            {
                auto comp1 = dynamic_cast<MalInteger*>(&(*car.next()))->numeric_to_str();
                auto comp2 = dynamic_cast<MalInteger*>(&(*cdr.next()))->numeric_to_str();
                if (comp1 < comp2)
                {
                    return mal_true;
                }
                else
                {
                    return mal_false;
                }
            }
            break;
        case MAL_RATIONAL:
            {
                auto comp1 = dynamic_cast<MalRational*>(&(*car.next()))->numeric_to_str();
                auto comp2 = dynamic_cast<MalRational*>(&(*cdr.next()))->numeric_to_str();
                if (comp1 < comp2)
                {
                    return mal_true;
                }
                else
                {
                    return mal_false;
                }
            }
            break;
        case MAL_FRACTIONAL:
            {
                auto comp1 = dynamic_cast<MalFractional*>(&(*car.next()))->numeric_to_str();
                auto comp2 = dynamic_cast<MalFractional*>(&(*cdr.next()))->numeric_to_str();
                if (comp1 < comp2)
                {
                    return mal_true;
                }
                else
                {
                    return mal_false;
                }
            }
            break;

        default:
            throw new NonNumericComparisonException(car.next()->to_str(), cdr.next()->to_str());
    }
});


Procedure mal_greater_equal([](Reader& tokens)->Reader
{
    PairPtr car, cdr;
    car.append(tokens.car());
    cdr.append(tokens.cdr());

    if (!is_mal_numeric(car.peek()->type()))
    {
        throw new NonNumericComparisonException(car.next()->to_str(), cdr.next()->to_str());
    }


    PairPtr mal_true, mal_false;
    mal_true.append(std::make_shared<MalBoolean>("true"));
    mal_false.append(std::make_shared<MalBoolean>("false"));

    if (car.peek()->type() != cdr.peek()->type())
    {
        return mal_false;
    }


    switch (car.peek()->type())
    {
        case MAL_INTEGER:
            {
                auto comp1 = dynamic_cast<MalInteger*>(&(*car.next()))->numeric_to_str();
                auto comp2 = dynamic_cast<MalInteger*>(&(*cdr.next()))->numeric_to_str();
                if (comp1 >= comp2)
                {
                    return mal_true;
                }
                else
                {
                    return mal_false;
                }
            }
            break;
        case MAL_RATIONAL:
            {
                auto comp1 = dynamic_cast<MalRational*>(&(*car.next()))->numeric_to_str();
                auto comp2 = dynamic_cast<MalRational*>(&(*cdr.next()))->numeric_to_str();
                if (comp1 >= comp2)
                {
                    return mal_true;
                }
                else
                {
                    return mal_false;
                }
            }
            break;
        case MAL_FRACTIONAL:
            {
                auto comp1 = dynamic_cast<MalFractional*>(&(*car.next()))->numeric_to_str();
                auto comp2 = dynamic_cast<MalFractional*>(&(*cdr.next()))->numeric_to_str();
                if (comp1 >= comp2)
                {
                    return mal_true;
                }
                else
                {
                    return mal_false;
                }
            }
            break;

        default:
            throw new NonNumericComparisonException(car.next()->to_str(), cdr.next()->to_str());
    }
});

Procedure mal_less_equal([](Reader& tokens)->Reader
{
    PairPtr car, cdr;
    car.append(tokens.car());
    cdr.append(tokens.cdr());

    if (!is_mal_numeric(car.peek()->type()))
    {
        throw new NonNumericComparisonException(car.next()->to_str(), cdr.next()->to_str());
    }


    PairPtr mal_true, mal_false;
    mal_true.append(std::make_shared<MalBoolean>("true"));
    mal_false.append(std::make_shared<MalBoolean>("false"));

    if (car.peek()->type() != cdr.peek()->type())
    {
        return mal_false;
    }


    switch (car.peek()->type())
    {
        case MAL_INTEGER:
            {
                auto comp1 = dynamic_cast<MalInteger*>(&(*car.next()))->numeric_to_str();
                auto comp2 = dynamic_cast<MalInteger*>(&(*cdr.next()))->numeric_to_str();
                if (comp1 <= comp2)
                {
                    return mal_true;
                }
                else
                {
                    return mal_false;
                }
            }
            break;
        case MAL_RATIONAL:
            {
                auto comp1 = dynamic_cast<MalRational*>(&(*car.next()))->numeric_to_str();
                auto comp2 = dynamic_cast<MalRational*>(&(*cdr.next()))->numeric_to_str();
                if (comp1 <= comp2)
                {
                    return mal_true;
                }
                else
                {
                    return mal_false;
                }
            }
            break;
        case MAL_FRACTIONAL:
            {
                auto comp1 = dynamic_cast<MalFractional*>(&(*car.next()))->numeric_to_str();
                auto comp2 = dynamic_cast<MalFractional*>(&(*cdr.next()))->numeric_to_str();
                if (comp1 <= comp2)
                {
                    return mal_true;
                }
                else
                {
                    return mal_false;
                }
            }
            break;
        default:
            throw new NonNumericComparisonException(car.next()->to_str(), cdr.next()->to_str());
    }
});


// list operators

Procedure mal_list([](Reader& tokens)->Reader
{
    auto list =  std::make_shared<MalList>(tokens);
    PairPtr result;
    result.append(list);
    return result;
});

Procedure mal_count([](Reader& tokens)->Reader
{
    PairPtr result;
    if (tokens.peek()->type() == MAL_LIST || tokens.peek()->type() == MAL_VECTOR)
    {
        auto size = tokens.next()->raw_to_str().size();
        auto count = std::make_shared<MalInteger>(size);
        result.append(count);
    }
    else
    {
        auto count = std::make_shared<MalInteger>(0);
        result.append(count);
    }

    return result;
});


Procedure mal_cons([](Reader& tokens)->Reader
{
    PairPtr temp, result;

    temp.append(tokens.next());
    auto type = tokens.peek()->type();
    if (type == MAL_LIST)
    {
        auto elements = tokens.next()->raw_to_str();
        for (auto element = elements.next(); element != nullptr; element = elements.next())
        {
            temp.append(element);
        }
        result.append(std::make_shared<MalList>(temp));
    }
    else if (type == MAL_VECTOR)
    {
        auto elements = tokens.next()->raw_to_str();
        for (auto element = elements.next(); element != nullptr; element = elements.next())
        {
            temp.append(element);
        }
        result.append(std::make_shared<MalVector>(temp));
    }
    else
    {
        throw new InvalidConsPairException(tokens.peek()->to_str());
    }

    return result;
});

Procedure mal_car([](Reader& tokens)->Reader
{
    auto type = tokens.peek()->type();
    if (is_mal_container(type))
    {
        PairPtr result;
        result.append(tokens.next()->raw_to_str().car());
        return result;
    }
    else
    {
        throw new InvalidConsPairException(tokens.peek()->to_str());
    }
});


Procedure mal_cdr([](Reader& tokens)->Reader
{
    auto type = tokens.peek()->type();
    if (is_mal_container(type))
    {
        PairPtr result, collector;
        auto elements = tokens.next()->raw_to_str();
        elements.next();                // discard car value
        for (auto element = elements.next(); element != nullptr; element = elements.next())
        {
            collector.append(element);
        }
        result.append(std::make_shared<MalList>(collector));
        return result;
    }
    else
    {
        throw new InvalidConsPairException(tokens.peek()->to_str());
    }
});

Procedure mal_is_empty([](Reader& tokens)->Reader
{
    PairPtr mal_true, mal_false;
    mal_true.append(std::make_shared<MalBoolean>("true"));
    mal_false.append(std::make_shared<MalBoolean>("false"));

    if (is_mal_container(tokens.peek()->type()))
    {
        if (tokens.peek()->type() == MAL_HASHMAP)
        {
            // WARNING: This function uses downcasting of a pointer from it's parent class to the
            // actual subclass. This is VERY questionable, and if possible a better solution should be found!
            HashMapInternal hm((dynamic_cast<MalHashmap*>(&(*tokens.next())))->internal_map());
            if (hm.size() == 0)
            {
                return mal_true;
            }
        }
        else
        {
            if (tokens.next()->raw_to_str().size() == 0)
            {
                return mal_true;
            }
        }
    }
    return mal_false;
});


// type predicates

Procedure mal_is_list([](Reader& tokens)->Reader
{
    PairPtr mal_true, mal_false;
    mal_true.append(std::make_shared<MalBoolean>("true"));
    mal_false.append(std::make_shared<MalBoolean>("false"));

    if (tokens.peek()->type() == MAL_LIST || tokens.peek()->type() == MAL_NULL)
    {
        return mal_true;
    }
    else
    {
        return mal_false;
    }
});


Procedure mal_is_number([](Reader& tokens)->Reader
{
    PairPtr mal_true, mal_false;
    mal_true.append(std::make_shared<MalBoolean>("true"));
    mal_false.append(std::make_shared<MalBoolean>("false"));

    if (is_mal_numeric(tokens.next()->type()))
    {
        return mal_true;
    }
    else
    {
        return mal_false;
    }
});


// printing

std::string filter_escapes(std::string source, bool complete = false)
{
    std::string s = "";

    if (source.empty())
    {
        return s;
    }

    for (size_t i = 0; i < source.size(); ++i)
    {
        char ch = source[i];

        if (ch == '\\')
        {
            if (complete)
            {
                i++;
                ch = source[i];
                if (ch == '\"' || ch == '\\' || ch == '\'')
                {
                    s += ch;
                }
                else if (ch == 'n')
                {
                    s += '\n';
                }
                else
                {
                    throw new UnbalancedStringException();
                }
            }
            else
            {
                i++;
                ch = source[i];
                if (ch == '\"' || ch == '\\' || ch == '\'')
                {
                    s += '\\';
                    s += ch;
                }
                else if (ch == 'n')
                {
                    s += '\\';
                    s += 'n';
                }
                else
                {
                    throw new UnbalancedStringException();
                }
            }
        }
        else if (ch != '\"')
        {
            s += ch;
        }
    }

    return s;
}



Procedure mal_str([](Reader& tokens)->Reader
{
    std::string s = "";

    if (tokens.size() > 0)
    {
        s += pr_str(tokens, false);
    }
    PairPtr str;
    str.append(std::make_shared<MalString>(s));
    return str;
});



Procedure mal_pr_str([](Reader& tokens)->Reader
{
    std::string s = "";

    if (tokens.size() > 0)
    {
        for (auto token = tokens.next(); token != nullptr; token = tokens.next())
        {
            PairPtr t;
            t.append(token);
            s += pr_str(t, true);
            if (tokens.peek() != nullptr)
            {
                s += " ";
            }
        }
    }

    PairPtr str;

    str.append(std::make_shared<MalString>(s));
    return str;
});


Procedure mal_prn([](Reader& tokens)->Reader
{
    if (tokens.size() > 0)
    {
        std::cout << mal_pr_str(tokens).values();
    }

    std::cout << '\n';

    PairPtr mal_nil;
    mal_nil.append(std::make_shared<MalNil>());
    return mal_nil;
});


Procedure mal_println([](Reader& tokens)->Reader
{
    if (tokens.values().length() != 0)
    {
        for (size_t i = 0; i < tokens.size(); ++i)
        {
            PairPtr temp;
            temp.append(tokens[i]);
            auto str = mal_str(temp);
            std::string s = str.values();
            std::cout << s;
            if (i < tokens.size()-1)
            {
                std::cout << " ";
            }
            temp.clear();
        }
    }
    std::cout << '\n';

    PairPtr mal_nil;
    mal_nil.append(std::make_shared<MalNil>());
    return mal_nil;
});
 */


void init_global_environment()
{
    // basic arithmetic
    repl_env.set(std::make_shared<Env_Primitive>(std::make_shared<MalSymbol>("+"), mal_plus, -2));
/*     repl_env.set(std::make_shared<Env_Primitive>(std::make_shared<MalSymbol>("-"), mal_minus, -1));
    repl_env.set(std::make_shared<Env_Primitive>(std::make_shared<MalSymbol>("*"), mal_multiply, -2));
    repl_env.set(std::make_shared<Env_Primitive>(std::make_shared<MalSymbol>("/"), mal_divide, 2));
    repl_env.set(std::make_shared<Env_Primitive>(std::make_shared<MalSymbol>("%"), mal_modulo, 2));

    // comparisons
    repl_env.set(std::make_shared<Env_Primitive>(std::make_shared<MalSymbol>("="), mal_equal, 2));
    repl_env.set(std::make_shared<Env_Primitive>(std::make_shared<MalSymbol>(">"), mal_greater_than, 2));
    repl_env.set(std::make_shared<Env_Primitive>(std::make_shared<MalSymbol>("<"), mal_less_than, 2));
    repl_env.set(std::make_shared<Env_Primitive>(std::make_shared<MalSymbol>(">="), mal_greater_equal, 2));
    repl_env.set(std::make_shared<Env_Primitive>(std::make_shared<MalSymbol>("<="), mal_less_equal, 2));
    repl_env.set(std::make_shared<Env_Primitive>(std::make_shared<MalSymbol>("!="), mal_not_equal, 2));

    // list manipulation
    repl_env.set(std::make_shared<Env_Primitive>(std::make_shared<MalSymbol>("list"), mal_list, -1));
    repl_env.set(std::make_shared<Env_Primitive>(std::make_shared<MalSymbol>("count"), mal_count, 1));
    repl_env.set(std::make_shared<Env_Primitive>(std::make_shared<MalSymbol>("cons"), mal_cons, 2));
    repl_env.set(std::make_shared<Env_Primitive>(std::make_shared<MalSymbol>("car"), mal_car, 1));
    repl_env.set(std::make_shared<Env_Primitive>(std::make_shared<MalSymbol>("cdr"), mal_cdr, 1));
    repl_env.set(std::make_shared<Env_Primitive>(std::make_shared<MalSymbol>("empty?"), mal_is_empty, 1));

    // type predicates
    repl_env.set(std::make_shared<Env_Primitive>(std::make_shared<MalSymbol>("list?"), mal_is_list, 1));
    repl_env.set(std::make_shared<Env_Primitive>(std::make_shared<MalSymbol>("number?"), mal_is_number, 1));

    // printing values
    repl_env.set(std::make_shared<Env_Primitive>(std::make_shared<MalSymbol>("prn"), mal_prn, -1));
    repl_env.set(std::make_shared<Env_Primitive>(std::make_shared<MalSymbol>("str"), mal_str, -1));
    repl_env.set(std::make_shared<Env_Primitive>(std::make_shared<MalSymbol>("pr-str"), mal_pr_str, -1));
    repl_env.set(std::make_shared<Env_Primitive>(std::make_shared<MalSymbol>("println"), mal_println, -1)); */
}
