/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c)      2010 Bryce Lelbach
    Copyright (c) 2013 Agustín Bergé

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#ifndef BOOST_SPIRIT_X3_DIRECTIVE_AS_HPP
#define BOOST_SPIRIT_X3_DIRECTIVE_AS_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/home/support/traits/container_traits.hpp>
#include <boost/spirit/home/support/traits/move_to.hpp>
#include <boost/type_traits/is_same.hpp>
#include <string>

namespace boost { namespace spirit { namespace x3
{

    template <typename Subject, typename T>
    struct as_directive : unary_parser<Subject, as_directive<Subject, T>>
    {
        typedef unary_parser<Subject, as_directive<Subject, T>> base_type;
        typedef Subject subject_type;
        typedef T attribute_type;

        static bool const has_attribute =
            !is_same<attribute_type, unused_type>::value;
        static bool const handles_container =
            traits::is_container<attribute_type>::value;

        as_directive(subject_type const& subject)
          : base_type(subject) {}
        
        template <typename Iterator, typename Context>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, attribute_type& attr) const
        {
            Iterator i = first;
            if(this->subject.parse(i, last, context, attr))
            {
                first = i;
                return true;
            }
            return false;
        }
        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Attribute& attr_) const
        {
            attribute_type attr;
            if(parse(first, last, context, attr))
            {
                traits::move_to(attr, attr_);
                return true;
            }
            return false;
        }
    };

    template <typename Subject, typename T>
    struct get_info<as_directive<Subject, T>>
    {
        typedef std::string result_type;
        std::string operator()(as_directive<Subject, T> const& parser) const
        {
            return "as{" + what(parser) + "}";
        }
    };
    
    template <typename T>
    struct as_gen
    {
        template<typename Subject>
        as_directive<typename extension::as_parser<Subject>::value_type, T>
        operator[](Subject const& subject) const
        {
            typedef
                as_directive<typename extension::as_parser<Subject>::value_type, T>
            result_type;

            return result_type(as_parser(subject));
        }
    };

    template <typename T>
    struct as : as_gen<T> {};

    auto const as_string = as_gen<std::string>();
    auto const as_wstring = as_gen<std::wstring>();

}}}

#endif
