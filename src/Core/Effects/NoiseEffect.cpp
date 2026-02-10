#include "NoiseEffect.hpp"
#include <ctime>

using namespace Gump;

NoiseEffect::NoiseEffect()
    : Effect("Noise", "shaders/effects/noise.frag")
{
    setupParameters();
}

void NoiseEffect::setupParameters()
{
    // Noise amount parameter
    EffectParameter amount("Amount", EffectParameter::Type::Float);
    amount.floatValue = 0.5f;
    amount.minValue = 0.0f;
    amount.maxValue = 1.0f;
    _parameters.push_back(amount);
    
    // Noise type parameter
    EffectParameter noiseType("Noise Type", EffectParameter::Type::Int);
    noiseType.intValue = 0; // 0=White, 1=Colored
    noiseType.minValue = 0;
    noiseType.maxValue = 1;
    _parameters.push_back(noiseType);
    
    // Seed parameter for randomness
    EffectParameter seed("Seed", EffectParameter::Type::Float);
    seed.floatValue = static_cast<float>(std::time(nullptr) % 10000);
    seed.minValue = 0.0f;
    seed.maxValue = 10000.0f;
    _parameters.push_back(seed);
}

void NoiseEffect::setShaderUniforms()
{
    if (_parameters.size() >= 3) {
        _shader->set("uAmount", _parameters[0].floatValue);
        _shader->set("uNoiseType", _parameters[1].intValue);
        _shader->set("uSeed", _parameters[2].floatValue);
    }
}
