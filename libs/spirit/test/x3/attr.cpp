/*=============================================================================
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2013 Agustín Bergé

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/detail/lightweight_test.hpp>
#include <boost/spirit/home/x3.hpp>

#include <boost/fusion/include/std_pair.hpp>
#include <boost/ref.hpp>

#include <iostream>
#include "test.hpp"

int
main()
{
    using spirit_test::test_attr;
    namespace x3 = boost::spirit::x3;

    using x3::attr;
    using x3::int_;
    using x3::unused_type;

    {
        int d = 0;
        BOOST_TEST(test_attr("", attr(1), d) && d == 1);

        int d1 = 1;
        BOOST_TEST(test_attr("", attr(d1), d) && d == 1);

        int d2 = 2;
        BOOST_TEST(test_attr("", attr(boost::ref(d2)), d) && d == 2);

        std::pair<int, int> p;
        BOOST_TEST(test_attr("1", int_ >> attr(1), p) && 
            p.first == 1 && p.second == 1);

        char c = '\0';
        BOOST_TEST(test_attr("", attr('a'), c) && c == 'a');

        // $$$ "test" is not compatible with std::string $$$
        //~ std::string str;
        //~ BOOST_TEST(test_attr("", attr("test"), str) && str == "test");
    }
    
    // $$$ not yet implementd $$$
    //~ {   // test lazy semantic predicate
        //~ int d = 0;
        //~ auto val_ = [d](unused_type) { return d; };
        //~ BOOST_TEST(test_attr("", attr(val_), d) && d == 1);

        //~ int d1 = 2;
        //~ auto ref_ = [&d1](unused_type) { return d1; };
        //~ BOOST_TEST(test_attr("", attr(ref_), d) && d == 2);
    //~ }

    //~ {   // testing lazy constructs
        //~ using boost::phoenix::val;
        //~ using boost::phoenix::ref;

        //~ int d = 0;
        //~ BOOST_TEST(test_attr("", attr(val(1)), d) && d == 1);

        //~ int d1 = 2;
        //~ BOOST_TEST(test_attr("", attr(ref(d1)), d) && d == 2);
    //~ }

    {
        std::string s;
        BOOST_TEST(test_attr("s", "s" >> x3::attr(std::string("123")), s) && 
            s == "123");
    }

    return boost::report_errors();
}
