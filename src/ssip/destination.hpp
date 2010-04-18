// destination.hpp -- SSIP request destination parameter parser interface
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

#ifndef MULTISPEECH_SSIP_DESTINATION_HPP
#define MULTISPEECH_SSIP_DESTINATION_HPP

#include <string>

#include <boost/regex.hpp>

#include <bobcat/cmdfinder>

namespace SSIP
{

class destination: private FBB::CmdFinder<void (destination::*)(void)>
{
public:
  // Available choices:
  enum choice
  {
    self,
    another,
    all,
    invalid
  };

  // Main constructor.
  destination(void);

  // Parse the request destination field. Usually it is used after
  // general command dispatching. Being provided by the request
  // tail as an argument it checks the first field as a destination
  // parameter and returns reference to following part of request.
  const std::string& parse(const std::string& request);

  // Return destination choice after parsing.
  choice selection(void) const;

  // Return destination id after parsing.
  // Meaningful only when selection is another.
  unsigned long id(void) const;

private:
  // Internal checkers.
  void check_self(void);
  void check_all(void);
  void check_another(void);

  // Parsing results:
  choice state;
  unsigned long value;

  // Table of recognized requests.
  static const Entry table[];

  // Id validator.
  static const boost::regex validator;
};

} // namespace SSIP

#endif
