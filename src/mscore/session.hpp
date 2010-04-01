// session.hpp -- General Multispeech session framework
/*
   Copyright (C) 2008 Igor B. Poretsky <poretsky@mlbox.ru>
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

// The session class provides general client interaction means.
// It is not for direct use, but rather for deriving actual
// session implementations. Derived class must implement pure
// virtual method perform() that will be used for parsing and
// executing client requests coming from the input source referenced
// by the constructor argument.

#ifndef MULTISPEECH_SESSION_HPP
#define MULTISPEECH_SESSION_HPP

#include <istream>
#include <string>

namespace multispeech
{

class session
{
protected:
  // Object constructor:
  explicit session(std::istream& client);

public:
  // Run the session loop:
  void operator()(void);

private:
  // Input source:
  std::istream& source;

  // Get a request string from client. Return true if success
  // or false if input source is exhausted.
  bool input(std::string& request);

  // Actual command parser must be implemented in derived class.
  // Return true to continue the session or false otherwise.
  virtual bool perform(std::string& request) = 0;
};

} // namespace multispeech

#endif
