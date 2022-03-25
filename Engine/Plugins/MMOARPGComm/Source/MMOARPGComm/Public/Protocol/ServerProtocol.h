#pragma once

#include "SimpleProtocolsDefinition.h" // Plugin: SimpleNetChannel

DEFINITION_SIMPLE_PROTOCOLS(GateStatusRequests, 2);
DEFINITION_SIMPLE_PROTOCOLS(GateStatusResponses, 3);

DEFINITION_SIMPLE_PROTOCOLS(PlayerRegisterInfoRequests, 12);
DEFINITION_SIMPLE_PROTOCOLS(PlayerRegisterInfoResponses, 13);

DEFINITION_SIMPLE_PROTOCOLS(PlayerQuitRequests, 14);
DEFINITION_SIMPLE_PROTOCOLS(PlayerQuitResponses, 15);

DEFINITION_SIMPLE_PROTOCOLS(UpdateCharacterGameplayDataRequests, 22);
DEFINITION_SIMPLE_PROTOCOLS(UpdateCharacterGameplayDataResponses, 23);
