//
// ulib - a collection of useful classes
// Copyright (C) 2006-2014 Michael Fink
//
/// \file ulib/config/Common.hpp common configuration
//
#pragma once

// switch off some common warnings
#pragma warning(disable: 4251) // 'C::m' : class 'T' needs to have dll-interface to be used by clients of class 'C'
#pragma warning(disable: 4275) // non dll-interface class 'T1' used as base for dll-interface class 'T2'

#if _MSC_VER > 1500
// some headers include "microsoft sdks\windows\v7.0a\include\intsafe.h"
// but some macros are redefined in stdint.h; include it here first, and prevent
// warnings
#pragma warning(push)
#pragma warning(disable: 4005) // 'M' : macro redefinition
#include <intsafe.h>
#include <stdint.h>
#pragma warning(pop)
#endif // _MSC_VER > 1500
