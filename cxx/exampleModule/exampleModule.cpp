/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/
#pragma once
#include "exampleModule.hpp"
//#include "astronomy.hpp"
#include "keynodes/keynodes.hpp"
#include "agents/SubdividingSearchAgent.hpp"
#include "agents/IsomorphicSearchAgent.hpp"
#include "agents/FindRadiusOfBlackHole.hpp"


using namespace astronomyModule;
//using namespace astronomyModule;

SC_IMPLEMENT_MODULE(AstronomyModule)

sc_result AstronomyModule::InitializeImpl()
{
  if (!astronomyModule::Keynodes::InitGlobal())
    return SC_RESULT_ERROR;

  SC_AGENT_REGISTER(SubdividingSearchAgent)
  SC_AGENT_REGISTER(IsomorphicSearchAgent)
  SC_AGENT_REGISTER(FindRadiusOfBlackHole)

  return SC_RESULT_OK;
}

sc_result AstronomyModule::ShutdownImpl()
{
  SC_AGENT_UNREGISTER(SubdividingSearchAgent)
  SC_AGENT_UNREGISTER(IsomorphicSearchAgent)
  SC_AGENT_UNREGISTER(FindRadiusOfBlackHole)

  return SC_RESULT_OK;
}
