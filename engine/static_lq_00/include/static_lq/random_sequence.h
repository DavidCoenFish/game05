#pragma once

class CommandEngineImplementation;
class InputDatabase;
class Snapshot;

namespace StaticLq
{
	class RandomSequence
	{
	public:
		RandomSequence(
			const uint32_t in_seed = 0
			);
		~RandomSequence();

		static const uint32_t GenerateSeed();

		/// generate a value
		const uint32_t Generate();
		/// warning, return [0 ... 1> not inclusive of 1.0, max value approaches 1.0, but may be similare to 0.9999999957
		const float GenerateFloat();
		/// warning, 1 based and inclusive of upper bounds, so in_sides == 4, result is of set [1,2,3,4]
		const int32_t GenerateDice(const int32_t in_sides);

		const uint32_t GetSeed() const { return _seed; }
		const int32_t GetItterationCount() const { return _itteration_count; }

	private:
		void Itterate();

	private:
		std::mt19937 _generator;

		/// the initial seed of the sequence
		const uint32_t _seed;
		/// index of the sequence, ie, count of nb generated
		uint32_t _itteration_count;
		/// current value
		uint32_t _trace = 0;

	};
}