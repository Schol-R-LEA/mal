#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <string>



class InvalidConversionException: public std::exception
{
public:
    InvalidConversionException(std::string s, std::string c): source(s), conversion(c) {};
    virtual const char* what() const noexcept { return ("Cannot convert " + source + " to " + conversion).c_str(); };
protected:
    std::string source, conversion;
};


class UnbalancedParenthesesException: public std::exception
{
public:
    UnbalancedParenthesesException() {};
};


class UnbalancedVectorException: public std::exception
{
public:
    UnbalancedVectorException() {};
};


class UnbalancedStringException: public std::exception
{
public:
    UnbalancedStringException() {};
};


class UnbalancedHashmapException: public std::exception
{
public:
    UnbalancedHashmapException() {};
};


class InvalidTokenStreamException: public std::exception
{
public:
    InvalidTokenStreamException() {};
};


class IncompleteComplexNumberException: public std::exception
{
public:
    IncompleteComplexNumberException() {};
};

class InvalidNumberException: public std::exception
{
public:
    InvalidNumberException(std::string v): number_value(v) {};
    virtual const char* what() const noexcept { return number_value.c_str(); };
protected:
    std::string number_value;
};

class InvalidBinaryNumberException: public InvalidNumberException
{
public:
    InvalidBinaryNumberException(std::string v): InvalidNumberException(v) {};
};

class InvalidOctalNumberException: public InvalidNumberException
{
public:
    InvalidOctalNumberException(std::string v): InvalidNumberException(v) {};
};

class InvalidHexNumberException: public InvalidNumberException
{
public:
    InvalidHexNumberException(std::string v): InvalidNumberException(v) {};
};


class InvalidIntegerException: public InvalidNumberException
{
public:
    InvalidIntegerException(std::string v): InvalidNumberException(v) {};
};

class InvalidRationalNumberException: public InvalidNumberException
{
public:
    InvalidRationalNumberException(std::string v): InvalidNumberException(v) {};
};

class InvalidFractionalNumberException: public InvalidNumberException
{
public:
    InvalidFractionalNumberException(std::string v): InvalidNumberException(v) {};
};

class InvalidComplexNumberException: public InvalidNumberException
{
public:
    InvalidComplexNumberException(std::string v): InvalidNumberException(v) {};
};

class InvalidHashmapException: public std::exception
{
public:
    InvalidHashmapException() {};
};


class IncompleteEscapeException: public std::exception
{
public:
    IncompleteEscapeException() {};
};


class InvalidMetaException: public std::exception
{
public:
    InvalidMetaException() {};
};


class InvalidEnvironmentSymbolException: public std::exception
{
public:
    InvalidEnvironmentSymbolException(std::string sym): symbol_value(sym) {};
    virtual const char* what() const noexcept { return symbol_value.c_str(); };
protected:
    std::string symbol_value;
};

class InvalidPrimitiveException: public std::exception
{
public:
    InvalidPrimitiveException() {};
};


class InvalidFunctionArgumentException: public std::exception
{
public:
    InvalidFunctionArgumentException(std::string sym): symbol_value(sym) {};
    virtual const char* what() const noexcept { return symbol_value.c_str(); };
protected:
    std::string symbol_value;
};


class MissingFunctionArgumentException: public std::exception
{
public:
    MissingFunctionArgumentException() {};
};



class TooManyInputsException: public std::exception
{
public:
    TooManyInputsException() {};
};


class ArityMismatchException: public std::exception
{
public:
    ArityMismatchException() {};
};


class ApplyingNonFunctionException: public std::exception
{
public:
    ApplyingNonFunctionException(std::string sym): symbol_value(sym) {};
    virtual const char* what() const noexcept { return symbol_value.c_str(); };
protected:
    std::string symbol_value;
};


class ProcedureNotFoundException: public std::exception
{
public:
    ProcedureNotFoundException(std::string sym): symbol_value(sym) {};
    virtual const char* what() const noexcept { return symbol_value.c_str(); };
protected:
    std::string symbol_value;
};


class SymbolNotInitializedException: public std::exception
{
public:
    SymbolNotInitializedException(std::string sym): symbol_value(sym) {};
    virtual const char* what() const noexcept { return symbol_value.c_str(); };
protected:
    std::string symbol_value;
};


class InvalidDefineException: public std::exception
{
public:
    InvalidDefineException(std::string sym): symbol_value(sym) {};
    virtual const char* what() const noexcept { return symbol_value.c_str(); };
protected:
    std::string symbol_value;
};


class InvalidLetException: public std::exception
{
public:
    InvalidLetException(std::string sym): symbol_value(sym) {};
    virtual const char* what() const noexcept { return symbol_value.c_str(); };
protected:
    std::string symbol_value;
};


class UnequalBindExprListsException: public std::exception
{
public:
    UnequalBindExprListsException(std::string b, std::string e): binds(b), exprs(e) {};
    virtual const char* what() const noexcept { return (binds + ": " + exprs).c_str(); };
protected:
    std::string binds, exprs;
};


class InvalidBindExprListsException: public std::exception
{
public:
    InvalidBindExprListsException(std::string b, std::string e): binds(b), exprs(e) {};
    virtual const char* what() const noexcept { return (binds + ": " + exprs).c_str(); };
protected:
    std::string binds, exprs;
};


class NonNumericComparisonException: public std::exception
{
public:
    NonNumericComparisonException(std::string b, std::string e): car(b), cdr(e) {};
    virtual const char* what() const noexcept { return (car + ": " + cdr).c_str(); };
protected:
    std::string car, cdr;
};



class NullTokenException: public std::exception
{
public:
    NullTokenException() {};
};


class InvalidConsPairException: public std::exception
{
public:
    InvalidConsPairException(std::string p): pair(p) {};
    virtual const char* what() const noexcept { return pair.c_str(); };
protected:
    std::string pair;
};


class IndexOutOfBoundsException: public std::exception
{
public:
    IndexOutOfBoundsException(std::string size, std::string index): m_size(size), m_index(index) {};
    virtual const char* what() const noexcept { return (m_size + " : " + m_index).c_str(); };
protected:
    std::string m_size, m_index;
};


class ImproperListException: public std::exception
{
public:
    ImproperListException(std::string p): pair(p) {};
    virtual const char* what() const noexcept { return pair.c_str(); };
protected:
    std::string pair;
};


#endif
