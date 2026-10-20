/*
   +----------------------------------------------------------------------+
   | Copyright (c) 2026 Roo J Solutions Ltd.                              |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Leon <leon@roojs.com>                                        |
   +----------------------------------------------------------------------+
*/

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
PHP_MINFO_FUNCTION(faiss);

#endif
