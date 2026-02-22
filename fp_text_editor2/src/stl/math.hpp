#pragma once

namespace spd {
	template <typename T>
	T min(T a, T b) {
		return a < b ? a : b;
	}

	template <typename T>
	T max(T a, T b) {
		return a > b ? a : b;
	}

	template <typename T>
	T clamp(T val, T minVal, T maxVal) {
		return max<T>(minVal, min<T>(val, maxVal));
	}
}