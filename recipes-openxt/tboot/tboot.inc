DESCRIPTION = "Trusted Boot"
HOMEPAGE = "http://tboot.sourceforge.net/"
SECTION = "bootloaders"
PRIORITY = "optional"
DEPENDS = "trousers"

# This is not a very good way to do this. Some update to this file like a
# change to the copyright date broke the checksum.
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://tboot/include/types.h;beginline=4;endline=32;md5=5a2b57a442b97cfc3d81ba639fda3ac1"

PR = "r6"

SRC_URI = "http://downloads.sourceforge.net/project/${BPN}/${BPN}/${BPN}-${PV}.tar.gz \
           file://tboot-config-cross-compile.patch;patch=1 \
           file://tboot-broken-lcptools-build.patch;patch=1 \
           file://prot-mem.patch;patch=1 \
           file://tboot-serial-card.patch;patch=1 \
           file://tboot-add-ehci-handoff.patch;patch=1 \
           file://tboot-xenclient-policy.patch;patch=1 \
           file://tboot-min-ram.patch;patch=1 \
           file://tboot-bypass-ffffffff-error.patch;patch=1 \
           file://tboot-adjust-grub2-modules.patch;patch=1 \
           file://set-tboot-private-region-as-reserved.patch;patch=1 \
           file://configure_tboot \
           file://lcp_data.bin \
           file://tboot-1.7.0-CVE-2014-5118.patch \
           "

EXTRA_OEMAKE += "TARGET_ARCH=x86_32 CROSS_COMPILE=${TARGET_PREFIX}"

do_compile() {
	oe_runmake SUBDIRS:="tboot" CC="${HOST_PREFIX}gcc ${TOOLCHAIN_OPTIONS}" CPP="${HOST_PREFIX}cpp ${TOOLCHAIN_OPTIONS}" LDFLAGS="" CFLAGS=""
	oe_runmake -C ${S}/tb_polgen CFLAGS+="-std=c99"
	oe_runmake -C ${S}/lcptools CFLAGS+="-std=c99"
	oe_runmake CFLAGS+=" -std=c99 "
	${STRIP} ${S}/tb_polgen/tb_polgen
}

do_install() {
	install -d ${D}/boot
	install -m 444 tboot/tboot.gz ${D}/boot/
        install -d ${D}/usr/share/xenclient
	oe_runmake -C ${S}/tb_polgen install DISTDIR=${D} 
	oe_runmake -C ${S}/lcptools install DISTDIR=${D}
	oe_runmake -C ${S}/utils install DISTDIR=${D}
}

FILES_${PN}-dbg += " /boot/tboot-syms "

FILES_${PN} += " \
        /boot/tboot.gz \
        /usr/sbin/txt-stat \
        /usr/sbin/parse_err \
        /usr/share/xenclient \
"
