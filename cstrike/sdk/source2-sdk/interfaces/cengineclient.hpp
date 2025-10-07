#pragma once

#include "../../virtual.hpp"

class CEngineClient {
   public:
    auto IsInGame() { return CALL_VIRTUAL(bool, 35U, this); }
    auto IsConnected() { return CALL_VIRTUAL(bool, 36U, this); }
};
