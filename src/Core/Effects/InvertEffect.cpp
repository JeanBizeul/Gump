#include "InvertEffect.hpp"

using namespace Gump;

InvertEffect::InvertEffect()
    : Effect("Invert Colors", "shaders/effects/invert.frag")
{
    // Call setupParameters after base constructor
    setupParameters();
}

void InvertEffect::setupParameters()
{
    // Invert effect has a simple intensity parameter
    EffectParameter intensity("Intensity", EffectParameter::Type::Float);
    intensity.floatValue = 1.0f;
    intensity.minValue = 0.0f;
    intensity.maxValue = 1.0f;
    _parameters.push_back(intensity);
}

void InvertEffect::setShaderUniforms()
{
    // Set intensity uniform
    if (!_parameters.empty()) {
        _shader->set("uIntensity", _parameters[0].floatValue);
    }
}
