//-----------------------------------------------------------------------------
// Copyright (c) 2017 Benjamin Buch
//
// https://github.com/bebuch/disposer
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _disposer__core__dimension_solve__hpp_INCLUDED_
#define _disposer__core__dimension_solve__hpp_INCLUDED_

#include "dimension.hpp"

#include "../tool/type_index.hpp"

#include <boost/hana/unique.hpp>
#include <boost/hana/sort.hpp>
#include <boost/hana/range.hpp>
#include <boost/hana/remove_at.hpp>
#include <boost/hana/remove_if.hpp>
#include <boost/hana/all.hpp>
#include <boost/hana/cartesian_product.hpp>


namespace disposer{


	/// \brief Pair of a dimension number and the index of its active type
	template < std::size_t D >
	struct dimension_index{
		/// \brief Dimension number
		static constexpr std::size_t d = D;

		/// \brief Index of the active type of the dimension
		std::size_t i;
	};


	template < std::size_t ... Ds >
	struct solved_dimensions:
		std::conditional_t< (sizeof...(Ds) > 0), hana::true_, hana::false_ >
	{
		static constexpr std::size_t index_count = sizeof...(Ds);

		constexpr solved_dimensions(dimension_index< Ds > ... is)
			: indexes(is ...) {}

		hana::tuple< dimension_index< Ds > ... > indexes;
	};


	/// \brief List of dimension numbers
	template < std::size_t ... Ds >
	struct dimension_numbers{
		/// \brief List of dimension numbers
		static constexpr auto values = hana::tuple_c< std::size_t, Ds ... >;

		/// \brief Position of a dimension number in values
		template < std::size_t D >
		static constexpr auto pos(hana::size_t< D >){
			return hana::index_if(values,
				hana::equal.to(hana::size_c< D >)).value();
		}
	};

	/// \brief A dimension_numbers object with sorted and
	///        summarized dimension numbers
	template < std::size_t ... Ds >
	constexpr auto dimension_numbers_c = hana::unpack(
		hana::unique(hana::sort(hana::tuple_c< std::size_t, Ds ... >)),
		[](auto ... d){
			return dimension_numbers< decltype(d)::value ... >{};
		});

	/// \brief Get dimension_numbers with sorted and
	///        summarized dimension numbers
	template < std::size_t ... Ds >
	using dimension_numbers_t =
		decltype(dimension_numbers_c< Ds ... >);



	/// \brief Tool to evaluate active dimension of inputs if possible
	template <
		typename DimensionList,
		template < typename ... > typename Template,
		std::size_t ... Ds >
	class dimension_solver{
	private:
		/// \brief Sorted and summarized dimension numbers
		using numbers = dimension_numbers_t< Ds ... >;

		/// \brief Tuple of ranges of all indexes
		static constexpr auto ranges = hana::unpack(numbers::values,
			[](auto ... d){
				return hana::make_tuple(hana::to_tuple(hana::make_range(
					hana::size_c< 0 >,
					hana::size(DimensionList::dimensions[d])
				)) ...);
			});

		/// \brief Get the value_type of a given numbers index
		template < std::size_t ... Is >
		static constexpr auto value_type_of(
			hana::tuple< hana::size_t< Is > ... > index
		){
			return hana::type_c< Template< typename decltype(
					+DimensionList::dimensions[hana::size_c< Ds >]
						[index[numbers::pos(hana::size_c< Ds >)]]
				)::type ... > >;
		}

		/// \brief Use ranges as is since there are no more known dimensions
		template < typename Ranges >
		static auto constexpr sub_ranges_from(Ranges ranges){
			return ranges;
		}

		/// \brief Make a tuple of subranges where every component holds one
		///        index of the known dimension KD
		template < std::size_t KD, std::size_t ... KDs, typename Ranges >
		static auto constexpr sub_ranges_from(Ranges ranges){
			auto const index_pos = numbers::pos(hana::size_c< KD >);
			auto const range = ranges[index_pos];
			auto const ranges_rest = hana::remove_at(ranges, index_pos);
			return hana::unpack(range, [=](auto ... ki){
					return hana::make_tuple(sub_ranges_from< KDs ... >(
						hana::insert(ranges_rest, index_pos,
							hana::make_tuple(ki))) ...);
				});
		}

		/// \brief Create subranges by known dimensions KDs
		template < std::size_t ... KDs >
		static auto constexpr sub_ranges = sub_ranges_from< KDs ... >(ranges);

		/// \brief hana::true_c if indexs are deducible by ranges,
		///        hana::false_c otherwise
		template < typename Ranges >
		static constexpr auto is_deducible_from(Ranges ranges){
			auto const indexes = hana::cartesian_product(ranges);
			auto types = hana::transform(indexes,
				[](auto index){
					return value_type_of(index);
				});
			auto const unique_size =
				hana::size(hana::to_set(types));
			return hana::size(types) == unique_size;
		}


		/// \brief hana::true_c if indexs are deducible by ranges if the
		///        dimensions KDs are known, hana::false_c otherwise
		template <
			std::size_t KD,
			std::size_t ... KDs,
			typename Ranges >
		static constexpr auto is_deducible_from(Ranges ranges){
			return hana::all(hana::transform(ranges, [=](auto sub_ranges){
					return is_deducible_from< KDs ... >(sub_ranges);
				}));
		}

		/// \brief hana::true_c if indexs are deducible by ranges if the
		///        dimensions KDs are known, hana::false_c otherwise
		template < std::size_t ... KDs >
		static constexpr auto is_deducible =
			is_deducible_from< KDs ... >(sub_ranges< KDs ... >);


		template < std::size_t D >
		struct index_deducer{
			template < std::size_t ... KDs >
			struct known_dimensions;

			template < std::size_t, std::size_t ... KDs >
			struct reduced_known_dimensions{
				using type = known_dimensions< KDs ... >;
			};

			template < std::size_t, std::size_t ... KDs >
			struct reduced_fn_ptr{
				using type = std::size_t(*)(type_index, decltype(KDs) ...);
			};

			template < std::size_t ... KDs >
			struct known_dimensions{
				/// \brief Get the index of dimension D
				template < typename ... SubRanges >
				static constexpr std::size_t fn(
					type_index ti,
					decltype(KDs) ... kis
				){
					if constexpr(sizeof...(KDs) > 0){
						return [](type_index ti, std::size_t ki, auto ... kis){
								constexpr std::array<
									typename reduced_fn_ptr< KDs ... >::type,
									sizeof...(SubRanges)
								> list{{
									reduced_known_dimensions< KDs ... >::type
										::template fn_address< SubRanges >() ...
								}};

								return list[ki](ti, kis ...);
							}(ti, kis ...);
					}else{
						constexpr std::array<
							std::pair< type_index, std::size_t >,
							sizeof...(SubRanges)
						> types{{
							std::pair(
								type_index::type_id< typename decltype(
									value_type_of(SubRanges{}))::type >(),
								decltype(+SubRanges{}[numbers::pos(
									hana::size_c< D >)])::value
							) ...
						}};

						std::size_t i = 0;
						for(auto t: types){
							if(ti == t.first) return t.second;
							++i;
						}

						// TODO: Remove the if as soon as GCC BUG is fixed
						if(i == types.size()){
							throw std::out_of_range(
								"type deduction failed; THIS IS A "
								"SERIOUS DISPOSER BUG!");
						}
						return i;
					}
				}

				/// \brief Get the address the corresponding fn
				template < typename Ranges >
				static constexpr auto fn_address(){
					if constexpr(sizeof...(KDs) > 0){
						return hana::unpack(Ranges{}, [](auto ... sub_ranges){
								return &fn< decltype(sub_ranges) ... >;
							});
					}else{
						auto const indexes = hana::cartesian_product(Ranges{});
						return hana::unpack(indexes, [](auto ... index){
								return &fn< decltype(index) ... >;
							});
					}
				}
			};
		};


		/// \brief A function that returns the index of dimension D after a call
		///        with the type index of the known result type and all known
		///        dimension indexes
		template < std::size_t D, std::size_t ... KDs >
		static constexpr auto index_fn =
			index_deducer< D >::template known_dimensions< KDs ... >
				::template fn_address< decltype(sub_ranges< KDs ... >) >();


		/// \brief Get index of dimension D when dimensions KDs are known
		template <
			std::size_t D,
			std::size_t ... KDs >
		static constexpr dimension_index< D > deduce_index(
			hana::size_t< D >,
			type_index const& type_index,
			hana::tuple< dimension_index< KDs > ... > const& known_indexes
		){
			return hana::unpack(known_indexes,
				[&type_index](auto const& ... known_index){
					return dimension_index< D >{index_fn< D, KDs ... >(
						type_index, known_index.i ...)};
				});
		}


	public:
		/// \brief Get all deducible dimensions when dimension KDs are already
		///        known
		template < std::size_t ... KDs >
		static constexpr auto solve(
			type_index const& type_index,
			hana::tuple< dimension_index< KDs > ... > const& known_indexes
		){
			if constexpr(is_deducible< KDs ... >){
				constexpr auto kds = hana::tuple_c< std::size_t, KDs ... >;
				constexpr auto unknown_dims = hana::remove_if(numbers::values,
					[kds](auto d){ return hana::contains(kds, d); });

				return hana::unpack(unknown_dims,
					[type_index, known_indexes](auto ... ds){
						return solved_dimensions{
								deduce_index(ds, type_index, known_indexes) ...
							};
					});
			}else{
				return solved_dimensions{};
			}
		}
	};



}


#endif