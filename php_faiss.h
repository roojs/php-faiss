#ifndef PHP_FAISS_H
#define PHP_FAISS_H

extern zend_module_entry faiss_module_entry;
#define phpext_faiss_ptr &faiss_module_entry

#define PHP_FAISS_VERSION "0.1.0"

#ifdef PHP_WIN32
# define PHP_FAISS_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
# define PHP_FAISS_API __attribute__ ((visibility("default")))
#else
# define PHP_FAISS_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#include "faiss_c_wrapper.h"

typedef struct {
	FaissIndex index;
	int dimension;
	zend_object std;
} php_faiss_index_object;

extern zend_class_entry *php_faiss_index_ce;

PHP_MINIT_FUNCTION(faiss);
PHP_MSHUTDOWN_FUNCTION(faiss);
PHP_RINIT_FUNCTION(faiss);
PHP_RSHUTDOWN_FUNCTION(faiss);
PHP_MINFO_FUNCTION(faiss);

#endif
