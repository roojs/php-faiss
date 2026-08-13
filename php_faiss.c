#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "zend_exceptions.h"
#include "php_faiss.h"
#include "ext/standard/info.h"

zend_class_entry *php_faiss_index_ce;

static zend_object_handlers php_faiss_index_object_handlers;

static void php_faiss_index_free_obj(zend_object *object)
{
	php_faiss_index_object *intern = (php_faiss_index_object *)((char *)(object)
		- XtOffsetOf(php_faiss_index_object, std));

	if (intern->index) {
		faiss_Index_free(intern->index);
		intern->index = NULL;
	}
	zend_object_std_dtor(object);
}

static zend_object *php_faiss_index_create_object(zend_class_entry *class_type)
{
	php_faiss_index_object *intern = zend_object_alloc(sizeof(php_faiss_index_object), class_type);

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->index = NULL;
	intern->dimension = 0;
	intern->std.handlers = &php_faiss_index_object_handlers;

	return &intern->std;
}

static int php_faiss_array_to_floats(zval *arr, float **out, size_t *out_len)
{
	HashTable *ht = Z_ARRVAL_P(arr);
	zval *val;
	size_t i = 0;

	*out_len = zend_hash_num_elements(ht);
	*out = ecalloc(*out_len, sizeof(float));

	ZEND_HASH_FOREACH_VAL(ht, val) {
		if (Z_TYPE_P(val) != IS_DOUBLE && Z_TYPE_P(val) != IS_LONG) {
			efree(*out);
			*out = NULL;
			return FAILURE;
		}
		(*out)[i++] = (float)zval_get_double(val);
	} ZEND_HASH_FOREACH_END();

	return SUCCESS;
}

static php_faiss_index_object *php_faiss_index_from_zend(zend_object *obj)
{
	php_faiss_index_object *intern = (php_faiss_index_object *)((char *)(obj)
		- XtOffsetOf(php_faiss_index_object, std));

	if (!intern->index) {
		zend_throw_exception_ex(zend_ce_exception, 0, "FaissIndex is not initialized");
		return NULL;
	}

	return intern;
}

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_faiss_create, 0, 1, FaissIndex, 0)
	ZEND_ARG_TYPE_INFO(0, dimension, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_faiss_load, 0, 1, FaissIndex, 0)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_faiss_add, 0, 1, IS_VOID, 0)
	ZEND_ARG_ARRAY_INFO(0, vector, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_faiss_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_faiss_dimension, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_faiss_save, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_faiss_search_with_ids, 0, 3, IS_ARRAY, 0)
	ZEND_ARG_ARRAY_INFO(0, query, 0)
	ZEND_ARG_TYPE_INFO(0, k, IS_LONG, 0)
	ZEND_ARG_ARRAY_INFO(0, ids, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(FaissIndex, create)
{
	zend_long dim;
	FaissIndexHNSW hnsw = NULL;
	php_faiss_index_object *intern;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(dim)
	ZEND_PARSE_PARAMETERS_END();

	if (dim <= 0) {
		zend_throw_exception_ex(zend_ce_exception, 0, "dimension must be positive");
		RETURN_THROWS();
	}

	if (faiss_IndexHNSWFlat_new(&hnsw, dim, 16) != 0 || !hnsw) {
		zend_throw_exception_ex(zend_ce_exception, 0, "faiss_IndexHNSWFlat_new failed");
		RETURN_THROWS();
	}

	object_init_ex(return_value, php_faiss_index_ce);
	intern = (php_faiss_index_object *)((char *)(Z_OBJ_P(return_value))
		- XtOffsetOf(php_faiss_index_object, std));
	intern->index = (FaissIndex)hnsw;
	intern->dimension = dim;
}

PHP_METHOD(FaissIndex, load)
{
	char *path;
	size_t path_len;
	FaissIndex index = NULL;
	int d;
	php_faiss_index_object *intern;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(path, path_len)
	ZEND_PARSE_PARAMETERS_END();

	if (faiss_read_index_fname(path, 0, &index) != 0 || !index) {
		zend_throw_exception_ex(zend_ce_exception, 0, "Cannot load FAISS index: %s", path);
		RETURN_THROWS();
	}

	d = faiss_Index_d(index);
	if (d <= 0) {
		faiss_Index_free(index);
		zend_throw_exception_ex(zend_ce_exception, 0, "Invalid index dimension");
		RETURN_THROWS();
	}

	object_init_ex(return_value, php_faiss_index_ce);
	intern = (php_faiss_index_object *)((char *)(Z_OBJ_P(return_value))
		- XtOffsetOf(php_faiss_index_object, std));
	intern->index = index;
	intern->dimension = d;
}

PHP_METHOD(FaissIndex, add)
{
	zval *vector_arr;
	float *data = NULL;
	size_t len = 0;
	php_faiss_index_object *intern;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(vector_arr)
	ZEND_PARSE_PARAMETERS_END();

	intern = php_faiss_index_from_zend(Z_OBJ_P(ZEND_THIS));
	if (!intern) {
		RETURN_THROWS();
	}

	if (php_faiss_array_to_floats(vector_arr, &data, &len) == FAILURE || !data) {
		zend_throw_exception_ex(zend_ce_exception, 0, "vector must be an array of numbers");
		RETURN_THROWS();
	}

	if (len != intern->dimension) {
		efree(data);
		zend_throw_exception_ex(zend_ce_exception, 0, "vector length %zu does not match index dimension %d",
			len, intern->dimension);
		RETURN_THROWS();
	}

	if (faiss_Index_add(intern->index, 1, data) != 0) {
		efree(data);
		zend_throw_exception_ex(zend_ce_exception, 0, "faiss_Index_add failed");
		RETURN_THROWS();
	}

	efree(data);
}

PHP_METHOD(FaissIndex, count)
{
	php_faiss_index_object *intern;

	ZEND_PARSE_PARAMETERS_NONE();

	intern = php_faiss_index_from_zend(Z_OBJ_P(ZEND_THIS));
	if (!intern) {
		RETURN_THROWS();
	}

	RETURN_LONG(faiss_Index_ntotal(intern->index));
}

PHP_METHOD(FaissIndex, dimension)
{
	php_faiss_index_object *intern;

	ZEND_PARSE_PARAMETERS_NONE();

	intern = php_faiss_index_from_zend(Z_OBJ_P(ZEND_THIS));
	if (!intern) {
		RETURN_THROWS();
	}

	RETURN_LONG(intern->dimension);
}

PHP_METHOD(FaissIndex, save)
{
	char *path;
	size_t path_len;
	php_faiss_index_object *intern;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(path, path_len)
	ZEND_PARSE_PARAMETERS_END();

	intern = php_faiss_index_from_zend(Z_OBJ_P(ZEND_THIS));
	if (!intern) {
		RETURN_THROWS();
	}

	if (faiss_write_index_fname(intern->index, path) != 0) {
		zend_throw_exception_ex(zend_ce_exception, 0, "Cannot save FAISS index: %s", path);
		RETURN_THROWS();
	}
}

PHP_METHOD(FaissIndex, searchWithIds)
{
	zval *query_arr;
	zval *ids_arr;
	zend_long k;
	float *data = NULL;
	size_t len = 0;
	float *distances = NULL;
	int64_t *labels = NULL;
	int64_t *ids = NULL;
	int64_t n_ids = 0;
	zend_long i;
	HashTable *ht;
	zval *val;
	FaissIDSelector sel = NULL;
	php_faiss_index_object *intern;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_ARRAY(query_arr)
		Z_PARAM_LONG(k)
		Z_PARAM_ARRAY(ids_arr)
	ZEND_PARSE_PARAMETERS_END();

	if (k <= 0) {
		zend_throw_exception_ex(zend_ce_exception, 0, "k must be positive");
		RETURN_THROWS();
	}

	intern = php_faiss_index_from_zend(Z_OBJ_P(ZEND_THIS));
	if (!intern) {
		RETURN_THROWS();
	}

	ht = Z_ARRVAL_P(ids_arr);
	n_ids = zend_hash_num_elements(ht);
	if (n_ids <= 0) {
		array_init(return_value);
		return;
	}

	ids = ecalloc(n_ids, sizeof(int64_t));
	i = 0;
	ZEND_HASH_FOREACH_VAL(ht, val) {
		if (Z_TYPE_P(val) != IS_LONG && Z_TYPE_P(val) != IS_DOUBLE) {
			efree(ids);
			zend_throw_exception_ex(zend_ce_exception, 0, "ids must be an array of integers");
			RETURN_THROWS();
		}
		ids[i++] = zval_get_long(val);
	} ZEND_HASH_FOREACH_END();

	if (php_faiss_array_to_floats(query_arr, &data, &len) == FAILURE || !data) {
		efree(ids);
		zend_throw_exception_ex(zend_ce_exception, 0, "query must be an array of numbers");
		RETURN_THROWS();
	}

	if (len != intern->dimension) {
		efree(ids);
		efree(data);
		zend_throw_exception_ex(zend_ce_exception, 0, "query length %zu does not match index dimension %d",
			len, intern->dimension);
		RETURN_THROWS();
	}

	if (faiss_IDSelectorBatch_new((FaissIDSelector**)&sel, n_ids, ids) != 0 || !sel) {
		efree(ids);
		efree(data);
		zend_throw_exception_ex(zend_ce_exception, 0, "faiss_IDSelectorBatch_new failed");
		RETURN_THROWS();
	}

	distances = ecalloc(k, sizeof(float));
	labels = ecalloc(k, sizeof(int64_t));

	if (faiss_Index_search_with_ids(intern->index, 1, data, k, sel, distances, labels) != 0) {
		faiss_IDSelector_free(sel);
		efree(ids);
		efree(data);
		efree(distances);
		efree(labels);
		zend_throw_exception_ex(zend_ce_exception, 0, "faiss_Index_search_with_ids failed");
		RETURN_THROWS();
	}

	faiss_IDSelector_free(sel);
	efree(ids);
	efree(data);

	array_init(return_value);
	for (i = 0; i < k; i++) {
		if (labels[i] < 0) {
			continue;
		}
		zval item;
		array_init(&item);
		add_assoc_long(&item, "id", labels[i]);
		add_assoc_double(&item, "distance", distances[i]);
		add_next_index_zval(return_value, &item);
	}

	efree(distances);
	efree(labels);
}

static const zend_function_entry php_faiss_index_methods[] = {
	PHP_ME(FaissIndex, create, arginfo_faiss_create, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(FaissIndex, load, arginfo_faiss_load, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	PHP_ME(FaissIndex, add, arginfo_faiss_add, ZEND_ACC_PUBLIC)
	PHP_ME(FaissIndex, count, arginfo_faiss_count, ZEND_ACC_PUBLIC)
	PHP_ME(FaissIndex, dimension, arginfo_faiss_dimension, ZEND_ACC_PUBLIC)
	PHP_ME(FaissIndex, save, arginfo_faiss_save, ZEND_ACC_PUBLIC)
	PHP_ME(FaissIndex, searchWithIds, arginfo_faiss_search_with_ids, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

static const zend_function_entry faiss_functions[] = {
	PHP_FE_END
};

PHP_MINIT_FUNCTION(faiss)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "FaissIndex", php_faiss_index_methods);
	php_faiss_index_ce = zend_register_internal_class_ex(&ce, NULL);
	php_faiss_index_ce->create_object = php_faiss_index_create_object;

	memcpy(&php_faiss_index_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	php_faiss_index_object_handlers.offset = XtOffsetOf(php_faiss_index_object, std);
	php_faiss_index_object_handlers.free_obj = php_faiss_index_free_obj;
	php_faiss_index_object_handlers.clone_obj = NULL;

	return SUCCESS;
}

PHP_MINFO_FUNCTION(faiss)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "faiss support", "enabled");
	php_info_print_table_row(2, "Version", PHP_FAISS_VERSION);
	php_info_print_table_end();
}

zend_module_entry faiss_module_entry = {
	STANDARD_MODULE_HEADER,
	"faiss",
	faiss_functions,
	PHP_MINIT(faiss),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(faiss),
	PHP_FAISS_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_FAISS
ZEND_GET_MODULE(faiss)
#endif
