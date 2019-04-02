# Copyright (c) 2017 LG Electronics, Inc.

EXTENDPRAUTO_append = "webos1"

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

# Add gallium, gallium-llvmpipe, opengl and enable wayland even without wayland in DISTRO_FEATURES
PACKAGECONFIG_qemuall = "opengl gbm egl gles dri wayland gallium"

# Add virgl gallium driver
GALLIUMDRIVERS_qemuall = "svga"
GALLIUMDRIVERS_LLVM_qemuall = "svga"
DRIDRIVERS_qemuall = "swrast"

# Enable wayland even without wayland in DISTRO_FEATURES
PLATFORMS_qemuall = "drm,wayland"
