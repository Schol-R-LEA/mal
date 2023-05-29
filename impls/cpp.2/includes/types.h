#ifndef MAL_TYPES_H
#define MAL_TYPES_H


/* The following code applies the GNU Readline library and the GNU GMP library,
   which are licensed under the GPL version 3.0. Please refer to the file
   'LICENSE' in the implementation subdirectory.
*/

#include <complex>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <gmpxx.h>
#include "exceptions.h"


class Environment;
class Env_Symbol;

typedef std::shared_ptr<Env_Symbol> EnvSymbolPtr;
typedef std::shared_ptr<Environment> EnvPtr;


enum MalType
{
    MAL_OBJECT,
    MAL_ATOM, MAL_SYMBOL, MAL_KEYWORD,
    MAL_STRING, MAL_BOOLEAN,
    MAL_COLLECTION, MAL_PAIR, MAL_VECTOR, MAL_HASHMAP,
    Mal_NUMBER, MAL_INTEGER, MAL_FRACTIONAL, MAL_RATIONAL, MAL_COMPLEX,
    MAL_PROCEDURE, MAL_PRIMITIVE, MAL_REST_ARG,
    MAL_PERIOD, MAL_COMMA,
    MAL_READER_MACRO, MAL_QUOTE, MAL_QUASIQUOTE,
    MAL_UNQUOTE, MAL_SPLICE_UNQUOTE, MAL_DEREF,
    MAL_META
};

extern std::string mal_type_name[];


class MalObject;
class MalPair;
class MalVector;
class MalHashmap;
class MalAtom;
class MalBoolean;
class MalSymbol;
class MalKeyword;
class MalString;
class MalInteger;
class MalProcedure;

typedef std::shared_ptr<MalObject> MalPtr;
typedef std::shared_ptr<MalPair> PairPtr;

// types for the internal representations for collection classes
typedef std::vector<MalPtr> InternalVector;

typedef std::unordered_map<MalPtr, MalPtr> InternalHashmap;


// Complex type
typedef std::complex<mpf_class> complex_mpf;



class MalObject
{
public:
    MalObject(MalType type): m_type(type) {};
    virtual std::string to_str() {return "";};
    MalType type() {return m_type;};
    std::string type_name() {return mal_type_name[m_type];};
    virtual size_t size() {return 1;};

    // type predicates
    virtual bool is_atom() {return false;};
    virtual bool is_symbol() {return false;};
    virtual bool is_keyword() {return false;};
    virtual bool is_string() {return false;};
    virtual bool is_boolean() {return false;};
    virtual bool is_collection() {return false;};
    virtual bool is_null() {return false;};
    virtual bool is_pair() {return false;};
    virtual bool is_list() {return false;};
    virtual bool is_vector() {return false;};
    virtual bool is_hashmap() {return false;};
    virtual bool is_number() {return false;};
    virtual bool is_integer() {return false;};
    virtual bool is_rational() {return false;};
    virtual bool is_fractional() {return false;};
    virtual bool is_complex() {return false;};
    virtual bool is_procedure() {return false;};
    virtual bool is_syntax() {return false;};

    // return values of the subclasses
    virtual std::string as_string() {throw InvalidConversionException(mal_type_name[this->m_type], mal_type_name[MAL_STRING]); return "";};
    virtual std::string as_keyword() {throw InvalidConversionException(mal_type_name[m_type], mal_type_name[MAL_KEYWORD]); return "";};
    virtual std::string as_symbol() {throw InvalidConversionException(mal_type_name[m_type], mal_type_name[MAL_SYMBOL]); return "";};
    virtual PairPtr as_pair() {throw InvalidConversionException(mal_type_name[m_type], mal_type_name[MAL_PAIR]); return nullptr;};
    virtual InternalVector as_vector() {throw InvalidConversionException(mal_type_name[m_type], mal_type_name[MAL_VECTOR]); InternalVector v; return v;};
    virtual InternalHashmap as_hashmap() {throw InvalidConversionException(mal_type_name[m_type], mal_type_name[MAL_HASHMAP]); InternalHashmap hm; return hm;};
    virtual bool as_boolean() {throw InvalidConversionException(mal_type_name[m_type], mal_type_name[MAL_BOOLEAN]); return false;};
    virtual mpz_class as_integer() {throw InvalidConversionException(mal_type_name[m_type], mal_type_name[MAL_INTEGER]); return 0;};
    virtual mpq_class as_rational() {throw InvalidConversionException(mal_type_name[m_type], mal_type_name[MAL_INTEGER]); return 0;};
    virtual mpf_class as_fractional() {throw InvalidConversionException(mal_type_name[m_type], mal_type_name[MAL_INTEGER]); return mpf_class(0);};
    virtual complex_mpf as_complex() {throw InvalidConversionException(mal_type_name[m_type], mal_type_name[MAL_INTEGER]); return complex_mpf(0, 0);};
    virtual MalPtr as_procedure() {throw InvalidConversionException(mal_type_name[m_type], mal_type_name[MAL_PROCEDURE]); return nullptr;};
protected:
    MalType m_type;
};



class MalAtom: public MalObject
{
public:
    MalAtom(MalType type): MalObject(type) {};
    virtual bool is_atom() {return true;};
};


class MalSymbol: public MalAtom
{
public:
    MalSymbol(std::string sym): MalAtom(MAL_SYMBOL), m_symbol(sym) {};
    virtual std::string to_str() {return m_symbol;};
    virtual bool is_symbol() {return true;};
    virtual std::string as_symbol() {return m_symbol;};

protected:
    std::string m_symbol;
};


class MalKeyword: public MalAtom
{
public:
    MalKeyword(std::string kw): MalAtom(MAL_KEYWORD), m_keyword(kw) {};
    virtual std::string to_str() {return ":" + m_keyword;};
    virtual bool is_keyword() {return true;};
    virtual std::string as_keyword() {return this->to_str();};
protected:
    std::string m_keyword;
};



class MalString: public MalAtom
{
public:
    MalString(std::string str): MalAtom(MAL_STRING), m_string(str) {};
    virtual std::string to_str() {return m_string;};
    virtual bool is_string() {return true;};
    virtual std::string as_string() {return m_string;};
protected:
    std::string m_string;
};


class MalBoolean: public MalAtom
{
public:
    MalBoolean(bool tof): MalAtom(MAL_BOOLEAN), m_boolean(tof) {};
    virtual std::string to_str() {return m_boolean ? "true" : "false";};
    virtual bool is_boolean() {return true;};
protected:
    bool m_boolean;
};


class MalCollection: public MalObject
{
public:
    MalCollection(MalType type): MalObject(type) {};
    virtual bool is_collection() {return true;};
};


class MalPair: public MalCollection, public std::enable_shared_from_this<MalPair>
{
public:
    MalPair(MalPtr car=nullptr, MalPtr cdr=nullptr): MalCollection(MAL_PAIR), m_car(car), m_cdr(cdr) {};
    virtual std::string to_str();
    virtual std::string to_str_continued();
    virtual bool is_null() {return (m_car == nullptr && m_cdr == nullptr);};
    virtual bool is_pair() {return true;};
    virtual bool is_list() {return (m_cdr == nullptr || m_cdr->is_pair());};
    virtual PairPtr as_pair() {return shared_from_this();};
    virtual size_t size();
    virtual MalPtr operator[](size_t index);
    virtual MalPtr car() {return m_car;};
    virtual MalPtr cdr() {return m_cdr;};
    virtual void add(MalPtr addition);       // inserts an element at the end of the list
protected:
    MalPtr m_car, m_cdr;
};


class MalVector: public MalCollection
{
public:
    MalVector(): MalCollection(MAL_VECTOR) {};
    MalVector(PairPtr value_list);
    MalVector(const InternalVector& value_list);
    virtual std::string to_str();
    virtual size_t size() {return m_vector.size();};
    virtual bool is_vector() {return true;};
protected:
    InternalVector m_vector;
};


class MalHashmap: public MalCollection
{
public:
    MalHashmap(): MalCollection(MAL_HASHMAP) {};
    MalHashmap(PairPtr value_list);
    virtual std::string to_str();
    virtual size_t size() {return m_hashmap.size();};
    virtual bool is_hashmap() {return true;};
protected:
    InternalHashmap m_hashmap;
};



class MalNumber: public MalAtom
{
public:
    MalNumber(MalType type): MalAtom(type) {};
    virtual bool is_number() {return true;};
};


class MalInteger: public MalNumber
{
public:
    MalInteger(mpz_class value): MalNumber(MAL_INTEGER), m_value(value) {};
    virtual bool is_integer() {return true;};
    virtual std::string to_str() {return m_value.get_str();};
    virtual mpz_class as_integer() {return m_value;};
protected:
    mpz_class m_value;
};


class MalRational: public MalNumber
{
public:
    MalRational(mpq_class value): MalNumber(MAL_RATIONAL), m_value(value) {};
    virtual bool is_rational() {return true;};
    virtual std::string to_str() {return m_value.get_str();};
    virtual mpq_class as_rational() {return m_value;};
protected:
    mpq_class m_value;
};


class MalFractional: public MalNumber
{
public:
    MalFractional(mpf_class value): MalNumber(MAL_FRACTIONAL), m_value(value) {};
    virtual bool is_fractional() {return true;};
    virtual std::string to_str();
    virtual mpf_class as_fractional() {return m_value;};
protected:
    mpf_class m_value;
};


class MalComplex: public MalNumber
{
public:
    MalComplex(complex_mpf value): MalNumber(MAL_COMPLEX), m_value(value) {};
    virtual bool is_complex() {return true;};
    virtual std::string to_str();
    virtual complex_mpf as_complex() {return m_value;};
protected:
    complex_mpf m_value;
};


class MalProcedure: public MalCollection
{
public:
    MalProcedure(PairPtr code, PairPtr parameters, EnvPtr parent, int arity): MalCollection(MAL_PROCEDURE), m_code(code), m_parameters(parameters), m_parent(parent), m_arity(arity) {};
    virtual bool is_procedure() {return true;};
    virtual PairPtr ast() {return m_code;};
    virtual PairPtr params() {return m_parameters;};
    virtual EnvPtr parent() {return m_parent;};
    virtual int arity() { return m_arity;};

protected:
    PairPtr m_code;
    PairPtr m_parameters;
    EnvPtr m_parent;
    int m_arity;
};



class MalPrimitive: public MalCollection
{
public:
    MalPrimitive(MalSymbol name, int arity): MalCollection(MAL_PRIMITIVE), m_name(name), m_arity(arity) {};
    virtual bool is_procedure() {return true;};
    virtual int arity() { return m_arity;};

protected:
    MalSymbol m_name;
    int m_arity;
};



class MalRestArg: public MalObject
{
public:
    MalRestArg(): MalObject(MAL_REST_ARG) {};
    virtual bool is_syntax() {return true;};
};



class MalPeriod: public MalObject
{
public:
    MalPeriod(): MalObject(MAL_PERIOD) {};
    virtual std::string to_str() {return ".";};
    virtual bool is_syntax() {return true;};
};



class MalComma: public MalObject
{
public:
    MalComma(): MalObject(MAL_COMMA) {};
    virtual std::string to_str() {return ",";};
    virtual bool is_syntax() {return true;};
};



class MalReaderMacro: public MalObject
{
public:
    MalReaderMacro(MalType type, MalPtr arg): MalObject(type), m_arg(arg) {};
    virtual std::string to_str() {return "<reader-macro>";};
    virtual bool is_syntax() {return true;};
protected:
    MalPtr m_arg;
};


class MalQuote: public MalReaderMacro
{
public:
    MalQuote(MalPtr arg): MalReaderMacro(MAL_QUOTE, arg) {};
    virtual std::string to_str() {return "(quote " + m_arg->to_str()  + ")";};
    virtual bool is_syntax() {return true;};
};



class MalQuasiquote: public MalReaderMacro
{
public:
    MalQuasiquote(MalPtr arg): MalReaderMacro(MAL_QUASIQUOTE, arg) {};
    virtual std::string to_str() {return "(quote " + m_arg->to_str()  + ")";};
    virtual bool is_syntax() {return true;};
};



class MalUnquote: public MalReaderMacro
{
public:
    MalUnquote(MalPtr arg): MalReaderMacro(MAL_UNQUOTE, arg) {};
    virtual std::string to_str() {return "(quote " + m_arg->to_str()  + ")";};
    virtual bool is_syntax() {return true;};
};



class MalSpliceUnquote: public MalReaderMacro
{
public:
    MalSpliceUnquote(MalPtr arg): MalReaderMacro(MAL_SPLICE_UNQUOTE, arg) {};
    virtual std::string to_str() {return "(quote " + m_arg->to_str()  + ")";};
    virtual bool is_syntax() {return true;};
};



class MalDeref: public MalReaderMacro
{
public:
    MalDeref(MalPtr arg): MalReaderMacro(MAL_DEREF, arg) {};
    virtual std::string to_str() {return "(quote " + m_arg->to_str()  + ")";};
    virtual bool is_syntax() {return true;};
};



class MalMeta: public MalReaderMacro
{
public:
    MalMeta(MalPtr applicant, MalPtr meta): MalReaderMacro(MAL_META, meta), m_applicant(applicant) {};
    virtual std::string to_str() {return "(with-meta " + m_applicant->to_str() + " " + m_arg->to_str()  + ")";};
    virtual bool is_syntax() {return true;};
protected:
    MalPtr m_applicant;
};


#endif