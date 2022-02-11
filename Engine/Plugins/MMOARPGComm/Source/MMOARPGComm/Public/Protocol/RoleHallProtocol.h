#pragma once

#include "SimpleProtocolsDefinition.h" // Plugin: SimpleNetChannel

DEFINITION_SIMPLE_PROTOCOLS(CharacterAppearancesRequests, 4);
DEFINITION_SIMPLE_PROTOCOLS(CharacterAppearancesResponses, 5);

DEFINITION_SIMPLE_PROTOCOLS(CheckCharacterNameRequests, 6);
DEFINITION_SIMPLE_PROTOCOLS(CheckCharacterNameResponses, 7);

DEFINITION_SIMPLE_PROTOCOLS(CreateCharacterRequests, 8);
DEFINITION_SIMPLE_PROTOCOLS(CreateCharacterResponses, 9);

DEFINITION_SIMPLE_PROTOCOLS(LoginToDSServerRequests, 10);
DEFINITION_SIMPLE_PROTOCOLS(LoginToDSServerResponses, 11);