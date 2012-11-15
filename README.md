vgtrk.paranoia_enabled [false]/true

vgtrk.udp_host [127.0.0.1]

vgtrk.udp_port [11111]

vgtrk_br.strong_paranoia [false]/true

vgtrk_br.strong_zend_error [false]/true

vgtrk_br.strong_php_verror [false]/true

vgtrk_br.strong_php_error_cb [false]/true

vgtrk_br.strong_zend_exception [false]/true

patches

to all files

#include "../ext/vgtrk_br/php_vgtrk_br.h"

Zend/zend_exceptions.c

ZEND_METHOD(exception, __construct)

        vgtrk_sender_string ("zend_exception", code, NULL, -1, message);

Zend/zend.c

ZEND_API void zend_error(int type, const char *format, ...)

        va_start(args, format);

        vgtrk_sender ("zend_error", type, error_filename, error_lineno, format, args);

        /* if we don't have a user defined error handler */

main/main.c

PHPAPI void php_verror(const char *docref, const char *params, int type, const char *format, va_list args TSRMLS_DC)

        int is_function = 0;

        vgtrk_sender("php_verror",type,NULL,-1,format,args);

        /* get error text into buffer and escape for html if necessary */

static void php_error_cb(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args)

        TSRMLS_FETCH();

        vgtrk_sender("php_error_cb",type,error_filename,error_lineno,format,args);

        buffer_len = vspprintf(&buffer, PG(log_errors_max_len), format, args);

