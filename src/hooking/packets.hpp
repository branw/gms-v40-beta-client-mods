#ifndef MAPLE_8F77A64D305E4289AEED008B80A26037_HPP
#define MAPLE_8F77A64D305E4289AEED008B80A26037_HPP

#include <Windows.h>
#include <cstdint>

void install_packet_breakpoints();

bool handle_out_packet_hooks(uintptr_t addr, PCONTEXT ctx);

bool handle_in_packet_hooks(uintptr_t addr, PCONTEXT ctx);

#endif//MAPLE_8F77A64D305E4289AEED008B80A26037_HPP
