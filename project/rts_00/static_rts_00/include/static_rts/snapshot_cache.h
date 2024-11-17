#pragma once

class Snapshot;
class SnapshotCacheImplementation;

namespace StaticRts
{
    class SnapshotCache
    {
    public:
        SnapshotCache(
            /// how many frames between keeping snapshots
            const int32_t in_frames_per_snapshot_low = 4,
            const int32_t in_low_threashold = 128,
            const int32_t in_frames_per_snapshot_high = 128,
            const int32_t in_high_threashold = 512
            );
        ~SnapshotCache();

        const int32_t CalculateFrameGap(
            const int32_t in_current_frame_id,
            const int32_t in_query_point
            );

        /// in multiplayer games, you can get late input. in campaign, you can rewind time?
        /// update the state of the cache, removing snapshots that fail validate
        void UpdateVaidate(
            const std::function<const bool(const int, const int)>& in_validate_snapshot
            );

        /// update the state of the cache, removing snapshots that are too frequent
        void UpdateSnapshotRetention(
            const int32_t in_current_frame_id
            );

        void AddSnapshot(
            const std::shared_ptr<Snapshot>& in_snapshot
            );

        const std::shared_ptr<Snapshot> SeekSnapshot(const int32_t in_current_frame_id) const;
        const std::shared_ptr<Snapshot> GetLastSnapshot() const;

    private:
        std::unique_ptr<SnapshotCacheImplementation> _implementation;

    };
}