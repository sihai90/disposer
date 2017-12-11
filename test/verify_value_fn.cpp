#include <disposer/core/verify_value_fn.hpp>
#include <disposer/core/accessory.hpp>

#include <string_view>

#define BOOST_TEST_MODULE dimension solve
#include <boost/test/included/unit_test.hpp>


using namespace disposer;
using namespace std::literals::string_view_literals;


struct accessory: add_log< accessory >{
	accessory()noexcept{}

	void log_prefix(log_key&&, logsys::stdlogb&)const{}
};


constexpr auto value = std::size_t();


BOOST_AUTO_TEST_CASE(test_1){
	auto const f1 = verify_value_fn(
		[](std::size_t, auto const){ return 0; });
	auto const f2 = verify_value_fn(
		[](std::size_t, auto const)noexcept{ return 0; });

	BOOST_CHECK_NO_THROW((f1(""sv, value, accessory{})));
	BOOST_CHECK_NO_THROW((f2(""sv, value, accessory{})));

	static_assert(!noexcept(f1(""sv, value, accessory{})));
	static_assert(noexcept(f2(""sv, value, accessory{})));
}

BOOST_AUTO_TEST_CASE(test_2){
	auto const f1 = verify_value_fn([](std::size_t){ return 0; });
	auto const f2 = verify_value_fn([](std::size_t)noexcept{ return 0; });

	BOOST_CHECK_NO_THROW((f1(""sv, value, accessory{})));
	BOOST_CHECK_NO_THROW((f2(""sv, value, accessory{})));

	static_assert(!noexcept(f1(""sv, value, accessory{})));
	static_assert(noexcept(f2(""sv, value, accessory{})));
}


struct fn3{
	template < typename Accessory >
	std::size_t operator()(std::size_t, Accessory)const{
		return 3;
	}
};

struct fn3_nothrow{
	template < typename Accessory >
	std::size_t operator()(std::size_t, Accessory)const noexcept{
		return 3;
	}
};

struct fn2{
	std::size_t operator()(std::size_t)const{
		return 2;
	}
};

struct fn2_nothrow{
	std::size_t operator()(std::size_t)const noexcept{
		return 2;
	}
};


BOOST_AUTO_TEST_CASE(test_4){
	auto const f1 = verify_value_fn(fn3{});
	auto const f2 = verify_value_fn(fn3_nothrow{});


	BOOST_CHECK_NO_THROW((f1(""sv, value, accessory{})));
	BOOST_CHECK_NO_THROW((f2(""sv, value, accessory{})));

	static_assert(!noexcept(f1(""sv, value, accessory{})));
	static_assert(noexcept(f2(""sv, value, accessory{})));
}

BOOST_AUTO_TEST_CASE(test_5){
	auto const f1 = verify_value_fn(fn2{});
	auto const f2 = verify_value_fn(fn2_nothrow{});


	BOOST_CHECK_NO_THROW((f1(""sv, value, accessory{})));
	BOOST_CHECK_NO_THROW((f2(""sv, value, accessory{})));

	static_assert(!noexcept(f1(""sv, value, accessory{})));
	static_assert(noexcept(f2(""sv, value, accessory{})));
}

BOOST_AUTO_TEST_CASE(test_6){
	struct fnl1: fn2, fn3{};
	auto const f1 = verify_value_fn(fnl1{});
	struct fnl2: fn2_nothrow, fn3{};
	auto const f2 = verify_value_fn(fnl2{});
	struct fnl3: fn2, fn3_nothrow{};
	auto const f3 = verify_value_fn(fnl3{});
	struct fnl4: fn2_nothrow, fn3_nothrow{};
	auto const f4 = verify_value_fn(fnl4{});


	BOOST_CHECK_NO_THROW((f1(""sv, value, accessory{})));
	BOOST_CHECK_NO_THROW((f2(""sv, value, accessory{})));
	BOOST_CHECK_NO_THROW((f3(""sv, value, accessory{})));
	BOOST_CHECK_NO_THROW((f4(""sv, value, accessory{})));

	static_assert(!noexcept(f1(""sv, value, accessory{})));
	static_assert(noexcept(f2(""sv, value, accessory{})));
	static_assert(!noexcept(f3(""sv, value, accessory{})));
	static_assert(noexcept(f4(""sv, value, accessory{})));
}

BOOST_AUTO_TEST_CASE(test_7){
	struct fnl1: fn2, fn3{};
	auto const f1 = verify_value_fn(fnl1{});
	struct fnl2: fn2_nothrow, fn3{};
	auto const f2 = verify_value_fn(fnl2{});
	struct fnl3: fn2, fn3_nothrow{};
	auto const f3 = verify_value_fn(fnl3{});
	struct fnl4: fn2_nothrow, fn3_nothrow{};
	auto const f4 = verify_value_fn(fnl4{});


	BOOST_CHECK_NO_THROW((f1(""sv, value, accessory{})));
	BOOST_CHECK_NO_THROW((f2(""sv, value, accessory{})));
	BOOST_CHECK_NO_THROW((f3(""sv, value, accessory{})));
	BOOST_CHECK_NO_THROW((f4(""sv, value, accessory{})));

	static_assert(!noexcept(f1(""sv, value, accessory{})));
	static_assert(noexcept(f2(""sv, value, accessory{})));
	static_assert(!noexcept(f3(""sv, value, accessory{})));
	static_assert(noexcept(f4(""sv, value, accessory{})));
}

