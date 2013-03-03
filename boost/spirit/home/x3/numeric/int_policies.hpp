/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2013 Agustín Bergé

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/
#ifndef BOOST_SPIRIT_X3_NUMERIC_INT_POLICIES_HPP
#define BOOST_SPIRIT_X3_NUMERIC_INT_POLICIES_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/support/numeric_utils/detail/extract_int.hpp>

namespace boost { namespace spirit { namespace x3
{

    ///////////////////////////////////////////////////////////////////////////
    //  Default (unsigned) int number policies
    ///////////////////////////////////////////////////////////////////////////
    template <
        typename T
      , unsigned Radix = 10
      , unsigned MinDigits = 1
      , int MaxDigits = -1>
    struct uint_policies
    {
        template <typename Iterator>
        static bool
        parse_sign(Iterator& /*first*/, Iterator const& /*last*/, bool& sign)
        {
            sign = true;
            return true;
        }

        template <typename Iterator, typename Attribute>
        static bool
        parse_n(Iterator& first, Iterator const& last, bool sign, Attribute& attr)
        {
            // check template parameter 'Radix' for validity
            static_assert(
                (Radix == 2 || Radix == 8 || Radix == 10 || Radix == 16),
                "Error Unsupported Radix");

            if (first == last)
                return false;

            typedef spirit::detail::extract_int<
                T, Radix, MinDigits, MaxDigits>
            extract_pos_type;

            typedef spirit::detail::extract_int<
                T, Radix, MinDigits, MaxDigits, spirit::detail::negative_accumulator<Radix> >
            extract_neg_type;

            Iterator save = first;
            bool hit;
            if (sign)
                hit = extract_pos_type::parse(first, last, attr);
            else
                hit = extract_neg_type::parse(first, last, attr);

            if (!hit)
            {
                first = save;
                return false;
            }
            return true;
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    //  Default (signed) int number policies
    ///////////////////////////////////////////////////////////////////////////
    template <
        typename T
      , unsigned Radix = 10
      , unsigned MinDigits = 1
      , int MaxDigits = -1>
    struct int_policies : uint_policies<T, Radix, MinDigits, MaxDigits>
    {
        template <typename Iterator>
        static bool
        parse_sign(Iterator& first, Iterator const& last, bool& sign)
        {
            (void)last;                  // silence unused warnings

            if (first == last)
                return false;

            // Extract the sign
            bool neg = *first == '-';
            if (neg || (*first == '+'))
            {
                ++first;
            }
            sign = !neg;
            return true;
        }
    };

}}}

#endif
