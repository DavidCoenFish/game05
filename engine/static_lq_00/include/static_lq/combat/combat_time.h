#pragma once

namespace StaticLq
{
class CombatTime
{
public:
	explicit CombatTime(
		const int32_t in_turn = 0,
		const int32_t in_segment = 0
		);
	~CombatTime();

	void AdvanceTime();
	const bool IsStartOfTurn() const;

	const int32_t GetTurn() const { return _turn; }
	const int32_t GetSegment() const { return _segment; }

private:
	int32_t _turn = 0;
	int32_t _segment = 0;

};
}
