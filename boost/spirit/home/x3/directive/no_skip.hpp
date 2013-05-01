/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2013 Agustín Bergé

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#ifndef BOOST_SPIRIT_X3_DIRECTIVE_NO_SKIP_HPP
#define BOOST_SPIRIT_X3_DIRECTIVE_NO_SKIP_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/support/context.hpp>
#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/home/support/traits/attribute_of.hpp>
#include <boost/spirit/home/x3/core/skip_over.hpp>
#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/utility/enable_if.hpp>

namespace boost { namespace spirit { namespace x3
{
    // same as lexeme[], but does not pre-skip
    template <typename Subject>
    struct no_skip_directive : unary_parser<Subject, no_skip_directive<Subject>>
    {
        typedef unary_parser<Subject, no_skip_directive<Subject> > base_type;
        static bool const is_pass_through_unary = true;

        no_skip_directive(Subject const& subject)
          : base_type(subject) {}

        typedef typename
            traits::attribute_of<Subject>::type
        attribute_type;
        static bool const has_attribute =
            !is_same<attribute_type, unused_type>::value;
        static bool const handles_container =
            Subject::handles_container;

        template <typename Iterator, typename Context, typename Attribute>
        typename enable_if<has_skipper<Context>, bool>::type
        parse(Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr) const
        {
            auto const& skipper = get<skipper_tag>(context);

            typedef unused_skipper<
                typename remove_reference<decltype(skipper)>::type>
            unused_skipper_type;
            unused_skipper_type unused_skipper(skipper);

            return this->subject.parse(
                first, last
              , make_context<skipper_tag>(unused_skipper, context)
              , attr);
        }
        template <typename Iterator, typename Context, typename Attribute>
        typename disable_if<has_skipper<Context>, bool>::type
        parse(Iterator& first, Iterator const& last
          , Context const& context, Attribute& attr) const
        {
            return this->subject.parse(
                first, last
              , context
              , attr);
        }
    };

    struct no_skip_gen
    {
        template <typename Subject>
        no_skip_directive<typename extension::as_parser<Subject>::value_type>
        operator[](Subject const& subject) const
        {
            typedef
                no_skip_directive<typename extension::as_parser<Subject>::value_type>
            result_type;

            return result_type(as_parser(subject));
        }
    };

    no_skip_gen const no_skip = no_skip_gen();
}}}

#endif
