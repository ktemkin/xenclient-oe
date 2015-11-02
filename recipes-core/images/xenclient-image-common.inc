
inherit populate_sdk
export STAGING_KERNEL_DIR

#
# When generating an SDK, export our sysroot for external consumption.
# This isn't the most "OE" way to do this, but we're confined by the existing 
# non-oe ways that OpenXT has already adopted.
#
do_populate_sdk_append() {

    # Ensure that our SDK directory exists...
    mkdir -p "${DEPLOY_DIR}/sdk"

    # ... and copy our generated sysroots there.
    cp -r "${WORKDIR}/sdk/image/${SDKPATH}/sysroots" "${DEPLOY_DIR}/sdk/"

}
