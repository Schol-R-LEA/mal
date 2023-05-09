#include <string>
#include <iostream>
#include <memory>
#include <map>
#include "types.h"
#include "exceptions.h"


size_t TokenVector::append(MalPtr token)
{
    tokens.push_back(token);

    return this->size();
}


size_t TokenVector::append(const TokenVector& t)
{
    for (std::vector<MalPtr>::const_iterator it = t.tokens.cbegin(); it != t.tokens.cend(); ++it)
    {
        this->tokens.push_back(*it);
    }

    return this->size();
}


MalPtr TokenVector::operator[](unsigned int i)
{
    return tokens[i];
}


std::string TokenVector::values()
{
    std::string s = "";

    for (std::vector<MalPtr>::iterator it = tokens.begin(); it != tokens.end(); ++it)
    {
        if (it != tokens.begin() && !(it->get()->type() == "Comma"))
        {
            s+= " ";
        }

        // if (it->get()->type() == "Quote")
        // {
        //     s += "(quote " + it->get()->value() + ")";
        // }
        // else if (it->get()->type() == "Quasiquote")
        // {
        //     s += "(quasiquote " + it->get()->value() + ")";
        // }
        // else if (it->get()->type() == "Comma")
        // {
        //     continue;
        //     // s += "(unquote " + it->get()->value() + ")";
        // }
        // else if (it->get()->type() == "Splice")
        // {
        //     s += "(splice " + it->get()->value() + ")";
        // }
        // else if (it->get()->type() == "Unsplice")
        // {
        //     s += "(unsplice " + it->get()->value() + ")";
        // }
        // else if (it->get()->type() == "Deref")
        // {
        //     s += "(deref " + it->get()->value() + ")";
        // }
        // else if (it->get()->type() == "Unquote")
        // {
        //     s += "(unquote " + it->get()->value() + ")";
        // }
        // else if (it->get()->type() == "Splice-unquote")
        // {
        //     s += "(splice-unquote " + it->get()->value() + ")";
        // }
        // else if (it->get()->type() == "Meta")
        // {
        //     if ((it+1) != tokens.end())
        //     {
        //         s += "(with-meta " + (it+1)->get()->value() + ' ' + it->get()->value() + ")";
        //         it++;
        //     }
        //     else
        //     {
        //         throw new InvalidMetaException();
        //     }
        // }
        // else
        {
            s += it->get()->value();
        }
    }
    return s;
}


std::string TokenVector::types()
{
    std::string s = "";

    for (std::vector<MalPtr>::iterator it = tokens.begin(); it != tokens.end(); ++it)
    {
        if (it != tokens.begin())
        {
            s += " ";
        }
        if (it->get()->type() == "List")
        {
            s += "List (" + it->get()->raw_value().types() + ")";
        }
        else if (it->get()->type() == "Vector")
        {
            s += "Vector [" + it->get()->raw_value().types() + "]";
        }
        else
        {
            s += it->get()->type();
        }
    }
    return s;
}



MalList::MalList(const TokenVector& l): MalType("{list}")
{
    list.append(const_cast<TokenVector&>(l));
}



MalVector::MalVector(const TokenVector& v): MalType("{vector}")
{
    vec.append(v);
}


MalHashmap::MalHashmap(TokenVector hm): MalType("{hash}")
{
    if (hm.size() > 0)
    {
        if (hm.size() % 2)
        {
            throw new InvalidHashmapException();
        }
        for (unsigned int i = 0; i < hm.size()-1; i+=2)
        {
            if (hm[i]->type() == "String" || hm[i]->type() == "Keyword")
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


std::string MalHashmap::value()
{
    std::string s = "{";

    for(auto it = hashmap.begin(); it != hashmap.end(); ++it)
    {
        if (it != hashmap.begin())
        {
            s += " ";
        }
        s += it->first + " ";

        if (it->second->type() == "Hash Map")
        {
            s += it->second->value();
        }
        else
        {
            s += it->second->value();
        }
    }
    s += '}';

    return s;
}


// std::string MalMeta::value()
// {
//     if (list.size() == 2)
//     {
//         return "(with-meta " + list[1]->value() + " " + list[0]->value() + ')';
//     }
//     else
//     {
//         throw new InvalidMetaException();
//     }
// }