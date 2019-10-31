// pipeline.hpp -- Process pipelines construction and control interface
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

// The class pipeline provides some infrastructure necessary
// for assembling and controlling pipelines from the shell commands.
// Special data type script is defined for the command set specifying.
// If standard output of the constructed pipeline is of interest
// it can be attached to a pipeline::consumer class object.
// The pipeline standard error stream is discarded.

#ifndef MULTISPEECH_PIPELINE_HPP
#define MULTISPEECH_PIPELINE_HPP

#include <string>
#include <ostream>
#include <queue>
#include <stack>

#include <boost/scoped_ptr.hpp>

#include <bobcat/fork>
#include <bobcat/pipe>
#include <bobcat/ofdstreambuf>

class pipeline:
  public std::ostream,
  private FBB::Fork
{
public:
  // Command set container. Commands are to be added by natural
  // order (from left to right) by the push() method.
  typedef std::stack<std::string> script;

  // Pipeline output consumer functor framework. Actual consumers
  // should be derived from this class and must provide implementation
  // for virtual method attach(). This method takes pipeline standard
  // output file descriptor as it's argument. This file descriptor is
  // opened for read by pipeline::run() and closed by pipeline::stop().
  class consumer
  {
  protected:
    consumer(void);
    ~consumer(void);

  public:
    virtual void attach(int fd) = 0;
  };

  // Construct / destroy:
  pipeline(void);
  ~pipeline(void);

  // Construct and run the pipeline. The first argument specifies
  // command set. The second one points to the pipeline standard
  // output destination if needed.
  void run(const script& task, consumer* sink_ptr = NULL);

  // Stop process immediately:
  void stop(void);

  // Finish pipeline building (flush buffered data if any and close connection):
  void complete(void);

  // Wait for the pipeline to complete execution:
  void wait(void);

  // Return true if process is running:
  bool active(void);

private:
  // Underlying methods of the Fork base class:
  void childProcess(void);
  void childRedirections(void);
  void parentProcess(void);
  void parentRedirections(void);

  // All started processes:
  std::queue<pid_t> children;

  // Current pipeline input file descriptor:
  int feeder;

  // Data streams control means:
  boost::scoped_ptr<FBB::OFdStreambuf> input;
  boost::scoped_ptr<FBB::Pipe> connector, outlet;

  // Temporary command storage:
  script commands;

  // Pipeline standard output consumer pointer:
  consumer* output_destination;
};

#endif
