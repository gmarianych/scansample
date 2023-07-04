#include "Scanner/Scanner.h"

namespace scan
{

Limiter::Limiter(uint64_t fileIoBytesPerPeriod, CmnDuration period)
    : fileIoBytesPerPeriod_(fileIoBytesPerPeriod),
      period_(period),
      startTime_(CmnClock::now())
{
}

void Limiter::OnFileIo(uint64_t bytesRead)
{
    ++fileIoCount_;
    fileIoBytesTotal_ += bytesRead;
    auto fromStart = CmnClock::now() - startTime_;
    auto periodsCount = fromStart / period_ + 1;
    auto neededFileIoBytesTotal = fileIoBytesPerPeriod_ * periodsCount;

    if (fileIoBytesTotal_ > neededFileIoBytesTotal)
    {
        auto neededPeriodsCount = fileIoBytesTotal_ / fileIoBytesPerPeriod_;
        auto neededFromStart = neededPeriodsCount * period_;

        auto neededOpDuration = neededFromStart / fileIoCount_;
        auto curOpDuration = fromStart / fileIoCount_;

        if (neededOpDuration < curOpDuration)
            throw std::runtime_error("calculation is WRONG !!!");
        CmnDuration delay = neededOpDuration - curOpDuration;
        totalDelay_ += delay;
        std::this_thread::sleep_for(delay);
    }
}

LimiterStats Limiter::GetStats()
{
    LimiterStats stat{};
    stat.totalBytes = fileIoBytesTotal_;
    stat.totalDelay = totalDelay_;
    stat.totalTime = CmnClock::now() - startTime_;
    stat.totalIoCount = fileIoCount_;

    auto periodsCount = stat.totalTime / period_;
    auto bytesPerPeriod = periodsCount ? fileIoBytesTotal_ / periodsCount : fileIoBytesTotal_;
    stat.bytesPerSec = bytesPerPeriod * std::chrono::seconds(1) / period_;

    return stat;
}

Scanner::Scanner()
{
    
}

void Scanner::scan(const fs::path& root, uint64_t bytesPerSecLimit)
{
    uint64_t bytesPerMs = bytesPerSecLimit / 1000;
    auto limiter = std::make_shared<Limiter>(bytesPerMs, std::chrono::nanoseconds(1000000ull));
    cmn::TraverseDirectory(root, exts_, [&](const fs::path& p)
    {
            auto file = std::make_shared<cmn::File>(p);
            auto limitedFile = std::make_shared<LimitedFile>(file, limiter);
            auto hash = cmn::CalcFileSha256(*limitedFile);
            //std::cout << p << " : " << hash << "\n";
            std::cout << ".";
    });

    auto stat = limiter->GetStats();
    
    auto totalDelayMs = std::chrono::duration_cast<std::chrono::milliseconds>(stat.totalDelay);
    auto totalTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(stat.totalTime);
    std::cout << "\nLimit bytes per sec : " << bytesPerSecLimit;
    std::cout << "\nSpeed bytes per sec : " << stat.bytesPerSec;
    std::cout << "\nTotal time ms       : " << totalTimeMs.count();
    //std::cout << "\nTotal delay ms      : " << totalDelayMs.count();
    std::cout << "\nTotal io bytes      : " << stat.totalBytes;
    std::cout << "\nTotal io count      : " << stat.totalIoCount;
}
} // namespace cmn
