//-----------------------------------------------------------------------------
// Copyright (c) 2017-2018 Benjamin Buch
//
// https://github.com/bebuch/disposer
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _disposer__tool__type_list_as_string__hpp_INCLUDED_
#define _disposer__tool__type_list_as_string__hpp_INCLUDED_

#include "ct_pretty_name.hpp"

#include <sstream>


namespace disposer{


	inline std::string type_list_as_string(
		std::unordered_set< type_index > const& list
	){
		std::ostringstream os;
		auto iter = begin(list);
		auto const e = end(list);
		if(iter != e){
			os << '[' << type_print(*iter) << ']';
			++iter;
		}
		for(; iter != e; ++iter){
			os << ", [" << type_print(*iter) << ']';
		}
		return os.str();
	}


}


#endif
