#include <disposer/output.hpp>

namespace hana = boost::hana;

using namespace hana::literals;
using namespace disposer::interface::module;

constexpr auto types = hana::tuple_t< int, char, float >;
constexpr auto types_set = hana::to_set(types);


int main(){
	constexpr auto m1 = disposer::make_io_lists(
			out("test1"_s, hana::type_c< int >)
		);
	static_assert(std::is_same_v< decltype(m1),
		hana::pair<
			hana::type< hana::map<> >,
			hana::type< hana::map<
				hana::pair< decltype("test1"_s),
					disposer::output< decltype("test1"_s), int > >
			> >
		> const >);

	constexpr auto m2 = disposer::make_io_lists(
			out("test1"_s, hana::type_c< int >),
			out("test2"_s, hana::type_c< char >),
			out("test3"_s, hana::type_c< float >)
		);
	static_assert(std::is_same_v< decltype(m2),
		hana::pair<
			hana::type< hana::map<> >,
			hana::type< hana::map<
				hana::pair< decltype("test1"_s),
					disposer::output< decltype("test1"_s), int > >,
				hana::pair< decltype("test2"_s),
					disposer::output< decltype("test2"_s), char > >,
				hana::pair< decltype("test3"_s),
					disposer::output< decltype("test3"_s), float > >
			> >
		> const >);

	constexpr auto m3 = disposer::make_io_lists(
			out("test1"_s, types)
		);
	static_assert(std::is_same_v< decltype(m3),
		hana::pair<
			hana::type< hana::map<> >,
			hana::type< hana::map<
				hana::pair< decltype("test1"_s),
					disposer::output< decltype("test1"_s), int, char, float > >
			> >
		> const >);

	constexpr auto m4 = disposer::make_io_lists(
			out("test1"_s, types_set)
		);
	static_assert(std::is_same_v< decltype(m4),
		hana::pair<
			hana::type< hana::map<> >,
			hana::type< hana::map<
				hana::pair< decltype("test1"_s),
					disposer::output< decltype("test1"_s), int, char, float > >
			> >
		> const >);

	constexpr auto m5 = disposer::make_io_lists(
			out("test1"_s, hana::type_c< int >),
			out("test2"_s, types),
			out("test3"_s, types_set)
		);
	static_assert(std::is_same_v< decltype(m5),
		hana::pair<
			hana::type< hana::map<> >,
			hana::type< hana::map<
				hana::pair< decltype("test1"_s),
					disposer::output< decltype("test1"_s), int > >,
				hana::pair< decltype("test2"_s),
					disposer::output< decltype("test2"_s), int, char, float > >,
				hana::pair< decltype("test3"_s),
					disposer::output< decltype("test3"_s), int, char, float > >
			> >
		> const >);
}
