#pragma once
#include <common.h>


namespace spd {
	template <typename T>
	class vector {
	public:
#pragma region constructors
		vector();

		~vector();

		vector(const vector& other);
		vector(vector&& other) noexcept;
#pragma endregion


#pragma region memory_management
		bool Reserve(size_t newCapacity);
		bool Resize(size_t newSize);
		bool ShrinkToFit();
#pragma endregion


#pragma region data_manipulation
		void Insert(const T& element, size_t idx);
		void PushBack(const T& element);
#pragma endregion


#pragma region getters
		inline size_t Size() const {
			return m_size;
		}

		inline size_t Capacity() const {
			return m_capacity;
		}

		inline const T* Data() const {
			return const_cast<const T*>(m_data);
		}
#pragma endregion

	private:
		constexpr static const size_t INITIAL_CAPACITY = 8ull;
		constexpr static const float GROWTH_FACTOR = 1.5f;

		bool Realloc(size_t newSize);
		bool Realloc();

		// calls destructor for each data
		void DestroyData(T* data, size_t size);

	protected:
		size_t m_size{};
		size_t m_capacity{};
		T* m_data{ nullptr };
	};
}


// -------------------- IMPLEMENTATION --------------------
#pragma region constructors

template<typename T>
inline spd::vector<T>::vector() {
	Realloc(INITIAL_CAPACITY);
	LOG_T("created vector at 0x%p, initial capacity: %llu\n", m_data, m_capacity);
}

template<typename T>
inline spd::vector<T>::~vector() {
	DestroyData(m_data, m_size);
	LOG_D("destroyed vector data\n");
}

template<typename T>
inline spd::vector<T>::vector(const vector& other) {
	Realloc(static_cast<size_t>(other.m_size * GROWTH_FACTOR));

	while (m_size < other.m_size) {
		new (m_data + m_size) T(std::copy(other.m_data[m_size]));
		m_size++;
	}

	LOG_D("copied %llu objects from vector at 0x%p into vector at 0x%p\n", m_size, other.m_data, m_data);
}

template<typename T>
inline spd::vector<T>::vector(vector&& other) noexcept {
	m_size = other.m_size;
	other.m_size = 0ull;

	m_capacity = other.m_capacity;
	other.m_capacity = 0ull;

	m_data = other.m_data;
	other.m_data = nullptr;
}

#pragma endregion


#pragma region memory_management

template<typename T>
inline bool spd::vector<T>::Reserve(size_t newCapacity) {
	if (newCapacity > m_capacity) {
		return Realloc(newCapacity);
	}
	else {
		LOG_W("trying to reserve smaller size than vector size (%llu to %llu)\n", m_capacity, newCapacity);
		return false;
	}
}

template<typename T>
inline bool spd::vector<T>::Resize(size_t newSize) {
	bool res{ true };

	// case 1: new size is smaller than old size
	// delete other objects
	if (newSize < m_size) {
		while (m_size > newSize) {
			m_data[m_size--].~T();
		}
	}
	// case 2: new size is bigger than current capacity
	// if new size exceeds capacity, try to realloc, if realloc fails then return false
	else if (newSize > m_capacity && !(res = Realloc(newSize))) {
		return false;
	}
	
	// if new size bigger than current size, create default objects
	if (newSize > m_size) {
		while (m_size < newSize) {
			new (m_data + m_size++) T();
		}
	}

	LOG_D("resized vector to %llu\n", newSize);
	return res;
}

template<typename T>
inline bool spd::vector<T>::ShrinkToFit() {
	return false;
}

#pragma endregion


#pragma region data_manipulation

template<typename T>
inline void spd::vector<T>::Insert(const T& element, size_t idx) {
	// ensure not inserting past last element
	assert(idx <= m_size);

	// if data full realloc
	if (m_size >= m_capacity) {
		Realloc();
	}

	// shift data to right if inserting in middle
	if (idx < m_size) {
		for (size_t i = m_size; i > idx; i--) {
			// Construct new object at i using the move constructor from i - 1
			// even if T has const members
			new (m_data + i) T(std::move(m_data[i - 1]));

			// 2. Manually destroy the old object at i-1
			m_data[i - 1].~T();
		}
		LOG_D("shifted %llu elements for insert\n", m_size - idx);
	}

	// placement new at desired idx
	new (m_data + idx) T(std::copy(element));

	LOG_D("inserted element into vector at idx %llu\n", idx);
	m_size++; // inc element count
}

template<typename T>
inline void spd::vector<T>::PushBack(const T& element) {
	Insert(element, m_size);
	LOG_D("pushed back element\n");
}

#pragma endregion



#pragma region private

template<typename T>
inline bool spd::vector<T>::Realloc(size_t newCapacity) {
	// allocate new data
	T* newData = SPD_ALLOC(T, newCapacity);
	if (!newData) {
		return false;
	}

	if (m_data) {
		// copy old data if new size at least as big
		if (newCapacity >= m_size && m_size) {
			for (size_t i = 0; i < m_size; i++) {
				new (newData + i) T(std::move(m_data[i]));

				// destroy old data
				m_data[i].~T();
			}

			// destroy old data
			DestroyData(m_data, m_size);
		}

		SPD_FREE(m_data);
	}

	m_data = newData;
	m_capacity = newCapacity;
	LOG_T("resized vector capacity to %llu\n", m_capacity);

	return true;
}

template<typename T>
inline bool spd::vector<T>::Realloc() {
	return Realloc(static_cast<size_t>(m_capacity * GROWTH_FACTOR));
}

template<typename T>
inline void spd::vector<T>::DestroyData(T* data, size_t size) {
	// nothing to destroy
	if (!size) {
		LOG_W("trying to destroy data, but size is 0\n");
		return;
	}

	for (size_t i = 0ull; i < size; i++) {
		data[i].~T();
	}
	LOG_T("destroyed %llu objects at 0x%p\n", size, data);
}

#pragma endregion
