/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(SPIRIT_KEYWORD_DETAIL_JAN_06_2013_1015AM)
#define SPIRIT_KEYWORD_DETAIL_JAN_06_2013_1015AM

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/spirit/home/x3/support/traits/attribute_of.hpp>
#include <boost/spirit/home/x3/support/traits/attribute_category.hpp>
#include <boost/spirit/home/x3/support/traits/make_attribute.hpp>
#include <boost/spirit/home/x3/support/traits/has_attribute.hpp>
#include <boost/spirit/home/x3/support/traits/is_substitute.hpp>
#include <boost/spirit/home/x3/support/traits/container_traits.hpp>
#include <boost/spirit/home/x3/core/detail/parse_into_container.hpp>
#include <boost/spirit/home/x3/support/traits/string_traits.hpp>

#include <boost/fusion/include/begin.hpp>
#include <boost/fusion/include/end.hpp>
#include <boost/fusion/include/advance.hpp>
#include <boost/fusion/include/empty.hpp>
#include <boost/fusion/include/front.hpp>
#include <boost/fusion/include/iterator_range.hpp>
#include <boost/fusion/include/as_deque.hpp>
#include <boost/fusion/include/mpl.hpp>

#include <boost/mpl/copy_if.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>

#include <boost/type_traits/add_reference.hpp>
#include <boost/type_traits/is_same.hpp>

namespace boost { namespace spirit { namespace x3
{
    template <typename Left, typename Right>
    struct keyword;
}}}

namespace boost { namespace spirit { namespace x3 { namespace detail
{
#if 0
    template <typename Parser, typename Context, typename Enable = void>
    struct sequence_size
    {
        static int const value = traits::has_attribute<Parser, Context>::value;
    };

    template <typename Parser, typename Context>
    struct sequence_size_subject
      : sequence_size<typename Parser::subject_type, Context> {};

    template <typename Parser, typename Context>
    struct sequence_size<Parser, Context
      , typename enable_if_c<(Parser::is_pass_through_unary)>::type>
      : sequence_size_subject<Parser, Context> {};

    template <typename L, typename R, typename Context>
    struct sequence_size<sequence<L, R>, Context>
    {
        static int const value =
            sequence_size<L, Context>::value +
            sequence_size<R, Context>::value;
    };

    struct pass_sequence_attribute_unused
    {
        typedef unused_type type;

        template <typename T>
        static unused_type
        call(T&)
        {
            return unused_type();
        }
    };

    template <typename Attribute>
    struct pass_sequence_attribute_front
    {
        typedef typename fusion::result_of::front<Attribute>::type type;

        static typename add_reference<type>::type
        call(Attribute& attr)
        {
            return fusion::front(attr);
        }
    };

    template <typename Attribute>
    struct pass_through_sequence_attribute
    {
        typedef Attribute& type;

        template <typename Attribute_>
        static Attribute_&
        call(Attribute_& attr)
        {
            return attr;
        }
    };

    template <typename Parser, typename Attribute>
    struct pass_sequence_attribute_used :
        pass_sequence_attribute_front<Attribute> {};

    template <typename Parser, typename Attribute, typename Enable = void>
    struct pass_sequence_attribute :
        mpl::if_<
            fusion::result_of::empty<Attribute>
          , pass_sequence_attribute_unused
          , pass_sequence_attribute_used<Parser, Attribute>>::type {};

    template <typename L, typename R, typename Attribute>
    struct pass_sequence_attribute<sequence<L, R>, Attribute>
      : pass_through_sequence_attribute<Attribute> {};

    template <typename Parser, typename Attribute>
    struct pass_sequence_attribute_subject :
        pass_sequence_attribute<typename Parser::subject_type, Attribute> {};

    template <typename Parser, typename Attribute>
    struct pass_sequence_attribute<Parser, Attribute
      , typename enable_if_c<(Parser::is_pass_through_unary)>::type>
      : pass_sequence_attribute_subject<Parser, Attribute> {};

    template <typename L, typename R, typename Attribute, typename Context
      , typename Enable = void>
    struct partition_attribute
    {
        static int const l_size = sequence_size<L, Context>::value;
        static int const r_size = sequence_size<R, Context>::value;

        // If you got an error here, then you are trying to pass
        // a fusion sequence with the wrong number of elements
        // as that expected by the (sequence) parser.
        static_assert(
            fusion::result_of::size<Attribute>::value == (l_size + r_size)
          , "Attribute does not have the expected size."
        );

        typedef typename fusion::result_of::begin<Attribute>::type l_begin;
        typedef typename fusion::result_of::advance_c<l_begin, l_size>::type l_end;
        typedef typename fusion::result_of::end<Attribute>::type r_end;
        typedef fusion::iterator_range<l_begin, l_end> l_part;
        typedef fusion::iterator_range<l_end, r_end> r_part;
        typedef pass_sequence_attribute<L, l_part> l_pass;
        typedef pass_sequence_attribute<R, r_part> r_pass;

        static l_part left(Attribute& s)
        {
            auto i = fusion::begin(s);
            return l_part(i, fusion::advance_c<l_size>(i));
        }

        static r_part right(Attribute& s)
        {
            return r_part(
                fusion::advance_c<l_size>(fusion::begin(s))
              , fusion::end(s));
        }
    };

    template <typename L, typename R, typename Attribute, typename Context>
    struct partition_attribute<L, R, Attribute, Context,
        typename enable_if_c<(!traits::has_attribute<L, Context>::value &&
            traits::has_attribute<R, Context>::value)>::type>
    {
        typedef unused_type l_part;
        typedef Attribute& r_part;
        typedef pass_sequence_attribute_unused l_pass;
        typedef pass_through_sequence_attribute<Attribute> r_pass;

        static unused_type left(Attribute&)
        {
            return unused;
        }

        static Attribute& right(Attribute& s)
        {
            return s;
        }
    };

    template <typename L, typename R, typename Attribute, typename Context>
    struct partition_attribute<L, R, Attribute, Context,
        typename enable_if_c<(traits::has_attribute<L, Context>::value &&
            !traits::has_attribute<R, Context>::value)>::type>
    {
        typedef Attribute& l_part;
        typedef unused_type r_part;
        typedef pass_through_sequence_attribute<Attribute> l_pass;
        typedef pass_sequence_attribute_unused r_pass;

        static Attribute& left(Attribute& s)
        {
            return s;
        }

        static unused_type right(Attribute&)
        {
            return unused;
        }
    };

    template <typename L, typename R, typename Attribute, typename Context>
    struct partition_attribute<L, R, Attribute, Context,
        typename enable_if_c<(!traits::has_attribute<L, Context>::value &&
            !traits::has_attribute<R, Context>::value)>::type>
    {
        typedef unused_type l_part;
        typedef unused_type r_part;
        typedef pass_sequence_attribute_unused l_pass;
        typedef pass_sequence_attribute_unused r_pass;

        static unused_type left(Attribute&)
        {
            return unused;
        }

        static unused_type right(Attribute&)
        {
            return unused;
        }
    };
#endif

    template <typename Keywords, typename Key, typename Subject>
    void add_keyword(const Keywords &keywords, const keyword_parser<Key,Subject> &parser)
    {
         keywords->add(traits::get_string_begin<char>(parser.left.str)
                      ,traits::get_string_end<char>(parser.left.str)
                      ,const_cast<Subject*>(parser.right.derived_ptr()));
    }


    template <typename Keywords, typename Left, typename Right>
    void add_keyword(const Keywords &keywords, const keyword<Left,Right> &parser)
        {
            add_keyword(keywords,parser.left);
            add_keyword(keywords,parser.right);
        }


    template <typename P, typename I>
        struct kwd_parser_holder
        {
            typedef I attribute_extractor;
            P *parser;
        };
    template <typename L, typename R>
    struct get_kwd_parser_types
    {
        typedef
            mpl::vector< typename L::right_type *, typename R::right_type * >
        type;
    };

    template <typename LL, typename LR, typename R>
    struct get_kwd_parser_types<keyword<LL, LR>, R>
    {
        typedef typename
            mpl::push_back<
                typename get_kwd_parser_types<LL, LR>::type
              , typename R::right_type *
            >::type
        type;
    };

    template <typename L, typename RL, typename RR>
    struct get_kwd_parser_types<L, keyword<RL, RR> >
    {
        typedef typename
            mpl::push_front<
                typename get_kwd_parser_types<RL, RR>::type
              , typename L::right_type *
            >::type
        type;
    };

    template <typename LL, typename LR, typename RL, typename RR>
    struct get_kwd_parser_types<keyword<LL, LR>, keyword<RL, RR>>
    {
        typedef
            mpl::joint_view<
                typename get_kwd_parser_types<LL, LR>::type
              , typename get_kwd_parser_types<RL, RR>::type
            >
        type;
    };

    template <typename L, typename R>
    struct keyword_parser_variant
    {
        // Get all sequence attribute types
        typedef typename get_kwd_parser_types<L, R>::type all_types;

        // Build a variant if filtered_types is not empty,
        // else just return unused_type
        typedef typename make_variant_over<all_types>::type type;

    };

}}}}

#endif
