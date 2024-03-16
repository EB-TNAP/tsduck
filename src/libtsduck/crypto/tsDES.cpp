//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2024, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------

#include "tsDES.h"
#include "tsSingleton.h"
#include "tsInitCryptoLibrary.h"


//----------------------------------------------------------------------------
// Basic implementation (one block)
//----------------------------------------------------------------------------

TS_BLOCK_CIPHER_DEFINE_PROPERTIES(ts::DES, DES, (u"DES", ts::DES::BLOCK_SIZE, ts::DES::KEY_SIZE));

ts::DES::DES() : BlockCipher(DES::PROPERTIES())
{
    // OpenSSL and Windows BCrypt can encrypt/decrypt in place.
    canProcessInPlace(true);
}

ts::DES::DES(const BlockCipherProperties& props) : BlockCipher(props)
{
    props.assertCompatibleBase(DES::PROPERTIES());
    // OpenSSL and Windows BCrypt can encrypt/decrypt in place.
    canProcessInPlace(true);
}

#if defined(TS_WINDOWS)

TS_STATIC_INSTANCE(ts::FetchBCryptAlgorithm, (BCRYPT_DES_ALGORITHM, BCRYPT_CHAIN_MODE_ECB), FetchECB);
void ts::DES::getAlgorithm(::BCRYPT_ALG_HANDLE& algo, size_t& length, bool& ignore_iv) const
{
    FetchECB::Instance().getAlgorithm(algo, length);
    // This is ECB mode, ignore IV which may be used by a upper chaining mode.
    ignore_iv = true;
}

#else

TS_STATIC_INSTANCE(ts::FetchCipherAlgorithm, ("DES-ECB", "legacy"), Algo);
const EVP_CIPHER* ts::DES::getAlgorithm() const
{
    return Algo::Instance().algorithm();
}

#endif


//----------------------------------------------------------------------------
// Template specialization for ECB mode.
//----------------------------------------------------------------------------

TS_BLOCK_CIPHER_DEFINE_PROPERTIES(ts::ECB<ts::DES>, ECB, (ts::DES::PROPERTIES(), u"ECB", false, ts::DES::BLOCK_SIZE, 0, 0));
ts::ECB<ts::DES>::ECB() : DES(ts::ECB<ts::DES>::PROPERTIES())
{
    // OpenSSL and Windows BCrypt can encrypt/decrypt in place.
    canProcessInPlace(true);
}

ts::ECB<ts::DES>::ECB(const BlockCipherProperties& props) : DES(props)
{
    props.assertCompatibleChaining(ts::ECB<ts::DES>::PROPERTIES());
    // OpenSSL and Windows BCrypt can encrypt/decrypt in place.
    canProcessInPlace(true);
}

#if defined(TS_WINDOWS)

void ts::ECB<ts::DES>::getAlgorithm(::BCRYPT_ALG_HANDLE& algo, size_t& length, bool& ignore_iv) const
{
    FetchECB::Instance().getAlgorithm(algo, length);
    // This is ECB mode, ignore IV which may be used by a upper chaining mode.
    ignore_iv = true;
}

#else

const EVP_CIPHER* ts::ECB<ts::DES>::getAlgorithm() const
{
    return Algo::Instance().algorithm();
}

#endif


//----------------------------------------------------------------------------
// Template specialization for CBC mode.
//----------------------------------------------------------------------------

// Specialization for CBC is currently disabled, see:
// https://stackoverflow.com/questions/78172656/openssl-how-to-encrypt-new-message-with-same-key-without-evp-encryptinit-ex-a
#if defined(TS_WINDOWS)

TS_BLOCK_CIPHER_DEFINE_PROPERTIES(ts::CBC<ts::DES>, CBC, (ts::DES::PROPERTIES(), u"CBC", false, ts::DES::BLOCK_SIZE, 0, ts::DES::BLOCK_SIZE));
ts::CBC<ts::DES>::CBC() : DES(ts::CBC<ts::DES>::PROPERTIES())
{
    canProcessInPlace(true);
}

ts::CBC<ts::DES>::CBC(const BlockCipherProperties& props) : DES(props)
{
    props.assertCompatibleChaining(ts::CBC<ts::DES>::PROPERTIES());
    canProcessInPlace(true);
}

TS_STATIC_INSTANCE(ts::FetchBCryptAlgorithm, (BCRYPT_DES_ALGORITHM, BCRYPT_CHAIN_MODE_CBC), FetchCBC);
void ts::CBC<ts::DES>::getAlgorithm(::BCRYPT_ALG_HANDLE& algo, size_t& length, bool& ignore_iv) const
{
    FetchCBC::Instance().getAlgorithm(algo, length);
    ignore_iv = false;
}

#if 0  // OpenSSL future implementation?
TS_STATIC_INSTANCE(ts::FetchCipherAlgorithm, ("DES-CBC", "legacy"), AlgoCBC);
const EVP_CIPHER* ts::CBC<ts::DES>::getAlgorithm() const
{
    return AlgoCBC::Instance().algorithm();
}
    #endif

#endif
