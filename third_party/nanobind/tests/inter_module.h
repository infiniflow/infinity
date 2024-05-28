#include <nanobind/nb_defs.h>

#if defined(SHARED_BUILD)
#  define EXPORT_SHARED NB_EXPORT
#else
#  define EXPORT_SHARED NB_IMPORT
#endif

struct EXPORT_SHARED Shared {
    int value;
};

extern EXPORT_SHARED Shared create_shared();
extern EXPORT_SHARED bool check_shared(const Shared &shared);
