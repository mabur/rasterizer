#pragma once

#include <algorithm>
#include <cassert>

template<typename T>
T min3(T a, T b, T c)
{
	return std::min(a, std::min(b, c));
}

template<typename T>
T max3(T a, T b, T c)
{
	return std::max(a, std::max(b, c));
}

template<typename Container, typename Value>
void fill(Container& container, Value value)
{
	using namespace std;
	fill(begin(container), end(container), value);
}

template<typename ContainerIn, typename ContainerOut>
void copy(const ContainerIn& in, ContainerOut& out)
{
	using namespace std;
	assert(in.size() == out.size());
	copy(begin(in), end(in), begin(out));
}
