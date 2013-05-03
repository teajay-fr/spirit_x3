/*=============================================================================
    Copyright (c) 2013 Agustín Bergé

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#include <boost/spirit/home/x3.hpp>

namespace workbench { namespace x3
{
    using namespace boost::spirit::x3;

    namespace detail
    {

        struct eval_empty {};
        struct eval_with_attr : eval_empty {};
        struct eval_with_attr_context : eval_with_attr {};
        struct eval_with_attr_context_pass : eval_with_attr_context {};
        
        template <typename F, typename Attribute, typename Context>
        static auto eval(F&& f, Attribute const& attribute, Context const& context, bool& pass, eval_empty)
         -> decltype(f())
        {
            return f();
        }
        
        template <typename F, typename Attribute, typename Context>
        static auto eval(F&& f, Attribute const& attribute, Context const& context, bool& pass, eval_with_attr)
         -> decltype(f(attribute))
        {
            return f(attribute);
        }

        template <typename F, typename Attribute, typename Context>
        static auto eval(F&& f, Attribute const& attribute, Context const& context, bool& pass, eval_with_attr_context)
         -> decltype(f(attribute, context))
        {
            return f(attribute, context);
        }

        template <typename F, typename Attribute, typename Context>
        static auto eval(F&& f, Attribute const& attribute, Context const& context, bool& pass, eval_with_attr_context_pass)
         -> decltype(f(attribute, context, pass))
        {
            return f(attribute, context, pass);
        }

    }

    template <typename F, typename Attribute, typename Context>
    auto lazy_eval(F&& f
      , Attribute const& attribute, Context const& context, bool& pass)
     -> decltype(detail::eval(f, attribute, context, pass, detail::eval_with_attr_context_pass()))
    {
        return detail::eval(f, attribute, context, pass, detail::eval_with_attr_context_pass());
    }

}}

int main()
{
    using namespace workbench;

    int attribute = 0;
    x3::unused_type context;
    bool pass = true;

    auto answer0 = [](){ return 42; };
    attribute = x3::lazy_eval(answer0, attribute, context, pass);
    
    auto answer1 = [](x3::unused_type){ return 42; };
    attribute = x3::lazy_eval(answer1, attribute, context, pass);
    
    auto answer2 = [](x3::unused_type, x3::unused_type){ return 42; };
    attribute = x3::lazy_eval(answer2, attribute, context, pass);

    auto answer3 = [](x3::unused_type, x3::unused_type, bool&){ return 42; };
    attribute = x3::lazy_eval(answer3, attribute, context, pass);

    struct multi_answer
    {
        int operator()() const { return 0; };
        int operator()(x3::unused_type) const { return 1; };
        int operator()(x3::unused_type, x3::unused_type) const { return 2; };
        int operator()(x3::unused_type, x3::unused_type, bool&) const { return 3; };
    };
    attribute = x3::lazy_eval(multi_answer(), attribute, context, pass);
}
