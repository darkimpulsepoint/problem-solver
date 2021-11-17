#pragma once

#include <sc-memory/cpp/kpm/sc_agent.hpp>

#include "keynodes/keynodes.hpp"
#include "FindRadiusOfBlackHole.generated.hpp"

namespace astronomyModule
{

class FindRadiusOfBlackHole : public ScAgent
{
  SC_CLASS(Agent, Event(Keynodes::question_find_black_hole_radius, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()
};

}
