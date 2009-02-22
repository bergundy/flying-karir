//
// async_udp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;
enum { max_length = 1024 };

class server
{
public:
  server(boost::asio::io_service& io_service, short port)
    : io_service_(io_service),
      socket_(io_service, udp::endpoint(udp::v4(), port))
  {
    socket_.async_receive_from(
        boost::asio::buffer(data_, max_length), sender_endpoint_,
        boost::bind(&server::handle_receive_from, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }

  void handle_receive_from(const boost::system::error_code& error,
      size_t bytes_recvd)
  {
    if (!error && bytes_recvd > 0)
    {
      socket_.async_send_to(
          boost::asio::buffer(data_, bytes_recvd), sender_endpoint_,
          boost::bind(&server::handle_send_to, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
    else
    {
      socket_.async_receive_from(
          boost::asio::buffer(data_, max_length), sender_endpoint_,
          boost::bind(&server::handle_receive_from, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
  }

  void handle_send_to(const boost::system::error_code& error, size_t bytes_sent)
  {
    socket_.async_receive_from(
        boost::asio::buffer(data_, max_length), sender_endpoint_,
        boost::bind(&server::handle_receive_from, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }

private:
  boost::asio::io_service& io_service_;
  udp::socket socket_;
  udp::endpoint sender_endpoint_;
  char data_[max_length];
};

class client
{
  client(boost::asio::io_service& io_service, char* ip, short port)
  : io_service_(io_service),
    socket_(io_service, udp::endpoint(udp::v4(), 0))
  {
    udp::resolver resolver(io_service);
    udp::resolver::query query(udp::v4(), ip, port);
    iter_ = resolver.resolve(query);
    socket_.async_receive_from(
        boost::asio::buffer(data_, max_length), endpoint_,
        boost::bind(&server::handle_receive_from, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));

  }
  
  void snd(char* request)
  {
    size_t request_length = std::strlen(request);
    socket_.send_to(boost::asio::buffer(request, request_length), *iter_);
  }
  
  char* rcv()
  {
    char reply[max_length];
    udp::endpoint sender_endpoint;
    size_t reply_length = socket_.receive_from(
        boost::asio::buffer(reply, max_length), sender_endpoint);




private:
    boost::asio::io_service& io_service_;
    udp::socket socket_;
    udp::resolver::iterator iter_;

}; 


int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: async_udp_echo_server <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    using namespace std; // For atoi.
    server s(io_service, atoi(argv[1]));

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

