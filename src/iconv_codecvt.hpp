// iconv_codecvt.hpp -- Charset conversion interface
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

#ifndef MULTISPEECH_ICONV_CODECVT_HPP
#define MULTISPEECH_ICONV_CODECVT_HPP

#include <locale>

#include <iconv.h>

class iconv_codecvt: public std::codecvt<wchar_t, char, std::mbstate_t>
{
public:
  // Constructing and destroying:
  iconv_codecvt(const char* input_charset, const char* output_charset, size_t refs = 0);
  virtual ~iconv_codecvt(void);

protected:
  // Actual workers:
  int do_encoding(void) const throw();
  bool do_always_noconv(void) const throw();
  int do_length(std::mbstate_t& state, const char* from, const char* end, size_t max) const;
  int do_max_length(void) const throw();

  std::codecvt_base::result
  do_in(std::mbstate_t& state,
        const char* from, const char* from_end, const char*& from_next,
        wchar_t* to, wchar_t* to_end, wchar_t*& to_next) const;

  std::codecvt_base::result
  do_out(std::mbstate_t& state,
         const wchar_t* from, const wchar_t* from_end, const wchar_t*& from_next,
         char* to, char* to_end, char*& to_next) const;

  std::codecvt_base::result
  do_unshift(std::mbstate_t& state,
             char* to, char* to_end, char*& to_next) const;

private:
  // Private data:
  static const char* internal_charset;
  iconv_t m_in_cd, m_out_cd;

  // Iconv wrapper:
  template<typename source_type, typename target_type>
  codecvt_base::result
  static convert(const iconv_t& m_cd,
                 const source_type* from,
                 const source_type* from_end,
                 const source_type*& from_next,
                 target_type* to,
                 target_type* to_end,
                 target_type*& to_next);
};

#endif
