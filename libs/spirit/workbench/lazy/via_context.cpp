/*=============================================================================
    Copyright (c) 2013 Agustín Bergé

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <boost/spirit/home/x3.hpp>

#include <boost/fusion/include/pair.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/utility/enable_if.hpp>

namespace workbench { namespace x3
{
    using namespace boost::spirit::x3;

    template <typename T>
    struct lazy_tagged
    {
        explicit lazy_tagged(T const& value)
          : value_(value) {}

        T value_;
    };
    struct lazy_tag
    {
        template <typename T>
        lazy_tagged<T> operator()(T const& value) const
        {
            return lazy_tagged<T>(value);
        }
    } static const lazy = {};

    template <typename F, typename Attribute, typename Context>
    auto lazy_eval(F const& f
      , Attribute const& attribute, Context const& context, bool& pass)
     -> decltype(f(attribute, context, pass))
    {
        return f(attribute, context, pass);
    }
    
    template <typename F, typename Attribute, typename Context>
    auto eval(lazy_tagged<F> const& f
      , Attribute const& attribute, Context const& context, bool& pass)
     -> decltype(lazy_eval(f.value_, attribute, context, pass))
    {
        return lazy_eval(f.value_, attribute, context, pass);
    }
    template <typename V, typename Attribute, typename Context>
    V const& eval(V const& v
      , Attribute const& attribute, Context const& context, bool& pass)
    {
        return v;
    }

    template <typename Subject, typename ...Args>
    struct lazy_arguments_parser : unary_parser<Subject, lazy_arguments_parser<Subject, Args...>>
    {
        typedef unary_parser<Subject, lazy_arguments_parser<Subject, Args...>> base_type;

        explicit lazy_arguments_parser(Subject const& subject, Args&&... args)
          : base_type(subject)
          , arguments(args...)
        {}
        
        template <int ArgIndex, typename Iterator, typename Context, typename Attribute>
        bool parse_(Iterator& first, Iterator const& last
          , Context& context, Attribute& attr, boost::mpl::true_) const
        {
            return this->subject.parse(first, last, context, attr);
        }
        template <int ArgIndex, typename Iterator, typename Context, typename Attribute>
        bool parse_(Iterator& first, Iterator const& last
          , Context& context_, Attribute& attr, boost::mpl::false_) const
        {
            auto lazy_arg = boost::get<ArgIndex>(this->arguments);
            typedef decltype(lazy_arg) lazy_arg_type;
            
            bool pass = true;
            auto arg = eval(lazy_arg.second, attr, context_, pass);

            auto context = 
                boost::spirit::make_context<lazy_arg_type::first_type>(arg, context_);
            return this->parse_<ArgIndex + 1>(
                first, last, context, attr
              , boost::mpl::bool_<ArgIndex + 1 == sizeof...(Args)>());
        }
        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Attribute& attr) const
        {
            return this->parse_<0>(first, last, context, attr, boost::mpl::false_());
        }

        boost::tuple<Args...> arguments;
    };

    /** parser gets arguments via context **/
    struct attr_parser : parser<attr_parser>
    {
        struct value_tag;

        static bool const has_attribute = true;

        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Attribute& attr_) const
        {
            auto&& value = boost::spirit::get<value_tag>(context);
            boost::spirit::traits::move_to(value, attr_);
            return true;
        }
    };

    struct attr_gen
    {
        template <typename Value>
        lazy_arguments_parser<
            attr_parser
          , boost::fusion::pair<attr_parser::value_tag, Value>
        >
        operator()(Value&& value) const
        {
            typedef 
                lazy_arguments_parser<
                    attr_parser
                  , boost::fusion::pair<attr_parser::value_tag, Value>
                >
                result_type;

            return result_type(attr_parser(), std::forward<Value>(value));
        }
    };

    attr_gen const attr = attr_gen();

}}

int main()
{
    using namespace workbench;

    auto answer = [](x3::unused_type, x3::unused_type, bool&){ return 42; };

    char const* empty = "";
    int attr;
    x3::parse(empty, empty+1, x3::attr(42), attr);
    x3::parse(empty, empty+1, x3::attr(x3::lazy(answer)), attr);
}
