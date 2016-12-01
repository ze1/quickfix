
#ifndef AO_COMMON_H__
#define AO_COMMON_H__

#if _WIN32
    #if _WIN64
        #define _X64
    #else
        #define _X32
        #define _X86
    #endif
#else
    #if __x86_64__ || __ppc64__
        #define _X64
    #else
        #define _X32
        #define _X86
    #endif
#endif

#ifdef _WIN32
    #include <winsock2.h>
    #include <windows.h>
    #include <time.h>
#else
    #include <sys/time.h>
    #include <sys/types.h>
#endif

#include <atomic>

///
// Конвертация из TDateTime в UnixTime
//
inline static time_t DateTime(double datetime) {

    return (time_t)(86400 * (datetime - 25569.0));
}

///
// Конвертация из UnixTime в TDateTime
//
inline static double DateTime(time_t usec) {

    return (long long)(25569 + usec / 86400) + ((double)(usec % 86400) / 86400.0);
}

///
// Разложение TDateTime на компоненты даты и времени в структуре tm
///
inline static bool DateTime(double datetime, tm& local) {

    time_t usec = DateTime(datetime);
    return localtime_s(&local, &usec) == 0;
}

///
// Текущие дата и время в формате TDateTime
///
inline static double DateTime() {

    time_t usec = time(NULL);
    return DateTime(usec);
}

inline static float Health(float *value = NULL) {

    static std::atomic<float> health = 0;

    if (!value) return health;

    float result = health;
    health = *value;
    return result;
}

inline static bool Connected(bool *value = NULL) {

    static std::atomic<bool> connected = false;

    if (!value) return connected;

    bool result = connected;
    connected = *value;
    return result;
}

#endif // AO_COMMON_H__