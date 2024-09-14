#include "static_lq_00/static_lq_pch.h"
#include "static_lq_00/random_sequence.h"

/*
	this.m_seed = in_seed % 233280;

 * return [0 ... 1> not inclusive of 1.0, just approaches 0.99999
 * @return {!number}
 .RandomSequence.prototype.Random = function() {
	this.m_seed = (this.m_seed * 9301 + 49297) % 233280;
	var result = this.m_seed / 233280;
	return result;
}


#include <random>

typedef std::mt19937 rng_type;
std::uniform_int_distribution<rng_type::result_type> udist(0, 7);
 */

static_lq_00::RandomSequence::RandomSequence(
    const uint32_t in_seed
    )
    : m_generator(in_seed)
    , m_seed(in_seed)
    , m_trace(0)
    , m_itteration_count(0)
{
    // nop
}

static_lq_00::RandomSequence::~RandomSequence()
{
    // nop
}

const uint32_t static_lq_00::RandomSequence::GenerateSeed()
{
    std::random_device random_device;
    return random_device();
}

const float static_lq_00::RandomSequence::GenerateFloat()
{
    Itterate();
    const float result = static_cast<float>(m_trace) / static_cast<float>(m_generator.max());
    return result;
}

const int32_t static_lq_00::RandomSequence::GenerateDice(const int32_t in_sides)
{
    Itterate();
    const int32_t result = (m_trace % in_sides) + 1;
    return result;
}

void static_lq_00::RandomSequence::Itterate()
{
	m_trace = m_generator();
    m_itteration_count += 1;
    return;
}

