#ifndef ANY_TO_BOOST_FUNCTION_HXX
# define ANY_TO_BOOST_FUNCTION_HXX

# include <boost/function.hpp>
# include <urbi/object/fwd.hh>

namespace urbi
{
  namespace object
  {
    template <typename T>
    T AnyToBoostFunction<T>::convert(T v)
    {
      // If you fail here, the given type is not supported for
      // conversion to boost::function
      return v;
    }


    // Return: True, Method: 0, Arguments: 0
    template <typename R, typename S>
    struct AnyToBoostFunction<R (*)(S)>
    {
      typedef boost::function1<R, S> type;
      static type
      convert(R (*v)(S))
      {
        return type(v);
      }
    };

    // Return: True, Method: 0, Arguments: 1
    template <typename R, typename S, typename Arg0>
    struct AnyToBoostFunction<R (*)(S, Arg0)>
    {
      typedef boost::function2<R, S, Arg0> type;
      static type
      convert(R (*v)(S, Arg0))
      {
        return type(v);
      }
    };

    // Return: True, Method: 0, Arguments: 2
    template <typename R, typename S, typename Arg0, typename Arg1>
    struct AnyToBoostFunction<R (*)(S, Arg0, Arg1)>
    {
      typedef boost::function3<R, S, Arg0, Arg1> type;
      static type
      convert(R (*v)(S, Arg0, Arg1))
      {
        return type(v);
      }
    };

    // Return: True, Method: 0, Arguments: 3
    template <typename R, typename S, typename Arg0, typename Arg1, typename Arg2>
    struct AnyToBoostFunction<R (*)(S, Arg0, Arg1, Arg2)>
    {
      typedef boost::function4<R, S, Arg0, Arg1, Arg2> type;
      static type
      convert(R (*v)(S, Arg0, Arg1, Arg2))
      {
        return type(v);
      }
    };

    // Return: True, Method: 0, Arguments: 4
    template <typename R, typename S, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
    struct AnyToBoostFunction<R (*)(S, Arg0, Arg1, Arg2, Arg3)>
    {
      typedef boost::function5<R, S, Arg0, Arg1, Arg2, Arg3> type;
      static type
      convert(R (*v)(S, Arg0, Arg1, Arg2, Arg3))
      {
        return type(v);
      }
    };

    // Return: True, Method: 1, Arguments: 0
    template <typename R, typename S>
    struct AnyToBoostFunction<R (S::*)()>
    {
      typedef boost::function1<R, S* > type;
      static type
      convert(R (S::*v)())
      {
        return type(v);
      }
    };

    // Return: True, Method: 1, Arguments: 1
    template <typename R, typename S, typename Arg0>
    struct AnyToBoostFunction<R (S::*)(Arg0)>
    {
      typedef boost::function2<R, S* , Arg0> type;
      static type
      convert(R (S::*v)(Arg0))
      {
        return type(v);
      }
    };

    // Return: True, Method: 1, Arguments: 2
    template <typename R, typename S, typename Arg0, typename Arg1>
    struct AnyToBoostFunction<R (S::*)(Arg0, Arg1)>
    {
      typedef boost::function3<R, S* , Arg0, Arg1> type;
      static type
      convert(R (S::*v)(Arg0, Arg1))
      {
        return type(v);
      }
    };

    // Return: True, Method: 1, Arguments: 3
    template <typename R, typename S, typename Arg0, typename Arg1, typename Arg2>
    struct AnyToBoostFunction<R (S::*)(Arg0, Arg1, Arg2)>
    {
      typedef boost::function4<R, S* , Arg0, Arg1, Arg2> type;
      static type
      convert(R (S::*v)(Arg0, Arg1, Arg2))
      {
        return type(v);
      }
    };

    // Return: True, Method: 1, Arguments: 4
    template <typename R, typename S, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
    struct AnyToBoostFunction<R (S::*)(Arg0, Arg1, Arg2, Arg3)>
    {
      typedef boost::function5<R, S* , Arg0, Arg1, Arg2, Arg3> type;
      static type
      convert(R (S::*v)(Arg0, Arg1, Arg2, Arg3))
      {
        return type(v);
      }
    };

    // Return: True, Method: 2, Arguments: 0
    template <typename R, typename S>
    struct AnyToBoostFunction<R (S::*)() const>
    {
      typedef boost::function1<R, const S* > type;
      static type
      convert(R (S::*v)() const)
      {
        return type(v);
      }
    };

    // Return: True, Method: 2, Arguments: 1
    template <typename R, typename S, typename Arg0>
    struct AnyToBoostFunction<R (S::*)(Arg0) const>
    {
      typedef boost::function2<R, const S* , Arg0> type;
      static type
      convert(R (S::*v)(Arg0) const)
      {
        return type(v);
      }
    };

    // Return: True, Method: 2, Arguments: 2
    template <typename R, typename S, typename Arg0, typename Arg1>
    struct AnyToBoostFunction<R (S::*)(Arg0, Arg1) const>
    {
      typedef boost::function3<R, const S* , Arg0, Arg1> type;
      static type
      convert(R (S::*v)(Arg0, Arg1) const)
      {
        return type(v);
      }
    };

    // Return: True, Method: 2, Arguments: 3
    template <typename R, typename S, typename Arg0, typename Arg1, typename Arg2>
    struct AnyToBoostFunction<R (S::*)(Arg0, Arg1, Arg2) const>
    {
      typedef boost::function4<R, const S* , Arg0, Arg1, Arg2> type;
      static type
      convert(R (S::*v)(Arg0, Arg1, Arg2) const)
      {
        return type(v);
      }
    };

    // Return: True, Method: 2, Arguments: 4
    template <typename R, typename S, typename Arg0, typename Arg1, typename Arg2, typename Arg3>
    struct AnyToBoostFunction<R (S::*)(Arg0, Arg1, Arg2, Arg3) const>
    {
      typedef boost::function5<R, const S* , Arg0, Arg1, Arg2, Arg3> type;
      static type
      convert(R (S::*v)(Arg0, Arg1, Arg2, Arg3) const)
      {
        return type(v);
      }
    };


    // Treat the case of argument-less functions manually
    template <typename R>
    R ignore_self(R (*f)(), urbi::object::rObject)
    {
      return f();
    }

    template <typename R>
    struct AnyToBoostFunction<R (*) ()>
    {
      typedef boost::function1<R, urbi::object::rObject> type;
      static type
      convert(R (*f) ())
      {
        return boost::bind(ignore_self<R>, f, _1);
      }
    };
  }
}
#endif

