#include "loop.h"

Loop *Loop::_instance = nullptr;
std::list<Loop::cb_t> Loop::_callbacks;
