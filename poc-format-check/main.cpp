using namespace std;

#include <iostream>
#include <cstdio>
#include <cstddef>
#include <cassert>
#include <type_traits>
#include <string_view>
#include <array>
#include <algorithm>
#include <functional>

#define DEBUG 0
#if (DEBUG)
#define CONSTEXPR
#else
#define CONSTEXPR constexpr
#endif

template <typename T>
CONSTEXPR inline T argument(T value)
{
	return value;
}

template <typename T>
CONSTEXPR inline const T* argument(const std::basic_string<T>& value)
{
	return value.c_str();
}

CONSTEXPR bool check_format_helper(const std::string_view& format, std::string_view::const_iterator pos)
{
	const std::array<char, 5> flags{ '-', '+', ' ', '0', '#' };
	const std::array<char, 12> specs{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '*' };
	const std::array<char, 6> mods{ 'h', 'l', 'L', 'z', 'j', 't' };
	const std::array<char, 19> types{ '%', 'd', 'i', 'u', 'f', 'F', 'e', 'E', 'g', 'G', 'x', 'X', 'a', 'A', 'o', 's', 'c', 'p', 'n' };
	const std::size_t valid_chars_size = flags.size() + specs.size() + mods.size() + types.size();
	const std::array<char, valid_chars_size> valid_chars
	{
		'-', '+', ' ', '0', '#',
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '*',
		'h', 'l', 'L', 'z', 'j', 't',
		'%', 'd', 'i', 'u', 'f', 'F', 'e', 'E', 'g', 'G', 'x', 'X', 'a', 'A', 'o', 's', 'c', 'p', 'n'
	};

	while (pos != std::cend(format))
	{
		pos = std::find(pos, std::cend(format), '%');
		if (pos == std::cend(format))
		{
			// NOTE(batuhan): Arguments weren't given
			return true;
		}
		// Skip '%'
		pos = std::next(pos);

		auto candidate = std::find_first_of(pos, std::cend(format),
											std::cbegin(types), std::cend(types));
		if (candidate == std::cend(format))
		{
			// NOTE(batuhan): Unterminated format string.
			return false;
		}

		auto invalid_char = std::find_if(pos, candidate,
										 [&valid_chars](const auto& elem) -> bool
		{
			return std::find(std::cbegin(valid_chars), std::cend(valid_chars), elem) == std::cend(valid_chars);
		});
		if (invalid_char < candidate)
		{
			// NOTE(batuhan): Unknown character in the format string.
			return false;
		}

		if (candidate[0] != '%')
		{
			return false;
		}

		pos = std::next(candidate);
	}

	return true;
}

template<typename First, typename... Rest>
CONSTEXPR bool check_format_helper(const std::string_view& format, std::string_view::const_iterator pos,
								   const First& first, const Rest&... rest)
{
	struct Type_Check
	{
		char type;
		bool result;
	};

	const std::array<char, 5> flags{ '-', '+', ' ', '0', '#' };
	const std::array<char, 12> specs{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '*' };
	const std::array<char, 6> mods{ 'h', 'l', 'L', 'z', 'j', 't' };
	const std::array<char, 19> types{ '%', 'd', 'i', 'u', 'f', 'F', 'e', 'E', 'g', 'G', 'x', 'X', 'a', 'A', 'o', 's', 'c', 'p', 'n' };
	const std::size_t valid_chars_size = flags.size() + specs.size() + mods.size() + types.size();
	const std::array<char, valid_chars_size> valid_chars
	{
		'-', '+', ' ', '0', '#',
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '*',
		'h', 'l', 'L', 'z', 'j', 't',
		'%', 'd', 'i', 'u', 'f', 'F', 'e', 'E', 'g', 'G', 'x', 'X', 'a', 'A', 'o', 's', 'c', 'p', 'n'
	};

	const auto check_type = [](const char identifier) -> bool
	{
		// TODO(batuhan): This is not complete
		const std::array<Type_Check, 18> type_checks
		{ {
			{ 'd', std::is_same_v<First, int> || std::is_same_v<First, short> },
			{ 'i', std::is_same_v<First, int> || std::is_same_v<First, short> },
			{ 'u', std::is_same_v<First, int> || std::is_same_v<First, short> },
			{ 'o', std::is_same_v<First, int> || std::is_same_v<First, short> },
			{ 'x', std::is_same_v<First, int> || std::is_same_v<First, short> },
			{ 'X', std::is_same_v<First, int> || std::is_same_v<First, short> },
			{ 'f', std::is_same_v<First, float> || std::is_same_v<First, double> },
			{ 'F', std::is_same_v<First, float> || std::is_same_v<First, double> },
			{ 'e', std::is_same_v<First, float> || std::is_same_v<First, double> },
			{ 'E', std::is_same_v<First, float> || std::is_same_v<First, double> },
			{ 'g', std::is_same_v<First, float> || std::is_same_v<First, double> },
			{ 'G', std::is_same_v<First, float> || std::is_same_v<First, double> },
			{ 'a', std::is_same_v<First, float> || std::is_same_v<First, double> },
			{ 'A', std::is_same_v<First, float> || std::is_same_v<First, double> },
			{ 'c', std::is_same_v<First, char> || std::is_same_v<First, unsigned char> || std::is_same_v<First, int> },
			{ 's', std::is_same_v<First, char*> || std::is_same_v<First, const char*> },
			{ 'p', std::is_pointer_v<First> },
			{ 'n', std::is_same_v<First, int*> }
		} };

		auto type_check = std::find_if(std::cbegin(type_checks), std::cend(type_checks),
									   [&identifier](const auto& elem) -> bool
		{
			return elem.type == identifier;
		});

		return type_check->result;
	};

	while (pos != std::cend(format))
	{
		pos = std::find(pos, std::cend(format), '%');
		if (pos == std::cend(format))
		{
			// NOTE(batuhan): Arguments were given but no format was found.
			return false;
		}
		// Skip '%'
		pos = std::next(pos);

		auto candidate_it = std::find_first_of(pos, std::cend(format),
											   std::cbegin(types), std::cend(types));
		if (candidate_it == std::cend(format))
		{
			// NOTE(batuhan): Unterminated format string.
			return false;
		}

		auto invalid_char_it = std::find_if(pos, candidate_it,
											[&valid_chars](const auto& elem) ->bool
		{
			return std::find(std::cbegin(valid_chars), std::cend(valid_chars), elem) == std::cend(valid_chars);
		});
		if (invalid_char_it < candidate_it)
		{
			// NOTE(batuhan): Unknown character in the format string.
			return false;
		}

		// Skip '%' symbol (format string %%)
		if (candidate_it[0] != '%')
		{
			if (check_type(candidate_it[0]))
			{
				return check_format_helper(format, std::next(candidate_it), argument(rest)...);
			}
			else
			{
				return false;
			}
		}

		pos = std::next(candidate_it);
	}

	return false;
}

template<typename... Args>
CONSTEXPR bool check_format(const std::string_view& format, Args&&... args)
{
	return check_format_helper(format, format.cbegin(), argument(args)...);
}

template <typename... Args>
CONSTEXPR void trace(const char* message, const Args&... args)
{
	char log[1024] = {};
	// TODO(batuhan): Why doesn't the line below work?
	// static_assert(check_format_helper(message)); 
	static_assert(check_format_helper(message, argument(args)...));
	std::snprintf(log, sizeof(log), message, argument(args)...);
}

int main(int arg_count, char** args)
{

#define fmt		"%#i %+s %i %u %+1.2f %c asdasd %% %d %s"
#define args	1, "abc", 1, 3, 2.34, 1, 5, "15123"
	printf(fmt, args);

#if !DEBUG
	static_assert(check_format(fmt, args));

	static_assert(check_format("") == true);
	static_assert(check_format("%d") == false);
	static_assert(check_format("%%") == true);
	static_assert(check_format("%%", 5) == false);
	static_assert(check_format("%d %%", 5) == true);
	static_assert(check_format("%d", "5") == false);
	static_assert(check_format("%f", 5) == false);
	static_assert(check_format("%f", 5) == false);
	static_assert(check_format("%d%d", 5, 4) == true);
	static_assert(check_format("%d%hhld", 4, "5") == false);
	static_assert(check_format("%d", 5, 4) == false);
	static_assert(check_format("%s", "hello world") == true);
	static_assert(check_format("%s", std::string("test")) == true);

#else
	bool res;
	res = check_format_helper(fmt, args);
#endif

	return 0;
}
