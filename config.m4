dnl $Id$
dnl config.m4 for extension vgtrk_br

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(vgtrk_br, for vgtrk_br support,
dnl Make sure that the comment is aligned:
dnl [  --with-vgtrk_br             Include vgtrk_br support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(vgtrk_br, whether to enable vgtrk_br support,
dnl Make sure that the comment is aligned:
dnl [  --enable-vgtrk_br           Enable vgtrk_br support])

if test "$PHP_VGTRK_BR" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-vgtrk_br -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/vgtrk_br.h"  # you most likely want to change this
  dnl if test -r $PHP_VGTRK_BR/$SEARCH_FOR; then # path given as parameter
  dnl   VGTRK_BR_DIR=$PHP_VGTRK_BR
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for vgtrk_br files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       VGTRK_BR_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$VGTRK_BR_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the vgtrk_br distribution])
  dnl fi

  dnl # --with-vgtrk_br -> add include path
  dnl PHP_ADD_INCLUDE($VGTRK_BR_DIR/include)

  dnl # --with-vgtrk_br -> check for lib and symbol presence
  dnl LIBNAME=vgtrk_br # you may want to change this
  dnl LIBSYMBOL=vgtrk_br # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $VGTRK_BR_DIR/lib, VGTRK_BR_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_VGTRK_BRLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong vgtrk_br lib version or lib not found])
  dnl ],[
  dnl   -L$VGTRK_BR_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(VGTRK_BR_SHARED_LIBADD)

  PHP_NEW_EXTENSION(vgtrk_br, vgtrk_br.c, $ext_shared)
fi
