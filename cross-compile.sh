IOS_BASE_SDK="6.1"
IOS_DEPLOY_TGT="3.2"

setenv_all()
{
# Add internal libs
export CXX="$DEVROOT/usr/bin/llvm-g++-4.2"
export CC="$DEVROOT/usr/bin/llvm-gcc-4.2"

export LD=$DEVROOT/usr/bin/ld
export AR=$DEVROOT/usr/bin/ar
export AS=$DEVROOT/usr/bin/as
export NM=$DEVROOT/usr/bin/nm
export RANLIB=$DEVROOT/usr/bin/ranlib
export LDFLAGS="-L$SDKROOT/usr/lib/"

export CPPFLAGS=$CFLAGS
export CXXFLAGS=$CFLAGS
}

setenv_arm6()
{
unset DEVROOT SDKROOT CFLAGS CC LD CPP CXX AR AS NM CXXCPP RANLIB LDFLAGS CPPFLAGS CXXFLAGS

export DEVROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer
export SDKROOT=$DEVROOT/SDKs/iPhoneOS$IOS_BASE_SDK.sdk

export CFLAGS="-arch armv6 -pipe -no-cpp-precomp -isysroot $SDKROOT -miphoneos-version-min=$IOS_DEPLOY_TGT -I$SDKROOT/usr/include/"

setenv_all
}

setenv_arm7()
{
unset DEVROOT SDKROOT CFLAGS CC LD CPP CXX AR AS NM CXXCPP RANLIB LDFLAGS CPPFLAGS CXXFLAGS

export DEVROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer
export SDKROOT=$DEVROOT/SDKs/iPhoneOS$IOS_BASE_SDK.sdk

export CFLAGS="-arch armv7 -pipe -no-cpp-precomp -isysroot $SDKROOT -miphoneos-version-min=$IOS_DEPLOY_TGT -I$SDKROOT/usr/include/"

setenv_all
}

setenv_arm7s()
{
unset DEVROOT SDKROOT CFLAGS CC LD CPP CXX AR AS NM CXXCPP RANLIB LDFLAGS CPPFLAGS CXXFLAGS

export DEVROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer
export SDKROOT=$DEVROOT/SDKs/iPhoneOS$IOS_BASE_SDK.sdk

export CFLAGS="-arch armv7s -pipe -no-cpp-precomp -isysroot $SDKROOT -I$SDKROOT/usr/include/"

setenv_all
}

setenv_i386()
{
unset DEVROOT SDKROOT CFLAGS CC LD CPP CXX AR AS NM CXXCPP RANLIB LDFLAGS CPPFLAGS CXXFLAGS

export DEVROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer
export SDKROOT=$DEVROOT/SDKs/iPhoneSimulator$IOS_BASE_SDK.sdk

export CFLAGS="-arch i386 -pipe -no-cpp-precomp -isysroot $SDKROOT -miphoneos-version-min=$IOS_DEPLOY_TGT"

setenv_all
}

create_outdir_lipo()
{
for lib_i386 in `find $LOCAL_OUTDIR/i386 -name "lib*\.a"`; do
    lib_arm6=`echo $lib_i386 | sed "s/i386/arm6/g"`
    lib_arm7=`echo $lib_i386 | sed "s/i386/arm7/g"`
    lib=`echo $lib_i386 | sed "s/i386\///g"`
    lipo -arch armv6 $lib_arm6 -arch armv7 $lib_arm7 -arch i386 $lib_i386 -create -output $lib
done
}

merge_libfiles()
{
DIR=$1
LIBNAME=$2

cd $DIR
for i in `find . -name "lib*.a"`; do
    $AR -x $i
done
$AR -r $LIBNAME *.o
rm -rf *.o __*
cd -
}
