/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman
    Copyright (c) 2013 Agustín Bergé
    http://spirit.sourceforge.net/

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#ifndef BOOST_SPIRIT_SUBCONTEXT_HPP
#define BOOST_SPIRIT_SUBCONTEXT_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/fusion/support/pair.hpp>
#include <boost/spirit/home/support/context.hpp>

namespace boost { namespace spirit {

    template <typename... T>
    struct subcontext;

    template <>
    struct subcontext<>
    {
        template <typename Context>
        subcontext(Context const& /*context*/)
        {}
    };

    template <typename T>
    struct subcontext<T>
      : boost::spirit::context<
            typename T::first_type, typename T::second_type
        >
    {
        typedef boost::spirit::context<
                typename T::first_type, typename T::second_type
            > context_type;

        template <typename Context>
        subcontext(Context const& context)
          : context_type(boost::spirit::get<typename T::first_type>(context))
        {}
    };

    template <typename T, typename... Tail>
    struct subcontext<T, Tail...>
      : subcontext<Tail...>
      , boost::spirit::context<
            typename T::first_type, typename T::second_type
          , subcontext<Tail...>
        >
    {
        typedef subcontext<Tail...> base_type;
        typedef boost::spirit::context<
                typename T::first_type, typename T::second_type
              , base_type
            > context_type;

        template <typename Context>
        subcontext(Context const& context)
          : base_type(context)
          , context_type(
                boost::spirit::get<typename T::first_type>(context)
              , *static_cast< base_type* >( this ) )
        {}

        using context_type::get;
    };

} } // namespace boost::spirit

#endif /*BOOST_SPIRIT_SUBCONTEXT_HPP*/
