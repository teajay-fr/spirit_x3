/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef BOOST_SPIRIT_X3_NUMERIC_INT_HPP
#define BOOST_SPIRIT_X3_NUMERIC_INT_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/core/skip_over.hpp>
#include <boost/spirit/home/x3/numeric/int_policies.hpp>
#include <cstdint>

namespace boost { namespace spirit { namespace x3
{
    ///////////////////////////////////////////////////////////////////////////
    template <
        typename T
      , unsigned Radix = 10
      , unsigned MinDigits = 1
      , int MaxDigits = -1
      , typename Policies = int_policies<T, Radix, MinDigits, MaxDigits>>
    struct int_parser
      : parser<int_parser<T, Radix, MinDigits, MaxDigits, Policies>>
      , Policies
    {
        typedef T attribute_type;
        static bool const has_attribute = true;
        
        int_parser() {}
        int_parser(Policies const& p)
          : Policies(p) {}

        template <typename Iterator, typename Context>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, attribute_type& attr) const
        {
            x3::skip_over(first, last, context);
            
            bool hit = true;
            Iterator save = first;
            
            bool sign = true;
            hit = Policies::parse_sign(first, last, sign);
            if(hit)
                hit = Policies::parse_n(first, last, sign, attr);

            if (!hit)
            {
                first = save;
                return false;
            }
            return true;
        }
        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr_) const
        {
            // this case is called when Attribute is not T
            attribute_type attr;
            if (parse(first, last, context, attr))
            {
                traits::move_to(attr, attr_);
                return true;
            }
            return false;
        }
    };

#define BOOST_SPIRIT_INT_PARSER(int_type, name)                                 \
    typedef int_parser<int_type> name##type;                                    \
    name##type const name = name##type();                                       \
    /***/

    BOOST_SPIRIT_INT_PARSER(long, long_)
    BOOST_SPIRIT_INT_PARSER(short, short_)
    BOOST_SPIRIT_INT_PARSER(int, int_)
    BOOST_SPIRIT_INT_PARSER(long long, long_long)

    BOOST_SPIRIT_INT_PARSER(int8_t, int8)
    BOOST_SPIRIT_INT_PARSER(int16_t, int16)
    BOOST_SPIRIT_INT_PARSER(int32_t, int32)
    BOOST_SPIRIT_INT_PARSER(int64_t, int64)

#undef BOOST_SPIRIT_INT_PARSER

}}}

#endif
