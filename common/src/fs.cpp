#include <common/fs.h>

#include "common/common.h"

namespace cmn
{
void TraverseDirectory(const fs::path& root, std::function<void(const fs::path&)> delegate)
{
    if (!fs::exists(root))
        return;

    std::error_code ec;
    fs::recursive_directory_iterator i(root, fs::directory_options::skip_permission_denied, ec);

    for(; i != fs::recursive_directory_iterator{}; ++i )
    {
        if (ec)
        {
            // log error
            continue;
        }

        bool isFile = fs::is_regular_file(i->path(), ec);
        if (!isFile)
            continue;

        if (ec)
        {
            // log error
            continue;
        }

        try
        {
            delegate(i->path());
        }
        catch(const std::exception& e)
        {
            // log error
        }
    }
}

void TraverseDirectory(const fs::path& root,
    const std::vector<std::wstring>& exts,
    std::function<void(const fs::path&)> delegate)
{
    cmn::TraverseDirectory(root, [&](const fs::path& p)
        {
            if (!p.has_extension())
                return;
            if (exts.empty())
                return delegate(p);
            for (auto& e : exts)
                if (p.extension().wstring() == e)
                    return delegate(p);
        });
}

File::File(const fs::path& path)
    : stream_(path)
{
    if (!stream_ || !stream_.good())
        throw std::runtime_error("Failed to open file: " + path.string());
}

bool File::read(Binary& bin)
{
    if (bin.empty())
        return false;

    if (!stream_)
        return false;

    stream_.read(reinterpret_cast<char*>(bin.data()), bin.size());
    const auto bytesRead = stream_.gcount();

    if (bytesRead <= 0)
        return false;
    bin.resize(bytesRead);
    return true;
}

void ReadFile(IFile& file, Binary& bin, OnBufferCb onBuffer)
{
    while (file.read(bin))
        onBuffer(bin);
}

std::string CalcFileSha256(IFile& file)
{
    Binary bin(1024);
    Sha256Hash hash;
    ReadFile(file, bin, [&](const Binary& bin)
        {
            hash.Update(const_cast<uint8_t*>(bin.data()), bin.size());
        });
    return hash.GetHash();
}
}
