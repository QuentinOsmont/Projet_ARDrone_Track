#ifndef LIBPORT_FORMAT_HH
# define LIBPORT_FORMAT_HH

# include <libport/system-warning-push.hh>
# include <boost/format.hpp>
# include <libport/system-warning-pop.hh>

namespace libport
{

  // Also accept 0-ary format strings.  Don't return it directly,
  // still consult boost::format to make sure there are no trailing
  // %s.
  inline
  std::string format(const std::string& fmt)
  {
    boost::format f(fmt);
    return str(f);
  }

  template <typename T0>
  inline
  std::string format(const std::string& fmt, const T0& arg0)
  {
    boost::format f(fmt);
    f % arg0;
    return str(f);
  }

  template <typename T0, typename T1>
  inline
  std::string format(const std::string& fmt, const T0& arg0, const T1& arg1)
  {
    boost::format f(fmt);
    f % arg0 % arg1;
    return str(f);
  }

  template <typename T0, typename T1, typename T2>
  inline
  std::string format(const std::string& fmt, const T0& arg0, const T1& arg1, const T2& arg2)
  {
    boost::format f(fmt);
    f % arg0 % arg1 % arg2;
    return str(f);
  }

  template <typename T0, typename T1, typename T2, typename T3>
  inline
  std::string format(const std::string& fmt, const T0& arg0, const T1& arg1, const T2& arg2, const T3& arg3)
  {
    boost::format f(fmt);
    f % arg0 % arg1 % arg2 % arg3;
    return str(f);
  }

  template <typename T0, typename T1, typename T2, typename T3, typename T4>
  inline
  std::string format(const std::string& fmt, const T0& arg0, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4)
  {
    boost::format f(fmt);
    f % arg0 % arg1 % arg2 % arg3 % arg4;
    return str(f);
  }

  template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5>
  inline
  std::string format(const std::string& fmt, const T0& arg0, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, const T5& arg5)
  {
    boost::format f(fmt);
    f % arg0 % arg1 % arg2 % arg3 % arg4 % arg5;
    return str(f);
  }

  template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
  inline
  std::string format(const std::string& fmt, const T0& arg0, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, const T5& arg5, const T6& arg6)
  {
    boost::format f(fmt);
    f % arg0 % arg1 % arg2 % arg3 % arg4 % arg5 % arg6;
    return str(f);
  }

  template <typename T0, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
  inline
  std::string format(const std::string& fmt, const T0& arg0, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, const T5& arg5, const T6& arg6, const T7& arg7)
  {
    boost::format f(fmt);
    f % arg0 % arg1 % arg2 % arg3 % arg4 % arg5 % arg6 % arg7;
    return str(f);
  }

}

#endif
