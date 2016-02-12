PROCESSOR=`uname -p`
PWD=`pwd`

export LD_LIBRARY_PATH="$PWD/.libs:/usr/local/odroidxu3/webkit/Dependencies/Root/lib"

$PWD/Programs/GtkLauncher --enable-webgl=TRUE --enable-accelerated-compositing=TRUE --enable-html5-local-storage=TRUE --enable-html5-database=TRUE --enable-file-access-from-file-uris=TRUE --enable-webaudio=TRUE --enable-offline-web-application-cache=TRUE --enable-webcl=TRUE $*
