// pipeline.cpp -- Process pipelines construction and control implementation
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

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include <string>
#include <ostream>
#include <vector>
#include <memory>

#include <bobcat/string>
#include <bobcat/redirector>

#include "pipeline.hpp"

using namespace std;
using namespace FBB;


// Construct / destroy:

pipeline::pipeline(void):
  ostream(0),
  output_destination(NULL)
{
}

pipeline::~pipeline(void)
{
  stop();
  wait();
}


// Public methods:

void
pipeline::run(const script& task, consumer* sink_ptr)
{
  commands = task;
  output_destination = sink_ptr;
  connector.reset(new Pipe);
  if (output_destination)
    outlet.reset(new Pipe);
  fork();
}

void
pipeline::stop(void)
{
  if (!children.empty())
    {
      kill(-children.front(), SIGTERM);
      kill(children.front(), SIGTERM);
    }
  if (output_destination)
    {
      close(outlet->readFd());
      output_destination = NULL;
    }
}

void
pipeline::complete(void)
{
  close(feeder);
  rdbuf(0);
  input.reset();
}

void
pipeline::wait(void)
{
  while (!children.empty())
    {
      waitpid(children.front(), NULL, 0);
      children.pop();
    }
}

bool
pipeline::active(void)
{
  if (!children.empty() &&
      (waitpid(children.front(), NULL, WNOHANG) == children.front()))
    {
      children.pop();
      wait();
    }
  return !children.empty();
}


// Private methods:

void
pipeline::childProcess(void)
{
  vector<string> parsed_command;
  String::split(&parsed_command, commands.top());
  auto_ptr<const char*> argv(String::argv(parsed_command));
  execvp(argv.get()[0], const_cast<char* const *>(argv.get()));
}

void
pipeline::childRedirections(void)
{
  int garbage = open("/dev/null", O_WRONLY);
  Redirector sink(garbage);
  connector->readOnly();
  connector->readFrom(STDIN_FILENO);
  if (!children.empty())
    {
      Redirector output(feeder);
      output.swallow(STDOUT_FILENO);
      close(feeder);
    }
  else if (output_destination)
    {
      outlet->writeOnly();
      outlet->writtenBy(STDOUT_FILENO);
    }
  else sink.swallow(STDOUT_FILENO);
  sink.swallow(STDERR_FILENO);
  close(garbage);
}

void
pipeline::parentProcess(void)
{
  children.push(pid());
  setpgid(pid(), children.front());
  commands.pop();
  if (!commands.empty())
    {
      connector.reset(new Pipe);
      fork();
    }
}

void
pipeline::parentRedirections(void)
{
  feeder = connector->writeOnly();
  if (children.empty())
    {
      input.reset(new OFdStreambuf(feeder, OFdStreambuf::CLOSE_FD));
      rdbuf(input.get());
      if (output_destination)
        output_destination->attach(outlet->readOnly());
    }
  else input->open(feeder);
}


// The pipeline::consumer class members:

pipeline::consumer::consumer(void)
{
}

pipeline::consumer::~consumer(void)
{
}
