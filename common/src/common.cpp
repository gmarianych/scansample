#include "common/common.h"
//#include <cryptopp/hex.h>
#include <Windows.h>

namespace cmn
{

void Sha256Hash::Update(uint8_t* data, size_t size)
{
    hash_.Update(data, size);
}

std::string Sha256Hash::GetHash()
{
    std::vector<uint8_t> digest;
    digest.resize(hash_.DigestSize());

    hash_.Final(&digest[0]);

    return to_hex(digest);
}

} // namespace cmn