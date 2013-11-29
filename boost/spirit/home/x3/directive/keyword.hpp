/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_KEYWORD_NOVEMBER_28_2013_1024PM)
#define SPIRIT_KEYWORD_NOVEMBER_28_2013_1024PM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/support/context.hpp>
#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/type_traits/add_reference.hpp>



namespace boost { namespace spirit { namespace x3
{

    template <typename Left, typename Right>
    struct keyword_directive : binary_parser<Left,Right, keyword_directive<Left,Right>>
    {
        typedef binary_parser<Left,Right, keyword_directive<Left,Right> > base_type;
        static const bool has_attribute = Right::has_attribute;

        keyword_directive(Left const& left, Right const& right)
          : base_type(left,right) {}

        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr) const
        {
            Iterator save = first;
            if (! this->left.parse(first, last, context, unused))
            {
                first = save;
                return false;
            }
            return this->right.parse(first,last,context,attr);
        }
    };

    template <typename Subject1>
    struct kwd_gen_lvl2
    {
        kwd_gen_lvl2(Subject1 const& sub1)
            :first_subject(sub1)
        {
        }

        template <typename Subject2>
        keyword_directive<typename extension::as_parser<Subject1>::value_type
                         ,typename extension::as_parser<Subject2>::value_type>
        operator[](Subject2 const& subject) const
        {
            typedef
                keyword_directive<typename extension::as_parser<Subject1>::value_type
                                 ,typename extension::as_parser<Subject2>::value_type >
            result_type;

            return result_type(as_parser(first_subject),as_parser(subject));
        }
        Subject1 first_subject;
    };
    struct kwd_gen_lvl1
    {
        template <typename Subject>
        kwd_gen_lvl2<typename extension::as_parser<Subject>::value_type>
        operator()(Subject const& subject) const
        {
            typedef
                kwd_gen_lvl2<typename extension::as_parser<Subject>::value_type>
            result_type;

            return result_type(as_parser(subject));
        }
    };

    kwd_gen_lvl1 const kwd = kwd_gen_lvl1();
}}}

#endif
