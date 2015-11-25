#ifndef EXC_H
#define EXC_H

#include <exception>

class exc: public std::exception
{
    const char* what() const throw();
};

#endif // EXC_H
