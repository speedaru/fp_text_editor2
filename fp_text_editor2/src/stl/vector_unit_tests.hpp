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

	~TestClass() {
		LOG_D("destructor\n");
	}
};

void PrintVector(const spd::vector<TestClass>& vec) {
	for (auto it = vec.begin(); it != vec.end(); ++it) {
		if (it != vec.begin()) {
			std::cout << ", ";
		}
		std::cout << (int)it->m_data;
	}
	std::cout << std::endl;
}

namespace spd {
	namespace unit_test {
		inline void Vector() {
			// create vector
			spd::vector<TestClass> vec;
			LOG_D("resizing vector to 3\n");
			vec.Resize(3);
			
			LOG_D("reserving 12 elements\n");
			vec.Reserve(12);

			LOG_D("pushing back test class isntance with 54\n");
			vec.PushBack(54);

			LOG_D("emplacing back test class isntance with 50\n");
			vec.EmplaceBack(50);

			LOG_D("inserting at idx 0 test class isntance with 55\n");
			vec.Emplace(0, TestClass(55));

			LOG_D("size before remove at: %llu\n", vec.Size());
			PrintVector(vec);

			LOG_D("removing element at idx 0\n");
			vec.RemoveAt(0);

			LOG_D("creating copy of vector\n");
			spd::vector<TestClass> vec2(vec);
		}
	}
}