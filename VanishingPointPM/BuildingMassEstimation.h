#pragma once

#include "Grammar.h"
#include "VanishingPoint.h"
#include <vector>
#include "Camera.h"

namespace bme {

	void estimate(int screen_width, int screen_height, const Camera& camera, cga::Grammar& grammar, const std::vector<vp::VanishingLine>& silhouette, std::vector<float>& pm_params);

}