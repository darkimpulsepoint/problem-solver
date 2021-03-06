#pragma once

#include <sc-memory/cpp/kpm/sc_agent.hpp>

#include "keynodes/keynodes.hpp"
#include "IsomorphicSearchAgent.generated.hpp"

namespace astronomyModule
{

class IsomorphicSearchAgent : public ScAgent
{
  SC_CLASS(Agent, Event(Keynodes::question_find_isomorphic_structures, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()
};

} // namespace exampleModule

