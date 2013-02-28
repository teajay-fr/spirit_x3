/*=============================================================================
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2013 Agustín Bergé

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef BOOST_SPIRIT_X3_AUXILIARY_ATTR_HPP
#define BOOST_SPIRIT_X3_AUXILIARY_ATTR_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/support/traits/container_traits.hpp>
#include <boost/spirit/home/support/traits/move_to.hpp>
#include <boost/type_traits/add_const.hpp>
#include <boost/type_traits/add_reference.hpp>
#include <boost/type_traits/decay.hpp>
#include <boost/type_traits/is_same.hpp>
#include <string>
#include <utility>

namespace boost { namespace spirit { namespace x3
{

    template <typename Value>
    struct attr_parser : parser<attr_parser<Value>>
    {
        typedef typename decay<Value>::type value_type;
        typedef Value attribute_type;

        static bool const has_attribute = true;
        static bool const handles_container =
            traits::is_container<attribute_type>::value;

        attr_parser(value_type const& value)
          : value_(value) {}
        attr_parser(value_type&& value)
          : value_(std::move(value)) {}

        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Attribute& attr_) const
        {
            value_type copy(value_);
            spirit::traits::move_to(copy, attr_);
            return true;
        }

        value_type const value_;

    private:
        // silence MSVC warning C4512: assignment operator could not be generated
        attr_parser& operator= (attr_parser const&);
    };
    
    template <typename Value>
    struct get_info<attr_parser<Value>>
    {
        typedef std::string result_type;
        std::string operator()(attr_parser<Value> const& /*p*/) const
        {
            return "attr";
        }
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
    };

    attr_gen const attr = attr_gen();

}}}

#endif
