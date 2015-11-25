#include "exc.h"

const char* exc::what() const throw()
{
    return "\n\nDequeing from an empty queue!\n\n";
}
