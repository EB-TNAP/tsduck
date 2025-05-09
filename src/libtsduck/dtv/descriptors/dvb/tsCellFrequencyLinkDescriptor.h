//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2025, Thierry Lelegard
// BSD-2-Clause license, see LICENSE.txt file or https://tsduck.io/license
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  Representation of a cell_frequency_link_descriptor
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsAbstractDescriptor.h"

namespace ts {
    //!
    //! Representation of a cell_frequency_link_descriptor
    //! @see ETSI EN 300 468, 6.2.6.
    //! @ingroup libtsduck descriptor
    //!
    class TSDUCKDLL CellFrequencyLinkDescriptor : public AbstractDescriptor
    {
    public:
        //!
        //! Subcell entry.
        //!
        struct TSDUCKDLL Subcell
        {
            Subcell() = default;                //!< Default constructor.
            uint8_t  cell_id_extension = 0;     //!< Cell id extension.
            uint64_t transposer_frequency = 0;  //!< Frequency in Hz.
        };

        //!
        //! List of subcell entries.
        //!
        using SubcellList = std::list<Subcell>;

        //!
        //! Cell entry.
        //!
        struct TSDUCKDLL Cell
        {
            Cell() = default;           //!< Default constructor.
            uint16_t    cell_id = 0;    //!< Cell id.
            uint64_t    frequency = 0;  //!< Frequency in Hz.
            SubcellList subcells {};    //!< List of subcells.
        };

        //!
        //! List of Cell entries.
        //!
        using CellList = std::list<Cell>;

        // CellFrequencyLinkDescriptor public members:
        CellList cells {};  //!< The list of cells and subcells.

        //!
        //! Default constructor.
        //!
        CellFrequencyLinkDescriptor();

        //!
        //! Constructor from a binary descriptor
        //! @param [in,out] duck TSDuck execution context.
        //! @param [in] bin A binary descriptor to deserialize.
        //!
        CellFrequencyLinkDescriptor(DuckContext& duck, const Descriptor& bin);

        // Inherited methods
        DeclareDisplayDescriptor();

    protected:
        // Inherited methods
        virtual void clearContent() override;
        virtual void serializePayload(PSIBuffer&) const override;
        virtual void deserializePayload(PSIBuffer&) override;
        virtual void buildXML(DuckContext&, xml::Element*) const override;
        virtual bool analyzeXML(DuckContext&, const xml::Element*) override;
    };
}
