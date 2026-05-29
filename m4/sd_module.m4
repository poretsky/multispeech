# Configure paths for libsndfile
#
# Designed especially for the Multispeech project
# by Artem Semenov <savoptik@altlinux.org>
#
#	Adds the following arguments to configure:
# --with-sdmodule-dir=DIR
#

AC_DEFUN([SD_MODULE_DIR],
[
    AC_ARG_WITH([sdmodule-dir],
        [AS_HELP_STRING([--with-sdmodule-dir=DIR], [directory for Speech Dispatcher modules @<:@default: $1@:>@])],
        [sdmodule_dir="$withval"],
        [sdmodule_dir="$1"]
    )
    AC_SUBST([sdmodule_dir])
])
