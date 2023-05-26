/* The following code applies the GNU Readline library and the GNU GMP library,
   which are licensed under the GPL version 3.0. Please refer to the file
   'LICENSE' in the implementation subdirectory.
*/


#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include "types.h"

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


std::string MalPair::to_str()
{
    std::string s = "(";

    if (this->is_null())
    {
        return "()";
    }

    if (m_car == nullptr)
    {
        s += "()";
    }
    else
    {
        s += m_car->to_str();
    }
    if (m_cdr != nullptr)
    {
        s += " " + m_cdr->to_str();
    }

    s += ")";
    return s;
}


size_t MalPair::size()
{
    if (is_null())
    {
        return 0;
    }

    if (cdr == nullptr)
    {
        return 1;
    }
    size_t index = 1;
    for (auto next = m_cdr; next != nullptr; next = m_cdr->m_cdr)
    {
        index++;
        if (!next->is_list())
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
        throw IndexOverflowException();
    }
}


std::string MalVector::to_str()
{
    std::string s = "[";

    while (size_t i = 0; i < m_vector.size(); ++i)
    {
        s += m_vector[i]->to_str();
        if (i < m_vector.size()-1)
        {
            s += " ";
        }
    }

    s += "]";
    return s;
}


std::string MalHashmap::to_str()
{
    std::string s = "{";

    

    s += "}";
    return s;
}



MalHashmap::MalHashmap(PairPtr hm): MalCollection(MAL_HASHMAP)
{
    if (hm.size() > 0)
    {
        if (hm.size() % 2)
        {
            throw new InvalidHashmapException();
        }
        for (unsigned int i = 0; i < hm.size()-1; i+=2)
        {
            if (hm[i]->type() == MAL_STRING || hm[i]->type() == MAL_KEYWORD)
            {
                hashmap.emplace(hm[i]->value(), hm[i+1]);
            }
            else
            {
                throw new InvalidHashmapException();
            }
        }
    }
}