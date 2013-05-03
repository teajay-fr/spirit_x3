/*=============================================================================
    Copyright (c) 2013 Agustín Bergé

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <boost/spirit/home/x3.hpp>

namespace workbench { namespace x3
{
    using namespace boost::spirit::x3;

    template <typename F, typename Attribute, typename Context>
    auto lazy_eval(F&& f
      , Attribute const& attribute, Context const& context, bool& pass)
     -> decltype(f(attribute, context, pass))
    {
        return f(attribute, context, pass);
    }

    struct lazy_tag {} static const lazy = {};

    /** separated parsers and tagged generator **/
    template <typename Value>
    struct attr_parser : parser<attr_parser<Value>>
    {
        typedef Value value_type;
        typedef Value attribute_type;

        static bool const has_attribute = true;

        attr_parser(value_type const& value)
          : value_(value) {}

        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Attribute& attr_) const
        {
            auto copy = value_;
            boost::spirit::traits::move_to(copy, attr_);
            return true;
        }

        value_type const value_;
    };

    template <typename Value>
    struct lazy_attr_parser : parser<lazy_attr_parser<Value>>
    {
        typedef Value value_type;
        typedef Value attribute_type;

        static bool const has_attribute = true;

        lazy_attr_parser(value_type const& value)
          : value_(value) {}

        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Attribute& attr_) const
        {
            bool pass = true;
            auto copy = lazy_eval(value_, attr_, context, pass);
            if (pass)
                boost::spirit::traits::move_to(copy, attr_);
            return pass;
        }

        value_type const value_;
    };

    struct attr_gen
    {
        template <typename Value>
        attr_parser<Value>
        operator()(Value&& value) const
        {
            typedef attr_parser<Value> result_type;

            return result_type(std::forward<Value>(value));
        }
        template <typename Value>
        lazy_attr_parser<Value>
        operator()(lazy_tag, Value&& value) const
        {
            typedef lazy_attr_parser<Value> result_type;

            return result_type(std::forward<Value>(value));
        }
    };

    attr_gen const attr = attr_gen();

}}

int main()
{
    using namespace workbench;

    auto answer = [](x3::unused_type, x3::unused_type, bool&){ return 42; };

    char const* empty = "";
    int attr;
    x3::parse(empty, empty+1, x3::attr(x3::lazy, answer), attr);
}
