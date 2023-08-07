export module Utility.String;
export import <string>;
export import <string_view>;

export namespace util
{
	using ::std::basic_string;
	using ::std::basic_string_view;

	using ::std::string;
	using ::std::wstring;
	using ::std::u8string;
	using ::std::u16string;
	using ::std::u32string;

	using ::std::string_view;
	using ::std::wstring_view;
	using ::std::u8string_view;
	using ::std::u16string_view;
	using ::std::u32string_view;

	using ::std::to_string;
	using ::std::to_wstring;
}
