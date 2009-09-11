/*
 * Copyright (c) 2008, Willow Garage, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Willow Garage, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* Author: Brian Gerkey */

/*
 * Publish a message N times, back to back
 */

#include <string>

#include <time.h>
#include <stdlib.h>

#include "ros/node.h"
#include <test_roscpp/TestArray.h>

class Publisher : public ros::Node
{
  public:
    Publisher(std::string n, uint32_t options) :
            ros::Node(n, options), connected(false) {}
    void sub_cb(const ros::SingleSubscriberPublisher&)
    {
      // It would be nice to publish here, but that doesn't seem to work
      // (causes the program to hang).
      connected = true;
    }
    void cb()
    {
      inmsg.counter++;
      publish("test_roscpp/pubsub_test", inmsg);
    }

    bool connected;
    test_roscpp::TestArray outmsg;
    test_roscpp::TestArray inmsg;
};

#define USAGE "USAGE: publish_n_fast {thread | nothread} <sz>"

int
main(int argc, char** argv)
{
  ros::init(argc, argv);

  if(argc != 3)
  {
    puts(USAGE);
    exit(-1);
  }

  Publisher* p;
  bool thread = true;
  if(!strcmp(argv[1],"nothread"))
  {
    p = new Publisher("publisher",ros::Node::DONT_START_SERVER_THREAD);
    thread = false;
  }
  else
    p = new Publisher("publisher",0);

  int sz = atoi(argv[2]);


  p->advertise("test_roscpp/pubsub_test", p->outmsg, &Publisher::sub_cb, 1);
  p->subscribe("test_roscpp/subpub_test", p->inmsg, &Publisher::cb, 1);

  bool published = false;
  // Loop until Ctrl-C is given (by rostest)
  while(p->ok())
  {
    // Sleep for 10ms
    if(thread)
    {
      struct timespec sleep_time = {0, 10000000};
      nanosleep(&sleep_time,NULL);
    }
    else
      p->tcprosServerUpdate();

    // Did we get a connection (and is this the first time)?
    if(p->connected && !published)
    {
      p->outmsg.counter=0;
      p->outmsg.set_float_arr_size(sz);
      p->publish("test_roscpp/pubsub_test", p->outmsg);
      published = true;

      // Don't quit here, because the message might not be out of our
      // outbox yet.  Instead rely on rostest to shut us down.
    }
  }


  delete p;
}
