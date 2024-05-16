//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2024, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  A singleton holding information on the current operating system.
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsSingleton.h"
#include "tsUString.h"

namespace ts {
    //!
    //! A singleton holding information on the current operating system.
    //! @ingroup system
    //!
    class TSDUCKDLL SysInfo
    {
        TS_DECLARE_SINGLETON(SysInfo);
    public:
        //!
        //! Operating system architecture.
        //! Note: unlisted architectures are not supported.
        //!
        enum SysArch {
            INTEL32,  //!< Intel IA-32, also known as x86.
            INTEL64,  //!< 64-bit extension of the IA-32 architecture, also known as AMD-64 or Intel x86-64.
            ARM32,    //!< 32-bit Arm (up to Armv7).
            ARM64,    //!< 64-bit Arm (Armv8 onwards), also known as aarch64.
            RISCV64,  //!< 64-bit RISC-V.
            S390X,    //!< 64-bit IBM s390x.
            PPC32,    //!< 32-bit PowerPC.
            PPC64,    //!< 64-bit PowerPC.
            MIPS32,   //!< 32-bit MIPS.
            MIPS64,   //!< 64-bit MIPS.
            SPARC,    //!< Sun SPARC.
        };
        //!
        //! Operating systems families.
        //! Note: unlisted systems are not supported.
        //!
        enum SysOS {
            LINUX,
            MACOS,
            BSD,
            WINDOWS
        };
        //!
        //! Operating systems flavor, typically a Linux distribution.
        //!
        enum SysFlavor {
            NONE,     //!< No divergent flavor in the system family (e.g. macOS, Windows).
            UNKNOWN,  //!< Unknown flavor, unable to identify the distro.
            FEDORA,   //!< Fedora Linux.
            REDHAT,   //!< Red Hat Entreprise Linux or one of its clones such as Rocky Linux, Alma Linux, CentOS.
            UBUNTU,   //!< Ubuntu Linux.
            DEBIAN,   //!< Debian Linux.
            RASPBIAN, //!< Raspbian Linux (Debian derivative on Raspberry Pi).
            ALPINE,   //!< Alpine Linux.
            FREEBSD,  //!< FreeBSD.
            NETBSD,   //!< NetBSD.
            OPENBSD,  //!< OpenBSD.
            DFLYBSD,  //!< DragonFlyBSD.
        };
        //!
        //! Get the architecture of the CPU on which we run.
        //! This can be different from the preprocessing directives. For instance, on Windows,
        //! the actual CPU can be INTEL64 but the compiled code can be INTEL32.
        //! @return The architecture of the CPU on which we run.
        //!
        SysArch arch() const { return _arch; }
        //!
        //! Get the operating system family.
        //! @return The operating system family.
        //!
        SysOS os() const { return _osFamily; }
        //!
        //! Get the operating system flavor.
        //! @return The operating system flavor.
        //!
        SysFlavor osFlavor() const { return _osFlavor; }
        //!
        //! Check if the CPU supports accelerated instructions for CRC32 computation.
        //! @return True if the CPU supports CRC32 instructions.
        //!
        bool crcInstructions() const { return _crcInstructions; }
        //!
        //! Get the operating system version.
        //! @return The operating system version.
        //!
        UString systemVersion() const { return _systemVersion; }
        //!
        //! Get the operating system major version as an integer.
        //! The exact meaning of this number is system dependent.
        //! @return The operating system major version or -1 if unknown.
        //!
        int systemMajorVersion() const { return _systemMajorVersion; }
        //!
        //! Get the operating system name.
        //! @return The operating system name.
        //!
        UString systemName() const { return _systemName; }
        //!
        //! Get the name of the system host.
        //! @return The name of the system host.
        //!
        UString hostName() const { return _hostName; }
        //!
        //! Get the name of the CPU architecure.
        //! @return The name of the CPU architecure.
        //!
        UString cpuName() const { return _cpuName; }
        //!
        //! Get system memory page size.
        //! @return The system memory page size in bytes.
        //!
        size_t memoryPageSize() const { return _memoryPageSize; }

    private:
        SysArch   _arch;
        UString   _cpuName;
        SysOS     _osFamily;
        SysFlavor _osFlavor = UNKNOWN;
        bool      _crcInstructions = false;
        int       _systemMajorVersion = -1;
        UString   _systemVersion {};
        UString   _systemName {};
        UString   _hostName {};
        size_t    _memoryPageSize = 0;
    };
}
