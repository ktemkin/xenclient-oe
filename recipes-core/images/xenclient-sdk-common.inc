#
# Base for OpenXT Platform SDK Images
#
# The recipes based around this include can be used to create simple
# redistributable "platform SDK" archives. These archives can be used to cross-
# compile packages for the OpenXT virtual machines (e.g. dom0, uivm, ndvm, etc)
# without investing the significant time necessary to build one's entire boot
# environment.
#
# Copyright (C) 2015 Assured Information Security, Inc.
# Author: Kyle J. Temkin <temkink@ainfosec.com>
#
# Released under the MIT license (see COPYING.MIT for the terms)
#

#
# TODO: Make this a bbclass?
#

inherit toolchain-scripts

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=3f40d7994397109285ec7b81fdeb3b58"

# The image off of which this SDK is based.
# The default value is the name of the current image, with -sdk replaced with -image.
BASE_IMAGE ?= "${@d.getVar('PN', True).replace('-sdk', '-image')}"

# The core sysroot that provides the development toolchain for our SDK.
#
# This is not the same as the development toolchain located within the image sysroot,
# if one exists, as the toolchain here is statically compiled to run on most supported 
# machines.
TOOLCHAIN_SYSROOT ?= "${SDK_SYS}"

# The target sysroot that provides the development environment for the machine,
# in the target binary format (cross-compile format). For OpenXT, this is almost
# always the name of the target machine.
TARGET_SYSROOT ?= "${MACHINE}"

# The destination directory for our resultant SDK images.
DEPLOY_DIR_SDK ?= "${DEPLOY_DIR}/sdk"

# Set the working directory in which we'll create our SDK.
B = "${WORKDIR}/${PN}"

# Set the number of threads to use when using xz compression.
XZ_THREADS ?= "${BB_NUMBER_THREADS}"

# Before we can generate an SDK, require that we've generated the base image,
# and generated the core SDK, which contains a statically linked toolchain. This
# toolchain, combined with the relevant machine's sysroot, forms our SDK.
#do_generate_sdk[depends] += " \
#    ${BASE_IMAGE}:do_populate_sdk \
#"

#
# Generate the script that the end user will source to use our SDK.
# adapted from openembedded core's toolchain-scripts.bbclass
#
generate_sdk_environment_script() {
  script="$1"

  # Generate a new script file for the 
  rm -f $script
  touch $script

  # Ensure that the script can run no matter the path in which it was located.
  echo 'export LOCALROOT=$(dirname $(readlink -f $_))' >> $script
  echo 'export TOOLCHAIN_ROOT=${LOCALROOT}/${TOOLCHAIN_SYSROOT}' >> $script
  echo 'export LOCALSDK=${LOCALROOT}/${TARGET_SYSROOT}' >> $script

  # Set up the core sysroot...
  echo 'export PATH=${TOOLCHAIN_ROOT}${bindir_nativesdk}:${TOOLCHAIN_ROOT}${bindir_nativesdk}/${MULTIMACH_TARGET_SYS}:$PATH' >> $script
  echo 'export PKG_CONFIG_SYSROOT_DIR=${LOCALSDK}' >> $script
  echo 'export PKG_CONFIG_PATH=${LOCALSDK}${libdir}/pkgconfig' >> $script
  echo 'export CONFIG_SITE=${LOCALROOT}/site-config-${MACHINE}' >> $script
  echo 'export CC=${TARGET_PREFIX}gcc' >> $script
  echo 'export CXX=${TARGET_PREFIX}g++' >> $script
  echo 'export GDB=${TARGET_PREFIX}gdb' >> $script
  echo 'export TARGET_PREFIX=${TARGET_PREFIX}' >> $script
  echo 'export CONFIGURE_FLAGS="--target=${TARGET_SYS} --host=${TARGET_SYS} --build=${SDK_ARCH}-linux --with-libtool-sysroot=${LOCALSDK}"' >> $script
  echo 'export CFLAGS="${TARGET_CC_ARCH} --sysroot=${LOCALSDK}"' >> $script
  echo 'export CXXFLAGS="${TARGET_CC_ARCH} --sysroot=${LOCALSDK}"' >> $script
  echo 'export LDFLAGS="${TARGET_LD_ARCH} --sysroot=${LOCALSDK}"' >> $script
  echo 'export CPPFLAGS="${TARGET_CC_ARCH} --sysroot=${LOCALSDK}"' >> $script
  echo 'export OECORE_NATIVE_SYSROOT="${LOCALSDK}"' >> $script
  echo 'export OECORE_TARGET_SYSROOT="${LOCALSDK}"' >> $script
  echo 'export OECORE_ACLOCAL_OPTS="-I ${LOCALSDK}/usr/share/aclocal"' >> $script
  echo 'export OECORE_DISTRO_VERSION="${DISTRO_VERSION}"' >> $script
  echo 'export OECORE_SDK_VERSION="${SDK_VERSION}"' >> $script
}


#
# Build our SDK.
#
fakeroot do_generate_sdk() {

    # Ensure that our working directory directory exists...
    mkdir -p "${B}"

    # Create symbolic links to our two main sysroots. Next, we'll archive
    # these with tar using the -h (--dereference) option, which will include
    # the links' targets in our archive. This saves us a massive copy.
    cp -r "${DEPLOY_DIR_SDK}/sysroots/${TOOLCHAIN_SYSROOT}" "${B}"
    cp -r "${STAGING_DIR}/${TARGET_SYSROOT}" "${B}"

    # Generate the source-able scripts that will allow our end users to
    # use our provided SDK.
    generate_sdk_environment_script "${B}/environment-setup-${MACHINE}"

    # Generate the site configuration that will be used by autotools and friends.
    toolchain_create_sdk_siteconfig "${B}/site-config-${MACHINE}"

    # Finally, tar up our completed sysroot.
    tar -czf "${DEPLOY_DIR_SDK}/${PN}.tar.gz" "." -C "${B}"

}
addtask do_generate_sdk before do_build
