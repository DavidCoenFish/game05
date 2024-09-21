#pragma once

class CommandEngine;
class InputDatabaseImplementation;
class Snapshot;
class SnapshotCache;

namespace static_rts_00
{
    /// snapshot has frame_id, and change_id
    /// snapshot cache is collection of snapshots valid for given change id
    /// stream to file as game runs?
    /// what about loading an input database into spectator mode, just not call add input
    class InputDatabase
    {
    public:
        template <typename IN_TYPE>
        InputDatabase(
            const IN_TYPE& in_data
            )
            : InputDatabase(&in_data, sizeof(IN_TYPE))
        {
            // Nop
        }

        InputDatabase(
            const uint8_t* const in_snapshot_init_data,
            const int32_t in_snapshot_init_data_size
            // const std::shared_ptr<InputDatabaseStream>& in_stream
            );
        ~InputDatabase();

        void Advance(
            SnapshotCache& in_out_snapshot_cache,
            const int32_t in_frame_id
            );

        std::shared_ptr<Snapshot> MakeLattestSnapshot(
            const CommandEngine& in_command_engine, 
            SnapshotCache& in_out_snapshot_cache
            );

        std::shared_ptr<Snapshot> SeekSnapshot(
            const CommandEngine& in_command_engine, 
            SnapshotCache& in_out_snapshot_cache,
            const int32_t in_frame_id
            );

        /// for multiplayer, the in_frame_id may be under the most curent frame id, which would invalidate snapshots more recent
        template <typename IN_TYPE>
        void AddInput(
            const int32_t in_frame_id,
            const uint8_t in_player_id,
            const uint8_t in_operation_id,
            const IN_TYPE& in_data
            )
        {
            AddInput(
                in_frame_id,
                in_player_id,
                in_operation_id,
                &in_data,
                sizeof(IN_TYPE)
                );
        }

        //void SaveToFile(const std::filesystem::path& in_path);
        //void LoadFromFIle(const std::filesystem::path& in_path);

    private:
        void AddInput(
            const int32_t in_frame_id,
            const uint8_t in_player_id,
            const uint8_t in_operation_id,
            const uint8_t* const in_data,
            const int32_t in_data_size
            );

    private:
        std::unique_ptr<InputDatabaseImplementation> _implementation;

    };
}