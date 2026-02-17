#pragma once
#include <iostream>
#include "vector.hpp"

#include "unit_tests.hpp"


void PrintVector(const spd::Vector<TestClass>& vec) {
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
			spd::Vector<TestClass> vec;
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

			spd::Vector<TestClass> vec2(vec);
		}
	}
}