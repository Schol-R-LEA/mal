/* The following code applies the GNU Readline library and the GNU GMP library,
   which are licensed under the GPL version 3.0. Please refer to the file
   'LICENSE' in the implementation subdirectory.
*/


#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "types.h"
#include "token_types.h"


std::string mal_type_name[] =
{
    "Object",
    "Atom", "Symbol", "Keyword",
    "String", "Boolean",
    "Collection", "Pair", "Vector", "Hashmap",
    "Number", "Integer", "Rational", "Fractional", "Complex",
    "Procedure", "Primitive", "Rest Arguments",
    "Quote", "Quasiquote",
    "Unquote", "Splice-Unquote", "Deref",
    "Meta"
};


std::string MalPair::to_str(bool print_readably)
{
    return "(" + to_str_continued(print_readably) +  ")";
}


std::string MalPair::to_str_continued(bool print_readably)
{
    // handle CAR pointer
    if (m_car == nullptr)
    {
        if (m_cdr == nullptr)
        {
            return "";
        }
        else
        {
            return "()" + m_cdr->to_str(print_readably);
        }
    }
    else if (m_cdr == nullptr)
    {
     return m_car->to_str();
    }
    else if (m_cdr->type() == MAL_PAIR)
    {
        auto cdr_pair = m_cdr->as_pair();
        MalPtr cadr = cdr_pair->car();
        MalPtr cddr = cdr_pair->cdr();
        if (cddr == nullptr)
        {
            if (cadr == nullptr)
            {
                return m_car->to_str(print_readably);
            }
            else
            {
                return m_car->to_str(print_readably) + " " + cadr->to_str();
            }
        }
        else if (cddr->type() == MAL_PAIR)
        {
            if (cadr->type() == MAL_PAIR)
            {
                return m_car->to_str(print_readably) + " . " + m_cdr->to_str(print_readably);
            }
            else
            {
                PairPtr cddr_pair = cddr->as_pair();
                return m_car->to_str()
                       + " " + cadr->to_str(print_readably)
                       + ((cddr_pair->car() == nullptr)
                          ? ""
                          : " " + cddr_pair->to_str_continued());
            }
        }
        else
        {
            if (m_car->type() == MAL_PAIR)
            {
                return m_car->to_str(print_readably)
                    + " . " + m_cdr->to_str(print_readably);
            }
            else
            {
                return m_car->to_str(print_readably)
                    + " " + cadr->to_str(print_readably)
                    + " . "
                    + cddr->to_str(print_readably);
            }
        }
    }
    else
    {
        return m_car->to_str(print_readably) + " . " + m_cdr->to_str(print_readably);
    }
}


size_t MalPair::size()
{
    if (is_null())
    {
        return 0;
    }

    if (m_cdr == nullptr)
    {
        return 1;
    }
    size_t index = 1;
    auto next = m_cdr;
    while (next != nullptr)
    {
        index++;
        if (next->is_list())
        {
            next = next->as_pair()->m_cdr;
        }
        else
        {
            break;
        }
    }

    return index;
}


MalPtr MalPair::operator[](size_t index)
{
    if (size() < index)
    {
        throw IndexOutOfBoundsException(std::to_string(size()), std::to_string(index));
    }

    if (index == 0)
    {
        return m_car;
    }

    size_t count = 1;
    PairPtr next;
    for (next = m_cdr->as_pair(); next != nullptr; next = next->m_cdr->as_pair(), count++)
    {
        if (count == index)
        {
            break;
        }
    }
    return next->car();
}



void MalPair::add(MalPtr addition)
{
    if (m_car == nullptr)
    {
        if (m_cdr == nullptr)
        {
            // insert to the first element of the list
            m_car = addition;
        }
        else if (m_cdr->type() == MAL_PAIR)
        {
            // recurse over the list
            m_cdr->as_pair()->add(addition);
        }
        else
        {
            throw ImproperListException(this->to_str());
        }
    }
    else if (m_cdr == nullptr)
    {
        // insert to the end of the list
        m_cdr = std::make_shared<MalPair>(addition);
    }
    else
    {
        if (m_cdr->type() == MAL_PAIR)
        {
            // recurse over the list
            m_cdr->as_pair()->add(addition);
        }
        else
        {
            throw ImproperListException(this->to_str());
        }
    }
}




MalVector::MalVector(MalPtr value): MalCollection(MAL_VECTOR)
{
    if (value != nullptr)
    {
        switch (value->type())
        {
            case MAL_PAIR:
                {
                    PairPtr value_list = value->as_pair();
                    if (value_list->car() != nullptr)
                    {
                        m_vector.push_back(value_list->car());
                    }

                    if (value_list->cdr() != nullptr && value_list->cdr()->type() == MAL_PAIR)
                    {
                        for (auto next = value_list->cdr()->as_pair(); next != nullptr; next = next->cdr()->as_pair())
                        {
                            if (next->car() != nullptr)
                            {
                                m_vector.push_back(next->car());
                            }

                            if (next->cdr() == nullptr)
                            {
                                break;
                            }
                            else if (next->cdr()->type() != MAL_PAIR)
                            {
                                m_vector.push_back(next->cdr());
                                break;
                            }
                        }
                    }
                }
                break;
            default:
                m_vector.push_back(value);
                break;
        }
    }
}


std::string MalVector::to_str(bool print_readably)
{
    std::string s = "[";

    for (size_t i = 0; i < m_vector.size(); ++i)
    {
        s += m_vector[i]->to_str(print_readably);
        if (i < m_vector.size()-1)
        {
            s += " ";
        }
    }

    s += "]";
    return s;
}


MalHashmap::MalHashmap(MalPtr hm): MalCollection(MAL_HASHMAP)
{
    if (hm == nullptr || hm->type() != MAL_PAIR)
    {
        throw InvalidHashmapException();
    }
    if (hm->size() > 0)
    {
        if (hm->size() % 2)
        {
            throw InvalidHashmapException();
        }

        PairPtr map = hm->as_pair();
        for (unsigned int i = 0; i < hm->size()-1; i+=2)
        {
            MalPtr key = (*map)[i];
            MalPtr value = (*map)[i+1];
            if ((key->type() == MAL_STRING) || (key->type() == MAL_KEYWORD))
            {
                m_hashmap.emplace(key, value);
            }
            else
            {
                throw InvalidHashmapException();
            }
        }
    }
}


std::string MalString::to_str(bool print_readably)
{
    if (print_readably)
    {
        std::string s = "\"";

        for (size_t i = 0; i < m_string.length(); i++)
        {
            char ch = m_string[i];

            switch (ch)
            {
                case '\\':
                case '\"':
                {
                    s += '\\';
                    s += ch;
                }
                    break;
                case '\n':
                {
                    s += '\\';
                    s += 'n';
                }
                    break;
                case '\t':
                    {
                    s += '\\';
                    s += 't';
                    }
                    break;
                default:
                {
                    s += ch;
                }
            }
        }

        return s + "\"";
    }
    else
    {
        return m_string;
    }
}






MalHashmap::MalHashmap(MalPtr key, MalPtr value): MalCollection(MAL_HASHMAP)
{
    if (key == nullptr || value == nullptr)
    {
        throw InvalidHashmapException();
    }

    if ((key->type() == MAL_STRING) || (key->type() == MAL_KEYWORD))
    {
        m_hashmap.emplace(key, value);
    }
    else
    {
        throw InvalidHashmapException();
    }
}


std::string MalHashmap::to_str(bool print_readably)
{
    std::string s = "{";

    for (auto it = m_hashmap.begin(); it != m_hashmap.end(); ++it)
    {
        if (it != m_hashmap.begin())
        {
            s += " ";
        }
        auto key = it->first;
        auto value = it->second;
        if (key->type() == MAL_STRING)
        {
            s += "\"" + key->to_str(print_readably) + "\"";
        }
        else
        {
            s += key->to_str(print_readably);
        }
        s += " ";
        if (value->type() == MAL_STRING)
        {
            s += "\"" + value->to_str(print_readably) + "\"";
        }
        else
        {
            s += value->to_str(print_readably);
        }
         value->to_str(print_readably);
    }

    s += "}";
    return s;
}




std::string MalFractional::to_str(bool print_readably)
{
    if (print_readably) {};  // discard the value of print_readably

    mp_exp_t decimal;
    std::string v = m_value.get_str(decimal);
    if (m_value < 0)
    {
        decimal++;
    }

    std::string s = v.substr(0, decimal) + '.' + (decimal == 0 ? "0" : v.substr(decimal));

    return s;
}



std::string MalComplex::to_str(bool print_readably)
{
    if (print_readably) {};  // discard the value of print_readably

    mp_exp_t real_decimal;
    std::string v = m_value.real().get_str(real_decimal);

    if (m_value.real() < 0)
    {
        real_decimal++;
    }

    std::string real_mantissa = v.substr(real_decimal);

    if (real_mantissa.length() > 0)
    {
        real_mantissa = '.' + real_mantissa;
    }

    std::string real_repr = v.substr(0, real_decimal) + real_mantissa;

    mp_exp_t imag_decimal;
    v = m_value.imag().get_str(imag_decimal);
    if (m_value.imag() < 0)
    {
        imag_decimal++;
    }

    std::string imag_mantissa = v.substr(imag_decimal);

    if (imag_mantissa.length() > 0)
    {
        imag_mantissa = '.' + imag_mantissa;
    }

    std::string imag_repr = v.substr(0, imag_decimal) + imag_mantissa;
    std::string s = real_repr + (m_value.imag() < 0 ? "" : "+") + imag_repr + 'i';

    return s;
}


MalInteger::MalInteger(MalPtr value): MalNumber(MAL_INTEGER)
{
    if (value == nullptr || !value->is_integer())
    {
        throw InvalidIntegerException(value->to_str());
    }
    else
    {
        m_value = value->as_integer();
    }
}


MalRational::MalRational(MalPtr value): MalNumber(MAL_RATIONAL)
{
    if (value == nullptr || !value->is_rational())
    {
        throw InvalidRationalNumberException(value->to_str());
    }
    else
    {
        m_value = value->as_rational();
    }
}


MalFractional::MalFractional(MalPtr value): MalNumber(MAL_FRACTIONAL)
{
    if (value == nullptr || !value->is_fractional())
    {
        throw InvalidFractionalNumberException(value->to_str());
    }
    else
    {
        m_value = value->as_fractional();
    }
}


MalComplex::MalComplex(MalPtr value): MalNumber(MAL_COMPLEX)
{
    if (value == nullptr || !value->is_complex())
    {
        throw InvalidComplexNumberException(value->to_str());
    }
    else
    {
        m_value = value->as_complex();
    }
}