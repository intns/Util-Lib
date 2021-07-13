#ifndef _MISC_HPP
#define _MISC_HPP

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string>
#include <types.hpp>
#include <vector>

namespace util {

static inline bool IsAnagramOf(std::string a, std::string b)
{
    std::sort(a.begin(), a.end(), std::less<char>());
    std::sort(b.begin(), b.end(), std::less<char>());
    return a == b;
}

static std::optional<std::vector<std::string>> IsAnagramOfList(std::string anagram, std::vector<std::string>& possibleCandidates)
{
    std::vector<std::string> foundAnagrams = {};

    for (std::string& candidate : possibleCandidates) {
        if (IsAnagramOf(anagram, candidate)) {
            foundAnagrams.push_back(candidate);
        }
    }

    if (foundAnagrams.size() > 0) {
        return foundAnagrams;
    }

    return std::nullopt;
}

static bool DumpFile(const std::string& originalFile, bool outFileUppercase = false)
{
    std::ifstream file(originalFile, std::ios_base::binary);
    if (!file.is_open()) {
        std::printf("File was unable to be opened\n");
        return false;
    }

    std::size_t size = static_cast<std::size_t>(std::filesystem::file_size(originalFile));
    std::unique_ptr<u8> filecontents(new u8[size]);
    file.read((char*)filecontents.get(), size);

    std::string filename = originalFile;
    std::string editedFileName = filename + ".hpp";

    if (outFileUppercase) {
        // Make filename UPPERCASE
        std::transform(filename.begin(), filename.end(), filename.begin(), ::toupper);
    }

    // Replace non-alphabet characters with _
    std::replace_if(
        filename.begin(), filename.end(), [](char c) { return !isalpha(c); }, '_');

    std::ostringstream contents;
    contents << "#include <types.hpp>\n\n";
    contents << "// Generated by UTIL-LIB\n";
    contents << "constexpr u8 " << filename << "[] = {\n";

    for (std::uintmax_t i = 0; i < size; i++) {
        if (i == 0) {
            contents << "\t";
        } else if (!(i % 0xC)) {
            contents << "\n\t";
        }

        contents << "0x" << std::hex << (static_cast<u32>(*(filecontents.get() + i)))
                 << ((i == static_cast<unsigned long long>(size) - 1) ? "\n" : ", ");
    }
    contents << "";
    contents << "};\n";
    contents << "constexpr std::uintmax_t " << filename << "_SIZE = sizeof(" << filename << ");\n";

    std::ofstream out(editedFileName);
    out.write(contents.str().c_str(), contents.str().size());
    out.close();

    return true;
}

}

#endif