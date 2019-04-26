/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : MpdConnectionException
 * @created     : Saturday Jun 01, 2019 12:53:12 UTC
 */

#include "MpdConnectionException.h"

MpdConnectionException::MpdConnectionException(){
    m_message = new std::string("Connection Error");
}

MpdConnectionException::MpdConnectionException(std::string &msg){
    m_message = &msg;
}

std::string MpdConnectionException::message(){
    return *m_message;
}
