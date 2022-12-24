#pragma once

#include "Platform/Core/CorePlatformDefines.h"

#include <new>
#include <assert.h>
MEMORYMANAGEMENT_START
#ifndef _NO_MEMORY_MANAGER_
#include "Platform/Core/MemoryManager.h"
#endif
MEMORYMANAGEMENT_END

#ifdef WIN32
#include <xmemory>
#endif

#ifdef __gnu_linux__
#include <stdlib.h>
#include <stdio.h>
#endif


