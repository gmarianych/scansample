#include <iostream>

#include "Scanner/Scanner.h"
#include "common/common.h"

auto gUsage = R"(
Usage:
scansample.exe <SCAN_ROOT> <BYTES_PER_SEC>
SCAN_ROOT has to be exist
BYTES_PER_SEC can't be 0
)";
int main(int argc, char** argv)
{
    
    try
    {
        if (argc != 3)
            throw std::runtime_error(gUsage);
        fs::path root = argv[1];
        auto bytesPerSec = std::stoull(argv[2]);
        if (!fs::exists(root) || bytesPerSec == 0)
            throw std::runtime_error(gUsage);

        scan::Scanner scanner;
        scanner.scan(root, bytesPerSec);
    }
    catch (const std::exception& e)
    {
        std::cout << "Error: " << e.what();
    }

    return 0;
}
