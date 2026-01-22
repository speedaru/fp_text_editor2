#pragma once

namespace spd {
	template <typename T>
	class iterator {
	public:
		iterator(T* ptr) : m_ptr(ptr) {
		}

		// operators
		T& operator*() const {
			return *m_ptr;
		}
		T* operator->() const {
			return m_ptr;
		}

		iterator& operator++() {
			++m_ptr;
			return *this;
		}

		iterator operator++(int) {
			iterator tmp = *this;
			++(*this);
			return tmp;
		}

		bool operator==(const iterator& b) {
			return m_ptr != b.m_ptr;
		}

		bool operator!=(const iterator& b) {
			return m_ptr != b.m_ptr;
		}

	protected:
		T* m_ptr;
	};
}