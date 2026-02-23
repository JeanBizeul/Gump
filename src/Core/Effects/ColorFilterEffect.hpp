#pragma once

#include "../Effect.hpp"

namespace Gump {

class ColorFilterEffect : public Effect {
public:
    ColorFilterEffect();

protected:
    void setupParameters() override;
    void setShaderUniforms() override;
};

} // namespace Gump
