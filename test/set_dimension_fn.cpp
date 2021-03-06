#include <disposer/core/set_dimension_fn.hpp>
#include <disposer/core/ref.hpp>

#include <boost/hana/tuple.hpp>


using namespace disposer;


constexpr solved_dimensions result{index_component< 0 >{0}};


template < typename Fn >
void test(Fn const& fn){
	fn(module_make_ref{optional_component< void >{},
		dimension_list{dimension_c< int, char >}, iops_ref{},
		module_make_data{}});
}


int main(){
	test(set_dimension_fn([](auto){ return result; }));
	test(set_dimension_fn([](auto&&){ return result; }));
	test(set_dimension_fn([](auto const&){ return result; }));
// 	test(set_dimension_fn([](auto const&&){ return result; })); // clang Failes

	// Must fail:
// 	test(set_dimension_fn([](auto&){ return result; }));
// 	test(set_dimension_fn([](auto)mutable{ return result; }));
// 	test(set_dimension_fn([](auto const&)mutable{ return result; }));
// 	test(set_dimension_fn([](auto&&)mutable{ return result; }));
// 	test(set_dimension_fn([](auto const&&)mutable{ return result; }));
// 	test(set_dimension_fn([](auto&)mutable{ return result; }));
}
