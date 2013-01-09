/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <string>
#include <vector>

#include <boost/detail/lightweight_test.hpp>

#include <boost/spirit/home/x3/operator/kleene.hpp>
#include <boost/spirit/home/x3/operator/plus.hpp>
#include <boost/spirit/home/x3/numeric.hpp>
#include <boost/spirit/home/x3/char.hpp>
#include <boost/spirit/home/x3/directive.hpp>

//~ #include <boost/phoenix/core.hpp>
//~ #include <boost/phoenix/operator.hpp>

#include <string>
#include <iostream>
#include "test.hpp"

struct x_attr
{
};

namespace boost { namespace spirit { namespace traits
{
    template <>
    struct container_value<x_attr>
    {
        typedef char type; // value type of container
    };

    template <>
    struct push_back_container<x_attr, char>
    {
        static bool call(x_attr& /*c*/, char /*val*/)
        {
            // push back value type into container
            return true;
        }
    };
}}}

int
main()
{
    using spirit_test::test;
    using spirit_test::test_attr;
    using boost::spirit::x3::char_;
    using boost::spirit::x3::alpha;
    using boost::spirit::x3::upper;
    using boost::spirit::x3::space;
    using boost::spirit::x3::digit;
    using boost::spirit::x3::digit;
    //~ using boost::spirit::x3::no_case;
    using boost::spirit::x3::int_;
    //~ using boost::spirit::x3::omit;
    using boost::spirit::x3::lit;
    //~ using boost::spirit::x3::_1;
    using boost::spirit::x3::lexeme;

    {
        BOOST_TEST(test("aaaaaaaa", +char_));
        BOOST_TEST(test("a", +char_));
        BOOST_TEST(!test("", +char_));
        BOOST_TEST(test("aaaaaaaa", +alpha));
        BOOST_TEST(!test("aaaaaaaa", +upper));
    }

    {
        BOOST_TEST(test(" a a aaa aa", +char_, space));
        BOOST_TEST(test("12345 678 9 ", +digit, space));
    }

    //~ {
        //~ BOOST_TEST(test("aBcdeFGH", no_case[+char_]));
        //~ BOOST_TEST(test("a B cde FGH  ", no_case[+char_], space));
    //~ }

    {
        std::vector<int> v;
        BOOST_TEST(test_attr("123 456 789 10", +int_, v, space) && 4 == v.size() &&
            v[0] == 123 && v[1] == 456 && v[2] == 789 &&  v[3] == 10);
    }

    {
        std::vector<std::string> v;
        BOOST_TEST(test_attr("a b c d", +lexeme[+alpha], v, space) && 4 == v.size() &&
            v[0] == "a" && v[1] == "b" && v[2] == "c" &&  v[3] == "d");
    }

    {
        BOOST_TEST(test("Kim Kim Kim", +lit("Kim"), space));
    }

    //~ {
        //~ // The following 2 tests show that omit does not inhibit explicit attributes

        //~ std::string s;
        //~ BOOST_TEST(test_attr("bbbb", omit[+char_('b')], s) && s == "bbbb");

        //~ s.clear();
        //~ BOOST_TEST(test_attr("b b b b ", omit[+char_('b')], s, space) && s == "bbbb");
    //~ }
/*
    { // actions
        namespace phx = boost::phoenix;

        std::vector<char> v;
        BOOST_TEST(test("bbbb", (+char_)[phx::ref(v) = _1]) && 4 == v.size() &&
            v[0] == 'b' && v[1] == 'b' && v[2] == 'b' &&  v[3] == 'b');
    }

    { // more actions
        namespace phx = boost::phoenix;

        std::vector<int> v;
        BOOST_TEST(test("1 2 3", (+int_)[phx::ref(v) = _1], space) && 3 == v.size() &&
            v[0] == 1 && v[1] == 2 && v[2] == 3);
    }
*/

    { // attribute customization

        x_attr x;
        test_attr("abcde", +char_, x);
    }
    return boost::report_errors();
}
