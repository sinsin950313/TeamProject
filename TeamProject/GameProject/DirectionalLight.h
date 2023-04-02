#pragma once

#include "Light.h"

namespace SSB
{
    class DirectionalLight : public Light
    {
    private:
		Shader* GetLightingShader() override;
    };
}
