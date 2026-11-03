#pragma once
#include "PracticeMode.h"

std::unique_ptr<PracticeMode> makeTankMode(graphics::Application& app, bool threeViews);
