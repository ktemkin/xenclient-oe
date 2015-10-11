PR .= ".1"

inherit autotools-brokensep update-alternatives

DEPENDS += "libselinux"
EXTRA_OECONF += "--enable-selinux --disable-acl"
