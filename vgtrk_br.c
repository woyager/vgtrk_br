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
#include "zend_execute.h"
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
    STD_PHP_INI_ENTRY("vgtrk_br.strong_paranoia",      "0", PHP_INI_ALL, OnUpdateBool, strong_paranoia, zend_vgtrk_br_globals, vgtrk_br_globals)
    STD_PHP_INI_ENTRY("vgtrk_br.strong_zend_error",      "0", PHP_INI_ALL, OnUpdateBool, strong_zend_error, zend_vgtrk_br_globals, vgtrk_br_globals)
    STD_PHP_INI_ENTRY("vgtrk_br.strong_php_verror",      "0", PHP_INI_ALL, OnUpdateBool, strong_php_verror, zend_vgtrk_br_globals, vgtrk_br_globals)
    STD_PHP_INI_ENTRY("vgtrk_br.strong_php_error_cb",      "0", PHP_INI_ALL, OnUpdateBool, strong_php_error_cb, zend_vgtrk_br_globals, vgtrk_br_globals)
    STD_PHP_INI_ENTRY("vgtrk_br.strong_zend_exception",      "0", PHP_INI_ALL, OnUpdateBool, strong_zend_exception, zend_vgtrk_br_globals, vgtrk_br_globals)
    STD_PHP_INI_ENTRY("vgtrk_br.debug_enabled","0",PHP_INI_ALL, OnUpdateBool, debug_enabled, zend_vgtrk_br_globals, vgtrk_br_globals)
PHP_INI_END()
/* }}} */

/* {{{ php_vgtrk_br_init_globals
 */
/*
static void php_vgtrk_br_init_globals(zend_vgtrk_br_globals *vgtrk_br_globals)
{
	vgtrk_br_globals->paranoia_enabled = 0;
	vgtrk_br_globals->debug_enabled = 0;
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
	if (VGTRK_BR_G(paranoia_enabled)){
		VGTRK_BR_G(old_error_cb)=zend_error_cb;
		zend_error_cb = vgtrk_error_cb;
	}
	VGTRK_BR_G(sockfd)=socket(AF_INET,SOCK_DGRAM,0);
	VGTRK_BR_G(servaddr).sin_family = AF_INET;
	VGTRK_BR_G(servaddr).sin_addr.s_addr=inet_addr(VGTRK_BR_G(udp_host));
	VGTRK_BR_G(servaddr).sin_port=htons(VGTRK_BR_G(udp_port));
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
	if (VGTRK_BR_G(paranoia_enabled) || VGTRK_BR_G(strong_paranoia)){
		VGTRK_BR_G(web_info)=emalloc(2048);
		*VGTRK_BR_G(web_info)=0;
		if (strncmp(sapi_module.name,"apache",5)==0){
	        	char* hostname = sapi_getenv("HTTP_HOST", 512 TSRMLS_CC);
	                char* uri = sapi_getenv("REQUEST_URI", 512 TSRMLS_CC);
	                char* reqid = sapi_getenv("HTTP_X_REQUEST_ID", 512 TSRMLS_CC);
	                spprintf(&(VGTRK_BR_G(web_info)),2048,"%s    %s    %s",reqid,hostname,uri);
	        }
//		vgtrk_br_fpm_info();
	}
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(vgtrk_br)
{
	if (VGTRK_BR_G(paranoia_enabled) || VGTRK_BR_G(strong_paranoia)){
		efree(VGTRK_BR_G(web_info));
	}
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

void vgtrk_br_fpm_info(){
#ifdef FPM_H
                if (strncmp(sapi_module.name,"fpm",3)==0){
			if (!zend_hash_exists(&EG(symbol_table),"_SERVER",8)){
				zend_auto_global* auto_global;
				if (zend_hash_find(CG(auto_globals),"_SERVER",8,(void**)&auto_global)!=FAILURE){
					auto_global->armed = auto_global->auto_global_callback(auto_global->name,auto_global->name_len TSRMLS_CC);
				}
			}
                        zval **server_pp;
                        zval **value_pp;
                        if (zend_hash_find(&EG(symbol_table),"_SERVER",8,(void**)&server_pp)!=FAILURE){
                                char* hostname = NULL;
                                char* uri = NULL;
                                char* reqid = NULL;
                                if(zend_hash_find(Z_ARRVAL_PP(server_pp),"HTTP_HOST",10,(void**)&value_pp)!=FAILURE){
					hostname = Z_STRVAL_PP(value_pp);
				}
				if(zend_hash_find(Z_ARRVAL_PP(server_pp),"REQUEST_URI",12,(void**)&value_pp)!=FAILURE){
					uri = Z_STRVAL_PP(value_pp);
				}
				if(zend_hash_find(Z_ARRVAL_PP(server_pp),"HTTP_X_REQUEST_ID",18,(void**)&value_pp)!=FAILURE){
					reqid = Z_STRVAL_PP(value_pp);
				}
                                spprintf(&(VGTRK_BR_G(web_info)),2048,"%s    %s    %s",reqid,hostname,uri);
                        }
                        else{
                                spprintf(&(VGTRK_BR_G(web_info)),2048,"%s    %s    %s","1","2","3");
                        }
                }
#endif
	return;
}


void vgtrk_error_cb (int type, const char* filename, const uint error_lineno, const char* format, va_list args)
{
	char * err_buffer=emalloc(4096);
	va_list copy;

	vspprintf(&err_buffer,4096,format,args);

	vgtrk_sender_string("standart",type,filename,error_lineno,err_buffer);
        va_copy(copy, args);
	VGTRK_BR_G(old_error_cb)(type,filename,error_lineno,err_buffer,copy);
        va_end(copy);

//	vgtrk_sender_internal(type, filename,error_lineno, format,args);
//	VGTRK_BR_G(old_error_cb)(type,filename,error_lineno,format,args);
	return;
}

void vgtrk_sender_internal (int type, const char* filename, const uint error_lineno, const char* format, va_list args)
{
	char * err_buffer;
	char * out_buffer;
	int buffer_len;
	const char *space = "";
	TSRMLS_FETCH();
	
	if (VGTRK_BR_G(paranoia_enabled) && (type & (E_ERROR + E_WARNING + E_PARSE + E_CORE_ERROR + E_COMPILE_ERROR + E_CORE_WARNING + E_COMPILE_WARNING))){
		char host[255];
		gethostname(host,255);
		struct timeval tv;
		gettimeofday(&tv,NULL);
		err_buffer = emalloc(4096);
		buffer_len = vspprintf(&err_buffer,4096,format,args);
		out_buffer = emalloc(buffer_len+2048);
		spprintf(&out_buffer,buffer_len+2048,"\1    %s    %d    %d    standart    %s    %s    %d    %s    %s    %s    %s",host,(int)tv.tv_sec,type,sapi_module.name,filename,error_lineno,get_active_class_name(&space TSRMLS_CC),get_active_function_name(TSRMLS_C),err_buffer,VGTRK_BR_G(web_info));
		sendto(VGTRK_BR_G(sockfd),out_buffer,strlen(out_buffer),0,(struct sockaddr *)&VGTRK_BR_G(servaddr),sizeof(VGTRK_BR_G(servaddr)));
		efree(err_buffer);
		efree(out_buffer);
	}

	return;
}

void vgtrk_sender (const char* f_type, int type, const char* filename, const uint error_lineno, const char* format,va_list args)
{
        char * err_buffer;
        char * out_buffer;
        int buffer_len;
	const char *space = "";
        TSRMLS_FETCH();


        if (VGTRK_BR_G(strong_paranoia)  && 
			(type & (E_ERROR + E_WARNING + E_PARSE + E_CORE_ERROR + E_COMPILE_ERROR + E_CORE_WARNING + E_COMPILE_WARNING)) && 
			(
				(strncmp(f_type,"zend_error",10)==0 && VGTRK_BR_G(strong_zend_error)) ||
				(strncmp(f_type,"php_verror",10)==0 && VGTRK_BR_G(strong_php_verror)) ||
				(strncmp(f_type,"php_error_cb",12)==0 && VGTRK_BR_G(strong_php_error_cb))
			)
		){

		vgtrk_br_fpm_info();

                char host[255];
                gethostname(host,255);
                struct timeval tv;
                gettimeofday(&tv,NULL);
                err_buffer = emalloc(4096);
                buffer_len = vspprintf(&err_buffer,4096,format,args);
                out_buffer = emalloc(buffer_len+2048);
                spprintf(&out_buffer,buffer_len+2048,"\1    %s    %d    %d    %s    %s    %s    %d    %s    %s    %s    %s",host,(int)tv.tv_sec,type,f_type,sapi_module.name,filename,error_lineno,get_active_class_name(&space TSRMLS_CC),get_active_function_name(TSRMLS_C),err_buffer,VGTRK_BR_G(web_info));
                sendto(VGTRK_BR_G(sockfd),out_buffer,strlen(out_buffer),0,(struct sockaddr *)&VGTRK_BR_G(servaddr),sizeof(VGTRK_BR_G(servaddr)));
                efree(err_buffer);
                efree(out_buffer);
        }
        return;
}

void vgtrk_sender_string(const char* f_type, int type, const char* error_filename, const uint error_lineno, const char * message){
        char * err_buffer;
        char * out_buffer;
        int buffer_len;
	const char *space = "";
        TSRMLS_FETCH();

        if ((VGTRK_BR_G(strong_paranoia) || (VGTRK_BR_G(paranoia_enabled))) &&
                        (
                                (strncmp(f_type,"zend_exception",13)==0 && VGTRK_BR_G(strong_zend_exception)) ||
				(strncmp(f_type,"php_verror",10)==0 && VGTRK_BR_G(strong_php_verror)) ||
				(strncmp(f_type,"zend_error",10)==0 && VGTRK_BR_G(strong_zend_error) && !(strlen(message)==2 && strncmp(message,"%s",2)==0)) ||
				(strncmp(f_type,"standart",8)==0)
                        )
                ){

		vgtrk_br_fpm_info();

                char host[255];
                gethostname(host,255);
                struct timeval tv;
                gettimeofday(&tv,NULL);
                err_buffer = emalloc(4096);
                buffer_len = spprintf(&err_buffer,4096,"%s",message);
                out_buffer = emalloc(buffer_len+2048);
                spprintf(&out_buffer,buffer_len+2048,"\1    %s    %d    %d    %s    %s    %s    %d    %s    %s    %s    %s",host,(int)tv.tv_sec,type,f_type,sapi_module.name,error_filename,error_lineno,get_active_class_name(&space TSRMLS_CC),get_active_function_name(TSRMLS_C),err_buffer,VGTRK_BR_G(web_info));
                sendto(VGTRK_BR_G(sockfd),out_buffer,strlen(out_buffer),0,(struct sockaddr *)&VGTRK_BR_G(servaddr),sizeof(VGTRK_BR_G(servaddr)));
                efree(err_buffer);
                efree(out_buffer);
        }
        return;
}
