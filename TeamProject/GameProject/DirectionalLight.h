#pragma once

#include "Light.h"

namespace SSB
{
    class DirectionalLight : public Light
    {
    private:
		HRESULT CreateLightingShader() override;
        LightLocationData UpdateLightData() override;
    };
}
