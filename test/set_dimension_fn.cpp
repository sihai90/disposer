#include <disposer/core/make_module.hpp>

#define BOOST_TEST_MODULE dimension solve
#include <boost/test/included/unit_test.hpp>


using namespace disposer;

using namespace hana::literals;
using namespace disposer::literals;
using namespace std::literals::string_view_literals;


using type_index = boost::typeindex::ctti_type_index;

template < std::size_t D >
using ic = index_component< D >;


BOOST_AUTO_TEST_CASE(test_1){
	constexpr auto list = dimension_list{
			dimension_c< double, char, float >
		};

	struct exec{
		constexpr void operator()()const{}
	};

	constexpr module_init_fn state_dummy{};
	constexpr exec_fn exec_dummy{exec{}};

	module_make_data module_data{};
	auto const make_module = [=](std::size_t i){
		return make_module_ptr(list,
			module_configure{
				set_dimension_fn([i](auto const&){
					return solved_dimensions{index_component< 0 >{i}};
				})
			}, module_data, state_dummy, exec_dummy);
	};

	{
		auto module_base_ptr = make_module(0);
		auto const module_ptr = dynamic_cast< module<
				type_list< double >,
				hana::tuple<>,
				hana::tuple<>,
				hana::tuple<>,
				void,
				exec
			>* >(module_base_ptr.get());
		BOOST_TEST(module_ptr != nullptr);
	}
	{
		auto module_base_ptr = make_module(1);
		auto const module_ptr = dynamic_cast< module<
				type_list< char >,
				hana::tuple<>,
				hana::tuple<>,
				hana::tuple<>,
				void,
				exec
			>* >(module_base_ptr.get());
		BOOST_TEST(module_ptr != nullptr);
	}
	{
		auto module_base_ptr = make_module(2);
		auto const module_ptr = dynamic_cast< module<
				type_list< float >,
				hana::tuple<>,
				hana::tuple<>,
				hana::tuple<>,
				void,
				exec
			>* >(module_base_ptr.get());
		BOOST_TEST(module_ptr != nullptr);
	}
	{
		BOOST_CHECK_THROW(make_module(3), std::out_of_range);
	}
}

BOOST_AUTO_TEST_CASE(test_2){
	constexpr auto list = dimension_list{
			dimension_c< double, char, float >,
			dimension_c< int, bool >
		};

	struct exec{
		constexpr void operator()()const{}
	};

	constexpr module_init_fn state_dummy{};
	constexpr exec_fn exec_dummy{exec{}};

	module_make_data module_data{};
	auto const module_base_ptr = make_module_ptr(list,
		module_configure{
			set_dimension_fn([](auto const&){
				return solved_dimensions{
					index_component< 0 >{2},
					index_component< 1 >{1}
				};
			})
		}, module_data, state_dummy, exec_dummy);
	auto const module_ptr = dynamic_cast< module<
			type_list< float, bool >,
			hana::tuple<>,
			hana::tuple<>,
			hana::tuple<>,
			void,
			exec
		>* >(module_base_ptr.get());
	BOOST_TEST(module_ptr != nullptr);
}

BOOST_AUTO_TEST_CASE(test_3){
	constexpr auto list = dimension_list{
			dimension_c< double, char, float >,
			dimension_c< int, bool >,
			dimension_c< short, unsigned, long, long long >
		};

	struct exec{
		constexpr void operator()()const{}
	};

	constexpr module_init_fn state_dummy{};
	constexpr exec_fn exec_dummy{exec{}};

	module_make_data module_data{};
	auto const module_base_ptr = make_module_ptr(list,
		module_configure{
			set_dimension_fn([](auto const&){
				return solved_dimensions{
					index_component< 0 >{2},
					index_component< 1 >{1},
					index_component< 2 >{3}
				};
			})
		}, module_data, state_dummy, exec_dummy);
	auto const module_ptr = dynamic_cast< module<
			type_list< float, bool, long long >,
			hana::tuple<>,
			hana::tuple<>,
			hana::tuple<>,
			void,
			exec
		>* >(module_base_ptr.get());
	BOOST_TEST(module_ptr != nullptr);
}
