# Copyright (c) 2012-2019 LG Electronics, Inc.

require conf/distro/include/webos-component-head.inc
require conf/distro/include/webos-preferred-providers.inc
require conf/distro/include/webos-recipe-blacklist.inc
require conf/distro/include/webos-preferred-versions.inc
require conf/distro/include/webos-features.inc
require conf/distro/include/webos-toolchain.inc
require conf/distro/include/webos-sdk.inc
require conf/distro/include/webos-icecc-components-exclusion.inc

# Disable building of static libraries
require conf/distro/include/no-static-libs.inc

#
# Form the codename of a release (WEBOS_DISTRO_RELEASE_CODENAME) from a codename
# that represents the major set of features of the platform
# (WEBOS_DISTRO_PLATFORM_CODENAME) and a codename that represents the features
# of the particular DISTRO_VERSION being released (WEBOS_DISTRO_VERSION_CODENAME).
# The two more or less correspond the the major and minor versions of
# DISTRO_VERSION, but allow there to be a name for them ahead of the assignment
# of actual version numbers. It is expected that there will be several version
# codenames with the same platform codename under active development at the same
# time.
#
# It is expected that other distros will override these definitions based on
# their codename schemes.
#
WEBOS_DISTRO_PLATFORM_CODENAME ?= "${DISTRO}"
WEBOS_DISTRO_VERSION_CODENAME ?= "${@base_get_metadata_git_branch('.', None).strip().translate(str.maketrans('','','()/'))}"

# Increase when release is not compatible to previous one which has the same distro platform
WEBOS_DISTRO_RELEASE_EPOCH ?= ""
WEBOS_DISTRO_RELEASE_CODENAME ?= "${WEBOS_DISTRO_PLATFORM_CODENAME}${WEBOS_DISTRO_RELEASE_EPOCH}-${WEBOS_DISTRO_VERSION_CODENAME}"

# This is the codename for the series of builds that are made from a branch.
# It appears in the image filenames. It is expected that other distros will
# override this based on their codename schemes.
WEBOS_DISTRO_BUILD_CODENAME ?= "${@base_get_metadata_git_branch('.', None).strip().translate(str.maketrans('','','()/'))}"

WEBOS_DISTRO_PRERELEASE ?= "devel"

#
# All of the types of official build jobs should override WEBOS_DISTRO_BUILD_ID
# in the following format: "[[<prefix>.]<site>-]<build-number>", where the
# optional <site> identifies the build server site, and the optional <prefix>
# distinguishes the different types of build jobs, for example, verification
# builds. <prefix> is left off for the "official" build and <site> is dropped
# for the authoritative "official" build. <build-number> is expected to be an
# integer. Note that WEBOS_DISTRO_BUILD_CODENAME is never included in
# WEBOS_DISTRO_BUILD_ID.
#
WEBOS_DISTRO_BUILD_ID ?= "unofficial"

# Only suffix image names with DATETIME when the build is unofficial; otherwise,
# use WEBOS_DISTRO_BUILD_ID. By default, the build codename (or, if it's defined,
# WEBOS_DISTRO_BRANCHPOINT_MICRO_VERSION with its hyphen replaced with a period)
# is always prepended to the suffix, but this can be overridden.
WEBOS_IMAGE_BUILD_CODENAME ?= "${@ \
    '-' + d.getVar('WEBOS_DISTRO_BRANCHPOINT_MICRO_VERSION', True).replace('-', '.') \
    if d.getVar('WEBOS_DISTRO_BRANCHPOINT_MICRO_VERSION', True) else \
    '-${WEBOS_DISTRO_BUILD_CODENAME}' \
}"

WEBOS_IMAGE_NAME_SUFFIX = "${WEBOS_IMAGE_BUILD_CODENAME}-${@oe.utils.conditional( \
                                  'WEBOS_DISTRO_BUILD_ID', 'unofficial', \
                                  '${DATETIME}', '${WEBOS_DISTRO_BUILD_ID}', d) }"
# Prevent "Bitbake's cached basehash does not match the one we just generated"
# errors:
WEBOS_IMAGE_NAME_SUFFIX[vardepsexclude] = "DATETIME"

# Without this, the shared state is always invalidated
# during the build which has different WEBOS_DISTRO_BUILD_ID
WEBOS_IMAGE_NAME_SUFFIX[vardepsexclude] += "WEBOS_DISTRO_BUILD_ID"

WEBOS_IMAGE_NAME_PARTITION_SUFFIX ?= ""
IMAGE_NAME = "${IMAGE_BASENAME}-${MACHINE}${WEBOS_IMAGE_NAME_PARTITION_SUFFIX}"
IMAGE_LINK_NAME = "${IMAGE_NAME}${WEBOS_IMAGE_NAME_SUFFIX}"

# This is used by image-vmdk.bbclass and default value "${IMGDEPLOYDIR}/${IMAGE_BASENAME}-${MACHINE}.ext3"
# cannot be used until it's fixed (probably with Yocto 2.1):
# http://lists.openembedded.org/pipermail/openembedded-core/2015-November/112785.html
# http://lists.openembedded.org/pipermail/openembedded-core/2015-November/112784.html
ROOTFS = "${IMGDEPLOYDIR}/${IMAGE_LINK_NAME}.ext4"

# Use a different naming scheme for webOS kernel images that uses the component
# name instead of the name of the image format (KERNEL_IMAGETYPE) and leaves off
# the version.
WEBOS_KERNEL_IMAGE_BASE_NAME_PARTITION_SUFFIX ?= ""
KERNEL_IMAGE_BASE_NAME = "${PREFERRED_PROVIDER_virtual/kernel}-${MACHINE}${WEBOS_KERNEL_IMAGE_BASE_NAME_PARTITION_SUFFIX}"
KERNEL_IMAGE_SYMLINK_NAME = "${KERNEL_IMAGE_BASE_NAME}${WEBOS_IMAGE_NAME_SUFFIX}"
# For qemuboot.bbclass to find existing kernel image (we remove ${KERNEL_IMAGETYPE} in kernel.bbclass).
QB_DEFAULT_KERNEL = "${KERNEL_IMAGETYPE}-${KERNEL_IMAGE_BASE_NAME}.bin"
# For live-vm-common.bbclass to find existing kernel image (we remove ${KERNEL_IMAGETYPE} in kernel.bbclass).
VM_DEFAULT_KERNEL = "${KERNEL_IMAGETYPE}-${KERNEL_IMAGE_BASE_NAME}.bin"

# By default, kernels for webOS don't need module tarballs to be deployed, but
# still define webOS-style base name for them in case some MACHINE.conf enables
# their deployment.
MODULE_TARBALL_DEPLOY ?= "0"
MODULE_IMAGE_BASE_NAME = "${KERNEL_IMAGE_BASE_NAME}-modules"
MODULE_IMAGE_SYMLINK_NAME = "${KERNEL_IMAGE_SYMLINK_NAME}-modules"

# webOS doesn't use kernel-image-name, so don't include it, which allows us to
# vardepsexclude KERNEL_IMAGE_BASE_NAME. This makes it possible to execute the
# kernel's do_deploy (which is quick) every time the kernel is built without
# having to also execute its do_install (which is lengthy), and thereby have its
# image filename match that of the corresponding rootfs image even if its
# contents are unchanged.
kernel_do_install[vardepsexclude] += "KERNEL_IMAGE_BASE_NAME"
kernel_do_install_append() {
	rm -vf $kerneldir/kernel-image-name
}

# Add WEBOS_DISTRO_RELEASE_CODENAME and WEBOS_DISTRO_BUILD_ID to what's displayed
# as the "build configuration".
BUILDCFG_VARS += "WEBOS_DISTRO_RELEASE_CODENAME WEBOS_DISTRO_BUILD_ID"

# Branch, revision and description of TOPDIR (build-* repository)
WEBOS_DISTRO_TOPDIR_BRANCH ?= "${@base_get_metadata_git_branch('${TOPDIR}', None).strip()}"
WEBOS_DISTRO_TOPDIR_REVISION ?= "${@base_get_metadata_git_revision('${TOPDIR}', None).strip()}"
WEBOS_DISTRO_TOPDIR_DESCRIBE ?= "${@webos_base_get_metadata_git_describe('${TOPDIR}', None).strip()}"
# Don't add WEBOS_DISTRO_TOPDIR_BRANCH, because in many cases it can be "(nobranch)"
BUILDCFG_VARS += "WEBOS_DISTRO_TOPDIR_REVISION WEBOS_DISTRO_TOPDIR_DESCRIBE"

# Add build timestamp to what's displayed in build-id.txt for use in pruning script
BUILDCFG_VARS += "DATETIME"

# Build time dependency on TUNE_PKGARCH recipes base-passwd, shadow, shadow-sysroot
# Drop this when upgrading oe-core to some release with this change:
# http://lists.openembedded.org/pipermail/openembedded-core/2013-November/085849.html
PACKAGE_ARCH_pn-xuser-account = "${MACHINE_ARCH}"

# Override these in webOS based distros to modify DISTRO_EXTRA_R*
WEBOS_DEFAULT_EXTRA_RDEPENDS = ""
WEBOS_DEFAULT_EXTRA_RRECOMMENDS = ""

DISTRO_EXTRA_RDEPENDS += " ${WEBOS_DEFAULT_EXTRA_RDEPENDS}"
DISTRO_EXTRA_RRECOMMENDS += " ${WEBOS_DEFAULT_EXTRA_RRECOMMENDS}"

WEBOSQEMUDEPS = "${@bb.utils.contains("INCOMPATIBLE_LICENSE", "GPLv3", "", "qemu-config",d)}"
DISTRO_EXTRA_RDEPENDS_append_qemuarm = " ${WEBOSQEMUDEPS}"
DISTRO_EXTRA_RDEPENDS_append_qemux86 = " ${WEBOSQEMUDEPS}"

CACHE = "${TMPDIR}/cache/${TCMODE}-${TCLIBC}${@['', '/' + str(d.getVar('MACHINE', True))][bool(d.getVar('MACHINE', True))]}${@['', '/' + str(d.getVar('SDKMACHINE', True))][bool(d.getVar('SDKMACHINE', True))]}"

USER_CLASSES ?= ""

# WEBOS_EXTRA_MACHINEOVERRIDES will appear in OVERRIDES before what's added by
# the machine architecture .inc file, which appears before MACHINE.
# WEBOS_EXTRA_MACHINEOVERRIDES is set in webos_machine_impl_dep.bbclass . Do it
# this way instead of just in the bbclass so that the position in OVERRIDES is
# fixed.
WEBOS_EXTRA_MACHINEOVERRIDES ?= ""
MACHINEOVERRIDES =. "${WEBOS_EXTRA_MACHINEOVERRIDES}"
MACHINEOVERRIDES[vardepsexclude] += "WEBOS_EXTRA_MACHINEOVERRIDES"
OVERRIDES[vardepsexclude] += "WEBOS_EXTRA_MACHINEOVERRIDES"

# WEBOS_EXTRA_MACHINEOVERRIDES_VARIANT will appear in OVERRIDES before what's added by
# the machine architecture .inc file, which appears before MACHINE.
# WEBOS_EXTRA_MACHINEOVERRIDES_VARIANT is set in webos_machine_variant_dep.bbclass .
# Do it this way instead of just in the bbclass so that the position in OVERRIDES is
# fixed.
WEBOS_EXTRA_MACHINEOVERRIDES_VARIANT ?= ""
MACHINEOVERRIDES =. "${WEBOS_EXTRA_MACHINEOVERRIDES_VARIANT}"
MACHINEOVERRIDES[vardepsexclude] += "WEBOS_EXTRA_MACHINEOVERRIDES_VARIANT"
OVERRIDES[vardepsexclude] += "WEBOS_EXTRA_MACHINEOVERRIDES_VARIANT"

# Use separate variable for PREFERRED_ARM_INSTRUCTION_SET.
# If you want to override distro selection for some group (e.g. based on
# MACHINE, architecture or some TUNE_PKGARCH) you apply the override
# only to PREFERRED_ARM_INSTRUCTION_SET variable, so that
# ARM_INSTRUCTION_SET assignments without any override in the recipes
# still work.
# E.g. distro default is thumb (for all MACHINEs)
# Then you find some issues with thumb1 on armv5te cores, so distro can
# change the default to "arm" with PREFERRED_ARM_INSTRUCTION_SET_armv5,
# but the recipe which explicitly asks for "thumb" still gets thumb.
PREFERRED_ARM_INSTRUCTION_SET ?= "thumb"
ARM_INSTRUCTION_SET = "${PREFERRED_ARM_INSTRUCTION_SET}"

# Work around FTBFS when building valgrind with ptest and thumb enabled
# valgrind/3.9.0-r8/valgrind-3.9.0/none/tests/arm/ldrt_arm.c
# {standard input}: Assembler messages:
# {standard input}:42: Error: cannot use post-indexing with this instruction -- `ldrt r6,[r5],#+132'
# {standard input}:42: Error: cannot use post-indexing with this instruction -- `ldrt r6,[r5],#132'
# ...
# ARM_INSTRUCTION_SET_pn-valgrind = "arm"
# and remove -O2 as work around for ICE:
# 3.9.0-r8/valgrind-3.9.0/none/tests/arm/vcvt_fixed_float_VFP.c:12:1: internal compiler error:
# Max. number of generated reload insns per insn is achieved (90)
# http://lists.openembedded.org/pipermail/openembedded-core/2014-September/097330.html
# both issues are still reproducible with 3.10.0 version imported from
# http://patchwork.openembedded.org/patch/83009/
# TARGET_CFLAGS_remove_pn-valgrind = "-O2"
PTEST_ENABLED_pn-valgrind = "0"
# parted and apr ptest currently doesn't build with gold
# http://lists.openembedded.org/pipermail/openembedded-core/2017-February/132917.html
PTEST_ENABLED_pn-parted = "0"
PTEST_ENABLED_pn-apr = "0"

INHERIT_DISTRO ?= "debian devshell sstate license buildstats-summary webos_base webos_no_static_libraries"

# Inherit icecc here, so that all builders have the same sstate signatures
INHERIT_DISTRO += "icecc"
# and then disable its function by default (so that people still need to explicity
# enable it in webos-local.conf if they have configured icecc and want to use it.
# You need to set _empty_ value in webos-local.conf to enable icecc function:
# ICECC_DISABLED = ""
ICECC_DISABLED ??= "1"

WARN_QA_append = " version-going-backwards webos-enh-sub-warning"
WEBOS_ERROR_QA_AUTOREV ?= "webos-enh-sub-autorev-error"
ERROR_QA_append = " webos-enh-sub-error acg-dirty acg-json ${WEBOS_ERROR_QA_AUTOREV} acg-api acg-perm ls2-role ls2-schema"
ERROR_QA_remove = " version-going-backwards"
# Move most test from WARN_QA to ERROR_QA to prevent silently re-introducing these issues
ERROR_QA_append = " ldflags useless-rpaths rpaths staticdev libdir xorg-driver-abi textrel already-stripped incompatible-license files-invalid installed-vs-shipped compile-host-path install-host-path pn-overrides infodir build-deps file-rdeps unknown-configure-option symlink-to-sysroot multilib invalid-packageconfig host-user-contaminated"
WARN_QA_remove = " ldflags useless-rpaths rpaths staticdev libdir xorg-driver-abi textrel already-stripped incompatible-license files-invalid installed-vs-shipped compile-host-path install-host-path pn-overrides infodir build-deps file-rdeps unknown-configure-option symlink-to-sysroot multilib invalid-packageconfig host-user-contaminated"

# Be more strict about warnings which currently aren't triggered by our builds
# to make sure that it stays that way
ERROR_QA_append = " \
    already-stripped \
    installed-vs-shipped \
    ldflags \
    libdir \
    rpaths \
    staticdev \
    textrel \
    unknown-configure-option \
    useless-rpaths \
    xorg-driver-abi \
"

# Default WEBOS_TARGET_* values (see webos_cmake.bbclass)
WEBOS_TARGET_CORE_OS ?= "rockhopper"
WEBOS_TARGET_MACHINE_IMPL ?= "emulator"
WEBOS_TARGET_MACHINE_VARIANT ?= "none"

# Change default PACKAGE_ARCH to MACHINE_ARCH, one of important goals for OE
# is to share binary feed and sstate between MACHINEs with the same architecture
# design of some webOS components doesn't support this very well - TUNE_PKGARCH
# recipes (runtime) depending on MACHINE_ARCH recipes, couple of recipes
# could be TUNE_PKGARCH if the MACHINE-specific changes are moved to separate
# config file provided by small MACHINE_ARCH recipe instead of using MACHINE
# variable in build-time to customize component.
# All officially supported MACHINEs have different architecture, so we weren't
# sharing any packages/sstate.
# Be aware that this wasn't tested with nativesdk.
PACKAGE_ARCH = "${MACHINE_ARCH}"

# For toolchain recipes keep TUNE_PKGARCH, to support default STAGING_BINDIR_TOOLCHAIN
# as defined in bitbake.conf
# STAGING_BINDIR_TOOLCHAIN = "${STAGING_DIR_NATIVE}${bindir_native}/${TUNE_PKGARCH}${TARGET_VENDOR}-${TARGET_OS}"
PACKAGE_ARCH_pn-binutils-cross = "${TUNE_PKGARCH}"
PACKAGE_ARCH_pn-gcc-cross = "${TUNE_PKGARCH}"
PACKAGE_ARCH_pn-gcc-cross-initial = "${TUNE_PKGARCH}"
# libgcc.inc: target=`echo ${MULTIMACH_TARGET_SYS} | sed -e s#-${SDKPKGSUFFIX}##`
PACKAGE_ARCH_pn-libgcc = "${TUNE_PKGARCH}"
# gcc-runtime.inc: mtarget=`echo ${MULTIMACH_TARGET_SYS} | sed -e s#-${SDKPKGSUFFIX}##`
PACKAGE_ARCH_pn-gcc-runtime = "${TUNE_PKGARCH}"
PACKAGE_ARCH_pn-meta-toolchain = "${TUNE_PKGARCH}"

# Respect PACKAGE_ARCH set in recipe, otherwise building sdk will fail to find
# packagegroup-cross-canadian-qemux86 package in MACHINE_ARCH feed (opkg-sdk.conf
# doesn't contain MACHINE_ARCH).
PACKAGE_ARCH_pn-packagegroup-cross-canadian-qemux86 = "${SDK_ARCH}-${SDKPKGSUFFIX}"

# The webOS OSE distro is packaged into these image types:
IMAGE_FSTYPES ?= "tar.gz"

# Only create a vmdk image when we're building for x86 emulator,
# i.e. when MACHINE is qemux86 or qemux86-64
# If IMAGE_FSTYPES_append_qemux86 = " vmdk" is used, the inherit of
# image-vmdk.bbclass in image.bbclass doesn't happen, because IMAGE_FSTYPES
# overrides are expanded too late.
IMAGE_FSTYPES := "${@ '${IMAGE_FSTYPES} vmdk' if '${MACHINE}'.startswith('qemux86') else '${IMAGE_FSTYPES}' }"

# "tar.bz2 ext3" is added by qemu.inc, replace tar.bz2 with tar.gz in it
IMAGE_FSTYPES := "${@ ' ${IMAGE_FSTYPES} '.replace(' tar.bz2 ',' tar.gz ') }"

PATH_prepend = "${WEBOS_EXTRA_PATH}"

DEPENDS_append_pn-cmake-native = " openssl-native libidn-native"

EXTRAOPKGCONFIG = ""
# with multilib libdir would became /usr/lib64 and we want opkg files in /usr/lib
OPKGLIBDIR_forcevariable := "${exec_prefix}/lib"

# We don't plan to ever support qt4
QT_DIR_NAME = ""

BB_DISKMON_DIRS = "\
    STOPTASKS,${TMPDIR},2G,100K \
    STOPTASKS,${DL_DIR},2G,100K \
    STOPTASKS,${SSTATE_DIR},2G,100K \
    STOPTASKS,/tmp,200M,100K \
    ABORT,${TMPDIR},1G,10K \
    ABORT,${DL_DIR},1G,10K \
    ABORT,${SSTATE_DIR},1G,10K \
    ABORT,/tmp,100M,10K \
"

# QEMU(x86/arm) based vmdk emulator need root as hda2 used in VirtualBox
SYSLINUX_ROOT_qemuall = "root=/dev/hda2"

# Select at most one of the two choices here for the console:
#   Redirect console output to keep it from spewing all over the Emulator window
#   (This is added to qemux86 kernel cmdline by openembedded-core/meta/classes/syslinux.bbclass)
APPEND_append_qemuall = " console=ttyS0 net.ifnames=0 biosdevname=0"
# - or -
#   If your build is not using the screen for graphics, and you're running an emulator,
#   you should be able to use the emulator's on-screen window as the console by
#   uncommenting this line (and commenting the above line out).
#   Note that this setting may interfere with a program that is trying to change the
#   mode of the display device
# APPEND_append_qemuall = " console=tty0"

MIRRORS =+ "\
ftp://.*/.*      http://downloads.yoctoproject.org/mirror/sources/ \n \
http://.*/.*     http://downloads.yoctoproject.org/mirror/sources/ \n \
https://.*/.*    http://downloads.yoctoproject.org/mirror/sources/ \n"

# The CONNECTIVITY_CHECK_URI's are used to test whether we can succesfully
# fetch from the network (and warn you if not). To disable the test set
# the variable to be empty.
CONNECTIVITY_CHECK_URIS ?= "git://git.yoctoproject.org/yocto-firewall-test;protocol=git;rev=HEAD \
                          https://eula-downloads.yoctoproject.org/index.php \
                          http://bugzilla.yoctoproject.org/report.cgi"

SANITY_TESTED_DISTROS ?= " \
    Ubuntu-14.04 \n \
    Ubuntu-16.04 \n \
    Ubuntu-18.04 \n \
"
