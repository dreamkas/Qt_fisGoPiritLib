#ifndef QFISGOPIRIT_GLOBAL_H
#define QFISGOPIRIT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QFISGOPIRIT_LIBRARY)
#  define QFISGOPIRIT_EXPORT Q_DECL_EXPORT
#else
#  define QFISGOPIRIT_EXPORT Q_DECL_IMPORT
#endif

#endif // QFISGOPIRIT_GLOBAL_H
