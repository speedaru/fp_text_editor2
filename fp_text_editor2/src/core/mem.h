#pragma once
#include <memory>
#include <cassert>

// defined in mem.cpp
inline size_t totalBytesAllocated{};
inline size_t totalBytesFreed{};
inline size_t bytesAllocated{};

#define SPD_ALLOC(type, count) spd::TrackedAlloc<type>(count, __RELATIVE_FILE__, __LINE__)
#define SPD_FREE(ptr) spd::TrackedFree(ptr, __RELATIVE_FILE__, __LINE__)

namespace spd {
	namespace {
		size_t _GetBytesAllocated() {
			assert(totalBytesAllocated > totalBytesFreed);
			return totalBytesAllocated - totalBytesFreed;
		}
	}

	template <typename T>
	inline T* TrackedAlloc(size_t count, const char* file, int line) {
		size_t size = count * sizeof(T);

		// allocate size for header + actual size
		size_t* base = static_cast<size_t*>(malloc(sizeof(size_t) + size));

		*base = size;
		totalBytesAllocated += size;
		
		T* ptr = reinterpret_cast<T*>(base + 1);

		LOG_D("ALLOC: 0x%p [%zu bytes] for %zu elements at %s:%d (allocated: %zu)\n",
			(void*)ptr, size, count, file, line, _GetBytesAllocated());

		return ptr;
	}

	template <typename T>
	inline void TrackedFree(T* ptr, const char* file, int line) {
		if (!ptr) {
			return;
		}

		void* base = static_cast<void*>(reinterpret_cast<size_t*>(ptr) - 1);
		size_t size = *static_cast<size_t*>(base);

		totalBytesFreed += size;

		logging::LOG_D("FREE: 0x%p [%zu bytes] at %s:%d (allocated: %zu)\n",
			base, size, file, line, _GetBytesAllocated());

		free(base);
	}
}

