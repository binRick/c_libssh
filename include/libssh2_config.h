/* Headers */
#ifndef HAVE_NETINET_IN_H
#define HAVE_NETINET_IN_H
#endif
/* #undef HAVE_WINSOCK2_H */

/* Functions */
/* #undef HAVE__STRICMP */
#ifndef HAVE_SNPRINTF
#define HAVE_SNPRINTF
#endif
/* #undef HAVE__SNPRINTF */

/* Workaround for platforms without POSIX strcasecmp (e.g. Windows) */
#ifndef HAVE_STRCASECMP
#ifndef HAVE_STRCASECMP
#endif
#ifdef HAVE__STRICMP
#define strcasecmp    _stricmp
#define HAVE_STRCASECMP
#endif
#endif

/* Symbols */
#define HAVE___FUNC__
#define HAVE___FUNCTION__

/* Workaround for platforms without C90 __func__ */
#ifndef HAVE___FUNC__
#ifdef HAVE___FUNCTION__
#define __func__    __FUNCTION__
#define HAVE___FUNC__
#endif
#endif
