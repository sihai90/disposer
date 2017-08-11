//-----------------------------------------------------------------------------
// Copyright (c) 2017 Benjamin Buch
//
// https://github.com/bebuch/disposer
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _disposer__core__module_data__hpp_INCLUDED_
#define _disposer__core__module_data__hpp_INCLUDED_

#include "accessory.hpp"

#include <boost/hana/for_each.hpp>
#include <boost/hana/unpack.hpp>
#include <boost/hana/slice.hpp>

#include <unordered_map>
#include <functional>


namespace disposer{


	using output_name_to_ptr_type
		= std::unordered_map< std::string, output_base* >;


	struct input_name_tag;
	struct output_name_tag;
	struct parameter_name_tag;


	/// \brief Accessory of a \ref module without log
	template < typename List >
	class module_data{
	public:
		/// \brief Constructor
		template < typename MakerList, typename MakeData, std::size_t ... I >
		module_data(
			MakerList const& maker_list,
			MakeData const& data,
			std::string_view location,
			std::index_sequence< I ... >
		)
			: list_(iops_make_data(
				iop_make_data(maker_list[hana::size_c< I >], data, location),
				location,
				hana::slice_c< 0, I >(detail::as_ref_list(list_))) ...)
		{
			(void)location; // GCC bug (silance unused warning)
		}


		/// \brief Get reference to an input-, output- or parameter-object via
		///        its corresponding compile time name
		template < typename Name >
		auto& operator()(Name const& name)noexcept{
			return extract(list_, name);
		}

		/// \brief Get reference to an input-, output- or parameter-object via
		///        its corresponding compile time name
		template < typename Name >
		auto const& operator()(Name const& name)const noexcept{
			return extract(list_, name);
		}


		output_name_to_ptr_type output_name_to_ptr(){
			output_name_to_ptr_type map;
			hana::for_each(detail::as_ref_list(list_), [&map](auto ref){
					auto const is_output
						= hana::is_a< output_name_tag >(ref.get().name);
					if constexpr(is_output){
						map.emplace(
							detail::to_std_string(ref.get().name),
							&ref.get());
					}
				});
			return map;
		}


	private:
		template < typename L, typename Name >
		static auto& extract(L& list, Name const& name)noexcept{
			using name_t = std::remove_reference_t< Name >;
			static_assert(
				hana::is_a< input_name_tag, name_t > ||
				hana::is_a< output_name_tag, name_t > ||
				hana::is_a< parameter_name_tag, name_t >,
				"name is not an input_name, output_name or parameter_name");
			return detail::extract(detail::as_ref_list(list), name);
		}

		/// \brief hana::tuple of the inputs, outputs and parameters
		List list_;
	};


}


#endif
