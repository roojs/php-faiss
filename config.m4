PHP_ARG_ENABLE(faiss, whether to enable faiss support,
[  --enable-faiss           Enable FAISS vector index support])

if test "$PHP_FAISS" != "no"; then
  PHP_REQUIRE_CXX()
  PKG_CHECK_MODULES([GLIB], [glib-2.0], [], [
    AC_MSG_ERROR([glib-2.0 not found — install libglib2.0-dev])
  ])
  PHP_EVAL_LIBLINE([$GLIB_LIBS], [FAISS_SHARED_LIBADD])
  PHP_EVAL_INCLINE([$GLIB_CFLAGS])
  dnl faiss_Index_add lives in faiss_c_wrapper.cpp, not libfaiss — link-test C++ API instead
  AC_MSG_CHECKING([for faiss C++ library])
  AC_LANG_PUSH([C++])
  save_LIBS="$LIBS"
  LIBS="$LIBS -lfaiss -lblas -llapack -lgomp -lpthread"
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <faiss/IndexHNSW.h>
]], [[
  faiss::IndexHNSWFlat index(4, 16);
  return index.d == 4 ? 0 : 1;
]])], [
    AC_MSG_RESULT([yes])
  ], [
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([libfaiss not found — install libfaiss-dev])
  ])
  LIBS="$save_LIBS"
  AC_LANG_POP([C++])
  PHP_NEW_EXTENSION(faiss, php_faiss.c faiss_c_wrapper.cpp, $ext_shared)
  dnl Order: static libfaiss then its deps (as-needed drops unused early libs)
  PHP_ADD_LIBRARY(faiss, 1, FAISS_SHARED_LIBADD)
  PHP_ADD_LIBRARY(lapack, 1, FAISS_SHARED_LIBADD)
  PHP_ADD_LIBRARY(blas, 1, FAISS_SHARED_LIBADD)
  PHP_ADD_LIBRARY(gomp, 1, FAISS_SHARED_LIBADD)
  PHP_ADD_LIBRARY(stdc++, 1, FAISS_SHARED_LIBADD)
  PHP_SUBST(FAISS_SHARED_LIBADD)
  PHP_SUBST(FAISS_SHARED_LDFLAGS)
fi
