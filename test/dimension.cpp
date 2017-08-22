#include <disposer/core/dimension.hpp>


template < typename ... T > struct morph{};


int main(){
	using namespace disposer;

	{
		// dimension construtors
		constexpr dimension< int, char, float > default_constructed;
		constexpr auto copy_constructed = default_constructed;
		constexpr auto move_constructed = std::move(default_constructed);
		(void)copy_constructed; (void)move_constructed;
	}

	{
		// construction by dimension_c
		constexpr auto variable_constructed = dimension_c< int, char, float >;
		(void)variable_constructed;
	}

	{
		// dimension_list construction by dimension_c
		auto list0 = dimension_list{};
		static_assert(std::is_same_v< decltype(list0), dimension_list<> >);
		auto list1 = dimension_list{
				dimension_c< int, char, float >
			};
		static_assert(std::is_same_v< decltype(list1), dimension_list<
				dimension< int, char, float >
			> >);
		auto list2 = dimension_list{
				dimension_c< int, char, float >,
				dimension_c< double, bool >
			};
		static_assert(std::is_same_v< decltype(list2), dimension_list<
				dimension< int, char, float >,
				dimension< double, bool >
			> >);
	}

	{
		// direct dimension_list construction
		auto list0 = dimension_list<>{};
		auto list1 = dimension_list<
				dimension< int, char, float >
			>{};
		auto list2 = dimension_list<
				dimension< int, char, float >,
				dimension< double, bool >
			>{};
		(void)list0; (void)list1; (void)list2;
	}

// 	constexpr auto invalid_0_type = dimension_c<>;
// 	constexpr auto invalid_1_type = dimension_c< int >;

	{
		// wrap_type_ref_in_t
		auto list = dimension_list{
				dimension_c< double, char, float >,
				dimension_c< int, bool >
			};

		auto list_f = free_type_c< long >;
		auto type_f_00 =
			list_f.convert(list, hana::tuple_c< std::size_t, 0, 0 >);
		static_assert(type_f_00 == hana::type_c< long >);
		auto type_f_10 =
			list_f.convert(list, hana::tuple_c< std::size_t, 1, 0 >);
		static_assert(type_f_10 == hana::type_c< long >);
		auto type_f_20 =
			list_f.convert(list, hana::tuple_c< std::size_t, 2, 0 >);
		static_assert(type_f_20 == hana::type_c< long >);
		auto type_f_01 =
			list_f.convert(list, hana::tuple_c< std::size_t, 0, 1 >);
		static_assert(type_f_01 == hana::type_c< long >);
		auto type_f_11 =
			list_f.convert(list, hana::tuple_c< std::size_t, 1, 1 >);
		static_assert(type_f_11 == hana::type_c< long >);
		auto type_f_21 =
			list_f.convert(list, hana::tuple_c< std::size_t, 2, 1 >);
		static_assert(type_f_21 == hana::type_c< long >);

		auto list_0 = type_ref_c< 0 >;
		auto type_0_00 =
			list_0.convert(list, hana::tuple_c< std::size_t, 0, 0 >);
		static_assert(type_0_00 == hana::type_c< double >);
		auto type_0_10 =
			list_0.convert(list, hana::tuple_c< std::size_t, 1, 0 >);
		static_assert(type_0_10 == hana::type_c< char >);
		auto type_0_20 =
			list_0.convert(list, hana::tuple_c< std::size_t, 2, 0 >);
		static_assert(type_0_20 == hana::type_c< float >);
		auto type_0_01 =
			list_0.convert(list, hana::tuple_c< std::size_t, 0, 1 >);
		static_assert(type_0_01 == hana::type_c< double >);
		auto type_0_11 =
			list_0.convert(list, hana::tuple_c< std::size_t, 1, 1 >);
		static_assert(type_0_11 == hana::type_c< char >);
		auto type_0_21 =
			list_0.convert(list, hana::tuple_c< std::size_t, 2, 1 >);
		static_assert(type_0_21 == hana::type_c< float >);

		auto list_1 = type_ref_c< 1 >;
		auto type_1_00 =
			list_1.convert(list, hana::tuple_c< std::size_t, 0, 0 >);
		static_assert(type_1_00 == hana::type_c< int >);
		auto type_1_10 =
			list_1.convert(list, hana::tuple_c< std::size_t, 1, 0 >);
		static_assert(type_1_10 == hana::type_c< int >);
		auto type_1_20 =
			list_1.convert(list, hana::tuple_c< std::size_t, 2, 0 >);
		static_assert(type_1_20 == hana::type_c< int >);
		auto type_1_01 =
			list_1.convert(list, hana::tuple_c< std::size_t, 0, 1 >);
		static_assert(type_1_01 == hana::type_c< bool >);
		auto type_1_11 =
			list_1.convert(list, hana::tuple_c< std::size_t, 1, 1 >);
		static_assert(type_1_11 == hana::type_c< bool >);
		auto type_1_21 =
			list_1.convert(list, hana::tuple_c< std::size_t, 2, 1 >);
		static_assert(type_1_21 == hana::type_c< bool >);

		auto wrap_10 = wrapped_type_ref_c< morph, 0 >;
		auto type_10_00 =
			wrap_10.convert(list, hana::tuple_c< std::size_t, 0, 0 >);
		static_assert(type_10_00 == hana::type_c< morph< double > >);
		auto type_10_10 =
			wrap_10.convert(list, hana::tuple_c< std::size_t, 1, 0 >);
		static_assert(type_10_10 == hana::type_c< morph< char > >);
		auto type_10_20 =
			wrap_10.convert(list, hana::tuple_c< std::size_t, 2, 0 >);
		static_assert(type_10_20 == hana::type_c< morph< float > >);
		auto type_10_01 =
			wrap_10.convert(list, hana::tuple_c< std::size_t, 0, 1 >);
		static_assert(type_10_01 == hana::type_c< morph< double > >);
		auto type_10_11 =
			wrap_10.convert(list, hana::tuple_c< std::size_t, 1, 1 >);
		static_assert(type_10_11 == hana::type_c< morph< char > >);
		auto type_10_21 =
			wrap_10.convert(list, hana::tuple_c< std::size_t, 2, 1 >);
		static_assert(type_10_21 == hana::type_c< morph< float > >);

		auto wrap_11 = wrapped_type_ref_c< morph, 1 >;
		auto type_11_00 =
			wrap_11.convert(list, hana::tuple_c< std::size_t, 0, 0 >);
		static_assert(type_11_00 == hana::type_c< morph< int > >);
		auto type_11_10 =
			wrap_11.convert(list, hana::tuple_c< std::size_t, 1, 0 >);
		static_assert(type_11_10 == hana::type_c< morph< int > >);
		auto type_11_20 =
			wrap_11.convert(list, hana::tuple_c< std::size_t, 2, 0 >);
		static_assert(type_11_20 == hana::type_c< morph< int > >);
		auto type_11_01 =
			wrap_11.convert(list, hana::tuple_c< std::size_t, 0, 1 >);
		static_assert(type_11_01 == hana::type_c< morph< bool > >);
		auto type_11_11 =
			wrap_11.convert(list, hana::tuple_c< std::size_t, 1, 1 >);
		static_assert(type_11_11 == hana::type_c< morph< bool > >);
		auto type_11_21 =
			wrap_11.convert(list, hana::tuple_c< std::size_t, 2, 1 >);
		static_assert(type_11_21 == hana::type_c< morph< bool > >);

		auto wrap_2 = wrapped_type_ref_c< morph, 0, 1 >;
		auto type_2_00 =
			wrap_2.convert(list, hana::tuple_c< std::size_t, 0, 0 >);
		static_assert(type_2_00 == hana::type_c< morph< double, int > >);
		auto type_2_10 =
			wrap_2.convert(list, hana::tuple_c< std::size_t, 1, 0 >);
		static_assert(type_2_10 == hana::type_c< morph< char, int > >);
		auto type_2_20 =
			wrap_2.convert(list, hana::tuple_c< std::size_t, 2, 0 >);
		static_assert(type_2_20 == hana::type_c< morph< float, int > >);
		auto type_2_01 =
			wrap_2.convert(list, hana::tuple_c< std::size_t, 0, 1 >);
		static_assert(type_2_01 == hana::type_c< morph< double, bool > >);
		auto type_2_11 =
			wrap_2.convert(list, hana::tuple_c< std::size_t, 1, 1 >);
		static_assert(type_2_11 == hana::type_c< morph< char, bool > >);
		auto type_2_21 =
			wrap_2.convert(list, hana::tuple_c< std::size_t, 2, 1 >);
		static_assert(type_2_21 == hana::type_c< morph< float, bool > >);

		auto wrap_r = wrapped_type_ref_c< morph, 1, 0, 1 >;
		auto type_r_00 =
			wrap_r.convert(list, hana::tuple_c< std::size_t, 0, 0 >);
		static_assert(type_r_00 == hana::type_c< morph< int, double, int > >);
		auto type_r_10 =
			wrap_r.convert(list, hana::tuple_c< std::size_t, 1, 0 >);
		static_assert(type_r_10 == hana::type_c< morph< int, char, int > >);
		auto type_r_20 =
			wrap_r.convert(list, hana::tuple_c< std::size_t, 2, 0 >);
		static_assert(type_r_20 == hana::type_c< morph< int, float, int > >);
		auto type_r_01 =
			wrap_r.convert(list, hana::tuple_c< std::size_t, 0, 1 >);
		static_assert(type_r_01 == hana::type_c< morph< bool, double, bool > >);
		auto type_r_11 =
			wrap_r.convert(list, hana::tuple_c< std::size_t, 1, 1 >);
		static_assert(type_r_11 == hana::type_c< morph< bool, char, bool > >);
		auto type_r_21 =
			wrap_r.convert(list, hana::tuple_c< std::size_t, 2, 1 >);
		static_assert(type_r_21 == hana::type_c< morph< bool, float, bool > >);

// 		auto invalid_type_11_30 =
// 			wrap_10.convert(list, hana::tuple_c< std::size_t, 3, 0 >);
// 		auto invalid_type_11_02 =
// 			wrap_10.convert(list, hana::tuple_c< std::size_t, 0, 2 >);

// 		auto invalid_type = wrapped_type_ref_c< morph, 2 >
// 			.convert(list, hana::tuple_c< std::size_t, 0, 0 >);
	}

	{
		using namespace hana::literals;

		// wrap_type_ref_in_t
		auto list = dimension_list{
				dimension_c< double, char, float >,
				dimension_c< int, bool >,
				dimension_c< short, unsigned, long, long long >
			};

		auto index_10_0 = dimension_solver< morph, 0 >::solve(list,
			hana::type_c< morph< double > >, hana::make_tuple());
		static_assert(index_10_0);
		static_assert(index_312100_10133.index_count == 1);
		static_assert(index_10_0.indexes[0_c].d == 0);
		static_assert(index_10_0.indexes[0_c].i == 0);

// 		auto index_10_1 = dimension_solver< morph, 0 >::solve(list,
// 			hana::type_c< morph< char > >, hana::make_tuple());
// 		static_assert(index_10_1);
// 		static_assert(index_312100_10133.index_count == 1);
// 		static_assert(index_10_1.indexes[0_c].d == 0);
// 		static_assert(index_10_1.indexes[0_c].i == 1);
//
// 		auto index_10_2 = dimension_solver< morph, 0 >::solve(list,
// 			hana::type_c< morph< float > >, hana::make_tuple());
// 		static_assert(index_10_2);
// 		static_assert(index_312100_10133.index_count == 1);
// 		static_assert(index_10_2.indexes[0_c].d == 0);
// 		static_assert(index_10_2.indexes[0_c].i == 2);
//
// 		auto index_11_0 = dimension_solver< morph, 1 >::solve(list,
// 			hana::type_c< morph< int > >, hana::make_tuple());
// 		static_assert(index_11_0);
// 		static_assert(index_312100_10133.index_count == 1);
// 		static_assert(index_11_0.indexes[0_c].d == 1);
// 		static_assert(index_11_0.indexes[0_c].i == 0);
//
// 		auto index_11_1 = dimension_solver< morph, 1 >::solve(list,
// 			hana::type_c< morph< bool > >, hana::make_tuple());
// 		static_assert(index_11_1);
// 		static_assert(index_312100_10133.index_count == 1);
// 		static_assert(index_11_1.indexes[0_c].d == 1);
// 		static_assert(index_11_1.indexes[0_c].i == 1);
//
// 		auto index_12_0 = dimension_solver< morph, 2 >::solve(list,
// 			hana::type_c< morph< short > >, hana::make_tuple());
// 		static_assert(index_12_0);
// 		static_assert(index_312100_10133.index_count == 1);
// 		static_assert(index_12_0.indexes[0_c].d == 2);
// 		static_assert(index_12_0.indexes[0_c].i == 0);
//
// 		auto index_12_1 = dimension_solver< morph, 2 >::solve(list,
// 			hana::type_c< morph< unsigned > >, hana::make_tuple());
// 		static_assert(index_12_1);
// 		static_assert(index_312100_10133.index_count == 1);
// 		static_assert(index_12_1.indexes[0_c].d == 2);
// 		static_assert(index_12_1.indexes[0_c].i == 1);
//
// 		auto index_12_2 = dimension_solver< morph, 2 >::solve(list,
// 			hana::type_c< morph< long > >, hana::make_tuple());
// 		static_assert(index_12_2);
// 		static_assert(index_312100_10133.index_count == 1);
// 		static_assert(index_12_2.indexes[0_c].d == 2);
// 		static_assert(index_12_2.indexes[0_c].i == 2);
//
// 		auto index_12_3 = dimension_solver< morph, 2 >::solve(list,
// 			hana::type_c< morph< long long > >, hana::make_tuple());
// 		static_assert(index_12_3);
// 		static_assert(index_312100_10133.index_count == 1);
// 		static_assert(index_12_3.indexes[0_c].d == 2);
// 		static_assert(index_12_3.indexes[0_c].i == 3);
//
//
// 		auto index_201_00 = dimension_solver< morph, 0, 1 >::solve(list,
// 			hana::type_c< morph< double, int > >, hana::make_tuple());
// 		static_assert(index_201_00);
// 		static_assert(index_312100_10133.index_count == 2);
// 		static_assert(index_201_00.indexes[0_c].d == 0);
// 		static_assert(index_201_00.indexes[0_c].i == 0);
// 		static_assert(index_201_00.indexes[1_c].d == 1);
// 		static_assert(index_201_00.indexes[1_c].i == 0);
//
// 		auto index_201_11 = dimension_solver< morph, 0, 1 >::solve(list,
// 			hana::type_c< morph< char, bool > >, hana::make_tuple());
// 		static_assert(index_201_11);
// 		static_assert(index_312100_10133.index_count == 2);
// 		static_assert(index_201_11.indexes[0_c].d == 0);
// 		static_assert(index_201_11.indexes[0_c].i == 1);
// 		static_assert(index_201_11.indexes[1_c].d == 1);
// 		static_assert(index_201_11.indexes[1_c].i == 1);
//
// 		auto index_202_13 = dimension_solver< morph, 0, 2 >::solve(list,
// 			hana::type_c< morph< char, long long > >, hana::make_tuple());
// 		static_assert(index_202_13);
// 		static_assert(index_312100_10133.index_count == 2);
// 		static_assert(index_202_13.indexes[0_c].d == 0);
// 		static_assert(index_202_13.indexes[0_c].i == 1);
// 		static_assert(index_202_13.indexes[1_c].d == 2);
// 		static_assert(index_202_13.indexes[1_c].i == 3);
//
// 		auto index_3120_103 = dimension_solver< morph, 1, 2, 0 >::solve(list,
// 			hana::type_c< morph< int, long long, char > >, hana::make_tuple());
// 		static_assert(index_3120_103);
// 		static_assert(index_312100_10133.index_count == 3);
// 		static_assert(index_3120_103.indexes[0_c].d == 1);
// 		static_assert(index_3120_103.indexes[0_c].i == 0);
// 		static_assert(index_3120_103.indexes[1_c].d == 2);
// 		static_assert(index_3120_103.indexes[1_c].i == 3);
// 		static_assert(index_3120_103.indexes[2_c].d == 0);
// 		static_assert(index_3120_103.indexes[2_c].i == 1);
//
// 		auto index_312100_10133 = dimension_solver< morph, 1, 2, 1, 0, 0 >
// 			::solve(list,
// 			hana::type_c< morph< int, long long, int, char, char > >,
// 			hana::make_tuple());
// 		*index_312100_10133;
// 		static_assert(index_312100_10133);
// 		static_assert(index_312100_10133.index_count == 3);
// 		static_assert(index_312100_10133.indexes[0_c].d == 1);
// 		static_assert(index_312100_10133.indexes[0_c].i == 0);
// 		static_assert(index_312100_10133.indexes[1_c].d == 2);
// 		static_assert(index_312100_10133.indexes[1_c].i == 3);
// 		static_assert(index_312100_10133.indexes[2_c].d == 0);
// 		static_assert(index_312100_10133.indexes[2_c].i == 1);
	}
}