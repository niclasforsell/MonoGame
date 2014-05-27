#pragma once

#include "predecls.h"
#include <_kernel.h>

namespace System {

enum class ThreadPrio
{
	Default = SCE_KERNEL_PRIO_FIFO_DEFAULT,
	Highest = SCE_KERNEL_PRIO_FIFO_HIGHEST,
	Lowest = SCE_KERNEL_PRIO_FIFO_LOWEST,
};

} // namespace System
