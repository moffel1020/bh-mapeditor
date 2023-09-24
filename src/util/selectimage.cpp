#include "selectimage.h"
#include "logger.h"
#include "nfd.hpp"
#include <string>

std::string selectImageFile() {
    NFD::Init();
    nfdchar_t* out;
    nfdfilteritem_t filter = {"Image", "png,jpg,jpeg"};
    auto result = NFD::OpenDialog(out, &filter, 1);
    if (result == NFD_OKAY) {
        NFD::FreePath(out);
        return std::string(out);
    }

    Logger::error("failed selecting image");
    return std::string("");
}