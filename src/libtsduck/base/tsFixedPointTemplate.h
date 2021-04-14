//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2021, Thierry Lelegard
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------

#pragma once

#if defined(TS_NEED_STATIC_CONST_DEFINITIONS)
template <typename INT, const size_t PREC, typename std::enable_if<std::is_integral<INT>::value && std::is_signed<INT>::value, int>::type N>
constexpr size_t ts::FixedPoint<INT,PREC,N>::PRECISION;

template <typename INT, const size_t PREC, typename std::enable_if<std::is_integral<INT>::value && std::is_signed<INT>::value, int>::type N>
constexpr INT ts::FixedPoint<INT,PREC,N>::FACTOR;
#endif

template <typename INT, const size_t PREC, typename std::enable_if<std::is_integral<INT>::value && std::is_signed<INT>::value, int>::type N>
const ts::FixedPoint<INT,PREC,N> ts::FixedPoint<INT,PREC,N>::MIN(std::numeric_limits<INT>::min(), true);

template <typename INT, const size_t PREC, typename std::enable_if<std::is_integral<INT>::value && std::is_signed<INT>::value, int>::type N>
const ts::FixedPoint<INT,PREC,N> ts::FixedPoint<INT,PREC,N>::MAX(std::numeric_limits<INT>::max(), true);
