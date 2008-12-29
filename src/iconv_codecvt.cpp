// iconv_codecvt.cpp -- Charset conversions
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

#include <algorithm>
#include <locale>
#include <cerrno>
#include <cstring>

#include <iconv.h>

#include "iconv_codecvt.hpp"

using namespace std;


// Internal representation charset:
const char* iconv_codecvt::internal_charset = "WCHAR_T";


// Constructing and destroying:

iconv_codecvt::iconv_codecvt(const char* input_charset, const char* output_charset, size_t refs):
  codecvt<wchar_t, char, mbstate_t>(refs)
{
  m_in_cd = (input_charset && strlen(input_charset)) ?
    iconv_open(internal_charset, input_charset) :
    reinterpret_cast<iconv_t>(-1);
  m_out_cd = (output_charset && strlen(output_charset)) ?
    iconv_open(output_charset, internal_charset) :
    reinterpret_cast<iconv_t>(-1);
}

iconv_codecvt::~iconv_codecvt(void)
{
  if (m_in_cd && (m_in_cd != reinterpret_cast<iconv_t>(-1)))
    iconv_close(m_in_cd);
  if (m_out_cd && (m_out_cd != reinterpret_cast<iconv_t>(-1)))
    iconv_close(m_out_cd);
}


// Actual functionality implementation:

int
iconv_codecvt::do_encoding(void) const throw()
{
  return sizeof(wchar_t) / sizeof(char);
}

bool
iconv_codecvt::do_always_noconv(void) const throw()
{
  return false;
}

int iconv_codecvt::do_max_length(void) const throw()
{
  return 1;
}

int
iconv_codecvt::do_length(mbstate_t& state,
                         const char* from, const char* end, size_t max) const
{
  return std::min(static_cast<size_t>(end - from), max);
}

codecvt_base::result
iconv_codecvt::do_in(mbstate_t& state,
                     const char* from, const char* from_end, const char*& from_next,
                     wchar_t* to, wchar_t* to_end, wchar_t*& to_next) const
{
  return convert(m_in_cd, from, from_end, from_next, to, to_end, to_next);
}

codecvt_base::result
iconv_codecvt::do_out(mbstate_t& state,
                      const wchar_t* from, const wchar_t* from_end, const wchar_t*& from_next,
                      char* to, char* to_end, char*& to_next) const
{
  return convert(m_out_cd, from, from_end, from_next, to, to_end, to_next);
}

codecvt_base::result
iconv_codecvt::do_unshift(mbstate_t& state,
                          char* to, char* to_end, char*& to_next) const
{
  const wchar_t* dummy = NULL;
  return convert(m_out_cd, dummy, dummy, dummy, to, to_end, to_next);
}


// Iconv wrapper:

template<typename source_type, typename target_type>
codecvt_base::result
iconv_codecvt::convert(const iconv_t& m_cd,
                       const source_type* from,
                       const source_type* from_end,
                       const source_type*& from_next,
                       target_type* to,
                       target_type* to_end,
                       target_type*& to_next)
{
  codecvt_base::result retval = codecvt_base::error;
  size_t src_length = (from_end - from) * sizeof(source_type);
  size_t dest_maxsize = (to_end - to) * sizeof(target_type);
  from_next=from;
  to_next=to;
  if (m_cd && (m_cd != reinterpret_cast<iconv_t>(-1)))
    {
      errno=0;
      if (iconv(m_cd, (char**)&from_next, &src_length,
                (char**)&to_next, &dest_maxsize)
          == (size_t)-1)
        switch (errno)
          {
          case EINVAL:
            retval = codecvt_base::partial;
            break;
          case E2BIG:
            retval = codecvt_base::partial;
            break;
          default:
            retval = codecvt_base::error;
            break;
          }
      else retval = codecvt_base::ok;
    }
  return retval;
}
