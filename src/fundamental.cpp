#include <format>
#include "fundamental.h"

std::string FundamentalList::toString()
{
    std::string result = "";
    for(FundamentalRef item : body) {
        result = std::format("{} {}", result, item->toString());
    }
    return std::format("[ {} ]", result);
}