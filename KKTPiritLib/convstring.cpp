// ============================================================================
// преобразования строк в разные кодировки
//http://www.cyberforum.ru/cpp/thread1913018.html

#include <string>
#include <locale>
#include <cstring>
#include <codecvt>
#include "convstring.h"

namespace conv {

    namespace multibyte {

        std::string convert(const std::wstring& s)
        {
            return convert(s.c_str());
        }
        std::wstring convert(const std::string& s)
        {
            return convert(s.c_str());
        }

        std::string convert(const wchar_t* s)
        {
            const size_t len = wcslen(s);
            std::string result(len, '\0');

#ifdef _MSC_VER
            size_t count = 0;
            const auto size_ = result.size();
            wcstombs_s(&count, &result[0], size_, s, size_);
#else
            std::mbstate_t state = std::mbstate_t();
            std::wcsrtombs(&result[0], &s, result.size(), &state);
#endif

            return result;
        }

        std::wstring convert(const char* s)
        {
            std::mbstate_t state = std::mbstate_t();

            size_t len;
            wchar_t wc[4] = {};
            std::wstring result;
            const char* end = s + std::strlen(s);
            while ((len = std::mbrtowc(wc, s, end - s, &state)) > 0)
            {
                result += wc; s += len;
                for (auto& ch : wc)  ch = L'\0';
            }
            return result;
        }

    }//namespace multibyte

    namespace utf8 {

        std::string  convert(const std::wstring& s)
        {
            typedef std::codecvt_utf8<wchar_t>
                convert_typeX;
            std::wstring_convert<convert_typeX, wchar_t>
                converterX;
            return converterX.to_bytes(s);
        }
        std::wstring convert(const std::string& s)
        {
            typedef std::codecvt_utf8<wchar_t>
                convert_typeX;
            std::wstring_convert<convert_typeX, wchar_t>
                converterX;
            return converterX.from_bytes(s);
        }

    }//namespace utf8 

    namespace stdlocal {

        std::wstring convert(const char* first, const size_t len, const std::locale& loc)
        {
            if (len == 0)
                return std::wstring();
            const std::ctype<wchar_t>& facet =
                std::use_facet< std::ctype<wchar_t> >(loc);
            const char* last = first + len;
            std::wstring result(len, L'\0');
            facet.widen(first, last, &result[0]);
            return result;
        }

        std::string convert(
            const wchar_t* first,
            const size_t len,
            const std::locale& loc,
            const char default_char
        )
        {
            if (len == 0)
                return std::string();
            const std::ctype<wchar_t>& facet =
                std::use_facet<std::ctype<wchar_t> >(loc);
            const wchar_t* last = first + len;
            std::string result(len, default_char);
            facet.narrow(first, last, default_char, &result[0]);
            return result;
        }

        std::string  convert(const wchar_t* s, const std::locale& loc, const char default_char)
        {
            return convert(s, std::wcslen(s), loc, default_char);
        }
        std::string  convert(const std::wstring& s, const std::locale& loc, const char default_char)
        {
            return convert(s.c_str(), s.length(), loc, default_char);
        }
        std::wstring convert(const char* s, const std::locale& loc)
        {
            return convert(s, std::strlen(s), loc);
        }
        std::wstring convert(const std::string& s, const std::locale& loc)
        {
            return convert(s.c_str(), s.length(), loc);
        }

    }//namespace stdlocal

    namespace unicode {
        std::string  ToCP866(const std::wstring& unicodeStr) {
            std::string result;
            for (wchar_t c : unicodeStr) {
                if (c >= 1040 && c <= 1087)
                    result += (char)(c - 912);
                else if (c >= 1088 && c <= 1103)
                    result += (char)(c - 864);
                else
                    result += (char)c;
            }

            return result;
        }

        std::wstring FromCP866(const std::string& cp866) {
            std::wstring result;
            for (char c : cp866) {
                if (c >= (char)128 && c <= (char)175) {
                    int i = (((int)c) & 0xFF) + 912;
                    result += (wchar_t)(i);
                }
                else if (c >= (char)224 && c <= (char)239) {
                    int i = (((int)c) & 0xFF) + 864;
                    result += (wchar_t)(i);
                }
                else
                    result += (wchar_t)c;
            }

            return result;
        }
    }// namespace unicode

    std::string convertCP1251toCP866(std::string cp1251)
    {
        std::string result;
        for (char c : cp1251) {
            if (c >= '\xC0' && c <= '\xEF')
                result += (c - 64);
            else if (c >= '\xF0' && c <= '\xFF')
                result += (c - 16);
            else
                result += c;
        }

        return result;
    }

}//namespace conv
