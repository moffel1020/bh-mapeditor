#include "selectimage.h"
#include "logger.h"
#include "nfd.hpp"
#include <string>

std::optional<std::string> selectImageFile() {
    NFD::Init();
    nfdchar_t* out;
    nfdfilteritem_t filter = {"Image", "png,jpg,jpeg"};
    auto result = NFD::OpenDialog(out, &filter, 1);
    if (result == NFD_OKAY) {
        std::string res = out;
        NFD::FreePath(out);
        NFD::Quit();
        return res;
    }

    Logger::error("failed selecting image");
    NFD::Quit();
    return {};
}
