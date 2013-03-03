/*=============================================================================
    Copyright (c) 2001-2013 Joel de Guzman
    Copyright (c) 2001-2011 Hartmut Kaiser
    Copyright (c) 2011      Bryce Lelbach
    Copyright (c) 2013 Agustín Bergé

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include "int.hpp"
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/at.hpp>
#include <boost/spirit/home/x3.hpp>

#include <iterator>
#include <locale>
#include <string>
#include <vector>

template <typename Num = int>
struct locale_aware_integer_policies : boost::spirit::x3::int_policies<Num>
{
    typedef boost::spirit::x3::int_policies<Num> base_type;

    char thousands_sep;
    std::string grouping;

    locale_aware_integer_policies(std::locale const& locale = std::locale())
    {
        std::numpunct<char> const& numpunct_facet =
            std::use_facet<std::numpunct<char>>(locale);

        thousands_sep = numpunct_facet.thousands_sep();
        grouping = numpunct_facet.grouping();
    }

    template <typename Iterator>
    bool parse_thousands_sep(Iterator& first, Iterator const& last) const
    {
        if (first == last)
            return false;

        if (*first == thousands_sep)
        {
            ++first;
            return true;
        }
        return false;
    }

    template <typename Iterator, typename Attribute>
    bool parse_n(Iterator& first, Iterator const& last
      , bool sign, Attribute& attr) const
    {
        namespace x3 = boost::spirit::x3;
        namespace spirit = boost::spirit;

        if(grouping.empty() || grouping[0] < 0 || grouping[0] == CHAR_MAX)
            return base_type::parse_n( first, last, sign, attr );

        bool stopped = false;
        std::vector<Num> groups;
        std::vector<std::size_t> group_sizes;
        do
        {
            Iterator start = first;

            Num group_n(0);
            if(x3::parse(first, last, x3::int_, group_n))
            {
                groups.push_back(group_n);
                group_sizes.push_back(std::distance(start, first));
            } else {
                return false;
            }
        } while(parse_thousands_sep(first, last));

        attr = Num();
        unsigned int exp = 0;
        char const* grouping_iter = &grouping.front();
        char const* grouping_end = &grouping.back();
        std::vector< Num >::reverse_iterator group_iter = groups.rbegin();
        for(
            std::vector< std::size_t >::reverse_iterator
                iter = group_sizes.rbegin()
              , end = group_sizes.rend()
              , last = group_sizes.rend() - 1
          ; iter != end
          ; ++iter, ++group_iter
          , grouping_iter != grouping_end
              ? ++grouping_iter
              : grouping_iter
        )
        {
            std::size_t const digits = *iter;
            if(
                (iter != last && digits == *grouping_iter)
             || (iter == last && (digits <= *grouping_iter || *grouping_iter < 0))
            )
            {
                Num group_n = *group_iter;

                using std::pow;
                group_n = group_n * pow(10, exp);

                if(sign)
                    attr = attr + group_n;
                else
                    attr = attr - group_n;
                exp += digits;
            } else {
                return false;
            }
        }
        return true;
    }
};

struct custom_numpunct_facet : std::numpunct<char>
{
    char do_decimal_point() const { return ','; }
    char do_thousands_sep() const { return '.'; }
    std::string do_grouping() const{ return "\x03\x02"; }
};

int
main()
{
    using spirit_test::test;
    using spirit_test::test_attr;
    
    std::locale custom_locale =
        std::locale(std::locale(), new custom_numpunct_facet());

#if INT_MAX != LLONG_MAX
    static_assert(sizeof(int) == 4, "unexpected int size");
    char const* max_int = "2.14.74.83.647";
    char const* int_overflow = "2.14.74.83.648";
    char const* min_int = "-2.14.74.83.648";
    char const* int_underflow = "-2.14.74.83.649";
#else
    static_assert(sizeof(int) == 8, "unexpected int size");
    char const* max_int = "92.23.37.20.36.85.47.75.807";
    char const* int_overflow = "92.23.37.20.36.85.47.75.808";
    char const* min_int = "-92.23.37.20.36.85.47.75.808";
    char const* int_underflow = "-92.23.37.20.36.85.47.75.809";
#endif
    
    char const* max_long_long = "92.23.37.20.36.85.47.75.807";
    char const* long_long_overflow = "92.23.37.20.36.85.47.75.808";
    char const* min_long_long = "-92.23.37.20.36.85.47.75.808";
    char const* long_long_underflow = "-92.23.37.20.36.85.47.75.809";

    ///////////////////////////////////////////////////////////////////////////
    //  signed integer tests
    ///////////////////////////////////////////////////////////////////////////
    {
        boost::spirit::x3::int_parser<
            int, 10, 1, -1
          , locale_aware_integer_policies<int>> int_(custom_locale);
        int i;

        BOOST_TEST(test("1.23.456", int_));
        BOOST_TEST(test_attr("1.23.456", int_, i));
        BOOST_TEST(i == 123456);

        BOOST_TEST(test("+1.23.456", int_));
        BOOST_TEST(test_attr("+1.23.456", int_, i));
        BOOST_TEST(i == 123456);

        BOOST_TEST(test("-1.23.456", int_));
        BOOST_TEST(test_attr("-1.23.456", int_, i));
        BOOST_TEST(i == -123456);

        BOOST_TEST(test(max_int, int_));
        BOOST_TEST(test_attr(max_int, int_, i));
        BOOST_TEST(i == INT_MAX);

        BOOST_TEST(test(min_int, int_));
        BOOST_TEST(test_attr(min_int, int_, i));
        BOOST_TEST(i == INT_MIN);

        BOOST_TEST(!test(int_overflow, int_));
        BOOST_TEST(!test_attr(int_overflow, int_, i));
        BOOST_TEST(!test(int_underflow, int_));
        BOOST_TEST(!test_attr(int_underflow, int_, i));

        BOOST_TEST(!test("-", int_));
        BOOST_TEST(!test_attr("-", int_, i));

        BOOST_TEST(!test("+", int_));
        BOOST_TEST(!test_attr("+", int_, i));

        // Bug report from Steve Nutt
        BOOST_TEST(!test_attr("5.36.87.09.120", int_, i));

        // with leading zeros
        BOOST_TEST(test("0.00.00.00.00.01.23.456", int_));
        BOOST_TEST(test_attr("0.00.00.00.00.01.23.456", int_, i));
        BOOST_TEST(i == 123456);
    }

    ///////////////////////////////////////////////////////////////////////////
    //  long long tests
    ///////////////////////////////////////////////////////////////////////////
    {
        boost::spirit::x3::int_parser<
            long long, 10, 1, -1
          , locale_aware_integer_policies<long long>> long_long(custom_locale);
        boost::long_long_type ll;

        BOOST_TEST(test("12.34.56.78.90.12.34.56.789", long_long));
        BOOST_TEST(test_attr("12.34.56.78.90.12.34.56.789", long_long, ll));
        BOOST_TEST(ll == 1234567890123456789LL);

        BOOST_TEST(test("-12.34.56.78.90.12.34.56.789", long_long));
        BOOST_TEST(test_attr("-12.34.56.78.90.12.34.56.789", long_long, ll));
        BOOST_TEST(ll == -1234567890123456789LL);

        BOOST_TEST(test(max_long_long, long_long));
        BOOST_TEST(test_attr(max_long_long, long_long, ll));
        BOOST_TEST(ll == LLONG_MAX);

        BOOST_TEST(test(min_long_long, long_long));
        BOOST_TEST(test_attr(min_long_long, long_long, ll));
        BOOST_TEST(ll == LLONG_MIN);

        BOOST_TEST(!test(long_long_overflow, long_long));
        BOOST_TEST(!test_attr(long_long_overflow, long_long, ll));
        BOOST_TEST(!test(long_long_underflow, long_long));
        BOOST_TEST(!test_attr(long_long_underflow, long_long, ll));
    }

    ///////////////////////////////////////////////////////////////////////////
    //  short_ and long_ tests
    ///////////////////////////////////////////////////////////////////////////
    {
        boost::spirit::x3::int_parser<
            short, 10, 1, -1
          , locale_aware_integer_policies<short>> short_(custom_locale);
        boost::spirit::x3::int_parser<
            long, 10, 1, -1
          , locale_aware_integer_policies<long>> long_(custom_locale);
        int i;

        BOOST_TEST(test("12.345", short_));
        BOOST_TEST(test_attr("12.345", short_, i));
        BOOST_TEST(i == 12345);

        BOOST_TEST(test("1.23.45.67.890", long_));
        BOOST_TEST(test_attr("1.23.45.67.890", long_, i));
        BOOST_TEST(i == 1234567890);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Check overflow is parse error
    ///////////////////////////////////////////////////////////////////////////
    {
        boost::spirit::x3::int_parser<
            boost::int8_t, 10, 1, -1
          , locale_aware_integer_policies<boost::int8_t>> int8_(custom_locale);
        char c;

        BOOST_TEST(!test_attr("999", int8_, c));

        int i;
        boost::spirit::x3::int_parser<
            short, 10, 1, -1
          , locale_aware_integer_policies<short>> short_(custom_locale);
        BOOST_TEST(!test_attr("32.769", short_, i, false));
        BOOST_TEST(!test_attr("41.234", short_, i, false));
    }
    
    // $$$ Not yet implemented $$$
    ///////////////////////////////////////////////////////////////////////////
    //  int_parser<unused_type> tests
    ///////////////////////////////////////////////////////////////////////////
    //~ {
        //~ using boost::spirit::unused_type;
        //~ boost::spirit::x3::int_parser<
        //~     unused_type, 10, 1, -1
        //~   , locale_aware_integer_policies<unused_type>> any_int(custom_locale);

        //~ BOOST_TEST(test("1.23.456", any_int));
        //~ BOOST_TEST(test("-1.23.456", any_int));
        //~ BOOST_TEST(test("-12.34.56.78.90.12.34.56.789", any_int));
    //~ }

    // $$$ Not yet implemented $$$
    //~ ///////////////////////////////////////////////////////////////////////////
    //~ //  action tests
    //~ ///////////////////////////////////////////////////////////////////////////
    //~ {
        //~ using boost::phoenix::ref;
        //~ using boost::spirit::_1;
        //~ using boost::spirit::x3::ascii::space;
        //~ int n, m;

        //~ BOOST_TEST(test("123", int_[ref(n) = _1]));
        //~ BOOST_TEST(n == 123);
        //~ BOOST_TEST(test_attr("789", int_[ref(n) = _1], m));
        //~ BOOST_TEST(n == 789 && m == 789);
        //~ BOOST_TEST(test("   456", int_[ref(n) = _1], space));
        //~ BOOST_TEST(n == 456);
    //~ }

    ///////////////////////////////////////////////////////////////////////////
    //  custom int tests
    ///////////////////////////////////////////////////////////////////////////
    {
        boost::spirit::x3::int_parser<
            int, 10, 1, -1
          , locale_aware_integer_policies<int>> int_(custom_locale);
        boost::spirit::x3::int_parser<
            custom_int, 10, 1, -1
          , locale_aware_integer_policies<custom_int>> int2(custom_locale);
        custom_int i;

        BOOST_TEST(test_attr("-1.23.456", int_, i));
        //~ BOOST_TEST(test_attr("-12", int2, i));
    }

    ///////////////////////////////////////////////////////////////////////////
    //  single-element fusion vector tests
    ///////////////////////////////////////////////////////////////////////////
    {
        boost::spirit::x3::int_parser<
            int, 10, 1, -1
          , locale_aware_integer_policies<int>> int_(custom_locale);
        boost::fusion::vector<int> i;

        BOOST_TEST(test_attr("-1.23.456", int_, i));
        BOOST_TEST(boost::fusion::at_c<0>(i) == -123456);
    }

    return boost::report_errors();
}
