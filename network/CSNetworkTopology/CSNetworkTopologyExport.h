#ifdef CSNETWORKTOPOLOGY_EXPORTS
#define NETWORK_TOPOLOGY_DLL_EXPORT __declspec(dllexport)
#else
#define NETWORK_TOPOLOGY_DLL_EXPORT __declspec(dllimport)
#endif