# LIBCURL_CHECK_CONFIG ([DEFAULT-ACTION], [MINIMUM-VERSION],
#                       [ACTION-IF-YES], [ACTION-IF-NO])
# ----------------------------------------------------------
#      David Shaw <dshaw@jabberwocky.com>   Jun-21-2005
#
# Checks for libcurl.  DEFAULT-ACTION is the string yes or no to
# specify whether to default to --with-libcurl or --without-libcurl.
# If not supplied, DEFAULT-ACTION is yes.  MINIMUM-VERSION is the
# minimum version of libcurl to accept.  Pass the version as a regular
# version number like 7.10.1. If not supplied, any version is
# accepted.  ACTION-IF-YES is a list of shell commands to run if
# libcurl was successfully found and passed the various tests.
# ACTION-IF-NO is a list of shell commands that are run otherwise.
# Note that using --without-libcurl does run ACTION-IF-NO.
#
# This macro defines HAVE_LIBCURL if a working libcurl setup is found,
# and sets @LIBCURL@ and @LIBCURL_CPPFLAGS@ to the necessary values.
# Other useful defines are LIBCURL_FEATURE_xxx where xxx are the
# various features supported by libcurl, and LIBCURL_PROTOCOL_yyy
# where yyy are the various protocols supported by libcurl.  Both xxx
# and yyy are capitalized.  See the list of AH_TEMPLATEs at the top of
# the macro for the complete list of possible defines.  Shell
# variables $libcurl_feature_xxx and $libcurl_protocol_yyy are also
# defined to 'yes' for those features and protocols that were found.
# Note that xxx and yyy keep the same capitalization as in the
# curl-config list (e.g. it's "HTTP" and not "http").
#
# Users may override the detected values by doing something like:
# LIBCURL="-lcurl" LIBCURL_CPPFLAGS="-I/usr/myinclude" ./configure
#
# For the sake of sanity, this macro assumes that any libcurl that is
# found is after version 7.7.2, the first version that included the
# curl-config script.  Note that it is very important for people
# packaging binary versions of libcurl to include this script!
# Without curl-config, we can only guess what protocols are available.

AC_DEFUN([CURLPP_CHECK_CONFIG],
[
  LIBCURL_CHECK_CONFIG

  if test x"$LIBCURL" != "x" ; then

  AC_ARG_WITH(curlpp,
     AC_HELP_STRING([--with-curlpp=DIR],[look for the curlpp library in DIR]),
     [_curlpp_with=$withval],[_curlpp_with=ifelse([$1],,[yes],[$1])])

  if test "$_curlpp_with" != "no" ; then

     AC_PROG_AWK

     _curlpp_version_parse="eval $AWK '{split(\$NF,A,\".\"); X=256*256*A[[1]]+256*A[[2]]+A[[3]]; print X;}'"

     _curlpp_try_link=yes

     if test -d "$_curlpp_with" ; then
        CPPFLAGS="${CPPFLAGS} -I$withval/include"
        LDFLAGS="${LDFLAGS} -L$withval/lib"
     fi

     AC_PATH_PROG([_curlpp_config],[curlpp-config])

     if test x$_curlpp_config != "x" ; then
        AC_CACHE_CHECK([for the version of curlpp],
	   [curlpp_cv_lib_curlpp_version],
           [curlpp_cv_lib_curlpp_version=`$_curlpp_config --version | $AWK '{print $[]2}'`])

	_curlpp_version=`echo $curlpp_cv_lib_curlpp_version | $_curlpp_version_parse`
	_curlpp_wanted=`echo ifelse([$2],,[0],[$2]) | $_curlpp_version_parse`

        if test $_curlpp_wanted -gt 0 ; then
	   AC_CACHE_CHECK([for curlpp >= version $2],
	      [curlpp_cv_lib_version_ok],
              [
   	      if test $_curlpp_version -ge $_curlpp_wanted ; then
	         curlpp_cv_lib_version_ok=yes
      	      else
	         curlpp_cv_lib_version_ok=no
  	      fi
              ])
        fi

	if test $_curlpp_wanted -eq 0 || test x$curlpp_cv_lib_version_ok = xyes ; then
           if test x"$CURLPP_CPPFLAGS" = "x" ; then
              CURLPP_CPPFLAGS=`$_curlpp_config --cflags`
           fi
           if test x"$CURLPP" = "x" ; then
              CURLPP=`$_curlpp_config --libs`
           fi
	else
           _curlpp_try_link=no
	fi

	unset _curlpp_wanted
     fi

     if test $_curlpp_try_link = yes ; then

        # we didn't find curl-config, so let's see if the user-supplied
        # link line (or failing that, "-lcurl") is enough.
        CURLPP=${CURLPP-"-lcurl"}

        AC_CACHE_CHECK([whether curlpp is usable],
           [curlpp_cv_lib_curlpp_usable],
           [
           _curlpp_save_cppflags=$CPPFLAGS
           CPPFLAGS="$CPPFLAGS $CURLPP_CPPFLAGS"
           _libcurl_save_libs=$LIBS
           LIBS="$LIBS $CURLPP"

           AC_LINK_IFELSE(AC_LANG_PROGRAM([#include <curlpp/cURLpp.hpp>],[
/* Try and use a few common options to force a failure if we are
   missing symbols or can't link. */
int x;	
cURLpp::initialize();
]),curlpp_cv_lib_curlpp_usable=yes,curlpp_cv_lib_curlpp_usable=no)

           CPPFLAGS=$_curlpp_save_cppflags
           LIBS=$_curlpp_save_libs
           unset _curlpp_save_cppflags
           unset _curlpp_save_libs
           ])

        if test $curlpp_cv_lib_curlpp_usable = yes ; then

           AC_DEFINE(HAVE_CURLPP,1,
             [Define to 1 if you have a functional curlpp library.])
           AC_SUBST(CURLPP_CPPFLAGS)
           AC_SUBST(CURLPP)
        fi
     fi

     unset _curlpp_try_link
     unset _curlpp_version_parse
     unset _curlpp_config
     unset _curlpp_feature
     unset _curlpp_features
     unset _curlpp_protocol
     unset _curlpp_protocols
     unset _curlpp_version
  fi

  if test x$_curlpp_with = xno || test x$libcurl_cv_lib_curl_usable != xyes ; then
     # This is the IF-NO path
     ifelse([$4],,:,[$4])
  else
     # This is the IF-YES path
     ifelse([$3],,:,[$3])
  fi

  unset _curlpp_with
  fi
])dnl
