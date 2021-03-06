//-----------------------------------------------------------------------------
// Copyright (c) 2015-2018 Benjamin Buch
//
// https://github.com/bebuch/disposer
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _disposer__core__exec_output__hpp_INCLUDED_
#define _disposer__core__exec_output__hpp_INCLUDED_

#include "exec_output_base.hpp"
#include "output_map_type.hpp"
#include "output.hpp"

#include "../tool/input_data.hpp"
#include "../tool/to_std_string_view.hpp"

#include <io_tools/make_string.hpp>

#include <logsys/log_base.hpp>

#include <boost/hana/tuple.hpp>

#include <functional>
#include <variant>


namespace disposer{


	/// \brief The output type while exec
	template < typename T >
	class unnamed_exec_output
		: public exec_output_base
		, public logsys::log_base{
	public:
		/// \brief The actual type
		using type = T;


		/// \brief Constructor
		unnamed_exec_output(
			std::size_t const id,
			std::string&& log_prefix,
			std::string_view name,
			std::size_t use_count
		)noexcept
			: exec_output_base(use_count)
			, logsys::log_base(io_tools::make_string("id(", id, ") ",
				std::move(log_prefix), "output(", name, ") ")) {}


		/// \brief Add given data to \ref data_
		template < typename ... Args >
		void emplace(Args&& ... args){
			data_.emplace_back(static_cast< Args&& >(args) ...);
		}

		/// \brief Add given data to \ref data_
		template < typename Arg >
		void push(Arg&& value){
			data_.push_back(static_cast< Arg&& >(value));
		}


		/// \brief Get a view to the data
		input_data_r< T > references()const{
			return data_;
		}

		/// \brief Get a reference to the data
		input_data_v< T > values()const{
			if(is_last_use()){
				return std::move(data_);
			}else{
				return data_;
			}
		}

		/// \brief Remove data on last cleanup call
		void cleanup()noexcept{
			if(is_cleanup()){
				log(
					[](logsys::stdlogb& os){
						os << "cleanup";
					},
					[this]{
						data_.clear();
					});
			}
		}


	private:
		/// \brief Putted data of the output
		std::vector< T > data_;
	};


	template < typename Name, typename T >
	struct exec_output_init_data{
		exec_output_init_data(
			disposer::output< Name, T >& output,
			output_map_type& output_map,
			std::size_t const id,
			std::string&& module_log_prefix
		)noexcept
			: output(output)
			, output_map(output_map)
			, id(id)
			, module_log_prefix(std::move(module_log_prefix)) {}

		disposer::output< Name, T >& output;
		output_map_type& output_map;
		std::size_t id;
		std::string module_log_prefix;
	};

	/// \brief The output type while exec
	template < typename Name, typename T >
	class exec_output: public unnamed_exec_output< T >{
	public:
		/// \brief Constructor
		exec_output(exec_output_init_data< Name, T >&& data)noexcept
			: unnamed_exec_output< T >(
				std::move(data.id),
				std::move(data.module_log_prefix),
				detail::to_std_string_view(name),
				data.output.use_count()
			)
		{
			data.output_map.emplace(&data.output, this);
		}

		/// \brief Compile time name of the output
		using name_type = Name;

		/// \brief Name object
		static constexpr auto name = name_type{};

		using unnamed_exec_output< T >::unnamed_exec_output;
	};


}


#endif
