PR .= ".1"

DRIDRIVERSXT = "swrast,i915,i965"
PACKAGECONFIG[dri] = "--enable-dri --with-dri-drivers=${DRIDRIVERSXT}, --disable-dri, dri2proto libdrm"