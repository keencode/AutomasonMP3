/** Static assertion macros 
* Borrowed from boost (see boost.org)
*/
#ifndef GMTL_STATIC_ASSERT_HPP
#define GMTL_STATIC_ASSERT_HPP

#include "gmtl_Defines.h"
#include "Util/gmtl_Meta.h"

#ifdef __BORLANDC__
//
// workaround for buggy integral-constant expression support:
#define GMTL_BUGGY_INTEGRAL_CONSTANT_EXPRESSIONS
#endif

namespace gmtl
{

// HP aCC cannot deal with missing names for template value parameters
template <bool x> struct STATIC_ASSERTION_FAILURE;

template <> struct STATIC_ASSERTION_FAILURE<true>{};

// HP aCC cannot deal with missing names for template value parameters
template<int x> struct static_assert_test{};

}

//
// Implicit instantiation requires that all member declarations be
// instantiated, but that the definitions are *not* instantiated.
//
// It's not particularly clear how this applies to enum's or typedefs;
// both are described as declarations [7.1.3] and [7.2] in the standard,
// however some compilers use "delayed evaluation" of one or more of
// these when implicitly instantiating templates.  We use typedef declarations
// by default, but try defining BOOST_USE_ENUM_STATIC_ASSERT if the enum
// version gets better results from your compiler...
//
// Implementation:
// Both of these versions rely on sizeof(incomplete_type) generating an error
// message containing the name of the incomplete type.  We use
// "STATIC_ASSERTION_FAILURE" as the type name here to generate
// an eye catching error message.  The result of the sizeof expression is either
// used as an enum initialiser, or as a template argument depending which version
// is in use...
// Note that the argument to the assert is explicitly cast to bool using old-
// style casts: too many compilers currently have problems with static_cast
// when used inside integral constant expressions.
//
#if !defined(GMTL_BUGGY_INTEGRAL_CONSTANT_EXPRESSIONS) && !defined(__MWERKS__)
#ifndef GMTL_MSVC
#define GMTL_STATIC_ASSERT( B ) \
   typedef ::gmtl::static_assert_test<\
      sizeof(::gmtl::STATIC_ASSERTION_FAILURE< (bool)( B ) >)>\
         GMTL_JOIN(gmtl_static_assert_typedef_, __LINE__)
#else
// __LINE__ macro broken when -ZI is used see Q199057
// fortunately MSVC ignores duplicate typedef's.
#define GMTL_STATIC_ASSERT( B ) \
   typedef ::gmtl::static_assert_test<\
      sizeof(::gmtl::STATIC_ASSERTION_FAILURE< (bool)( B ) >)\
      > gmtl_static_assert_typedef_
#endif
#else
// alternative enum based implementation:
#define GMTL_STATIC_ASSERT( B ) \
   enum { GMTL_JOIN(gmtl_static_assert_enum_, __LINE__) \
      = sizeof(::gmtl::STATIC_ASSERTION_FAILURE< (bool)( B ) >) }
#endif


#endif // GMTL_STATIC_ASSERT_HPP
