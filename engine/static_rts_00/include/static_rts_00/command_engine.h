#pragma once

class CommandEngineImplementation;
class InputDatabase;
class Snapshot;

namespace static_rts_00
{
    class CommandEngine
    {
    public:
        CommandEngine(
            const int32_t in_version = 0
            );
        ~CommandEngine();

        static std::unique_ptr<InputDatabase> MakeInputDatabase(
            const int8_t in_player_count = 1,
            const int32_t in_map_seed = 0,
            const int16_t in_map_width = 128,
            const int16_t in_map_height = 128
            );

        static void ApplyCommandSelect(
            InputDatabase& in_out_input_database,
            const uint8_t in_player_id,
            const int32_t in_unit_id
            );

        static void ApplyCommandMoveSelected(
            InputDatabase& in_out_input_database,
            const uint8_t in_player_id,
            const float in_x,
            const float in_y
            );

        static std::shared_ptr<Snapshot> BuildZeroSnapshot(
            const uint8_t* const in_data,
            const int32_t in_data_size
            );

        /// construct a new snapshot based on the current snapshot and using the commands from the input lambda
        std::shared_ptr<Snapshot> BuildSnapshot(
            const Snapshot& in_base_snapshot,
            const int32_t in_target_frame_id,
            const int32_t in_target_change_id,
            /// expect data source callback to be primed with correct offset to start from, returns true if there are still more commands before target_frame)id
            const std::function<bool(
                int32_t& out_frame_id,
                //int32_t& out_change_id,
                uint8_t& out_player_id,
                uint8_t& out_operation_id,
                uint8_t*& out_data,
                int32_t& out_data_size
                )>& in_data_source_callback
            );

    private:
        std::unique_ptr<CommandEngineImplementation> _implementation;

    };
}