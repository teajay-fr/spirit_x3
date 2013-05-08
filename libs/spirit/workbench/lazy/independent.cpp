/*=============================================================================
    Copyright (c) 2013 Agustín Bergé

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <boost/spirit/home/x3.hpp>
#include <boost/tuple/tuple.hpp>

namespace workbench { namespace x3
{
    using namespace boost::spirit::x3;

    template <typename F, typename Attribute, typename Context>
    auto lazy_eval(F&& f
      , Attribute const& attribute, Context const& context, bool& pass)
     -> decltype(f(attribute, context, pass))
    {
        return f(attribute, context, pass);
    }
    /*template <typename F, typename Attribute, typename Context>
    F&& lazy_eval(F&& f
      , Attribute const& attribute, Context const& context, bool& pass)
    {
        return f;
    }*/

    template <typename Generator>
    struct lazy_parser : parser<lazy_parser<Generator>>
    {
        typedef parser<lazy_parser<Generator>> base_type;
        typedef Generator subject_type;
        
        static bool const has_attribute = true; // how would I know...?
        /*static bool const has_attribute =
            !is_same<attribute_type, unused_type>::value;
        static bool const handles_container =
            traits::is_container<attribute_type>::value;*/

        lazy_parser(Generator const& generator)
          : generator(generator) {}

        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Attribute& attr) const
        {
            bool _dummy_pass = true;
            auto parser = as_parser(eval(attr, context, _dummy_pass));

            Iterator i = first;
            if(parser.parse(i, last, context, attr))
            {
                first = i;
                return true;
            }
            return false;
        }
        
        Generator generator;
        
        template <typename Attribute, typename Context>
        auto eval(Attribute& attr, Context& context, bool& pass) const
          -> decltype(lazy_eval(this->generator, attr, context, pass))
        {
            return lazy_eval(this->generator, attr, context, pass);
        }
    };

    template <typename Generator, typename ...Args>
    struct lazy_generator_parser : parser<lazy_generator_parser<Generator, Args...>>
    {
        typedef parser<lazy_generator_parser<Generator, Args...>> base_type;
        typedef Generator subject_type;
        static const std::size_t generator_arity = sizeof...(Args);
        
        static bool const has_attribute = true; // how would I know...?
        /*static bool const has_attribute =
            !is_same<attribute_type, unused_type>::value;
        static bool const handles_container =
            traits::is_container<attribute_type>::value;*/

        lazy_generator_parser(Generator const& generator, Args const&... arguments)
          : generator(generator)
          , arguments(arguments...) {}

        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Attribute& attr) const
        {
            bool _dummy_pass = true;
            auto parser = as_parser(eval(attr, context, _dummy_pass));

            Iterator i = first;
            if(parser.parse(i, last, context, attr))
            {
                first = i;
                return true;
            }
            return false;
        }
        
        Generator generator;
        boost::tuple<Args...> arguments;
        
        template <typename Attribute, typename Context, typename ...T>
        auto eval(Attribute& attr, Context& context, bool& pass, boost::mpl::true_, T&&... v) const
         -> decltype(this->generator(std::forward<T>(v)...))
        {
            return this->generator(std::forward<T>(v)...);
        }
        template <typename Attribute, typename Context, typename ...T>
        auto eval(Attribute& attr, Context& context, bool& pass, boost::mpl::false_, T&&... v) const
         -> decltype(this->eval(attr, context, pass, boost::mpl::bool_<sizeof...(T) + 1 == sizeof...(Args)>()
              , std::forward<T>(v)..., lazy_eval(boost::get<sizeof...(T)>(this->arguments), attr, context, pass)))
        {
            return this->eval(attr, context, pass, boost::mpl::bool_<sizeof...(T) + 1 == sizeof...(Args)>()
              , std::forward<T>(v)..., lazy_eval(boost::get<sizeof...(T)>(this->arguments), attr, context, pass));
        }
        template <typename Attribute, typename Context>
        auto eval(Attribute& attr, Context& context, bool& pass) const
         -> decltype(this->eval(attr, context, pass, boost::mpl::false_()))
        {
            return this->eval(attr, context, pass, boost::mpl::false_());
        }
    };

    struct lazy_gen
    {
        template <typename Parser>
        lazy_parser<Parser>
        operator()(Parser const& parser) const
        {
            return lazy_parser<Parser>(parser);
        }
        template <typename Generator, typename... T>
        lazy_generator_parser<Generator, T...>
        operator()(Generator const& generator, T const&... args) const
        {
            return lazy_generator_parser<Generator, T...>(generator, args...);
        }
    };

    lazy_gen const lazy = lazy_gen();

}}
    
namespace workbench { namespace x3
{

    /** independent parser and generator **/
    template <typename Value>
    struct attr_parser : parser<attr_parser<Value>>
    {
        typedef Value value_type;
        typedef Value attribute_type;

        static bool const has_attribute = true;

        attr_parser(value_type const& value)
          : value_(value) {}

        template <typename Iterator, typename Context, typename Attribute>
        bool parse(Iterator& first, Iterator const& last
          , Context& context, Attribute& attr_) const
        {
            auto copy = value_;
            boost::spirit::traits::move_to(copy, attr_);
            return true;
        }

        value_type const value_;
    };

    struct attr_gen
    {
        template <typename Value>
        attr_parser<Value>
        operator()(Value&& value) const
        {
            typedef attr_parser<Value> result_type;

            return result_type(std::forward<Value>(value));
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
    x3::parse(empty, empty+1, x3::lazy(x3::attr, answer), attr);
}
