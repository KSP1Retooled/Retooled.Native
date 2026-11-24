#pragma once

#include <optional>

#include "retooled.h"

namespace retooled {

template<typename T>
class Once {
private:
	std::optional<T> value;

public:
	Once () = default;
	Once (T value) : value (value) {}

	template<typename F>
	const T &get_or_init (F &&func)
	{
		if (value.has_value ())
			return *value;

		value = func ();
		return *value;
	}
};

template<>
class Once<QueryResult> {
private:
	int32_t value = -2;

public:
	Once () = default;
	Once (QueryResult result) : value (static_cast<int32_t> (result)) {}

	template<typename F>
	QueryResult get_or_init (F &&func)
	{
		if (value != -2)
			return static_cast<QueryResult> (value);

		QueryResult result = func ();
		value = static_cast<int32_t> (result);
		return result;
	}
};

} // namespace retooled
