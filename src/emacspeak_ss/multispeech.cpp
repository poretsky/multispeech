// multispeech.cpp -- Multilingual speech server
/*
   Copyright (C) 2008 Igor B. Poretsky <poretsky@mlbox.ru>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  
*/

#include <iostream>

#include "speech_server.hpp"
#include "multispeech.hpp"
#include "frontend.hpp"

using namespace std;

class application: public multispeech
{
private:
  speech_server* bootstrap(void)
  {
    return new frontend;
  }

  bool is_spd(void)
  {
    return false;
  }

  void report_error(const string& msg)
  {
    cerr << msg << endl;
  }
};

int main(int argc, char* argv[])
{
  application app;
  return app.execute(argc, argv);
}
