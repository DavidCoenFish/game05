#pragma once

/// is it wierd if we design for a map that may resize, new players added. 
/// is moving around so much data per frame may be weird? even the map? (512x512 -> 64kb)
class Snapshot
{
public:
    /// if buildings can fly, how is that expressed? pos -> float. just use Unit as building, drone, army unit
    struct TUnit
    {
        int32_t _unit_id;
        float _x;
        float _y;

        int8_t _rotation;
        int8_t _height; // negative for burrowed? 0 for ground, 127 for flight? 
        int8_t _velocity;
        int8_t _max_velocity;

        uint8_t _current_health;
        uint8_t _max_health;
        int8_t _player_id;
        int8_t _type; // just the size? have an ablity array for invokable ablities

        int8_t _current_action; // gather, move, patroll, attack, attack move, hold pos, death, garrisoned
        int8_t _build_queue; // count of things waiting to build
        int8_t _max_acceleration;
        /// don't need the count of units garrisoned? move to an array
        int8_t _pad;

        int8_t _ablity[4];
        int8_t _ablity_data[4]; // shield, cooldown, gathered food, death animation timer?
    };
    /// dead units -> food, but how to play death animation, via current action? when unit is playing death/ changing to food, player id changes to invalid?
    struct TFood
    {
        int16_t _x;
        int16_t _y;
        int32_t _amount;
    };
    /// is this just the next step of the path finding path? or will the snapshot hold an array of all move waypoints?
    /// do want snapshot to be the complete info for the game state, in that selecting a uint may show the waypoint path it is trying to take
    struct TMove
    {
        /// unit index refers to the _unit_array index
        int32_t _unit_array_index;
        int32_t _waypoint_index;
        float _destination_x;
        float _destination_y;
    };
    /// currently using an ablity that is targetting a unit
    struct TUnitTarget
    {
        int32_t _unit_array_index;
        int8_t _player_id;
        int32_t _player_unit_index;
    };
    /// currently using an ablity that is targetting a location
    struct TLocationTarget
    {
        int32_t _unit_array_index;
        float _x;
        float _y;
    };
    struct TCamera
    {
        float _x;
        float _y;
        float _orientation;
        float _zoom;
        int32_t _cut_id; // does the camera interpolate, or jump cut
        int8_t _player_id;
    };
    struct TControlGroups
    {
        int8_t _player_id;
        int8_t _control_group;
        int32_t _control_group_index_offset;
        int32_t _control_group_index_size;
    };
    struct TGarrisoned
    {
        int32_t _host_unit_array_index;
        int32_t _passenger_unit_array_index;
    };


    Snapshot(
        const int32_t in_frame_id,
        const int32_t in_change_id,
        const int32_t in_map_width,
        const int32_t in_map_height,
        const std::vector<TUnit>& in_unit_array,
        const std::vector<TFood>& in_food_array,
        const std::vector<TMove>& in_move_array,
        const std::vector<TUnitTarget>& in_unit_target_array,
        const std::vector<TUnitTarget>& in_location_target_array,
        const std::vector<TCamera>& in_camera_array,
        const std::vector<TControlGroups>& in_control_groups_array,
        const std::vector<int32_t>& in_control_groups_unit_id_array,
        const std::vector<uint8_t>& in_map
        );
    ~Snapshot();

    const int32_t GetFrameId() const { return _frame_id; }
    const int32_t GetChangeId() const { return _change_id; }
    const std::vector<TUnit>& GetUnitArrayRef() const { return _unit_array; }
    const std::vector<TFood>& GetFoodArrayRef() const { return _food_array; }
    const std::vector<TMove>& GetMoveArrayRef() const { return _move_array; }
    const std::vector<TUnitTarget>& GetUnitTargetArrayRef() const { return _unit_target_array; }
    const std::vector<TUnitTarget>& GetLocationTargetArrayRef() const { return _location_target_array; }
    const std::vector<TCamera>& GetCameraArrayRef() const { return _camera_array; }
    const std::vector<TControlGroups>& GetControlGroupsArrayRef() const { return _control_groups_array; }
    const std::vector<int32_t>& GetControlGroupsIndexArrayRef() const { return _control_groups_index_array; }
    const std::vector<uint8_t>& GetMapRef() const { return _map; }

private:
    int32_t _frame_id;
    int32_t _change_id;
    int32_t _map_width;
    int32_t _map_height;

    // building, drones and army units
    std::vector<TUnit> _unit_array;

    std::vector<TFood> _food_array;

    std::vector<TMove> _move_array;

    std::vector<TUnitTarget> _unit_target_array;

    std::vector<TUnitTarget> _location_target_array;

    std::vector<TCamera> _camera_array;

    std::vector<TControlGroups> _control_groups_array;
    std::vector<int32_t> _control_groups_index_array;

    std::vector<uint8_t> _map;

    /// if units roll on the ground, is that expressed in the snapshot, or is that render data that is derrived off the snapshot?
    //struct TTransform
    //{
    //    int32_t _unit_index;
    //    float _at[3];
    //    float _up[3];
    //};
    //std::vector<TTransform> _transform_array;

    /*
    each player has a visiblity map, array of ints, each cell having a count of players with visiblity
    but this could be derrived off the snapshot
    */

};
