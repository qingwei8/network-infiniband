#ifdef NETWORK_EXPORTS
    #define NETWORK_DLL_EXPORT __declspec(dllexport)
#else
    #define NETWORK_DLL_EXPORT __declspec(dllimport)
#endif