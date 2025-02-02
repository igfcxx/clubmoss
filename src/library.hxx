#ifndef CLUBMOSS_LIBRARY_HXX
#define CLUBMOSS_LIBRARY_HXX

#include "module/preprocessor/preprocessor.hxx"
#include "module/optimizer/optimizer.hxx"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
#    define _export __declspec(dllexport)
#elif defined(__GNUC__) && ((__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3))
#    define _export __attribute__((visibility("default")))
#else
#    define _export
#endif

_export int search(int threads);

_export int preprocess(int threads);

_export void set_log_callback(void (*callback)(const char*));

#ifdef __cplusplus
}
#endif

#endif //CLUBMOSS_LIBRARY_HXX
