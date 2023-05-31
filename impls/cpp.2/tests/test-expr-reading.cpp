#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include <typeinfo>
#include <iostream>
#include <complex>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <gmpxx.h>
#include "exceptions.h"
#include "types.h"


TEST_SUITE("constructing atoms and lists")
{
    TEST_CASE("integer")
    {
        MalInteger test(mpz_class(69));
        CHECK(test.is_atom());
        CHECK(test.is_list() == false);
        CHECK(test.is_number());
        CHECK(test.is_integer());
        CHECK(test.as_integer() == mpz_class(69));
        CHECK(test.to_str() == "69");
    }

    TEST_CASE("rational number")
    {
        MalRational test(mpq_class("23/17"));
        CHECK(test.as_rational() == mpq_class("23/17"));
        CHECK(test.to_str() == "23/17");
    }


    TEST_CASE("floating-point number")
    {
        MalFractional test(mpf_class(42.5));
        CHECK(test.as_fractional() == mpf_class(42.5));
        CHECK(test.to_str() == "42.5");
    }

    TEST_CASE("complex number")
    {
        MalComplex test(complex_mpf(17.69, -23.42));
        CHECK(test.as_complex() == complex_mpf(17.69, -23.42));
        CHECK(test.to_str() == "17.690000000000001279-23.4200000000000017053i");
    }



    TEST_CASE("symbol #1")
    {
        MalSymbol test = MalSymbol("foo");
        CHECK(test.is_atom());
        CHECK(test.is_list() == false);
        CHECK(test.is_number() == false);
        CHECK(test.to_str() == "foo");
    }

    TEST_CASE("symbol #2")
    {
        MalSymbol test = MalSymbol("an-atom");
        CHECK(test.to_str() == "an-atom");
    }

    TEST_CASE("null list")
    {
        MalPair test = MalPair();
        CHECK(test.is_atom() == false);
        CHECK(test.is_list() == true);
        CHECK(test.to_str() == "()");
        CHECK(test.to_str(true) == "()");
    }

    TEST_CASE("nested null list")
    {
        MalPair test = MalPair(std::make_shared<MalPair>());
        CHECK(test.to_str() == "(())");
        CHECK(test.to_str(true) == "(())");
    }

    TEST_CASE("double nested null list")
    {
        PairPtr test = std::make_shared<MalPair>(std::make_shared<MalPair>(std::make_shared<MalPair>()));
        CHECK(test->to_str() == "((()))");
        CHECK(test->to_str(true) == "((()))");
    }

    TEST_CASE("paired nested null list")
    {
        PairPtr test = std::make_shared<MalPair>(std::make_shared<MalPair>(), std::make_shared<MalPair>(std::make_shared<MalPair>()));
        CHECK(test->to_str() == "(() ())");
        CHECK(test->to_str(true) == "(() ())");
    }


    TEST_CASE("list of one element")
    {
        PairPtr test = std::make_shared<MalPair>(std::make_shared<MalSymbol>("foo"));
        CHECK(test->is_null() == false);
        CHECK(test->to_str() == "(foo)");
        CHECK(test->to_str(true) == "(foo)");
        MalPtr test_car = test->car();
        CHECK(test_car->type() ==  MAL_SYMBOL);
        CHECK(test_car->as_symbol() == "foo");
        CHECK(test->size() == 1);
    }

    TEST_CASE("improper list of two elements")
    {
        PairPtr test = std::make_shared<MalPair>(std::make_shared<MalSymbol>("foo"), std::make_shared<MalSymbol>("bar"));
        MalPtr test_car = test->car();
        CHECK(test_car->to_str() == "foo");
        MalPtr test_cdr = test->cdr();
        CHECK(test_cdr->to_str() == "bar");
        CHECK(test->to_str() == "(foo . bar)");
        CHECK(test->to_str(true) == "(foo . bar)");
    }


    TEST_CASE("proper list of two integer elements")
    {
        PairPtr test = std::make_shared<MalPair>(std::make_shared<MalInteger>(42), std::make_shared<MalPair>(std::make_shared<MalInteger>(69)));
        MalPtr test_car = test->car();
        CHECK(test_car->type() == MAL_INTEGER);
        mpz_class test_num = test_car->as_integer();
        CHECK(test_num == 42);
        PairPtr test_cdr = test->cdr()->as_pair();
        CHECK(test_cdr->type() == MAL_PAIR);
        CHECK(test_cdr->to_str() == "(69)");
        MalPtr test_cadr = test_cdr->car();
        CHECK(test_cadr->type() == MAL_INTEGER);
        test_num = test_cadr->as_integer();
        CHECK(test_num == 69);
        CHECK(test->to_str() == "(42 69)");
        CHECK(test->to_str(true) == "(42 69)");
    }


    TEST_CASE("proper list of two symbol elements")
    {
        PairPtr test = std::make_shared<MalPair>(std::make_shared<MalSymbol>("foo"), std::make_shared<MalPair>(std::make_shared<MalSymbol>("bar")));
        CHECK(test->car()->type() == MAL_SYMBOL);
        auto test_car = test->car();
        CHECK(test_car->as_symbol() == "foo");
        CHECK(test->cdr()->type() == MAL_PAIR);
        PairPtr test_cdr = test->cdr()->as_pair();
        MalPtr test_cadr = test_cdr->car();
        CHECK(test_cadr->type() == MAL_SYMBOL);
        CHECK(test_cadr->as_symbol() == "bar");
        CHECK(test->to_str() == "(foo bar)");
        CHECK(test->size() == 2);
    }



    TEST_CASE("proper list of three elements")
    {
        PairPtr test = std::make_shared<MalPair>(std::make_shared<MalSymbol>("foo"), std::make_shared<MalPair>(std::make_shared<MalSymbol>("bar"), std::make_shared<MalPair>(std::make_shared<MalSymbol>("baz"))));
        CHECK(test->car()->type() == MAL_SYMBOL);
        MalPtr test_car = test->car();
        CHECK(test_car->to_str() == "foo");
        CHECK(test->cdr()->type() == MAL_PAIR);
        PairPtr test_cdr = test->cdr()->as_pair();
        MalPtr test_cadr = test_cdr->car();
        CHECK(test_cadr->type() == MAL_SYMBOL);
        CHECK(test_cadr->to_str() == "bar");
        CHECK(test->to_str() == "(foo bar baz)");
        CHECK((*test)[1]->to_str() == "bar");
        CHECK(test->size() == 3);
    }


    TEST_CASE("improper list of three elements")
    {
        PairPtr test = std::make_shared<MalPair>(std::make_shared<MalSymbol>("foo"), std::make_shared<MalPair>(std::make_shared<MalSymbol>("bar"), std::make_shared<MalSymbol>("baz")));
        CHECK(test->car()->type() == MAL_SYMBOL);
        MalPtr test_car = test->car();
        CHECK(test_car->to_str() == "foo");
        CHECK(test->cdr()->type() == MAL_PAIR);
        PairPtr test_cdr = test->cdr()->as_pair();
        MalPtr test_cadr = test_cdr->car();
        CHECK(test_cadr->type() == MAL_SYMBOL);
        CHECK(test_cadr->to_str() == "bar");
        MalPtr test_cddr = test_cdr->cdr();
        CHECK(test_cddr->type() == MAL_SYMBOL);
        CHECK(test_cddr->to_str() == "baz");
        CHECK(test->to_str() == "(foo bar . baz)");
    }

    TEST_CASE("list of a symbol and a list")
    {
        PairPtr test = std::make_shared<MalPair>(std::make_shared<MalSymbol>("foo"), std::make_shared<MalPair>(std::make_shared<MalPair>(std::make_shared<MalSymbol>("bar"), std::make_shared<MalPair>(std::make_shared<MalSymbol>("baz")))));
        CHECK(test->car()->type() == MAL_SYMBOL);
        MalPtr test_car = test->car();
        CHECK(test_car->to_str() == "foo");
        CHECK(test->cdr()->type() == MAL_PAIR);
        PairPtr test_cdr = test->cdr()->as_pair();
        MalPtr test_cadr = test_cdr->car();
        CHECK(test_cadr->type() == MAL_PAIR);
        PairPtr test_inner_list = test_cadr->as_pair();
        CHECK(test_inner_list->to_str() == "(bar baz)");
        CHECK(test->to_str() == "(foo (bar baz))");
        CHECK(test->size() == 2);
    }


    TEST_CASE("list of a list and a symbol")
    {
        PairPtr test = std::make_shared<MalPair>(std::make_shared<MalPair>(std::make_shared<MalSymbol>("foo"),
                                       std::make_shared<MalPair>(std::make_shared<MalSymbol>("bar"))),
                              std::make_shared<MalPair>(std::make_shared<MalSymbol>("baz")));
        CHECK(test->car()->type() == MAL_PAIR);
        PairPtr test_car = test->car()->as_pair();
        CHECK(test_car->car()->to_str() == "foo");
        PairPtr test_cdar = test_car->cdr()->as_pair();
        CHECK(test_cdar->car()->to_str() == "bar");
        CHECK(test->cdr()->type() == MAL_PAIR);
        PairPtr test_cdr = test->cdr()->as_pair();
        CHECK(test_cdr->car()->to_str() == "baz");
        CHECK(test->to_str() == "((foo bar) baz)");
        CHECK(test->size() == 2);
    }


    TEST_CASE("list of a list and two symbols")
    {
        PairPtr test = std::make_shared<MalPair>(std::make_shared<MalPair>(std::make_shared<MalSymbol>("foo"),
                                       std::make_shared<MalPair>(std::make_shared<MalSymbol>("bar"))),
                              std::make_shared<MalPair>(std::make_shared<MalSymbol>("baz"),
                                       std::make_shared<MalPair>(std::make_shared<MalSymbol>("quux"))));
        CHECK(test->car()->type() == MAL_PAIR);
        PairPtr test_car = test->car()->as_pair();
        CHECK(test_car->car()->to_str() == "foo");
        PairPtr test_cdar = test_car->cdr()->as_pair();
        CHECK(test_cdar->car()->to_str() == "bar");
        CHECK(test->cdr()->type() == MAL_PAIR);
        PairPtr test_cdr = test->cdr()->as_pair();
        CHECK(test_cdr->car()->to_str() == "baz");
        PairPtr test_cddr = test_cdr->cdr()->as_pair();
        CHECK(test_cddr->car()->to_str() == "quux");
        CHECK(test->to_str() == "((foo bar) baz quux)");
        CHECK(test->size() == 3);
    }


    TEST_CASE("binary tree of ordered pairs")
    {
        PairPtr test = std::make_shared<MalPair>(std::make_shared<MalPair>(std::make_shared<MalPair>(std::make_shared<MalSymbol>("foo"), std::make_shared<MalSymbol>("quux")), std::make_shared<MalPair>(std::make_shared<MalSymbol>("bar"), std::make_shared<MalSymbol>("baz"))),
                             (std::make_shared<MalPair>(std::make_shared<MalPair>(std::make_shared<MalInteger>(1), std::make_shared<MalInteger>(2)), std::make_shared<MalPair>(std::make_shared<MalInteger>(3), std::make_shared<MalInteger>(4)))));
        CHECK(test->to_str() == "(((foo . quux) . (bar . baz)) . ((1 . 2) . (3 . 4)))");
    }

    TEST_CASE("list of ordered pairs")
    {
        PairPtr test = std::make_shared<MalPair>(std::make_shared<MalPair>(std::make_shared<MalSymbol>("foo"), std::make_shared<MalSymbol>("quux")), std::make_shared<MalPair>(std::make_shared<MalPair>(std::make_shared<MalSymbol>("bar"), std::make_shared<MalSymbol>("baz"))));
        CHECK(test->to_str() == "((foo . quux) (bar . baz))");
        CHECK(test->size() == 2);
    }


    TEST_CASE("appending single element to list")
    {
        PairPtr test = std::make_shared<MalPair>();
        CHECK(test->to_str() == "()");
        test->add(std::make_shared<MalSymbol>("foo"));
        CHECK(test->to_str() == "(foo)");
        test->add(std::make_shared<MalKeyword>("bar"));
        CHECK(test->to_str() == "(foo :bar)");
        test->add(std::make_shared<MalInteger>(69));
        CHECK(test->to_str() == "(foo :bar 69)");
        test->add(std::make_shared<MalRational>(mpq_class("17/23")));
        CHECK(test->to_str() == "(foo :bar 69 17/23)");
        CHECK(test->to_str(true) == "(foo :bar 69 17/23)");
    }



    TEST_CASE("empty vector")
    {
        MalVector test;
        CHECK(test.to_str() == "[]");
        CHECK(test.size() == 0);
    }


    TEST_CASE("vector of one element")
    {
        PairPtr source = std::make_shared<MalPair>(std::make_shared<MalString>("foo"));
        MalVector test(source);
        CHECK(test.to_str() == "[foo]");
        CHECK(test.size() == 1);
    }


    TEST_CASE("vector of two elements")
    {
        PairPtr source = std::make_shared<MalPair>(std::make_shared<MalString>("foo"), std::make_shared<MalPair>(std::make_shared<MalString>("bar")));
        MalVector test(source);
        CHECK(test.to_str() == "[foo bar]");
        CHECK(test.size() == 2);
    }


    TEST_CASE("vector of three elements")
    {
        PairPtr source = std::make_shared<MalPair>(std::make_shared<MalString>("foo"), std::make_shared<MalPair>(std::make_shared<MalString>("bar"), std::make_shared<MalPair>(std::make_shared<MalString>("baz"))));
        MalVector test(source);
        CHECK(test.to_str() == "[foo bar baz]");
        CHECK(test.size() == 3);
    }

    TEST_CASE("vector of four elements from appended list")
    {
        PairPtr test = std::make_shared<MalPair>();
        test->add(std::make_shared<MalSymbol>("foo"));
        test->add(std::make_shared<MalKeyword>("bar"));
        test->add(std::make_shared<MalInteger>(69));
        test->add(std::make_shared<MalRational>(mpq_class("17/23")));
        MalVector test_vec(test);
        CHECK(test_vec.to_str() == "[foo :bar 69 17/23]");
        CHECK(test_vec.size() == 4);
    }


    TEST_CASE("empty hashmap")
    {
        MalHashmap test;
        CHECK(test.to_str() == "{}");
        CHECK(test.size() == 0);
    }




    TEST_CASE("hashmap with one entry")
    {
        PairPtr source = std::make_shared<MalPair>(std::make_shared<MalString>("foo"), std::make_shared<MalPair>(std::make_shared<MalString>("bar")));
        MalHashmap test(source);
        CHECK(test.to_str() == "{\"foo\" \"bar\"}");
        CHECK(test.size() == 1);
    }


    TEST_CASE("hashmap with two entries")
    {
        PairPtr source = std::make_shared<MalPair>(
            std::make_shared<MalString>("foo"),
            std::make_shared<MalPair>(std::make_shared<MalString>("bar"),
                                      std::make_shared<MalPair>(std::make_shared<MalKeyword>("baz"),
                                                                std::make_shared<MalPair>(std::make_shared<MalInteger>(128)))));
        MalHashmap test(source);
        CHECK(test.to_str() == "{:baz 128 \"foo\" \"bar\"}");
        CHECK(test.size() == 2);
    }

}
