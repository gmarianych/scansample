#pragma once
#include <vector>
#include <map>
#include <mutex>
#include <memory>
#include <future>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <common/fs.h>

namespace scan
{

typedef std::chrono::steady_clock CmnClock;
typedef CmnClock::time_point CmnTime;
typedef CmnClock::duration CmnDuration;

struct LimiterStats
{
    CmnDuration totalDelay{};
    CmnDuration totalTime{};
    uint64_t totalIoCount{};
    uint64_t totalBytes{};
    uint64_t bytesPerSec{};
};

class Limiter
{
    const uint64_t fileIoBytesPerPeriod_;
    const CmnDuration period_;
    const CmnTime startTime_;

    uint64_t fileIoCount_{};
    uint64_t fileIoBytesTotal_{};

    CmnDuration totalDelay_{};
public:
    Limiter(uint64_t fileIoBytesPerPeriod, CmnDuration period);
    void OnFileIo(uint64_t bytesRead);
    LimiterStats GetStats();
};

class LimitedFile : public cmn::IFile
{
    std::shared_ptr<IFile> next_;
    std::shared_ptr<Limiter> limiter_;
public:
    LimitedFile(const std::shared_ptr<IFile>& next, const std::shared_ptr<Limiter>& limiter)
        :next_(next), limiter_(limiter)
    {}
    bool read(cmn::Binary& bin)
    {
        if (!next_->read(bin))
            return false;
        limiter_->OnFileIo(bin.size());
        return true;
    }
};

class Scanner
{
    std::vector<std::wstring> exts_;// {L".txt", L".vms", L".iso", L".sav", L".vdi", L".7z"};
public:
    Scanner();
    void scan(const fs::path& root, uint64_t bytesPerSecLimit);
};

}