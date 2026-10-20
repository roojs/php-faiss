ext_shared=yes

PHP_REQUIRE_CXX()
PKG_CHECK_MODULES([GLIB], [glib-2.0], [], [
  AC_MSG_ERROR([glib-2.0 not found — install libglib2.0-dev])
])
PHP_EVAL_LIBLINE([$GLIB_LIBS], [FAISS_SHARED_LIBADD])
PHP_EVAL_INCLINE([$GLIB_CFLAGS])

PHP_NEW_EXTENSION(faiss, php_faiss.c faiss_c_wrapper.cpp, yes)
dnl Order: static libfaiss then its deps (as-needed drops unused early libs)
PHP_ADD_LIBRARY(faiss, 1, FAISS_SHARED_LIBADD)
PHP_ADD_LIBRARY(lapack, 1, FAISS_SHARED_LIBADD)
PHP_ADD_LIBRARY(blas, 1, FAISS_SHARED_LIBADD)
PHP_ADD_LIBRARY(gomp, 1, FAISS_SHARED_LIBADD)
PHP_ADD_LIBRARY(stdc++, 1, FAISS_SHARED_LIBADD)
PHP_SUBST(FAISS_SHARED_LIBADD)
