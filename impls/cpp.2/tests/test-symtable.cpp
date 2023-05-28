#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#include <typeinfo>
#include <iostream>
#include <sstream>
#include "atom.h"
#include "symbol_table.h"


TEST_SUITE("constructing symbol table nodes")
{
    TEST_CASE("Single symbol table entry")
    {
        Symbol* sym = new Symbol("foo");
        SymbolTableNode* root = new SymbolTableNode(sym);
        CHECK(root->to_string() == "foo");
        CHECK(root->seek("foo")->get_symbol() == sym);
        CHECK(root->seek("bar") == nullptr);
        CHECK(root->seek("baz") == nullptr);
        CHECK(root->seek("quux") == nullptr);
        CHECK(root->size() == 1);
        delete root;
        CHECK(sym != nullptr);
        delete sym;
    }

    TEST_CASE("insert second table entry to left")
    {
        Symbol* sym[] = { new Symbol("foo"), new Symbol("bar")};
        SymbolTableNode* root = new SymbolTableNode(sym[0]);
        root->insert(sym[1]);
        CHECK(root->to_string() == "bar, foo");
        CHECK(root->seek("foo")->get_symbol() == sym[0]);
        CHECK(root->seek("bar")->get_symbol() == sym[1]);
        CHECK(root->seek("baz") == nullptr);
        CHECK(root->seek("quux") == nullptr);
        CHECK(root->size() == 2);
        delete root;
        CHECK(sym[0] != nullptr);
        CHECK(sym[1] != nullptr);
        delete sym[0];
        delete sym[1];
    }

    TEST_CASE("insert second table entry to right")
    {
        Symbol* sym[] = { new Symbol("foo"), new Symbol("quux")};
        SymbolTableNode* root = new SymbolTableNode(sym[0]);
        root->insert(sym[1]);
        CHECK(root->to_string() == "foo, quux");
        CHECK(root->seek("foo")->get_symbol() == sym[0]);
        CHECK(root->seek("quux")->get_symbol() == sym[1]);
        CHECK(root->seek("bar") == nullptr);
        CHECK(root->seek("bar") == nullptr);
        CHECK(root->size() == 2);
        delete root;
        CHECK(sym[0] != nullptr);
        CHECK(sym[1] != nullptr);
        delete sym[0];
        delete sym[1];
    }

    TEST_CASE("insert three table entries, two to the left and one to the right")
    {
        Symbol* sym[] = { new Symbol("foo"), new Symbol("bar"), new Symbol("baz"), new Symbol("quux")};
        SymbolTableNode* root = new SymbolTableNode(sym[0]);
        root->insert(sym[1]);
        root->insert(sym[2]);
        root->insert(sym[3]);
        CHECK(root->to_string() == "bar, baz, foo, quux");
        CHECK(root->seek("foo")->get_symbol() == sym[0]);
        CHECK(root->seek("bar")->get_symbol() == sym[1]);
        CHECK(root->seek("baz")->get_symbol() == sym[2]);
        CHECK(root->seek("quux")->get_symbol() == sym[3]);
        CHECK(root->size() == 4);
        delete root;
        CHECK(sym[0] != nullptr);
        CHECK(sym[1] != nullptr);
        CHECK(sym[2] != nullptr);
        CHECK(sym[3] != nullptr);
        delete sym[0];
        delete sym[1];
        delete sym[2];
        delete sym[3];
    }
}

TEST_SUITE("constructing and operating on a symbol table")
{
    Symbol* sym[] = { new Symbol("foo"), new Symbol("bar"), new Symbol("baz"), new Symbol("quux")};
    TEST_CASE("Single symbol table entry")
    {
        SymbolTable* top = new SymbolTable("<top-level>");
        top->insert(sym[0]);
        SymbolTableNode* sn = top->find_local("foo");
        CHECK(sn != nullptr);
        CHECK(sn->get_symbol() == sym[0]);
        SymbolTableNode* sn2 = top->find_in_scope("foo");
        CHECK(sn2 != nullptr);
        CHECK(sn2->get_symbol() == sym[0]);
        REQUIRE_THROWS(top->insert(sym[0]));
        Symbol* foo2 = new Symbol("foo");
        REQUIRE_THROWS(top->insert(foo2));
        delete top;
        delete foo2;
    }

    TEST_CASE("Second symbol table entry")
    {
        SymbolTable* top = new SymbolTable("<top-level>");
        top->insert(sym[0]);
        top->insert(sym[1]);
        SymbolTableNode* sn = top->find_local("foo");
        CHECK(sn != nullptr);
        CHECK(sn->get_symbol() == sym[0]);
        SymbolTableNode* sn2 = top->find_in_scope("foo");
        CHECK(sn2 != nullptr);
        CHECK(sn2->get_symbol() == sym[0]);
        sn = top->find_local("bar");
        CHECK(sn != nullptr);
        CHECK(sn->get_symbol() == sym[1]);
        sn2 = top->find_in_scope("bar");
        CHECK(sn2 != nullptr);
        CHECK(sn2->get_symbol() == sym[1]);

        delete top;
    }

    TEST_CASE("Child symbol table")
    {
        SymbolTable* top = new SymbolTable("<top-level>");
        top->insert(sym[0]);
        SymbolTable ch("child 1");
        top->new_scope(ch);
        ch.insert(sym[1]);
        CHECK(ch.get_parent() == top);
        SymbolTableNode* sn = top->find_local("foo");
        CHECK(sn != nullptr);
        CHECK(sn->get_symbol() == sym[0]);
        SymbolTableNode* sn2 = ch.find_in_scope("foo");
        CHECK(sn2 != nullptr);
        CHECK(sn2->get_symbol() == sym[0]);
        sn = top->find_local("bar");
        CHECK(sn == nullptr);
        sn2 = ch.find_in_scope("bar");
        CHECK(sn2 != nullptr);
        CHECK(sn2->get_symbol() == sym[1]);
        SymbolTable ch2("child 2");
        ch.new_scope(ch2);
        CHECK(ch2.get_parent() == &ch);
        ch2.insert(sym[2]);
        ch2.insert(sym[3]);
        sn = top->find_local("baz");
        CHECK(sn == nullptr);
        sn2 = ch2.find_in_scope("baz");
        CHECK(sn2 != nullptr);
        CHECK(sn2->get_symbol() == sym[2]);
        sn = ch2.find_in_scope("foo");
        CHECK(sn != nullptr);
        CHECK(sn->get_symbol() == sym[0]);
        sn2 = ch2.find_in_scope("bar");
        CHECK(sn2->get_symbol() == sym[1]);
        CHECK(top->depth() == 0);
        CHECK(top->size() == 1);
        CHECK(top->size_in_scope() == 1);
        CHECK(ch.depth() == 1);
        CHECK(ch.size() == 1);
        CHECK(ch.size_in_scope() == 2);
        CHECK(ch2.depth() == 2);
        CHECK(ch2.size() == 2);
        CHECK(ch2.size_in_scope() == 4);
        CHECK(ch.get_description() == "child 1");
        CHECK(ch2.get_description() == "child 2");
        CHECK(ch2.to_string() == "<top-level>:\nfoo\n  child 1:\n  bar\n    child 2:\n    baz, quux\n");
        delete top;
    }
}