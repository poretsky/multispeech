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

  // Request parsing takes place at the object construction stage.
  // The main constructor should be provided by the reference
  // to the request tail beginning from the destination field.
  destination(const std::string& request);

  // Return reference to the request tail after the
  // destination field for further processing.
  const std::string& beyond(void) const;

  // Return destination choice.
  choice selection(void) const;

  // Return destination session id.
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
