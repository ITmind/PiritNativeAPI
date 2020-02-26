#pragma once
#ifndef CONVSTRING_H
#define CONVSTRING_H
// ============================================================================
// преобразования строк в разные кодировки
//http://www.cyberforum.ru/cpp/thread1913018.html

#include <string>
//#include <locale>
//#include <cstring>
//#include <codecvt>

namespace conv {

    namespace multibyte {
        std::string  convert(const wchar_t* s);
        std::wstring convert(const char* s);
        std::string  convert(const std::wstring& s);
        std::wstring convert(const std::string& s);
    }// namespace multibyte 

    namespace utf8 {
        std::string  convert(const std::wstring& s);
        std::wstring convert(const std::string& s);
    }// namespace utf8 

    namespace unicode {
        std::string  ToCP866(const std::wstring& s);
        std::wstring FromCP866(const std::string& s);
    }// namespace unicode 

    namespace stdlocal {

        std::string convert(
            const wchar_t* s,
            const size_t len,
            const std::locale& loc = std::locale(),
            const char default_char = '?'
        );

        std::string convert(
            const wchar_t* s,
            const std::locale& loc = std::locale(""),
            const char default_char = '?'
        );
        std::string convert(
            const std::wstring& s,
            const std::locale& loc = std::locale(""),
            const char default_char = '?'
        );

        std::wstring convert(
            const char* s,
            const std::locale& loc = std::locale("")
        );
        std::wstring convert(
            const std::string& s,
            const std::locale& loc = std::locale("")
        );

        std::wstring convert(
            const char* s,
            const size_t len,
            const std::locale& loc = std::locale()
        );

    } //namespace stdlocal 

    std::string convertCP1251toCP866(std::string cp1251);
}//namespace conv
#endif