//-----------------------------------------------------------------------------
// Copyright (c) 2015 Benjamin Buch
//
// https://github.com/bebuch/disposer
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)
//-----------------------------------------------------------------------------
#ifndef _disposer_config_hpp_INCLUDED_
#define _disposer_config_hpp_INCLUDED_

#include "chain.hpp"


namespace disposer{ namespace config{


	chain_list load(std::string const& filename);


} }


#endif