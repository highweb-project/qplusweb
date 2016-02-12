clear

echo "======================================================"
echo " Web Service Library build system of SeedOpenPlatform"
echo "======================================================"

# choose build type
dependentfolderpath="/usr/local/webkit/odroidxu3/Dependencies/Root/lib"

if [ -d $dependentfolderpath ]
then

buildoption="--disable-webkit2 --disable-battery-status --enable-gamepad --enable-geolocation --enable-svg --enable-svg-fonts --enable-video --enable-webgl --enable-web-audio --disable-debug --enable-jit --enable-engine --enable-egl --enable-gles2 --host=arm-linux-gnueabihf --enable-odroid --enable-webcl --enable-webcl-conformance-test"
else
echo "not exist folder. invalid path : $dependentfolderpath"
exit
fi

export LD_LIBRARY_PATH="$dependentfolderpath" && export PKG_CONFIG_PATH="$dependentfolderpath/pkgconfig" && ../autogen.sh $buildoption

echo "===================================================="
echo "build option: $buildoption"
if [ $? -eq 0 ]
then
echo "Success configuration of WebEngine. Let's go make!!!"
else
echo "Fail to configure of WebEngine."
fi
echo "===================================================="

