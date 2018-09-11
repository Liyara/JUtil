#ifndef JUTIL_TRAITS_H
#define JUTIL_TRAITS_H

#include "Core/Macro.h"

#ifdef JUTIL_CPP11

#include "TypeManipulators.hpp"
#include "Validator.hpp"
#include "declval.hpp"
#include "Convert.hpp"
#include "BaseOf.hpp"
#include "IsPointer.hpp"
#include "IsVoid.hpp"
#include "IsArray.hpp"
#include "IsReference.hpp"
#include "IsRVal.hpp"
#include "IsNullPointer.hpp"
#include "IsFunction.h"
#include "IsClass.hpp"
#include "IsFloatingPoint.hpp"
#include "IsArithmatic.hpp"
#include "IsInteger.hpp"

namespace jutil JUTIL_PUBLIC_ {}

#endif

#endif // JUTIL_TRAITS_H
