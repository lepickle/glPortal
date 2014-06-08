#include "TcpSession.hpp"
#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <fstream>
#include <string>
#include <boost/filesystem.hpp>


namespace glPortal {
  TcpSession::TcpSession(boost::asio::io_service& io_service): socket(io_service){}

  tcp::socket& TcpSession::getSocket(){
    return socket;
  }

  void TcpSession::setController(std::shared_ptr<Controller> controller){
    this->controller = controller;
  }
  
  void TcpSession::start(){
    socket.async_read_some(boost::asio::buffer(data, max_length),
                            boost::bind(&TcpSession::handle_read, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
  }

  void TcpSession::handle_read(const boost::system::error_code& error,
                               size_t bytes_transferred)
  {

    std::string message(data, data + bytes_transferred);
    std::string response = controller->setMessage(message);

    if(!error) {
      boost::asio::async_write(socket,
                               boost::asio::buffer(response, response.size() + bytes_transferred),
                               boost::bind(&TcpSession::handle_write, this,
                                           boost::asio::placeholders::error));
    } else {
      delete this;
    }
  }

  void TcpSession::handle_write(const boost::system::error_code& error)
  {
    if (!error){
      socket.async_read_some(boost::asio::buffer(data, max_length),
                              boost::bind(&TcpSession::handle_read, this,
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred));
    } else {
      delete this;
    }
  }
}