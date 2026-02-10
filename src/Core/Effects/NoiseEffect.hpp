#pragma once

#include "../Effect.hpp"

namespace Gump {

class NoiseEffect : public Effect {
public:
    NoiseEffect();

protected:
    void setupParameters() override;
    void setShaderUniforms() override;
};

} // namespace Gump
