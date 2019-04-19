# Copyright (c) 2018-2019 LG Electronics, Inc.

SUMMARY = "VAL API implementation library Raspberry Pi"
AUTHOR = "Kwanghee Lee <ekwang.lee@lge.com>"
SECTION = "webos/libs"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=e3fc50a88d0a364313df4b21ef20c29e"

PROVIDES = "val-impl"

inherit webos_component
inherit webos_enhanced_submissions
inherit webos_cmake
inherit webos_library
inherit webos_test_provider
inherit webos_machine_dep
inherit webos_public_repo

SRC_URI = "${WEBOSOSE_GIT_REPO_COMPLETE}"
S = "${WORKDIR}/git"

DEPENDS = "udev glib-2.0 pmloglib libpbnjson libdrm videooutput-adaptation-layer-api"

WEBOS_VERSION = "1.0.0-4_ad7aaa8fb5c02496830c4cb6fd8ff321ab910f10"
PR = "r1"

COMPATIBLE_MACHINE = "^rpi$"
