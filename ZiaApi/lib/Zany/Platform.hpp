/*
** EPITECH PROJECT, 2018
** api
** File description:
** Platform.hpp
*/

#pragma once

#if !defined(_WIN32) && !defined(__WIN32__) && !defined(WIN32)
# define ZANY_ISUNIX

namespace zany {
static constexpr bool isUnix = true;
static constexpr bool isWindows = false;
}

#else
# define ZANY_ISWINDOWS
# include <windows.h>
# include <string>

namespace zany {
static constexpr bool isUnix = false;
static constexpr bool isWindows = true;

inline std::string GetLastErrorAsString() {
    DWORD   errorMessageID = ::GetLastError();
    if(errorMessageID == 0)
        return std::string();

    LPSTR   messageBuffer = nullptr;
    size_t  size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errorMessageID,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR) &messageBuffer,
        0,
        NULL
    );
    
    std::string message(messageBuffer, size);
    LocalFree(messageBuffer);
    return message;
}

}

#endif

#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define ZANY_DLL __attribute__ ((dllexport))
  #else
    #define ZANY_DLL __declspec(dllexport)
  #endif
  #pragma warning(disable: 4251)
#else
  #if __GNUC__ >= 4
    #define ZANY_DLL __attribute__ ((visibility ("default")))
  #else
    #define ZANY_DLL
  #endif
#endif