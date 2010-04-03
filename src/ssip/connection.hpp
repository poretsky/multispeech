// connection.hpp -- Client connection interface
/*
   Copyright (C) 2010 Igor B. Poretsky <poretsky@mlbox.ru>
   This file is part of Multispeech.

   Multispeech is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   Multispeech is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Multispeech; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  
*/

// This class is one of the session ancestors and it's purpose
// is to provide iostream interface to the socket passed
// to the constructor as an argument.

#ifndef MULTISPEECH_SSIP_CONNECTION_HPP
#define MULTISPEECH_SSIP_CONNECTION_HPP

#include <boost/shared_ptr.hpp>

#include <bobcat/ifdstream>
#include <bobcat/ofdstream>

namespace SSIP
{

class connection
{
protected:
  // Object constructor:
  explicit connection(int fd);

  // Input and output stream are created as dynamic objects.
  boost::shared_ptr<FBB::IFdStream> input;
  boost::shared_ptr<FBB::OFdStream> output;
};

} // namespace SSIP

#endif
