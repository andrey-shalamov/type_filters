#include <iostream>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/vector.hpp>
 
template <template <typename> class Pred, typename TUPLE, typename Res = std::tuple<>>
struct Filter;

template <template <typename> class Pred, typename Res> 
struct Filter<Pred, std::tuple<>, Res>
{
    using type = Res;
};

template <template <typename> class Pred, typename T, typename ... Ts, typename ... TRes> 
struct Filter<Pred, std::tuple<T, Ts...>, std::tuple<TRes...>> :
    Filter<Pred,
           std::tuple<Ts...>,
           typename std::conditional<Pred<T>::value,
                              std::tuple<TRes..., T>,
                              std::tuple<TRes...>
                              >::type
  >
{
};
 

struct tag_x {};
struct tag_y {};

struct A { typedef tag_x tag; };
struct B { typedef tag_y tag; };
struct C { typedef tag_x tag; };

template <typename T>
using has_tag_x = std::is_same<tag_x, typename T::tag>;

struct f1
{
    template<typename T>
    void operator()(const T& t) const
    {
        std::cout << typeid(t).name() << std::endl;
    }
};

///////////////////

template<template<typename...> class To, typename From> struct tuple_change;

template<template<typename...> class To, template<typename...> class From, typename ... Args>
struct tuple_change<To, From<Args...>>
{
  typedef To<Args...> type;
};

template<template<typename...> class To, typename From>
struct to_boost;

template<template<typename...> class To, typename...Args >
struct to_boost<To, std::tuple<Args...> > :
  tuple_change< boost::mpl::vector, std::tuple<Args...> >
{ };

////////////////////////

int main()
{
    typedef Filter
            <
                has_tag_x,
                std::tuple<A, B, C>
            >::type
            filter_t;
    typedef typename to_boost<boost::mpl::vector, filter_t >::type boost_vec;
    static_assert
    (
        std::is_same
        <
            boost::mpl::vector<A, C>,
            boost_vec
        >::value,
        "unexpected"
    );
    
    boost::mpl::for_each<boost_vec>(f1());
}

