# Copyright (c) 2014-2018 LG Electronics, Inc.

SUMMARY = "Systemd service files for system services"
AUTHOR = "Sangwoo Kang <sangwoo82.kang@lge.com>"
SECTION = "webos/base"
LICENSE = "Apache-2.0 & MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

VIRTUAL-RUNTIME_rdx-utils ?= "rdxd"
RDEPENDS_${PN} = "${VIRTUAL-RUNTIME_init_manager} ${VIRTUAL-RUNTIME_rdx-utils}"
RPROVIDES_${PN} = "initscripts initscripts-functions"
PROVIDES = "initscripts"

# TODO: systemd dependency is for fake initctl.
# The dependency needs to be deleted after deleting fake initctl.
DEPENDS = "systemd"

WEBOS_VERSION = "3.0.0-14_2946b1a274da2278e8baf49bdee8388da688efad"
PR = "r10"

inherit webos_component
inherit webos_enhanced_submissions
inherit webos_cmake
inherit webos_distro_dep
inherit webos_distro_variant_dep
inherit webos_public_repo

SRC_URI = "${WEBOSOSE_GIT_REPO_COMPLETE}"
S = "${WORKDIR}/git"
FILESEXTRAPATHS_prepend := "${THISDIR}/initscripts:"

WAM_SYSTEMD_SCRIPT_DEST = "${D}${sysconfdir}/systemd/system/scripts/webapp-mgr.sh"

SRC_URI_append = " file://0001-mask-getty-tty1.service.patch"

do_install_append_webos() {
    if [ -f ${WAM_SYSTEMD_SCRIPT_DEST} ] && [ "${PREFERRED_PROVIDER_virtual/webruntime}" = "webruntime" ]; then
        # TODO: Move these to webapp-mgr.sh.in in source code
        sed -i '/export ENABLE_LAUNCH_OPTIMIZATION=1/a\\n    # setup limit on max gpu memory usage\n    export MAX_GPU_MEM_LIMIT=60' ${WAM_SYSTEMD_SCRIPT_DEST}
        sed -i '/export ENABLE_LAUNCH_OPTIMIZATION=1/a\\n    # setup 2 Mb limitation mse audio buffer size\n    export MSE_AUDIO_BUFFER_SIZE_LIMIT=2097152' ${WAM_SYSTEMD_SCRIPT_DEST}
        sed -i '/export ENABLE_LAUNCH_OPTIMIZATION=1/a\\n    # setup 15 Mb limitation mse video buffer size\n    export MSE_VIDEO_BUFFER_SIZE_LIMIT=15728640' ${WAM_SYSTEMD_SCRIPT_DEST}
        sed -i '/export ENABLE_LAUNCH_OPTIMIZATION=1/a\\n    # # Enable blink features\n    export ENABLE_BLINK_FEATURES=AudioFocusExtension,MediaSourceIsSupportedExtension,MediaTimelineOffset,UMSExtension' ${WAM_SYSTEMD_SCRIPT_DEST}
        sed -i '/--enable-aggressive-release-policy \\/a\        --autoplay-policy=no-user-gesture-required \\' ${WAM_SYSTEMD_SCRIPT_DEST}
        sed -i '/--enable-aggressive-release-policy \\/a\        --enable-neva-ime \\' ${WAM_SYSTEMD_SCRIPT_DEST}
        sed -i '/--enable-aggressive-release-policy \\/a\        --enable-blink-features=\$ENABLE_BLINK_FEATURES \\' ${WAM_SYSTEMD_SCRIPT_DEST}
        sed -i '/--enable-aggressive-release-policy \\/a\        --max-timeupdate-event-frequency=150 \\' ${WAM_SYSTEMD_SCRIPT_DEST}
        sed -i '/--enable-aggressive-release-policy \\/a\        --mse-audio-buffer-size-limit=\$MSE_AUDIO_BUFFER_SIZE_LIMIT \\' ${WAM_SYSTEMD_SCRIPT_DEST}
        sed -i '/--enable-aggressive-release-policy \\/a\        --mse-video-buffer-size-limit=\$MSE_VIDEO_BUFFER_SIZE_LIMIT \\' ${WAM_SYSTEMD_SCRIPT_DEST}
        sed -i '/--enable-aggressive-release-policy \\/a\        --no-sandbox \\' ${WAM_SYSTEMD_SCRIPT_DEST}
        sed -i '/--enable-aggressive-release-policy \\/a\        --fps-counter-layout=tl \\' ${WAM_SYSTEMD_SCRIPT_DEST}
        sed -i '/--enable-aggressive-release-policy \\/a\        --force-gpu-mem-available-mb=\$MAX_GPU_MEM_LIMIT \\' ${WAM_SYSTEMD_SCRIPT_DEST}
        sed -i -e "s/WATCHDOG_RENDER/WATCHDOG_RENDERER/gI" -e "s/watchdog-render/watchdog-renderer/gI" ${WAM_SYSTEMD_SCRIPT_DEST}
        sed -i -e "s/skia-font-cache-size-mb/skia-font-cache-limit-mb/gI" -e "s/skia-image-cache-size-mb/skia-resource-cache-limit-mb/gI" ${WAM_SYSTEMD_SCRIPT_DEST}
    fi
}
