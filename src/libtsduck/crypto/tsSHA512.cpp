//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2024, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsSHA512.h"
#include "tsSingleton.h"
#include "tsInitCryptoLibrary.h"

#if defined(TS_WINDOWS)

TS_STATIC_INSTANCE(ts::FetchBCryptAlgorithm, (BCRYPT_SHA512_ALGORITHM), Fetch);

void ts::SHA512::getAlgorithm(::BCRYPT_ALG_HANDLE& algo, size_t& length) const
{
    Fetch::Instance().getAlgorithm(algo, length);
}

#else

TS_STATIC_INSTANCE(ts::FetchHashAlgorithm, ("SHA512"), Preset);

const EVP_MD_CTX* ts::SHA512::referenceContext() const
{
    return Preset::Instance().referenceContext();
}

#endif
