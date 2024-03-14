//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2024, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  DVB-CISSA AES-based TS packet encryption.
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsCBC.h"
#include "tsAES128.h"

namespace ts {
    //!
    //! DVB-CISSA AES-based TS packet encryption.
    //! (CISSA = Common IPTV Software-oriented Scrambling Algorithm).
    //! @ingroup crypto
    //! @see ETSI TS 103 127, chapter 6
    //!
    class TSDUCKDLL DVBCISSA: public CBC<AES128>
    {
        TS_NOCOPY(DVBCISSA);
    public:
        //! Default constructor.
        DVBCISSA();
        //! Destructor.
        virtual ~DVBCISSA() override;
    protected:
        TS_BLOCK_CIPHER_DECLARE_PROPERTIES(DVBCISSA);
    };
}
