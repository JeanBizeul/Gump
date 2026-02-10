#include "ColorFilterEffect.hpp"

using namespace Gump;

ColorFilterEffect::ColorFilterEffect()
    : Effect("Color Filter", "shaders/effects/color_filter.frag")
{
    setupParameters();
}

void ColorFilterEffect::setupParameters()
{
    // Filter color parameter
    EffectParameter filterColor("Filter Color", EffectParameter::Type::Color);
    filterColor.colorValue[0] = 1.0f; // R
    filterColor.colorValue[1] = 0.5f; // G
    filterColor.colorValue[2] = 0.0f; // B
    filterColor.colorValue[3] = 1.0f; // A
    _parameters.push_back(filterColor);
    
    // Intensity parameter
    EffectParameter intensity("Intensity", EffectParameter::Type::Float);
    intensity.floatValue = 0.5f;
    intensity.minValue = 0.0f;
    intensity.maxValue = 1.0f;
    _parameters.push_back(intensity);
    
    // Blend mode parameter
    EffectParameter blendMode("Blend Mode", EffectParameter::Type::Int);
    blendMode.intValue = 0; // 0=Multiply, 1=Screen, 2=Overlay
    blendMode.minValue = 0;
    blendMode.maxValue = 2;
    _parameters.push_back(blendMode);
}

void ColorFilterEffect::setShaderUniforms()
{
    if (_parameters.size() >= 3) {
        _shader->set("uFilterColor", glm::vec4(
            _parameters[0].colorValue[0],
            _parameters[0].colorValue[1],
            _parameters[0].colorValue[2],
            _parameters[0].colorValue[3]
        ));
        _shader->set("uIntensity", _parameters[1].floatValue);
        _shader->set("uBlendMode", _parameters[2].intValue);
    }
}
