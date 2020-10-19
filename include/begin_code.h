#ifndef PD_DEBUG
#define NDEBUG
#endif

#if defined(__linux__) || defined(__gnu_linux__)
    #define DDB_API extern
#elif defined(_WIN32)
    #if defined(DDB_EXPORT_DLL)
        #define DDB_API __declspec(dllexport) extern
    #else
        #define DDB_API __declspec(dllimport) extern
    #endif
#else
    #error "This platform is not currently supported"
#endif

/* Suggests to the compiler that a function should be inline. */
#ifndef DDB_INLINE
    #ifdef __GNUC__
        #define DDB_INLINE inline
        #ifndef __inline__
            #define __inline__inline
        #endif
    #elif defined(_MSC_VER) || defined(__BORLANDC__) || \
          defined(__DMC__) || defined(__SC__) || \
          defined(__WATCOMC__) || defined(__LCC__) || \
          defined(__DECC) || defined(__CC_ARM)
        #define DDB_INLINE __inline
        #ifndef __inline__
            #define __inline__ __inline
        #endif
    #endif
#endif

#ifndef DDB_FORCE_INLINE
    #if ((defined(__GNUC__) && (__GNUC__ >= 4))) || defined(__clang__)
        #define DDB_FORCE_INLINE __attribute__((always_inline)) static __inline__
    #elif defined(_MSC_VER)
        #define DDB_FORCE_INLINE __forceinline
    #else
        #define DDB_FORCE_INLINE static PD_INLINE
    #endif
#endif
