
#ifndef DATAMANAGEMENT_EXPORT_H
#define DATAMANAGEMENT_EXPORT_H

#ifdef DATAMANAGEMENT_STATIC_DEFINE
#  define DATAMANAGEMENT_EXPORT
#  define DATAMANAGEMENT_NO_EXPORT
#else
#  ifndef DATAMANAGEMENT_EXPORT
#    ifdef DataManagement_EXPORTS
        /* We are building this library */
#      define DATAMANAGEMENT_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define DATAMANAGEMENT_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef DATAMANAGEMENT_NO_EXPORT
#    define DATAMANAGEMENT_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef DATAMANAGEMENT_DEPRECATED
#  define DATAMANAGEMENT_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef DATAMANAGEMENT_DEPRECATED_EXPORT
#  define DATAMANAGEMENT_DEPRECATED_EXPORT DATAMANAGEMENT_EXPORT DATAMANAGEMENT_DEPRECATED
#endif

#ifndef DATAMANAGEMENT_DEPRECATED_NO_EXPORT
#  define DATAMANAGEMENT_DEPRECATED_NO_EXPORT DATAMANAGEMENT_NO_EXPORT DATAMANAGEMENT_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef DATAMANAGEMENT_NO_DEPRECATED
#    define DATAMANAGEMENT_NO_DEPRECATED
#  endif
#endif

#endif /* DATAMANAGEMENT_EXPORT_H */
