
#pragma once

#include <exception>
#include <string>


class SQLException 
    : public std::exception
{
private:
    std::string message;  // Detailed exception message
    std::string query;    // The query that caused the exception
    std::string fullMessage;  // Full message including both the exception message and the query

public:
    SQLException(const std::string& msg = "", const std::string& qry = "", const int errorCode = 0)
        : message(msg), query(qry)
    {
        // Construct the full message combining the exception message and the query
        fullMessage = message + "\nFrom Query: " + query + "\nError Code:" + std::to_string(errorCode);
    }

    virtual const char* what() const noexcept override
    {
        return fullMessage.c_str();
    }

    // Getter function for the query string
    const std::string& getQuery() const
    {
        return query;
    }
};
