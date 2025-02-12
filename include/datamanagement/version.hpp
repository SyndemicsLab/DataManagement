#ifndef VERSION_HPP_
#define VERSION_HPP_

#define DATAMANAGEMENT_VER_MAJOR 1
#define DATAMANAGEMENT_VER_MINOR 15
#define DATAMANAGEMENT_VER_PATCH 0

#define DATAMANAGEMENT_TO_VERSION(major, minor, patch)                         \
    (major * 10000 + minor * 100 + patch)
#define DATAMANAGEMENT_VERSION                                                 \
    DATAMANAGEMENT_TO_VERSION(DATAMANAGEMENT_VER_MAJOR,                        \
                              DATAMANAGEMENT_VER_MINOR,                        \
                              DATAMANAGEMENT_VER_PATCH)

#endif
