#ifdef TCPIP_EXPORTS
    #define TCPIP_DLL_EXPORT __declspec(dllexport)
#else
    #define TCPIP_DLL_EXPORT __declspec(dllimport)
#endif