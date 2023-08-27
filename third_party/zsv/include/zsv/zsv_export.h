#ifndef ZSV_EXPORT_H
#define ZSV_EXPORT_H

#ifdef __EMSCRIPTEN__
# include <emscripten.h>
# define ZSV_EXPORT EMSCRIPTEN_KEEPALIVE
#else
# define ZSV_EXPORT
#endif

#endif
