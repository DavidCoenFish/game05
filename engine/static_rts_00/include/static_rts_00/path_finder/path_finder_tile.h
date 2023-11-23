#pragma once


namespace static_rts_00
{
    class Map;

    class Tile
    {
    public:
        typedef std::function<void(const VectorShort2& in_location, const uint8_t in_data)> TMapChangeCallback;
        typedef std::function<void(const uint8_t in_tile_id)> TTileChangeCallback;

        enum class TVar
        {
            WIDTH = 7,
            HEIGHT = 7,
            COUNT_WITH_OVERLAP = (WIDTH + 1) * (HEIGHT * 1)
        };

        explicit Tile(
            const VectorShort2& in_location_origin,
            const uint8_t in_traversable_mask
            );
        ~Tile();

        const TMapChangeCallback MakeMapChangeCallback();

        void AddChangeCallback(const TTileChangeCallback& in_callback);

        void VisitBoundaryRegions(const std::vector<void(const VectorShort& in_location, const uint8_t in_region_id)>& in_visitor) const;
        void VisitBoundaryTouchingLocation(const std::vector<void(const VectorShort& in_location)>& in_visitor, const VectorShort& in_location) const;

        const bool GetDistance(
            uint8_t& out_distance,
            const VectorShort& in_location_a,
            const VectorShort& in_location_b
            );

        const bool VisitPath(
            const std::vector<void(const VectorShort& in_location)>& in_visitor,
            const VectorShort& in_location_a,
            const VectorShort& in_location_b
            );

    private:
        VectorShort2 _location_origin;
        int32_t _change_id;
        uint8_t _tile_id;
        std::bitset<COUNT_WITH_OVERLAP> _traversable;
        uint8_t _traversable_mask;

        std::array<
            std::pair<int32_t, std::array<uint8_t, COUNT_WITH_OVERLAP>>, 
            COUNT_WITH_OVERLAP> _change_id_distance_map_array;

        std::array<uint8_t, COUNT_WITH_OVERLAP> _region_map;

        std::vector<TTileChangeCallback> _tile_change_callback_array;

    };
}
