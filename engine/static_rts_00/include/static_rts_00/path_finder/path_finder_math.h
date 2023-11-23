#pragma once


namespace static_rts_00
{
    namespace PathFinderMath
    {
        void VisitAdjacentLocations(
            const VectorShort2& in_location,
            const std::function<void(const VectorShort2& in_location)>& in_visitor
            );
        
        /// return -1 for out of bounds?
        const int32_t MakeOffset(
            const int32_t in_width,
            const int32_t in_height,
            const VectorShort2& in_origin,
            const VectorShort2& in_location
            );

        const VectorShort2 ConvertFloatToLocation(
            const VectorFloat2& in_coord
            );

        const VectorFloat2 ConvertLocationToFloat(
            const VectorShort2& in_location
            );

        /// generate an array of tiles for the given map size
        void VisitTiles(
            const std::function<void(const int8_t in_tile_id, const VectorShort2& in_origin)>& in_visitor,
            const int32_t in_map_width,
            const int32_t in_map_height
            );

        // get tiles for location
        void VisitLocationTile(
            const std::function<void(const int8_t in_tile_id)>& in_visitor,
            const int32_t in_map_width,
            const int32_t in_map_height,
            const VectorShort2& in_location
            );

    }
}
