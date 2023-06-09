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
#include "reader.h"


TEST_SUITE("constructing atoms and lists")
{
    TEST_CASE("integer")
    {
        Tokenizer src;
        MalPtr test(src.tokenize("69"));
        CHECK(test->is_atom());
        CHECK(test->is_list() == false);
        CHECK(test->is_number());
        CHECK(test->is_integer());
        CHECK(test->as_integer() == mpz_class(69));
        CHECK(test->to_str() == "69");
    }

    TEST_CASE("rational number")
    {
        Tokenizer src;
        MalPtr test(src.tokenize("23/17"));
        CHECK(test->is_atom());
        CHECK(test->is_list() == false);
        CHECK(test->is_number());
        CHECK(test->is_rational());
        CHECK(test->as_rational() == mpq_class("23/17"));
        CHECK(test->to_str() == "23/17");
    }


    TEST_CASE("floating-point number")
    {
        Tokenizer src;
        MalPtr test(src.tokenize("42.5"));
        CHECK(test->is_atom());
        CHECK(test->is_list() == false);
        CHECK(test->is_number());
        CHECK(test->is_fractional());
        CHECK(test->as_fractional() == mpf_class(42.5));
        CHECK(test->to_str() == "42.5");
    }

    TEST_CASE("complex number")
    {
        Tokenizer src;
        MalPtr test(src.tokenize("17.69-23.42i"));
        CHECK(test->is_atom());
        CHECK(test->is_list() == false);
        CHECK(test->is_number());
        CHECK(test->is_complex());
        CHECK(test->to_str() == "17.69-23.42i");
    }



    TEST_CASE("symbol #1")
    {
        Tokenizer src;
        MalPtr test(src.tokenize("foo"));
        CHECK(test->is_atom());
        CHECK(test->is_list() == false);
        CHECK(test->is_number() == false);
        CHECK(test->is_symbol() == true);
        CHECK(test->to_str() == "foo");
    }

    TEST_CASE("symbol #2")
    {
        Tokenizer src;
        MalPtr test(src.tokenize("an-atom"));
        CHECK(test->is_atom());
        CHECK(test->is_list() == false);
        CHECK(test->is_number() == false);
        CHECK(test->is_symbol() == true);
        CHECK(test->to_str() == "an-atom");
    }


    TEST_CASE("keyword")
    {
        Tokenizer src;
        MalPtr test(src.tokenize(":bar"));
        CHECK(test->is_atom());
        CHECK(test->is_list() == false);
        CHECK(test->is_number() == false);
        CHECK(test->is_symbol() == false);
        CHECK(test->is_keyword() == true);
        CHECK(test->to_str() == ":bar");
    }

    TEST_CASE("empty string")
    {
        Tokenizer src;
        MalPtr test(src.tokenize("\"\""));
        CHECK(test->is_atom());
        CHECK(test->is_list() == false);
        CHECK(test->is_number() == false);
        CHECK(test->is_symbol() == false);
        CHECK(test->is_string() == true);
        CHECK(test->to_str() == "");
        CHECK(test->to_str(true) == "\"\"");
    }


    TEST_CASE("simple string")
    {
        Tokenizer src;
        MalPtr test(src.tokenize("\"foo bar\""));
        CHECK(test->is_atom());
        CHECK(test->is_list() == false);
        CHECK(test->is_number() == false);
        CHECK(test->is_symbol() == false);
        CHECK(test->is_string() == true);
        CHECK(test->to_str() == "foo bar");
        CHECK(test->to_str(true) == "\"foo bar\"");
    }

    TEST_CASE("string with escaped quote")
    {
        Tokenizer src;
        MalPtr test(src.tokenize("\"foo\\\" bar\""));
        CHECK(test->is_atom());
        CHECK(test->is_list() == false);
        CHECK(test->is_number() == false);
        CHECK(test->is_symbol() == false);
        CHECK(test->is_string() == true);
        CHECK(test->to_str() == "foo\" bar");
        CHECK(test->to_str(true) == "\"foo\\\" bar\"");
    }


    TEST_CASE("string with escaped newline")
    {
        Tokenizer src;
        MalPtr test(src.tokenize("\"foo\\n bar\""));
        CHECK(test->is_atom());
        CHECK(test->is_list() == false);
        CHECK(test->is_number() == false);
        CHECK(test->is_symbol() == false);
        CHECK(test->is_string() == true);
        CHECK(test->to_str() == "foo\n bar");
        CHECK(test->to_str(true) == "\"foo\\n bar\"");
    }


    TEST_CASE("null list")
    {
        Tokenizer src;
        MalPtr test(src.tokenize("()"));
        CHECK(test != nullptr);
        CHECK(test->is_pair());
        CHECK(test->is_list());
        CHECK(test->is_null());
        CHECK(test->to_str() == "()");
        CHECK(test->to_str(true) == "()");
    }



    TEST_CASE("null list with a leading space")
    {
        Tokenizer src;
        MalPtr parsed(src.tokenize(" ()"));
        CHECK(parsed != nullptr);
        CHECK(parsed->is_pair());
        CHECK(parsed->is_list());
        CHECK(parsed->is_null());
        CHECK(parsed->to_str() == "()");
        CHECK(parsed->to_str(true) == "()");
    }

    TEST_CASE("null list with a median space")
    {
        Tokenizer src;
        MalPtr parsed(src.tokenize("( )"));
        CHECK(parsed != nullptr);
        CHECK(parsed->is_pair());
        CHECK(parsed->is_list());
        CHECK(parsed->is_null());
        CHECK(parsed->to_str() == "()");
        CHECK(parsed->to_str(true) == "()");
    }

    TEST_CASE("null list with a trailing space")
    {
        Tokenizer src;
        MalPtr parsed(src.tokenize("() "));
        CHECK(parsed != nullptr);
        CHECK(parsed->is_pair());
        CHECK(parsed->is_list());
        CHECK(parsed->is_null());
        CHECK(parsed->to_str() == "()");
        CHECK(parsed->to_str(true) == "()");
    }


    TEST_CASE("nested null list")
    {
        Tokenizer src;
        MalPtr parsed(src.tokenize("(())"));
        CHECK(parsed != nullptr);
        CHECK(parsed->is_pair());
        CHECK(parsed->is_list());
        CHECK(parsed->is_null() == false);
        CHECK(parsed->to_str() == "(())");
        CHECK(parsed->to_str(true) == "(())");
        MalPtr test = parsed->as_pair()->car();
        CHECK(test != nullptr);
        CHECK(test->is_list());
        CHECK(test->is_null());
        CHECK(test->to_str() == "()");
        CHECK(test->to_str(true) == "()");
    }


    TEST_CASE("double nested null list")
    {
        Tokenizer src;
        MalPtr parsed(src.tokenize("((()))"));
        CHECK(parsed != nullptr);
        CHECK(parsed->is_list());
        CHECK(parsed->is_null() == false);
        CHECK(parsed->to_str() == "((()))");
        CHECK(parsed->to_str(true) == "((()))");
        MalPtr test = parsed->as_pair()->car();
        CHECK(test->to_str() == "(())");
        CHECK(test->to_str(true) == "(())");
    }

    TEST_CASE("paired nested null list")
    {
        Tokenizer src;
        MalPtr parsed(src.tokenize("(() ())"));
        CHECK(parsed->to_str() == "(() ())");
        CHECK(parsed->to_str(true) == "(() ())");
    }


    TEST_CASE("list of one element")
    {
        Tokenizer src;
        MalPtr parsed(src.tokenize("(foo)"));
        CHECK(parsed->is_list() == true);
        CHECK(parsed->is_null() == false);
        CHECK(parsed->size() == 1);
        CHECK(parsed->to_str() == "(foo)");
        CHECK(parsed->to_str(true) == "(foo)");
        MalPtr test = parsed->as_pair()->car();
        CHECK(test->size() == 1);
        CHECK(test->is_list() == false);
        CHECK(test->is_null() == false);
        CHECK(test->to_str() == "foo");
        CHECK(test->to_str(true) == "foo");
    }


    TEST_CASE("proper list of two integer elements")
    {
        Tokenizer src;
        MalPtr parsed(src.tokenize("(42 69)"));
        CHECK(parsed->is_list() == true);
        CHECK(parsed->is_null() == false);
        CHECK(parsed->size() == 2);
        CHECK(parsed->to_str() == "(42 69)");
        CHECK(parsed->to_str(true) == "(42 69)");
        PairPtr test = parsed->as_pair();
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
        Tokenizer src;
        MalPtr parsed(src.tokenize("(foo bar)"));
        CHECK(parsed->is_list() == true);
        CHECK(parsed->is_null() == false);
        CHECK(parsed->size() == 2);
        CHECK(parsed->to_str() == "(foo bar)");
        CHECK(parsed->to_str(true) == "(foo bar)");
        PairPtr test = parsed->as_pair();
        auto test_car = test->car();
        CHECK(test_car->type() == MAL_SYMBOL);
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
        Tokenizer src;
        MalPtr parsed(src.tokenize("(foo bar baz)"));
        CHECK(parsed->is_list() == true);
        CHECK(parsed->is_null() == false);
        CHECK(parsed->size() == 3);
        CHECK(parsed->to_str() == "(foo bar baz)");
        CHECK(parsed->to_str(true) == "(foo bar baz)");
        PairPtr test = parsed->as_pair();
        MalPtr test_car = test->car();
        CHECK(test->size() == 3);
        CHECK(test_car->type() == MAL_SYMBOL);
        CHECK(test_car->to_str() == "foo");
        PairPtr test_cdr = test->as_pair()->cdr()->as_pair();
        CHECK(test_cdr->type() == MAL_PAIR);
        MalPtr test_cadr = test_cdr->car();
        CHECK(test_cadr->type() == MAL_SYMBOL);
        CHECK(test_cadr->to_str() == "bar");
        CHECK((*(test->as_pair()))[1]->to_str() == "bar");
    }


    TEST_CASE("improper list of three elements")
    {
        Tokenizer src;
        MalPtr test(src.tokenize("(foo bar . baz)"));
        CHECK(test->to_str() == "(foo bar . baz)");
        CHECK (test->type() == MAL_PAIR);
        PairPtr test_pair = test->as_pair();
        CHECK(test_pair->car()->type() == MAL_SYMBOL);
        MalPtr test_car = test_pair->car();
        CHECK(test_car->to_str() == "foo");
        CHECK(test_pair->cdr()->type() == MAL_PAIR);
        PairPtr test_cdr = test->as_pair()->cdr()->as_pair();
        CHECK(test_cdr->to_str() == "(bar . baz)");
        MalPtr test_cadr = test_cdr->car();
        CHECK(test_cadr->type() == MAL_SYMBOL);
        CHECK(test_cadr->to_str() == "bar");
        MalPtr test_cddr = test_cdr->cdr();
        CHECK(test_cddr->type() == MAL_SYMBOL);
        CHECK(test_cddr->to_str() == "baz");
    }

    TEST_CASE("list of a symbol and a list")
    {
        Tokenizer src;
        MalPtr parsed(src.tokenize("(foo (bar baz))"));
        CHECK(parsed->is_list() == true);
        CHECK(parsed->is_null() == false);
        CHECK(parsed->size() == 2);
        CHECK(parsed->to_str() == "(foo (bar baz))");
        CHECK(parsed->to_str(true) == "(foo (bar baz))");
        PairPtr test_pair = parsed->as_pair();
        CHECK(test_pair->car()->type() == MAL_SYMBOL);
        MalPtr test_car = test_pair->car();
        CHECK(test_car->to_str() == "foo");
        CHECK(test_pair->cdr()->type() == MAL_PAIR);
        PairPtr test_cdr = test_pair->cdr()->as_pair();
        MalPtr test_cadr = test_cdr->car();
        CHECK(test_cadr->type() == MAL_PAIR);
        PairPtr test_inner_list = test_cadr->as_pair();
        CHECK(test_inner_list->to_str() == "(bar baz)");
    }


    TEST_CASE("list of a list and a symbol")
    {
        Tokenizer src;
        MalPtr parsed(src.tokenize("((foo bar) baz)"));
        CHECK(parsed->is_list() == true);
        CHECK(parsed->is_null() == false);
        CHECK(parsed->size() == 2);
        CHECK(parsed->to_str() == "((foo bar) baz)");
        CHECK(parsed->to_str(true) == "((foo bar) baz)");
        PairPtr test_pair = parsed->as_pair();
        CHECK(test_pair->car()->type() == MAL_PAIR);
        PairPtr test_car = test_pair->car()->as_pair();
        CHECK(test_car->car()->to_str() == "foo");
        PairPtr test_cdar = test_car->cdr()->as_pair();
        CHECK(test_cdar->car()->to_str() == "bar");
        CHECK(test_pair->cdr()->type() == MAL_PAIR);
        PairPtr test_cdr = test_pair->cdr()->as_pair();
        CHECK(test_cdr->car()->to_str() == "baz");
    }


    TEST_CASE("list of a list and two symbols")
    {
        Tokenizer src;
        MalPtr test(src.tokenize("((foo bar) baz quux)"));
        CHECK(test->type() == MAL_PAIR);
        auto test_pair = test->as_pair();
        PairPtr test_car = test_pair->car()->as_pair();
        CHECK(test_car->car()->to_str() == "foo");
        PairPtr test_cdar = test_car->cdr()->as_pair();
        CHECK(test_cdar->car()->to_str() == "bar");
        CHECK(test->as_pair()->cdr()->type() == MAL_PAIR);
        PairPtr test_cdr = test_pair->cdr()->as_pair();
        CHECK(test_cdr->car()->to_str() == "baz");
        PairPtr test_cddr = test_cdr->cdr()->as_pair();
        CHECK(test_cddr->car()->to_str() == "quux");
        CHECK(test->to_str() == "((foo bar) baz quux)");
        CHECK(test->size() == 3);
    }


    TEST_CASE("binary tree of ordered pairs")
    {
        Tokenizer src;
        MalPtr test(src.tokenize("(((foo . quux) . (bar . baz)) . ((1 . 2) . (3 . 4)))"));
        CHECK(test->to_str() == "(((foo . quux) . (bar . baz)) . ((1 . 2) . (3 . 4)))");
    }

    TEST_CASE("list of ordered pairs")
    {
        Tokenizer src;
        MalPtr test(src.tokenize("((foo . quux) (bar . baz))"));

        CHECK(test->to_str() == "((foo . quux) (bar . baz))");
        CHECK(test->size() == 2);
    }


    TEST_CASE("vector of one element")
    {
        Tokenizer src;
        MalPtr test(src.tokenize("[foo]"));
        CHECK(test->to_str() == "[foo]");
        CHECK(test->size() == 1);
    }


    TEST_CASE("vector of two elements")
    {
        Tokenizer src;
        MalPtr test(src.tokenize("[foo bar]"));
        CHECK(test->to_str() == "[foo bar]");
        CHECK(test->size() == 2);
    }


    TEST_CASE("vector of three elements")
    {
        Tokenizer src;
        MalPtr test(src.tokenize("[foo bar baz]"));
        CHECK(test->to_str() == "[foo bar baz]");
        CHECK(test->size() == 3);
    }



    TEST_CASE("hashmap with one entry")
    {
        Tokenizer src;
        MalPtr test(src.tokenize("{\"foo\" \"bar\"}"));
        CHECK(test->to_str() == "{\"foo\" \"bar\"}");
        CHECK(test->size() == 1);
    }


    TEST_CASE("hashmap with two entries")
    {
        Tokenizer src;
        MalPtr test(src.tokenize("{\"foo\" \"bar\" :baz 128}"));
        CHECK(test->size() == 2);
    }


    TEST_CASE("hashmap with four entries")
    {
        Tokenizer src;
        MalPtr test(src.tokenize("{\"foo\" \"bar\" :baz 128 :quux 42 :zark 69}"));
        CHECK(test->size() == 4);
    }
}
