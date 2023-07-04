#pragma once
#include <filesystem>
#include <functional>
#include <fstream>

namespace fs = std::filesystem;
namespace cmn
{
void TraverseDirectory(const fs::path& root, std::function<void(const fs::path&)> delegate);
void TraverseDirectory(const fs::path& root,
                       const std::vector<std::wstring>& exts,
                       std::function<void(const fs::path&)> delegate);

using Binary = std::vector<uint8_t>;

class IFile
{
public:
    virtual ~IFile() = default;
    virtual bool read(Binary& bin) = 0;
};

class File : public IFile
{
    std::ifstream stream_;
public:
    File(const fs::path& path);
    bool read(Binary& bin);
};

using OnBufferCb = std::function<void(const Binary& bin)>;
void ReadFile(IFile& file, Binary& bin, OnBufferCb onBuffer);

using OnProgressCb = std::function<void(uint64_t cur, uint64_t total)>;
std::string CalcFileSha256(IFile& file);
}
