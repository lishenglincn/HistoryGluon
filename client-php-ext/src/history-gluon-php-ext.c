#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "php.h"
#include "history-gluon.h"
#include "history-gluon-php-ext.h"

static zend_function_entry php_history_gluon_functions[] = {
    PHP_FE(history_gluon_create_context, NULL)
    PHP_FE(history_gluon_free_context, NULL)
    PHP_FE(history_gluon_add_uint, NULL)
    PHP_FE(history_gluon_delete, NULL)
    {NULL, NULL, NULL}
};

zend_module_entry php_history_gluon_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_HISTORY_GLUON_NAME,
    php_history_gluon_functions,
    PHP_MINIT(history_gluon),
    PHP_MSHUTDOWN(history_gluon),
    PHP_RINIT(history_gluon),
    NULL,
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_HISTORY_GLUON_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_HISTORY_GLUON
ZEND_GET_MODULE(php_history_gluon)
#endif

/* -------------------------------------------------------------------------------------
 * Static member and functions
 * ---------------------------------------------------------------------------------- */
static history_gluon_context_t *g_ctx = NULL;;

static history_gluon_result_t validateContext(history_gluon_context_t ctx)
{
	if (!g_ctx || ctx != g_ctx)
		return HGLERR_INVALID_CONTEXT;
	return HGL_SUCCESS;
}

/* -------------------------------------------------------------------------------------
 * Exported functions
 * ---------------------------------------------------------------------------------- */
PHP_MINIT_FUNCTION(history_gluon)
{
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(history_gluon)
{
	return SUCCESS;
}

PHP_RINIT_FUNCTION(history_gluon)
{
	return SUCCESS;
}

PHP_FUNCTION(history_gluon_create_context)
{
	if (!g_ctx)
		g_ctx = history_gluon_create_context();
	RETURN_LONG((long)g_ctx);
}

PHP_FUNCTION(history_gluon_free_context)
{
	if (!g_ctx) {
		history_gluon_free_context(g_ctx);
		g_ctx = NULL;
	}
}

PHP_FUNCTION(history_gluon_add_uint)
{
	// get arguments
	long l_ctx, l_id, l_sec, l_ns, l_data; 
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lllll",
	                          &l_ctx, &l_id, &l_sec, &l_ns, &l_data) == FAILURE)
		RETURN_NULL();
	
	history_gluon_context_t ctx = (history_gluon_context_t *)l_ctx;
	uint64_t id = l_id;
	struct timespec ts = {l_sec, l_ns};
	uint64_t data = l_data;

	int ret = validateContext(ctx);
	if (ret != HGL_SUCCESS)
		RETURN_LONG(ret);

	// call the library function and return the return.
	RETURN_LONG(history_gluon_add_uint(ctx, id, &ts, data));
}

PHP_FUNCTION(history_gluon_delete)
{
	// get arguments
	long l_ctx, l_id, l_sec, l_ns, l_del_way, z_num_deleted;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lllllz",
	                          &l_ctx, &l_id, &l_sec, &l_ns, &l_del_way,
	                          &z_num_deleted) == FAILURE)
		RETURN_NULL();
	
	history_gluon_context_t ctx = (history_gluon_context_t *)l_ctx;
	uint64_t id = l_id;
	struct timespec ts = {l_sec, l_ns};
	history_gluon_delete_way_t delete_way = l_del_way;
	uint64_t num_deleted;

	history_gluon_result_t ret;
	ret = validateContext(ctx);
	if (ret != HGL_SUCCESS)
		RETURN_LONG(ret);

	// call the library function and return the return.
	ret = history_gluon_delete(ctx, id, &ts, delete_way, &num_deleted);
	ZVAL_LONG(&z_num_deleted, num_deleted);
	RETURN_LONG((long)ret);
}
