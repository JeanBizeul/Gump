#pragma once

#include "../Effect.hpp"

namespace Gump {

class BlurEffect : public Effect {
public:
    BlurEffect();

protected:
    void setupParameters() override;
    void setShaderUniforms() override;
};

} // namespace Gump
