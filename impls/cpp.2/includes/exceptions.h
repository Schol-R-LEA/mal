#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <string>

class UnbalancedParenthesesException
{
public:
    UnbalancedParenthesesException() {};
};


class UnbalancedVectorException
{
public:
    UnbalancedVectorException() {};
};


class UnbalancedStringException
{
public:
    UnbalancedStringException() {};
};


class UnbalancedHashmapException
{
public:
    UnbalancedHashmapException() {};
};


class IncompleteComplexNumberException
{
public:
    IncompleteComplexNumberException() {};
};

class InvalidNumberException
{
public:
    InvalidNumberException(std::string v): number_value(v) {};
    std::string value() const { return number_value; };
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

class InvalidComplexNumberException: public InvalidNumberException
{
public:
    InvalidComplexNumberException(std::string v): InvalidNumberException(v) {};
};

class InvalidHashmapException
{
public:
    InvalidHashmapException() {};
};


class IncompleteEscapeException
{
public:
    IncompleteEscapeException() {};
};


class InvalidMetaException
{
public:
    InvalidMetaException() {};
};


class InvalidPrimitiveException
{
public:
    InvalidPrimitiveException() {};
};


class InvalidFunctionArgumentException
{
public:
    InvalidFunctionArgumentException() {};
};

class TooManyInputsException
{
public:
    TooManyInputsException() {};
};


class ArityMismatchException
{
public:
    ArityMismatchException() {};
};

#endif