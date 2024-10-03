#include "static_lq/static_lq_pch.h"
#include "static_lq/random_sequence.h"

StaticLq::RandomSequence::RandomSequence(
    const uint32_t in_seed
    )
    : _generator(in_seed)
    , _seed(in_seed)
    , _trace(0)
    , _itteration_count(0)
{
    // nop
}

StaticLq::RandomSequence::~RandomSequence()
{
    // nop
}

const uint32_t StaticLq::RandomSequence::GenerateSeed()
{
    std::random_device random_device;
    return random_device();
}

const uint32_t StaticLq::RandomSequence::Generate()
{
    Itterate();
    return _trace;
}

const float StaticLq::RandomSequence::GenerateFloat()
{
    Itterate();
    const float result = static_cast<float>(_trace) / static_cast<float>(_generator.max());
    return result;
}

const int32_t StaticLq::RandomSequence::GenerateDice(const int32_t in_sides)
{
    Itterate();
    const int32_t result = (_trace % in_sides) + 1;
    return result;
}

void StaticLq::RandomSequence::Itterate()
{
	_trace = _generator();
    _itteration_count += 1;
    return;
}

