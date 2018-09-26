#ifndef INCLUDE_MCU_SERVEICE_H
#define INCLUDE_MCU_SERVEICE_H

#include <string>
#include <vector>

#include <RCF/RCF.hpp>
#include <SF/vector.hpp>

RCF_BEGIN(I_DATACOLLECT, "I_DATACOLLECT")
RCF_METHOD_V1(void, send_vdata_2_host, const std::vector<unsigned char> &)
RCF_END(I_DATACOLLECT)

RCF_BEGIN(I_DATADISPATCH, "I_DATADISPATCH")
RCF_METHOD_V1(void, dispatch_vdata_to_client, std::vector<unsigned char> &)
RCF_END(I_DATADISPATCH)

#endif // ! INCLUDE_MCU_SERVEICE_H
