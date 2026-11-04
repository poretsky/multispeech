# Configure extra path
#
# SYNOPSIS
#
# EXTRA_PATH_OPTION([NAME], [DESCRIPTION], [DEFAULT])
#
# DESCRIPTION
#
#	Adds the following arguments to configure:
# --with-NAME-dir=DIR
# and sets Makefile variable NAME_dir accordingly.
#
# Designed especially for the Multispeech project
# by Igor B. Poretsky <poretsky@mlbox.ru>
#

AC_DEFUN([EXTRA_PATH_OPTION],
[
    AC_ARG_WITH([$1-dir],
        [AS_HELP_STRING([--with-$1-dir=DIR], [$2 @<:@default: $3@:>@])],
        [$1_dir="$withval"],
        [$1_dir="$3"]
    )
    AC_SUBST([$1_dir])
])
