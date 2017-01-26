//-----------------------------------------------------------------------------
// Copyright (c) 2015-2017 Benjamin Buch
//
// https://github.com/bebuch/disposer
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _disposer__chain__hpp_INCLUDED_
#define _disposer__chain__hpp_INCLUDED_

#include "module_ptr.hpp"
#include "id_generator.hpp"
#include "log_base.hpp"
#include "log.hpp"


#include <mutex>
#include <string>
#include <vector>
#include <condition_variable>


namespace disposer{


	/// \brief A process chain
	///
	/// Properties:
	/// - no 2 identical modules (in different executions) must running
	///   simultaneously
	/// - it must not be overtaken
	class chain{
	public:
		/// \brief Construct a proccess chain
		///
		/// \param modules A list of modules
		/// \param generate_id Reference to a id_generator
		/// \param name The name of the proccess chain
		/// \param group A reference to the group name
		///
		/// The id increase for the id_generator is calculated over all modules.
		chain(
			std::vector< module_ptr >&& modules,
			id_generator& generate_id,
			std::string const& name,
			std::string const& group
		);

		chain(chain const&) = delete;
		chain(chain&&) = delete;

		chain& operator=(chain const&) = delete;
		chain& operator=(chain&&) = delete;


		/// \brief Execute the proccess chain
		///
		/// 1 A new id is generated by the id_generator.
		/// 2 The first module is executed
		/// 3.1 Outputs are mapped to inputs of later modules
		/// 3.2 Next module is executed
		/// 3.3 if not last module then back to 3.1
		///
		/// If a module throws an exception,
		void exec();


		std::string const name;

		std::string const& group;


	private:
		template < typename F >
		void process_module(
			std::size_t i,
			std::size_t run,
			F const& action,
			char const* action_name
		);

		std::vector< module_ptr > modules_;

		std::size_t id_increase_;
		id_generator& generate_id_;

		std::atomic< std::size_t > next_run_;
		std::vector< std::size_t > ready_run_;

		std::vector< std::mutex > mutexes_;
		std::condition_variable cv_;
	};


}


#endif
