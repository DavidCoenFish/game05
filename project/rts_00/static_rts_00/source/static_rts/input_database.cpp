#include "static_rts_00/static_rts_pch.h"
#include "static_rts_00/input_database.h"

#include "common/util/vector_helper.h"

class InputDatabaseImplementation
{
public:
    InputDatabaseImplementation(
        const uint8_t* const in_snapshot_init_data,
        const int32_t in_snapshot_init_data_size
        )
    {
        VectorHelper::AppendData(_data, in_snapshot_init_data, in_snapshot_init_data_size);
    }

    std::shared_ptr<Snapshot> MakeLattestSnapshot(
        const CommandEngine& in_command_engine, 
        SnapshotCache& in_out_snapshot_cache
        )
    {
        return nullptr;
    }

    std::shared_ptr<Snapshot> SeekSnapshot(
        const CommandEngine& in_command_engine, 
        SnapshotCache& in_out_snapshot_cache,
        const int32_t in_frame_id
        )
    {
        return nullptr;
    }

    void AddInput(
        const int32_t in_frame_id,
        const uint8_t in_player_id,
        const uint8_t in_operation_id,
        const uint8_t* const in_data,
        const int32_t in_data_size
        )
    {
        return;
    }

private:
    std::vector<uint8_t> _data;

    /// Note: multiple players can have input on the same frame, need to sort, and for fairness, could cycle the player turn with frame id
    struct FrameData
    {
        /// allow for sparse data? else have a FrameData for each frame....
        int32_t _frame_id; 
        /// A snapshot can be invalidated but input being added/ changed after the snapshot was generated
        int32_t _change_id;
        /// the offset in the data that has the first input 
        int32_t _data_offset;
    };

};


InputDatabase::InputDatabase(
    const uint8_t* const in_snapshot_init_data,
    const int32_t in_snapshot_init_data_size
    // const std::shared_ptr<InputDatabaseStream>& in_stream
    )
{
    _implementation = std::make_unique<InputDatabaseImplementation>(
        in_snapshot_init_data,
        in_snapshot_init_data_size
        );
}

InputDatabase::~InputDatabase()
{
    // Nop
}

std::shared_ptr<Snapshot> InputDatabase::MakeLattestSnapshot(
    const CommandEngine& in_command_engine, 
    SnapshotCache& in_out_snapshot_cache
    )
{
    return _implementation->MakeLattestSnapshot(
        in_command_engine,
        in_out_snapshot_cache
        );
}

std::shared_ptr<Snapshot> InputDatabase::SeekSnapshot(
    const CommandEngine& in_command_engine, 
    SnapshotCache& in_out_snapshot_cache,
    const int32_t in_frame_id
    )
{
    return _implementation->SeekSnapshot(
        in_command_engine,
        in_out_snapshot_cache,
        in_frame_id
        );
}

void InputDatabase::AddInput(
    const int32_t in_frame_id,
    const uint8_t in_player_id,
    const uint8_t in_operation_id,
    const uint8_t* const in_data,
    const int32_t in_data_size
    )
{
    return _implementation->AddInput(
        in_frame_id,
        in_player_id,
        in_operation_id,
        in_data,
        in_data_size
        );
}


