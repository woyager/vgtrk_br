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

#include <sys/socket.h>
#include <netinet/in.h>

#ifndef PHP_VGTRK_BR_H
#define PHP_VGTRK_BR_H

extern zend_module_entry vgtrk_br_module_entry;
#define phpext_vgtrk_br_ptr &vgtrk_br_module_entry

#ifdef PHP_WIN32
#	define PHP_VGTRK_BR_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_VGTRK_BR_API __attribute__ ((visibility("default")))
#else
#	define PHP_VGTRK_BR_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(vgtrk_br);
PHP_MSHUTDOWN_FUNCTION(vgtrk_br);
PHP_RINIT_FUNCTION(vgtrk_br);
PHP_RSHUTDOWN_FUNCTION(vgtrk_br);
PHP_MINFO_FUNCTION(vgtrk_br);

PHP_FUNCTION(confirm_vgtrk_br_compiled);	/* For testing, remove later. */

ZEND_BEGIN_MODULE_GLOBALS(vgtrk_br)
	ZEND_API void (*old_error_cb) (int type, const char *error_filename, const uint error_lineno, const char * format, va_list args);
	zend_bool paranoia_enabled;
	zend_bool strong_paranoia;
	zend_bool strong_zend_error;
	zend_bool strong_php_verror;
	zend_bool strong_php_error_cb;
	zend_bool strong_zend_exception;
	zend_bool debug_enabled;
	signed long udp_port;
	char * udp_host;
	int sockfd;
	struct sockaddr_in servaddr;
	char * web_info;
ZEND_END_MODULE_GLOBALS(vgtrk_br)

/* In every utility function you add that needs to use variables 
   in php_vgtrk_br_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as VGTRK_BR_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define VGTRK_BR_G(v) TSRMG(vgtrk_br_globals_id, zend_vgtrk_br_globals *, v)
#else
#define VGTRK_BR_G(v) (vgtrk_br_globals.v)
#endif

#endif	/* PHP_VGTRK_BR_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

void vgtrk_error_cb(int type, const char *error_filename, const uint error_lineno, const char * format, va_list args);
void vgtrk_sender(const char* f_type, int type, const char *error_filename, const uint error_lineno, const char * format, va_list args);
void vgtrk_sender_string(const char* f_type, int type, const char* error_filename, const uint error_lineno, const char * message);
void vgtrk_sender_internal(int type, const char *error_filename, const uint error_lineno, const char * format, va_list args);
void vgtrk_br_fpm_info();
