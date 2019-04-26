/**
 * @author      : Sam Bell (sam@mimpidev.com)
 * @file        : MpdConnectionException
 * @created     : Saturday Jun 01, 2019 12:41:46 UTC
 */

#ifndef MPDCONNECTIONEXCEPTION_H

#define MPDCONNECTIONEXCEPTION_H
#include <exception>
#include <string>


class MpdConnectionException : public std::exception {
    public:
        MpdConnectionException();
        MpdConnectionException(std::string &msg);
        std::string message();
    private:
        std::string *m_message;
};

#endif /* end of include guard MPDCONNECTIONEXCEPTION_H */

