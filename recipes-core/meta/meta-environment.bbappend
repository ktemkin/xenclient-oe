#
# Fix do_generate_content for use with populate_sdk.
#
# Tmis task is designed to work with the ADT (application development toolkit) installer,
# which we currently don't have the infrastructure to support. Unfortunately, it breaks
# the basic populate_sdk, which we'd _like_ to support. This method restores support for
# populate_sdk.
#

#Modified from toolchain-scripts.bbclass
toolchain_create_sdk_env_script_with_real_multimach () {
    # Create environment setup script
    script=${SDK_OUTPUT}/${SDKPATH}/environment-setup-${REAL_MULTIMACH_TARGET_SYS}
    rm -f $script
    touch $script
    echo 'export PATH=${SDKPATHNATIVE}${bindir_nativesdk}:${SDKPATHNATIVE}${bindir_nativesdk}/${REAL_MULTIMACH_TARGET_SYS}:$PATH' >> $script
    echo 'export PKG_CONFIG_SYSROOT_DIR=${SDKTARGETSYSROOT}' >> $script
    echo 'export PKG_CONFIG_PATH=${SDKTARGETSYSROOT}${libdir}/pkgconfig' >> $script
    echo 'export CONFIG_SITE=${SDKPATH}/site-config-${REAL_MULTIMACH_TARGET_SYS}' >> $script
    echo 'export CC=${TARGET_PREFIX}gcc' >> $script
    echo 'export CXX=${TARGET_PREFIX}g++' >> $script
    echo 'export GDB=${TARGET_PREFIX}gdb' >> $script
    echo 'export TARGET_PREFIX=${TARGET_PREFIX}' >> $script
    echo 'export CONFIGURE_FLAGS="--target=${TARGET_SYS} --host=${TARGET_SYS} --build=${SDK_ARCH}-linux --with-libtool-sysroot=${SDKTARGETSYSROOT}"' >> $script
    if [ "${TARGET_OS}" = "darwin8" ]; then
    	echo 'export TARGET_CFLAGS="-I${SDKTARGETSYSROOT}${includedir}"' >> $script
    	echo 'export TARGET_LDFLAGS="-L${SDKTARGETSYSROOT}${libdir}"' >> $script
    	# Workaround darwin toolchain sysroot path problems
    	cd ${SDK_OUTPUT}${SDKTARGETSYSROOT}/usr
    	ln -s /usr/local local
    fi
    echo 'export CFLAGS="${TARGET_CC_ARCH} --sysroot=${SDKTARGETSYSROOT}"' >> $script
    echo 'export CXXFLAGS="${TARGET_CC_ARCH} --sysroot=${SDKTARGETSYSROOT}"' >> $script
    echo 'export LDFLAGS="${TARGET_LD_ARCH} --sysroot=${SDKTARGETSYSROOT}"' >> $script
    echo 'export CPPFLAGS="${TARGET_CC_ARCH} --sysroot=${SDKTARGETSYSROOT}"' >> $script
    echo 'export OECORE_NATIVE_SYSROOT="${SDKPATHNATIVE}"' >> $script
    echo 'export OECORE_TARGET_SYSROOT="${SDKTARGETSYSROOT}"' >> $script
    echo 'export OECORE_ACLOCAL_OPTS="-I ${SDKPATHNATIVE}/usr/share/aclocal"' >> $script
    echo 'export OECORE_DISTRO_VERSION="${DISTRO_VERSION}"' >> $script
    echo 'export OECORE_SDK_VERSION="${SDK_VERSION}"' >> $script
}


do_generate_content_append() {
    toolchain_create_sdk_env_script_with_real_multimach
}

