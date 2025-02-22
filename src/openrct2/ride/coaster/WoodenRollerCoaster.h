/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../drawing/ImageId.hpp"
#include "../../paint/Boundbox.h"
#include "../../paint/Paint.h"
#include "../../world/Location.hpp"
#include "../TrackPaint.h"

#include <cstdint>

struct SpriteBoundBox2
{
    ImageIndex ImageIdA;
    ImageIndex ImageIdB;
    CoordsXYZ offset;
    ::BoundBoxXYZ BoundBox;
};

// Magic number 4 refers to the number of track blocks in a diagonal track element
static constexpr const int8_t WoodenRCDiagonalSupports[4][NumOrthogonalDirections] = {
    { -1, -1, -1, -1 }, // sequence 0
    { 2, 3, 4, 5 },     // sequence 1
    { 4, 5, 2, 3 },     // sequence 2
    { -1, -1, -1, -1 }  // sequence 3
};

template<bool isClassic> ImageId WoodenRCGetTrackColour(const PaintSession& session)
{
    if (isClassic)
        return session.TrackColours[SCHEME_TRACK];
    else
        return session.TrackColours[SCHEME_TRACK].IsRemap()
            ? session.TrackColours[SCHEME_TRACK]
            : session.TrackColours[SCHEME_TRACK].WithPrimary(session.TrackColours[SCHEME_SUPPORTS].GetPrimary());
}

ImageId WoodenRCGetRailsColour(PaintSession& session);

template<bool isClassic>
PaintStruct* WoodenRCTrackPaint(
    PaintSession& session, uint8_t direction, ImageIndex imageIdTrack, ImageIndex imageIdRails, const CoordsXYZ& offset,
    const BoundBoxXYZ& boundBox)
{
    ImageId imageId = WoodenRCGetTrackColour<isClassic>(session).WithIndex(imageIdTrack);
    ImageId railsImageId = WoodenRCGetRailsColour(session).WithIndex(imageIdRails);

    PaintAddImageAsParentRotated(session, direction, imageId, offset, boundBox);
    return PaintAddImageAsChildRotated(session, direction, railsImageId, offset, boundBox);
}

template<bool isClassic> void WoodenRCTrackPaintBb(PaintSession& session, const SpriteBoundBox2* bb, int16_t height)
{
    if (bb->ImageIdA == 0)
        return;

    ImageId imageId = WoodenRCGetTrackColour<isClassic>(session).WithIndex(bb->ImageIdA);
    PaintAddImageAsParent(
        session, imageId, { bb->offset.x, bb->offset.y, height + bb->offset.z },
        { { bb->BoundBox.offset.x, bb->BoundBox.offset.y, height + bb->BoundBox.offset.z }, bb->BoundBox.length });
    if (bb->ImageIdB != 0)
    {
        ImageId railsImageId = WoodenRCGetRailsColour(session).WithIndex(bb->ImageIdB);
        PaintAddImageAsChild(
            session, railsImageId, { bb->offset.x, bb->offset.y, height + bb->offset.z },
            { { bb->BoundBox.offset, height + bb->BoundBox.offset.z }, bb->BoundBox.length });
    }
}

TRACK_PAINT_FUNCTION GetTrackPaintFunctionClassicWoodenRCFallback(int32_t trackType);
