//-----------------------------------------------------------------------------
// Copyright (c) 2017-2018 Benjamin Buch
//
// https://github.com/bebuch/disposer
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _disposer__core__set_dimension_fn__hpp_INCLUDED_
#define _disposer__core__set_dimension_fn__hpp_INCLUDED_

#include "dimension_numbers.hpp"

#include "../tool/comma_separated_output.hpp"

#include <logsys/stdlogb.hpp>

#include <boost/hana/functional/arg.hpp>
#include <boost/hana/for_each.hpp>
#include <boost/hana/greater.hpp>


namespace disposer{


	/// \brief Tag for set_dimension_fn
	struct set_dimension_fn_tag;

	/// \brief Chose a type from at least one dimension_list
	template < typename Fn >
	class set_dimension_fn{
	public:
		/// \brief Hana tag to identify set_dimension_fn
		using hana_tag = set_dimension_fn_tag;

		/// \brief A set_dimension_fn always solves a dimension
		static constexpr bool is_free_type = false;


		/// \brief Default construtor
		constexpr set_dimension_fn()
			noexcept(std::is_nothrow_default_constructible_v< Fn >)
			: fn_() {}

		/// \brief Construtor
		constexpr explicit set_dimension_fn(Fn const& fn)
			noexcept(std::is_nothrow_copy_constructible_v< Fn >)
			: fn_(fn) {}

		/// \brief Construtor
		constexpr explicit set_dimension_fn(Fn&& fn)
			noexcept(std::is_nothrow_move_constructible_v< Fn >)
			: fn_(std::move(fn)) {}


		/// \brief Checks result type
		template < std::size_t DimensionIndex, bool IsSolved >
		static constexpr void check_if_dimension_is_already_solved(
			hana::size_t< DimensionIndex >,
			hana::bool_< IsSolved > is_solved
		)noexcept{
			static_assert(is_solved,
				"dimension number DimensionIndex is already solved");
		}

		/// \brief Checks result type
		template < std::size_t DimensionCount, std::size_t ... SolvedIndexes >
		static constexpr void check_if_solved_dimensions_are_valid(
			hana::size_t< DimensionCount > dim_count,
			hana::tuple< hana::size_t< SolvedIndexes > ... > numbers
		)noexcept{
			auto const are_solved_dims_valid =
				hana::all_of(numbers, hana::less.than(dim_count));
			static_assert(are_solved_dims_valid,
				"At least one of the solved dimension indexes is equal or "
				"greater than the count of dimensions");
		}

		/// \brief Checks result type
		template < typename DimensionList, typename ResultType >
		static constexpr void check_result_type()noexcept{
			static_assert(is_solved_dimensions_v< ResultType >,
				"result of Fn(auto ref) in set_dimension_fn must "
				"be solved_dimensions< Ds ... > with sizeof...(Ds) > 0");

			static_assert(ResultType::index_count > 0,
				"result of Fn(auto ref) in set_dimension_fn must "
				"solve at least one dimension, you must return an object of "
				"solved_dimensions< Ds ... > with sizeof...(Ds) > 0");

			auto const dim_count = hana::size(DimensionList::dimensions);
			auto const numbers = ResultType::dimension_numbers();
			check_if_solved_dimensions_are_valid(dim_count, numbers);

			hana::for_each(numbers, [](auto const index){
				check_if_dimension_is_already_solved(index,
					hana::size(DimensionList::dimensions[index]) >
					hana::size_c< 1 >);
			});
		}

		/// \brief Helper for noexcept of operator()
		template < typename Ref >
		static constexpr bool calc_noexcept()noexcept{
			static_assert(std::is_invocable_v< Fn const, Ref >,
				"Wrong function signature for set_dimension_fn, expected:\n"
				"solved_dimensions< Ds ... > function(auto ref)");

			check_result_type< typename Ref::dimension_list,
				std::invoke_result_t< Fn const, Ref > >();
			return std::is_nothrow_invocable_v< Fn const, Ref >;
		}

		/// \brief Calls the actual function
		template < typename Ref >
		auto operator()(Ref const& ref)const
		noexcept(calc_noexcept< Ref >()){
			auto const types = [](auto ic){
				return Ref::dimension_list
					::dimensions[hana::size_c< ic.d >];
			};

			auto const type_count = [types](auto ic){
				return hana::size(types(ic));
			};

			using solved_type = std::invoke_result_t< Fn const, Ref >;

			return ref.log(
				[types](
					logsys::stdlogb& os,
					std::optional< solved_type > const& solved_dims
				){
					os << "set dimension number";

					if constexpr(solved_type::index_count > 1){
						os << "s";
					}

					os << " ";

					if(!solved_dims){
						hana::unpack(
							solved_type::dimension_numbers(),
							[&os](auto ... d){
								detail::comma_separated_output(
									os, std::size_t(d) ...);
							});
						return;
					}

					hana::unpack(solved_dims->indexes,
						[&os, types](auto ... ic){
							detail::comma_separated_output(os,
								std::make_tuple(ic.d, " to ",
								detail::get_type_name(ic.i, types(ic))) ...);
						});
				}, [&]{
					auto solved_dims = std::invoke(fn_, ref);

					hana::unpack(solved_dims.indexes, [type_count](auto ... ic){
						if(((ic.i < type_count(ic)) && ...)) return;

						std::ostringstream os;
						os << "index is out of range: ";

						using namespace std::literals::string_view_literals;

						detail::comma_separated_output(os, std::make_tuple(
							"dimension number ", ic.d, " has ",
							type_count(ic).value, " types (index ",
							ic.i, " is ", (ic.i < type_count(ic)
								? "valid"sv : "invalid"sv), ")") ...);

						throw std::out_of_range(os.str());
					});

					return solved_dims;
				});
		}

	private:
		Fn fn_;
	};


}


#endif
