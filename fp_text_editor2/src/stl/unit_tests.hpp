#pragma once
#include "common.h"

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
