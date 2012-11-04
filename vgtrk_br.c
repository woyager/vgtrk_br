/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "zend.h"
#include "ext/standard/info.h"
#include "php_vgtrk_br.h"
#include <sys/time.h>
#include "SAPI.h"

ZEND_DECLARE_MODULE_GLOBALS(vgtrk_br)

/* True global resources - no need for thread safety here */
static int le_vgtrk_br;

/* {{{ vgtrk_br_functions[]
 *
 * Every user visible function must have an entry in vgtrk_br_functions[].
 */
const zend_function_entry vgtrk_br_functions[] = {
	PHP_FE(confirm_vgtrk_br_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE_END	/* Must be the last line in vgtrk_br_functions[] */
};
/* }}} */

/* {{{ vgtrk_br_module_entry
 */
zend_module_entry vgtrk_br_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"vgtrk_br",
	vgtrk_br_functions,
	PHP_MINIT(vgtrk_br),
	PHP_MSHUTDOWN(vgtrk_br),
	PHP_RINIT(vgtrk_br),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(vgtrk_br),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(vgtrk_br),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_VGTRK_BR
ZEND_GET_MODULE(vgtrk_br)
#endif

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("vgtrk_br.paranoia_enabled",      "0", PHP_INI_ALL, OnUpdateBool, paranoia_enabled, zend_vgtrk_br_globals, vgtrk_br_globals)
    STD_PHP_INI_ENTRY("vgtrk_br.udp_host", "127.0.0.1", PHP_INI_ALL, OnUpdateString, udp_host, zend_vgtrk_br_globals, vgtrk_br_globals)
    STD_PHP_INI_ENTRY("vgtrk_br.udp_port","11111",PHP_INI_ALL,OnUpdateLong, udp_port, zend_vgtrk_br_globals, vgtrk_br_globals)
PHP_INI_END()
/* }}} */

/* {{{ php_vgtrk_br_init_globals
 */
/*
static void php_vgtrk_br_init_globals(zend_vgtrk_br_globals *vgtrk_br_globals)
{
	vgtrk_br_globals->paranoia_enabled = 0;
	vgtrk_br_globals->udp_host = "127.0.0.1";
	vgtrk_br_globals->udp_port = 11111;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(vgtrk_br)
{
	REGISTER_INI_ENTRIES();
	VGTRK_BR_G(old_error_cb)=zend_error_cb;
	zend_error_cb = vgtrk_error_cb;
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(vgtrk_br)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(vgtrk_br)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(vgtrk_br)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(vgtrk_br)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "vgtrk_br support", "enabled");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_vgtrk_br_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_vgtrk_br_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "vgtrk_br", arg);
	RETURN_STRINGL(strg, len, 0);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

void vgtrk_error_cb (int type, const char* filename, const uint error_lineno, const char* format, va_list args)
{
	char * err_buffer;
	char * out_buffer;
	int buffer_len;
	TSRMLS_FETCH();

	if (VGTRK_BR_G(paranoia_enabled) && (type == E_WARNING || type == E_USER_WARNING || type == E_USER_ERROR || type == E_USER_ERROR)){
		char host[255];
		char web_info[512]="";
		if (strncmp(sapi_module.name,"apache",5)==0){
			char* hostname = sapi_getenv("HTTP_HOST", 512 TSRMLS_CC);
			char* uri = sapi_getenv("REQUEST_URI", 512 TSRMLS_CC);
			char* reqid = sapi_getenv("HTTP_X_REQUEST_ID", 512 TSRMLS_CC);
			spprintf(&web_info,512,"%s    %s    %s",reqid,hostname,uri);
		}
		gethostname(host,255);
		struct timeval tv;
		gettimeofday(&tv,NULL);
		err_buffer = emalloc(PG(log_errors_max_len));
		buffer_len = vspprintf(&err_buffer,PG(log_errors_max_len),format,args);
		out_buffer = emalloc(buffer_len+1024);
		spprintf(&out_buffer,buffer_len+1024,"%s    %d    %d    %s    %d    %s    %s",host,tv.tv_sec,type,filename,error_lineno,err_buffer,web_info);
		printf("%s",out_buffer);
		efree(err_buffer);
		efree(out_buffer);
	}
	VGTRK_BR_G(old_error_cb)(type,filename,error_lineno,format,args);
	return;
}
