#include "BlurEffect.hpp"

using namespace Gump;

BlurEffect::BlurEffect()
    : Effect("Blur", "shaders/effects/blur.frag")
{
    setupParameters();
}

void BlurEffect::setupParameters()
{
    // Blur radius parameter
    EffectParameter radius("Radius", EffectParameter::Type::Float);
    radius.floatValue = 5.0f;
    radius.minValue = 0.0f;
    radius.maxValue = 20.0f;
    _parameters.push_back(radius);
    
    // Blur quality parameter (number of samples)
    EffectParameter quality("Quality", EffectParameter::Type::Int);
    quality.intValue = 8;
    quality.minValue = 4;
    quality.maxValue = 16;
    _parameters.push_back(quality);
}

void BlurEffect::setShaderUniforms()
{
    if (_parameters.size() >= 2) {
        _shader->set("uRadius", _parameters[0].floatValue);
        _shader->set("uQuality", _parameters[1].intValue);
    }
}
