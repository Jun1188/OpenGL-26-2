#pragma once
#include <string_view>
#include <vector>

namespace assembly {
struct GuideSite {
    const char* file;
    const char* marker;
    const char* purpose;
};

struct GuideRecipe {
    const char* topic;
    const char* goal;
    const char* before;
    const char* after;
    const char* reason;
    const char* debug;
    const char* expected;
    const char* document;
    std::vector<GuideSite> sites;
};

const std::vector<GuideRecipe>& guideRecipes();
const GuideRecipe* findGuideRecipe(std::string_view topic);
} // namespace assembly
