#pragma once
#include <vector>
#include <map>
#include <mutex>
#include <memory>
#include <future>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cryptopp/sha.h>

#include <windows.h>

#define CALL_AND_THROW_ON_ERROR_STATUS(var, func)       \
    auto var = func;                                    \
    if(var)                                             \
        throw std::runtime_error(std::string(__FUNCTION__) + " : "#func" fails, err=" + cmn::to_hex(var))

#define CHECK_WAPI_BOOL(func)          \
    if(!func)                          \
        throw std::runtime_error(std::string(__FUNCTION__) + " : "#func" fails, err=" + cmn::to_hex(GetLastError()))

#define CHACK_WAPI_RETVAL(var, func) \
    auto var = func;                 \
    if(!var)                         \
        throw std::runtime_error(std::string(__FUNCTION__) + " : "#func" fails, err=" + cmn::to_hex(GetLastError()))

namespace cmn
{

template< typename T >
std::string to_hex(T i)
{
    std::stringstream stream;
    stream << std::setfill('0') << std::setw(sizeof(T) * 2) << std::hex << (uint64_t)i;
    return stream.str();
}

inline std::string to_hex(const char* buf, size_t sz)
{
    std::string res;
    for (size_t i = 0; i < sz; ++i)
        res += to_hex((unsigned char)buf[i]);
    return res;
}

inline std::string to_hex(const uint8_t* buf, size_t sz)
{
    return to_hex(reinterpret_cast<const char*>(buf), sz);
}

inline std::string to_hex(const std::vector<uint8_t>& buf)
{
    return to_hex(reinterpret_cast<const char*>(&buf[0]), buf.size());
}
template<class F>
class ScopedAction
{
    bool active_ = true;
    F f_;
public:
    ScopedAction(F f) : f_(f) {}
    void release() { active_ = false; }
    ~ScopedAction() { if (active_)f_(); }
};

template<class F>
ScopedAction<F> scopedAction(F f) { return ScopedAction<F>(f); }

template <typename T, typename Deleter>
auto MakeSmartPtr(T* ptr, Deleter deleter)
{
    return std::unique_ptr<T, Deleter>(ptr, deleter);
}

std::vector<uint8_t> GetBinaryKey(const std::wstring& key, const std::wstring& name);

std::vector<uint8_t> ProtectData(const std::vector<uint8_t>& data, const std::vector<uint8_t>& entropy);
std::vector<uint8_t> UnprotectData(const std::vector<uint8_t>& data, const std::vector<uint8_t>& entropy);

inline void SetValueImpl(HKEY hKey, const std::wstring& name, const std::wstring& v)
{
    const BYTE* ptr = v.empty() ? nullptr : reinterpret_cast<const BYTE*>(&v[0]);
#pragma warning(suppress: 26472)
    const DWORD sz = static_cast<DWORD>((v.size() + 1) * sizeof(v[0]));
    CALL_AND_THROW_ON_ERROR_STATUS(err, RegSetValueExW(hKey, name.c_str(), 0, REG_SZ, ptr, sz));
}

inline void SetValueImpl(HKEY hKey, const std::wstring& name, int32_t v)
{
    const BYTE* ptr = reinterpret_cast<const BYTE*>(&v);
    CALL_AND_THROW_ON_ERROR_STATUS(err, RegSetValueExW(hKey, name.c_str(), 0, REG_DWORD, ptr, sizeof(v)));
}

inline void SetValueImpl(HKEY hKey, const std::wstring& name, int64_t v)
{
    const BYTE* ptr = reinterpret_cast<const BYTE*>(&v);
    CALL_AND_THROW_ON_ERROR_STATUS(err, RegSetValueExW(hKey, name.c_str(), 0, REG_QWORD, ptr, sizeof(v)));
}

template <class... Args>
void SetRegistryValues(const std::wstring& key, const Args &&... args)
{
    HKEY hKey{};
    DWORD disp{};
    CALL_AND_THROW_ON_ERROR_STATUS(err, RegCreateKeyExW(HKEY_LOCAL_MACHINE, key.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE,
        KEY_SET_VALUE, nullptr, &hKey, &disp));
    auto hKeyGuard = MakeSmartPtr(hKey, RegCloseKey);
    ((SetValueImpl(hKey, args.first, args.second)), ...);
}

class Sha256Hash
{
    CryptoPP::SHA256 hash_;
public:
    void Update(uint8_t* data, size_t size);
    std::string GetHash();
};
}