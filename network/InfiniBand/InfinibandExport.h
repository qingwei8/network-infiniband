#ifdef INFINIBAND_EXPORTS
#define INFINIBAND_DLL_EXPORT __declspec(dllexport)
#else
#define INFINIBAND_DLL_EXPORT __declspec(dllimport)
#endif