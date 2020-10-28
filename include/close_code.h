#undef DDB_API
#undef DDB_INLINE
#undef DDB_FORCE_INLINE

/* Reset structure packing at previous byte alignment. */
#if defined(_MSC_VER) || defined(__MWERKS__) || defined(__BORLANDC__)
    #ifdef __BORLANDC__
        #pragma nopackwarning
    #endif
    #pragma pack(pop)
#endif
