#pragma once
#include <stl/vector.hpp>


struct TestClass {
	const uint8_t m_data{};

	TestClass() {
		LOG_D("default constructor\n");
	}
	TestClass(uint8_t data) : m_data(data) {
		LOG_D("default constructor\n");
	}

	// copy constructor
	TestClass(const TestClass&) {
		LOG_D("copy constructor\n");
	}
	// copy assignment
	void operator=(const TestClass&) {
		LOG_D("copy assignment\n");
	}

	// move constructor
	TestClass(TestClass&&) noexcept {
		LOG_D("move constructor\n");
	}
	// move assignment
	void operator=(TestClass&&) noexcept {
		LOG_D("move assignment\n");
	}
};

namespace spd {
	namespace unit_test {
		inline void Vector() {
			// create vector
			spd::vector<TestClass> vec;
			vec.Resize(3);
			vec.Reserve(12);

			vec.PushBack(TestClass(0x54));
			vec.Insert(TestClass(0x55), 0);

			spd::vector<TestClass> vec2(vec);
		}
	}
}