#pragma once

#include "../Effect.hpp"

namespace Gump {

class InvertEffect : public Effect {
public:
    InvertEffect();

protected:
    void setupParameters() override;
    void setShaderUniforms() override;
};

} // namespace Gump
