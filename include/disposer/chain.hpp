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
#include "merge.hpp"

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
		/// \param config_chain configuration data from config file
		/// \param generate_id Reference to a id_generator
		chain(
			module_maker_list const& maker_list,
			types::merge::chain const& config_chain,
			id_generator& generate_id
		);


		/// \brief Disable all modules and destruct chain
		~chain();


		/// \brief Chaines are not copyable
		chain(chain const&) = delete;

		/// \brief Chaines are not movable
		chain(chain&&) = delete;


		/// \brief Chaines are not copyable
		chain& operator=(chain const&) = delete;

		/// \brief Chaines are not movable
		chain& operator=(chain&&) = delete;


		/// \brief Execute the proccess chain
		///
		/// The chain must be enabled, otherwise an exception is thrown.
		///
		/// 1 A new id is generated by the id_generator.
		/// 2 The first module is executed
		/// 3.1 Outputs are mapped to inputs of later modules
		/// 3.2 Next module is executed
		/// 3.3 if not last module then back to 3.1
		///
		/// If a module throws an exception,
		void exec();


		/// \brief Enables the chain for exec calls
		///
		/// The modules can load and init resources.
		void enable();

		/// \brief Disables the chain for exec calls
		///
		/// The modules can unload and uninit resources.
		void disable()noexcept;


		/// \brief Name of the chain
		std::string const name;


	private:
		/// \brief Handles the exec and the cleanup of a module
		template < typename F >
		void process_module(
			std::size_t const i,
			std::size_t const run,
			F const& action,
			std::string_view action_name
		);


		/// \brief List of modules
		std::vector< module_ptr > const modules_;


		/// \brief Referenz to the id_generator
		id_generator& generate_id_;


		/// \brief Count of exec() calls
		std::atomic< std::size_t > next_run_;

		/// \brief One entry per module, value is the last run id
		///
		/// The run id is generated by next_run_ in exec()
		std::vector< std::size_t > ready_run_;


		/// \brief One mutex per module
		std::vector< std::mutex > module_mutexes_;

		/// \brief Manages the module_mutexes_
		std::condition_variable module_cv_;


		/// \brief Mutex for enable and disable
		std::mutex enable_mutex_;

		/// \brief true after successfull enable() call
		///
		/// Call disable() to set it to false.
		std::atomic< bool > enabled_;

		/// \brief Count of running exec() calls
		std::atomic< std::size_t > exec_calls_count_;

		/// \brief Manages exec() and enable() / disable() calls
		std::condition_variable enable_cv_;
	};


	/// \brief A resource guard for chain enable/disable
	class chain_enable_guard{
	public:
		/// \brief Calls enable on the chain object
		chain_enable_guard(chain& c)
			: chain_(c)
		{
			chain_.enable();
		}

		/// \brief Calls disable on the chain object
		~chain_enable_guard(){
			chain_.disable();
		}


	private:
		/// \brief The chain object
		chain& chain_;
	};


}


#endif
