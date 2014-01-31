/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_KEYWORD_JAN_07_2013_1131AM)
#define SPIRIT_KEYWORD_JAN_07_2013_1131AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/support/traits/attribute_of.hpp>
#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/string/keyword.hpp>
#include <boost/spirit/home/x3/operator/detail/sequence.hpp>
#include <boost/spirit/home/x3/operator/detail/keyword.hpp>
#include <boost/spirit/home/x3/string/symbols.hpp>

namespace boost { namespace spirit { namespace x3
{
    template <typename Left, typename Right>
    struct keyword : binary_parser< Left, Right, keyword<Left , Right>>
    {
        typedef binary_parser<Left, Right, keyword< Left, Right>> base_type;

        keyword(Left left, Right right)
            : base_type(left, right) {

        }
#if 0
        template <typename Iterator, typename Context, typename Attribute>
        bool parse(
            Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr) const
        {
            return false;
        }
    };

    template <typename Left, typename Right>
    struct parsing_keyword : binary_parser< Left, Right, parsing_keyword<Left , Right>>
    {
        typedef binary_parser<Left, Right, parsing_keyword< Left, Right>> base_type;


        parsing_keyword(Left left, Right right)
            : base_type(left, right)
        {

        }
#endif
        template <typename Iterator, typename Context, typename Attribute>
        bool parse(
            Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr) const
        {

          typedef detail::get_kwd_parser_types<Left, Right, Context, Attribute> parser_types;
//          typedef typename make_variant_over<typename parser_types::type>::type parser_variant;
//          typedef tst<char, parser_variant> Lookup;
          /*static Lookup lookup;
          if(lookup.empty())
            {
              parser_types::add_keyword(lookup,*this);
            }

//          parser_types subject;
            x3::skip_over(first, last, context);
            if(parser_variant *subparser =
                    lookup.find(first,last,tst_pass_through()))
            {
                return boost::apply_visitor(
                      detail::keyword_parse_visitor<Iterator,Context,Attribute>(first,last,context,attr),*subparser);
            }
*/
            return false;
        }
    };

    template <typename Left, typename Right>
    inline parsing_keyword<
        typename extension::as_parser<Left>::value_type
      , typename extension::as_parser<Right>::value_type>
    operator/(Left const& left, Right const& right)
    {
        return {as_parser(left), as_parser(right)};
    }
    template <typename LeftKey, typename LeftSubject, typename Right>
    inline keyword<
          typename extension::as_parser< keyword_parser<LeftKey,LeftSubject> >::value_type
        , typename extension::as_parser< Right>::value_type>
    operator/(keyword_parser<LeftKey,LeftSubject> const &left, Right const &right)
    {
       return {as_parser(left),as_parser(right)};
    }
}}}

namespace boost { namespace spirit { namespace x3 { namespace traits
{
    template <typename LeftKey, typename LeftSubject, typename RightKey, typename RightSubject, typename Context>
    struct attribute_of<x3::keyword<keyword_parser<LeftKey,LeftSubject>, keyword_parser<RightKey,RightSubject>>, Context>
        : x3::detail::attribute_of_sequence<LeftSubject, RightSubject, Context> {};

    template <typename LeftKey, typename LeftSubject, typename RightKey, typename RightSubject, typename Context>
    struct attribute_of<x3::parsing_keyword<keyword_parser<LeftKey,LeftSubject>, keyword_parser<RightKey,RightSubject>>, Context>
        : x3::detail::attribute_of_sequence<LeftSubject, RightSubject, Context> {};
}}}}

#endif
