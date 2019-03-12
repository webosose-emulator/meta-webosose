# Copyright (c) 2014-2019 LG Electronics, Inc.

EXTENDPRAUTO_append = "webos3"

PACKAGECONFIG ??= ""
PACKAGECONFIG[valgrind] = ",,valgrind,"

# These are used by gallium-llvm in mesa
WEBOS_NO_STATIC_LIBRARIES_WHITELIST = "libLLVMAnalysis.a libLLVMAsmPrinter.a libLLVMBitWriter.a libLLVMCodeGen.a libLLVMCore.a libLLVMExecutionEngine.a libLLVMInstCombine.a libLLVMipa.a libLLVMJIT.a libLLVMMC.a libLLVMMCJIT.a libLLVMMCParser.a libLLVMObjCARCOpts.a libLLVMObject.a libLLVMRuntimeDyld.a libLLVMScalarOpts.a libLLVMSelectionDAG.a libLLVMSupport.a libLLVMTarget.a libLLVMTransformUtils.a libLLVMX86AsmParser.a libLLVMX86AsmPrinter.a libLLVMX86CodeGen.a libLLVMX86Desc.a libLLVMX86Disassembler.a libLLVMX86Info.a libLLVMX86Utils.a"

# Skipped libraries just because we don't need them now:
# libLLVMArchive.a libLLVMAsmParser.a libLLVMBitReader.a libLLVMDebugInfo.a libLLVMInstrumentation.a libLLVMInterpreter.a libLLVMipo.a libLLVMIRReader.a libLLVMLinker.a libLLVMMCDisassembler.a libLLVMOption.a libLLVMTableGen.a libLLVMVectorize.a libLTO.a libprofile_rt.a

# Backport fix from:
# http://lists.openembedded.org/pipermail/openembedded-devel/2015-October/103627.html
# until it is included in our version of meta-oe
PACKAGES_DYNAMIC = "^libllvm${LLVM_RELEASE}-.*$"

do_configure_prepend() {
    # WARNING: llvm's configure will look in the build machine's /usr/include for
    # valgrind/valgrind.h if it doesn't find in the the target sysroot, so only
    # check for it if we know that it's there (i.e., when we have a dependency on
    # valgrind).
    if ${@bb.utils.contains('DEPENDS', 'valgrind', 'false', 'true', d)}; then
        # This is ineffectual because OE only looks for configure.ac in ${S}, and
        # llvm has it under autoconf:
        #   sed -i -e '/^AC_CHECK_HEADERS(\[valgrind\/valgrind.h\])$/ d' autoconf/configure.ac
        # So we have to edit configure directly:
        sed -i -e '/^for ac_header/ s:valgrind/valgrind\.h::' ${S}/configure
    fi
}
