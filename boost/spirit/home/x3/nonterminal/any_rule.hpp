/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman
    Copyright (c) 2013 Agustín Bergé

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#ifndef BOOST_SPIRIT_X3_NONTERMINAL_ANY_RULE_HPP
#define BOOST_SPIRIT_X3_NONTERMINAL_ANY_RULE_HPP

#if defined(_MSC_VER)
#pragma once
#endif

#include <boost/assert.hpp>
#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/support/context.hpp>
#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/home/support/traits/container_traits.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/decay.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/utility/enable_if.hpp>
#include <string>

#if !defined(BOOST_SPIRIT_NO_RTTI)
#include <typeinfo>
#endif

namespace boost { namespace spirit { namespace x3 {
    
    template<
        typename Iterator
      , typename Attribute = unused_type
      , typename Context = unused_type
    >
    struct any_rule;

    template<typename Iterator, typename Attribute, typename Context>
    struct any_rule_parser : parser<any_rule_parser<Iterator, Attribute, Context>>
    {
        typedef any_rule<Iterator, Attribute, Context> rule_type;
        
        typedef Attribute attribute_type;

        static bool const has_attribute =
            !is_same<Attribute, unused_type>::value;
        static bool const handles_container =
            traits::is_container<Attribute>::value;
        
        explicit any_rule_parser(rule_type const* rule_ptr)
          : _ptr(rule_ptr) {}

        template <typename Iterator_, typename Context_, typename Attribute_>
        bool parse(Iterator_& first, Iterator_ const& last
          , Context_ const& context, Attribute_& attr) const
        {
            return _ptr->parse(first, last, context, attr);
        }
        
        std::string get_info() const
        {
            return _ptr->get_info();
        }

        rule_type const* _ptr;
    };

    struct any_rule_base {};

    template <typename Iterator, typename Attribute, typename Context>
    struct any_rule : any_rule_base
    {
        typedef Iterator iterator_type;
        typedef Attribute attribute_type;
        typedef Context context_type;

        typedef any_rule_parser<Iterator, Attribute, Context> parser_type;
        
#if !defined(BOOST_SPIRIT_NO_RTTI)
        any_rule(char const* name = typeid(any_rule).name())
#else
        any_rule(char const* name = "unnamed")
#endif
          : _content(nullptr), _name(name) {}

        template <typename Expr>
#if !defined(BOOST_SPIRIT_NO_RTTI)
        explicit any_rule(Expr const& expr, char const* name = typeid(any_rule).name())
#else
        explicit any_rule(Expr const& expr, char const* name = "unnamed")
#endif
          : _content(make_holder(expr)), _name(name) {}

        ~any_rule() { delete _content; }

        parser_type alias() const
        {
            return parser_type(this);
        }

        template <typename Expr>
        any_rule& operator=(Expr const& expr)
        {
            placeholder* new_content = make_holder(expr);

            delete _content;
            _content = new_content;

            return *this;
        }

        template<typename Iterator_, typename Context_>
        bool parse(Iterator_& first, Iterator_ const& last
          , Context_ const& context, attribute_type& attr) const
        {
            BOOST_STATIC_ASSERT_MSG(
                (is_same<iterator_type, Iterator_>::value)
              , "Incompatible iterator used"
            );

            BOOST_ASSERT_MSG(
                (_content != nullptr)
              , "Invalid use of uninitialized any_rule"
            );
            return _content->parse(first, last, context, attr);
        }
        template<typename Iterator_, typename Context_, typename Attribute_>
        bool parse(Iterator_& first, Iterator_ const& last
          , Context_ const& context, Attribute_& attr_) const
        {
            Attribute attr;
            if(parse(first, last, context, attr))
            {
                traits::move_to(attr, attr_);
                return true;
            }
            return false;
        }
        
        std::string get_info() const
        {
            BOOST_ASSERT_MSG(
                (_content != nullptr)
              , "Invalid use of uninitialized any_rule"
            );
            return _name + ("{" + _content->get_info() + "}");
        }

    private:
        struct placeholder
        {
            virtual bool parse(iterator_type& first, iterator_type const& last
              , context_type const& context, attribute_type& attr) const = 0;

            virtual std::string get_info() const = 0;

            virtual ~placeholder() {}
        };
        
        template <typename Parser>
        struct holder : placeholder
        {
            explicit holder(Parser const& parser)
              : _parser(parser) {}

            virtual bool parse(iterator_type& first, iterator_type const& last
              , context_type const& context, attribute_type& attr) const
            {
                return _parser.parse(first, last, context, attr);
            }

            virtual std::string get_info() const
            {
                return what(_parser);
            }

            Parser const _parser;
        };

        template <typename Expr>
        holder<typename decay<typename extension::as_parser<Expr>::type>::type>*
        make_holder(Expr const& expr)
        {
            typedef typename decay<
                typename extension::as_parser<Expr>::type
            >::type parser_type;

            return new holder<parser_type>(as_parser(expr));
        }

    private:
        placeholder* _content;
        char const* _name;
        
    private:
        // non-copyable
        any_rule(any_rule const&);
    };
    
    template <typename Iterator, typename Attribute, typename Context>
    struct get_info<any_rule< Iterator, Attribute, Context>>
    {
        typedef std::string result_type;
        std::string operator()(
            any_rule<Iterator, Attribute, Context> const& parser ) const
        {
            return parser.get_info();
        }
    };

    namespace extension
    {
        
        template <typename Derived>
        struct as_parser<Derived
            , typename enable_if<is_base_of<any_rule_base, Derived>>::type>
        {
            typedef typename Derived::parser_type type;
            typedef typename Derived::parser_type value_type;
            static type call(Derived const& p)
            {
                return p.alias();
            }
        };

    } // namespace extension

} } } // namespace boost::spirit::x3

#endif /*BOOST_SPIRIT_X3_NONTERMINAL_ANY_RULE_HPP*/
