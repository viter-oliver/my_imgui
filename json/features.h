#ifndef CPPTL_JSON_FEATURES_H_INCLUDED
#define CPPTL_JSON_FEATURES_H_INCLUDED
#include "forwards.h"

namespace Json {

class JSON_API Features
{
public:
    static Features all();
    static Features strictMode();
    Features();
    bool allowComments_;
    bool strictRoot_;
};
} // namespace Json
#endif // CPPTL_JSON_FEATURES_H_INCLUDED