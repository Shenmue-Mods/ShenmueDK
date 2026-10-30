#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace shendk {

std::string CleanFilePath(std::string_view path);
std::uint32_t GeneratePathHash(std::string_view path);
std::uint64_t GetPathHash(std::string_view path);

} // namespace shendk
