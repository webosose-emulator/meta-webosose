# Copyright (c) 2013-2018 LG Electronics, Inc.

SUMMARY = "The core of the Luna Surface Manager (compositor)"
AUTHOR  = "Anupam Kaul <anupam.kaul@lge.com>"
SECTION = "webos/libs"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

DEPENDS = "qtdeclarative wayland-native qtwayland qtwayland-native qt-features-webos pmloglib webos-wayland-extensions"

WEBOS_VERSION = "1.1.0-3_28131e554f5e569ad27a96955251869bb1921f45"
PR = "r42"

inherit webos_qmake5
inherit webos_enhanced_submissions
inherit webos_lttng
inherit webos_public_repo

SRC_URI = "${WEBOSOSE_GIT_REPO_COMPLETE}"
S = "${WORKDIR}/git"

OE_QMAKE_PATH_HEADERS = "${OE_QMAKE_PATH_QT_HEADERS}"

# Enable LTTng tracing capability when enabled in webos_lttng class
EXTRA_QMAKEVARS_PRE += "${@ 'CONFIG+=lttng' if '${WEBOS_LTTNG_ENABLED}' == '1' else '' }"

EXTRA_QMAKEVARS_PRE += "${PACKAGECONFIG_CONFARGS}"

# We don't support configuring via cmake
EXTRA_QMAKEVARS_POST += "CONFIG-=create_cmake"

FILES_${PN}-dev += " \
    ${OE_QMAKE_PATH_QT_ARCHDATA}/mkspecs/* \
    ${OE_QMAKE_PATH_LIBS}/*.prl \
"

do_install_append() {
    sed -i 's@prefix=${STAGING_DIR_HOST}@prefix=@g;s@-L${STAGING_DIR_HOST} @ @g;' ${D}${libdir}/pkgconfig/*.pc
    sed -i "s@-L${STAGING_LIBDIR}@-L\${libdir}@g" ${D}${libdir}/pkgconfig/*.pc
    if ${@bb.utils.contains('PACKAGECONFIG', 'compositor', 'true', 'false', d)}; then
        install -d ${D}${datadir}/webos-keymap
        ${STAGING_DIR_NATIVE}${OE_QMAKE_PATH_QT_BINS}/generate_qmap ${D}${datadir}/webos-keymap/webos-keymap.qmap
    fi
}

VIRTUAL-RUNTIME_gpu-libs ?= ""
RDEPENDS_${PN} += "${VIRTUAL-RUNTIME_gpu-libs}"

PROVIDES += "${@bb.utils.contains('PACKAGECONFIG', 'compositor', 'surface-manager', '', d)}"

inherit webos_system_bus
inherit webos_qmllint

# qt-features-webos have its own logic to install system bus files reason for
# that is because only qmake knows where substitued files will be placed.
WEBOS_SYSTEM_BUS_SKIP_DO_TASKS = "1"

PACKAGECONFIG ??= "multi-input"
PACKAGECONFIG[compositor] = "CONFIG+=compositor_base,,qt-features-webos-native"
PACKAGECONFIG[multi-input] = ",CONFIG+=no_multi_input,"
PACKAGECONFIG[cursor-theme] = "CONFIG+=cursor_theme,,"

PACKAGECONFIG_webos = "compositor cursor-theme"

SRC_URI_append = " file://0001-change-WEBOS_COMPOSITOR_PLATFORM-to-minimalegl.patch"

PACKAGES =+ "${PN}-base"

FILES_${PN}-base += " \
    ${OE_QMAKE_PATH_QML}/WebOSCompositorBase/ \
    ${OE_QMAKE_PATH_QML}/WebOSCompositor/ \
    ${OE_QMAKE_PATH_BINS}/ \
    ${datadir}/icons/ \
    ${datadir}/webos-keymap/webos-keymap.qmap \
    ${WEBOS_SYSTEM_BUS_DIRS} \
"
RDEPENDS_${PN}-base += "xkeyboard-config qml-webos-framework qml-webos-bridge qml-webos-components"
