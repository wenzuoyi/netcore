#ifndef GLOBAL_DEFINITION_H_
#define GLOBAL_DEFINITION_H_
#ifndef API_EXPORT_FLAG
#define API_EXPORT_HEADER __declspec(dllimport)
#else
#define API_EXPORT_HEADER __declspec(dllexport)
#endif
#endif // GLOBAL_DEFINITION_H_
