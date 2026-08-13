#ifndef PHP_FAISS_H
#define PHP_FAISS_H

extern zend_module_entry faiss_module_entry;
#define phpext_faiss_ptr &faiss_module_entry

#define PHP_FAISS_VERSION "0.1.0"

#include "faiss_c_wrapper.h"

typedef struct {
	FaissIndex index;
	int dimension;
	zend_object std;
} php_faiss_index_object;

extern zend_class_entry *php_faiss_index_ce;

PHP_MINIT_FUNCTION(faiss);
PHP_MSHUTDOWN_FUNCTION(faiss);
PHP_MINFO_FUNCTION(faiss);

#endif
