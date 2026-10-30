#include "shendk/utils/path_hash.h"
#include "shendk/utils/murmurhash2.h"

#include <cctype>

namespace shendk {
namespace {

bool Slash(char value) { return value == '/' || value == '\\'; }

std::size_t HashedSuffix(std::string_view value) {
    std::int32_t digits = 0, groups = 0;
    std::uint32_t scanned = 0;
    for (std::size_t cursor = value.size(); cursor-- > 0;) {
        const char ch = static_cast<char>(std::tolower(static_cast<unsigned char>(value[cursor])));
        if (ch == '.') {
            if (digits == 8) {
                digits = 0;
                if (++groups == 2 && scanned + 1 >= 0x12)
                    return cursor;
            }
        } else if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f')) {
            ++digits;
        } else {
            digits = 0;
            groups = 0;
        }
        ++scanned;
    }
    return std::string_view::npos;
}

} // namespace

std::string CleanFilePath(std::string_view path) {
    std::string result;
    result.reserve(path.size());
    for (std::size_t index = 0; index < path.size();) {
        if (Slash(path[index])) {
            while (index + 1 < path.size() && Slash(path[index + 1]))
                ++index;
        }
        if (path[index] == '.' && index && index + 1 < path.size() && Slash(path[index - 1]) && Slash(path[index + 1])) {
            index += 2;
            if (index == path.size())
                break;
        }
        char value = path[index++];
        if (value == '\\')
            value = '/';
        result.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(value))));
    }
    return result;
}

std::uint32_t GeneratePathHash(std::string_view path) {
    constexpr std::uint64_t punctuation = 0x2000000000ffdULL;
    std::string filtered;
    filtered.reserve(path.size());
    for (const char source : path) {
        const int character = std::tolower(static_cast<unsigned char>(source));
        const int bit = character - 46;
        if ((character >= 'a' && character <= 'z') || (bit >= 0 && bit <= 49 && ((punctuation >> bit) & 1U)))
            filtered.push_back(static_cast<char>(character));
    }
    const std::uint64_t originalLength = filtered.size();
    const std::size_t suffix = HashedSuffix(filtered);
    if (suffix != std::string_view::npos)
        filtered.resize(suffix + 9);
    const std::uint64_t hashLength = filtered.size();
    const std::uint32_t hash = HashData(filtered.data(), static_cast<std::uint32_t>(hashLength));
    return 0x1003fU * hash + 0x2001fU * static_cast<std::uint32_t>((originalLength * hashLength) % 0xffffffffULL);
}

std::uint64_t GetPathHash(std::string_view path) {
    std::string normalized = CleanFilePath(path);
    const std::size_t suffix = HashedSuffix(normalized);
    std::uint32_t secondary = 0;
    if (suffix != std::string_view::npos) {
        for (std::size_t index = suffix + 1; index < suffix + 9; ++index) {
            const char ch = normalized[index];
            secondary = (secondary << 4U) | static_cast<std::uint32_t>(ch <= '9' ? ch - '0' : ch - 'a' + 10);
        }
        normalized.replace(suffix + 10, 8, "00000000");
    }
    return (static_cast<std::uint64_t>(GeneratePathHash(normalized)) << 32U) | secondary;
}

} // namespace shendk
