#!/usr/bin/env bash
#-----------------------------------------------------------------------------
#
#  TSDuck - The MPEG Transport Stream Toolkit
#  Copyright (c) 2005-2025, Thierry Lelegard
#  BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
#
#  Get configuration for DTAPI on current Linux system.
#
#-----------------------------------------------------------------------------

URL_BASE=https://www.dektec.com
HTML_URL=$URL_BASE/downloads/SDK/
GENERIC_URL=$URL_BASE/products/SDK/DTAPI/Downloads/LatestLinuxSDK
CACERT_URL=https://curl.se/ca/cacert.pem

SCRIPT=$(basename $BASH_SOURCE)
ROOTDIR=$(cd $(dirname $BASH_SOURCE)/..; pwd)
BINDIR="$ROOTDIR/bin"

# Get local operating system, speed up if already defined in make.
[[ -z $LOCAL_OS ]] && SYSTEM=$(uname -s | tr A-Z a-z) || SYSTEM="$LOCAL_OS"
[[ -z $LOCAL_ARCH ]] && ARCH=$(uname -m) || ARCH="$LOCAL_ARCH"

info() { echo >&2 "$SCRIPT: $*"; }
error() { echo >&2 "$SCRIPT: $*"; exit 1; }

# Check if DTAPI is supported on the current system.
dtapi-support()
{
    # Environment variable NODTAPI disables the usage of DTAPI.
    [[ -n "$NODTAPI" ]] && return -1

    # DTAPI is supported on Linux and Windows only.
    case "$SYSTEM" in
        linux|cygwin*|mingw*|msys*)
            ;;
        *)
            return -1
    esac

    # DTAPI is supported on Intel CPU only. Speed up if already defined in make.
    [[ $ARCH == x86_64 || $ARCH == i?86 ]] || return -1

    # DTAPI is compiled with the GNU libc and is not supported on systems not using it.
    # Alpine Linux uses musl libc => not supported (undefined reference to __isnan).
    [[ -e /etc/alpine-release ]] && return -1

    # Seems to be a supported distro.
    return 0
}

# Get DTAPI header file.
get-header()
{
    dtapi-support || return 0
    # Start with fast alternative on Linux.
    local header="$BINDIR/LinuxSDK/DTAPI/Include/DTAPI.h"
    if [[ -e "$header" ]]; then
        echo "$header"
    else
        case "$SYSTEM" in
            linux)
                header=$(find 2>/dev/null "$BINDIR/LinuxSDK/DTAPI" -path "*/DTAPI/Include/DTAPI.h" | head -1)
                ;;
            cygwin*)
                header=$(find 2>/dev/null /cygdrive/c/Program\ Files*/Dektec -path "*/DTAPI/Include/DTAPI.h" | head -1)
                ;;
            mingw*|msys*)
                header=$(find 2>/dev/null /c/Program\ Files*/Dektec -path "*/DTAPI/Include/DTAPI.h" | head -1)
                ;;
            *)
                header=
        esac
        [[ -e "$header" ]] && echo "$header"
    fi
}

# Get DTAPI include directory.
get-include()
{
    local header="$(get-header)"
    [[ -n "$header" ]] && dirname "$header"
}

# Get the root directory of the DTAPI.
get-dtapi()
{
    local include="$(get-include)"
    [[ -n "$include" ]] && dirname "$include"
}

# Compute an integer version from a x.y.z version string.
int-version()
{
    local fields=(${1//./ } 0 0 0)
    local v=0
    for ((i=0;$i<3;i++)); do
        v=$((($v * 100) + ${fields[$i]}))
    done
    echo $v
}

# Get DTAPI object file.
get-object()
{
    local dtapidir=$(get-dtapi)
    [[ -z "$dtapidir" ]] && return 0

    # Get object file from platform name.
    local objname=
    if ${OPT_M32:-false}; then
        objname=DTAPI.o
    elif [[ $ARCH == x86_64 ]]; then
        objname=DTAPI64.o
    else
        objname=DTAPI.o
    fi

    # In some older versions, there were several subdirectories under Lib, for distinct GCC versions.
    # If only one object file is present, use this one without checking GCC version.
    local count=0
    local objfile=
    for obj in "$dtapidir"/Lib/*/$objname; do
        if [[ -e "$obj" ]]; then
            objfile="$obj"
            count=$(($count+1))
        fi
    done

    if [[ $count -gt 1 ]]; then
        # More than one object file exist, restart search.
        objfile=

        # Get gcc executable from external $GCC or default.
        GCC=${GCC:-${CXX:-${CC:-$(which gcc 2>/dev/null)}}}
        [[ -z "$GCC" ]] && return 0

        # Get gcc version from external $GCC_VERSION or $GCCVERSION.
        local gccversion=${GCCVERSION:-$GCC_VERSION}
        gccversion=${gccversion:-$("$GCC" -dumpversion 2>/dev/null)}
        gccversion=$(int-version $gccversion)

        # Find the DTAPI object with highest version, lower than or equal to GCC version.
        local objversion=0
        local dirversion=
        local dirname=
        for obj in "$dtapidir"/Lib/*/$objname; do
            dirname=$(basename $(dirname "$obj"))
            dirversion=${dirname#GCC}
            dirversion=${dirversion%%_*}
            dirversion=$(int-version $dirversion)
            if [[ ($dirversion -le $gccversion) && ($dirversion -gt $objversion) ]]; then
                objfile="$obj"
                objversion=$dirversion
            fi
        done
    fi

    [[ -n "$objfile" ]] && echo "$objfile"
}

# Merge an URL with its base.
# The base is the argument. The URL is read from stdin.
merge-url()
{
    local ref="$1"
    local url
    read url

    if [[ -n "$url" ]]; then
        if [[ $url == *:* ]]; then
            echo "$url"
        elif [[ $url == /* ]]; then
            echo "$URL_BASE$url"
        elif [[ $ref == */ ]]; then
            echo "$ref$url"
        else
            ref=$(dirname "$ref")
            echo "$ref/$url"
        fi
    fi
}

# Check SSL/TLS connectivity, try with latest certificates from curl.se.
# This can be necessary when the Let's Encrypt root of trust is unknown
# to curl (see "DST Root CA X3" issue). Can be called any number of times,
# the actual check is done only once.
# Get options.
CMD_ALL=true
CMD_DOWNLOAD=false
CMD_TARBALL=false
CMD_DTAPI=false
CMD_HEADER=false
CMD_INCLUDE=false
CMD_OBJECT=false
CMD_URL=false
CMD_SUPPORT=false
OPT_FORCE=false
OPT_M32=false

while [[ $# -gt 0 ]]; do
    case "$1" in
        --download)
            CMD_DOWNLOAD=true
            CMD_ALL=false
            ;;
        --tarball)
            CMD_TARBALL=true
            CMD_ALL=false
            ;;
        --dtapi)
            CMD_DTAPI=true
            CMD_ALL=false
            ;;
        --header)
            CMD_HEADER=true
            CMD_ALL=false
            ;;
        --include)
            CMD_INCLUDE=true
            CMD_ALL=false
            ;;
        --object)
            CMD_OBJECT=true
            CMD_ALL=false
            ;;
        --url)
            CMD_URL=true
            CMD_ALL=false
            ;;
        --support)
            CMD_SUPPORT=true
            CMD_ALL=false
            ;;
        --force)
            OPT_FORCE=true
            ;;
        --m32)
            OPT_M32=true
            ;;
        *)
            error "invalid option $1 (use --dtapi --header --object --include --url --support --tarball --download --force --m32)"
            ;;
    esac
    shift
done

# Execute commands. Download first if requested.
$CMD_DOWNLOAD && download-dtapi
$CMD_TARBALL && get-tarball
$CMD_DTAPI && get-dtapi
$CMD_HEADER && get-header
$CMD_INCLUDE && get-include
$CMD_OBJECT && get-object
$CMD_URL && get-url
$CMD_SUPPORT && dtapi-support && echo supported

if $CMD_ALL; then
    echo "DTAPI_ROOT=$(get-dtapi)"
    echo "DTAPI_HEADER=$(get-header)"
    echo "DTAPI_INCLUDE=$(get-include)"
    echo "DTAPI_OBJECT=$(get-object)"
    echo "DTAPI_TARBALL=$(get-tarball)"
    echo "DTAPI_URL=$(get-url)"
fi
exit 0
