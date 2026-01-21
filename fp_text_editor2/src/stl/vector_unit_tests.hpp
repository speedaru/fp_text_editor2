#pragma once
#include <stl/vector.hpp>


struct TestClass {
	uint8_t m_data{};

	TestClass() : m_data(0) {
		LOG_D("default constructor\n");
	}
	TestClass(uint8_t data) : m_data(data) {
		LOG_D("default constructor\n");
	}

	// copy constructor
	TestClass(const TestClass& other) : m_data(other.m_data) {
		LOG_D("copy constructor\n");
	}
	// copy assignment
	void operator=(const TestClass& other) {
		LOG_D("copy assignment\n");
		m_data = other.m_data;
	}

	// move constructor
	TestClass(TestClass&& other) noexcept : m_data(other.m_data) {
		LOG_D("move constructor\n");
	}
	// move assignment
	void operator=(TestClass&& other) noexcept {
		LOG_D("move assignment\n");
		m_data = other.m_data;
		other.m_data = 0;
	}
};

void PrintVector(const spd::vector<TestClass>& vec) {
	const TestClass* start = vec.Data();
	const TestClass* end = start + vec.Size();
	for (const TestClass* it = start; it < end; it++) {
		if (it != start) {
			std::cout << ", ";
		}
		std::cout << (int)(it->m_data);
	}
	std::cout << std::endl;
}

namespace spd {
	namespace unit_test {
		inline void Vector() {
			// create vector
			spd::vector<TestClass> vec;
			vec.Resize(3);
			vec.Reserve(12);

			vec.PushBack(TestClass(54));
			vec.Insert(TestClass(55), 0);

			LOG_D("size before remove at: %llu\n", vec.Size());
			PrintVector(vec);
			vec.RemoveAt(0);

			spd::vector<TestClass> vec2(vec);
		}
	}
}