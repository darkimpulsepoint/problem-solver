#pragma once

#include <iostream>
#include <thread>
#include <sc-memory/cpp/sc_memory.hpp>
#include <sc-memory/cpp/sc_stream.hpp>
#include "FindRadiusOfBlackHole.hpp"
#include <sc-kpm/sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-kpm/sc-agents-common/utils/AgentUtils.hpp>
using namespace std;
using namespace utils;

bool check_is_element_of_concept(unique_ptr<ScMemoryContext>& context, ScAddr el, ScAddr el_concept){
    ScIterator3Ptr iterator3Ptr = context->Iterator3(el_concept, ScType::EdgeAccess, ScType(0));
    while(iterator3Ptr->Next()){
        if(iterator3Ptr->Get(2)==el) return true;
    }

    return false;
}

class value{
private:
    double mainPart;
    int power;

public:
    value (double main_part, int power){
        this->mainPart=main_part;
        this->power=power;
    }

    double get_main_part(){
        return mainPart;
    }

    int get_power(){
        return power;
    }

    bool is_bigger_than(value v){
        if(this->power> v.get_power()) return true;
        if(this->power <v.get_power()) return false;
        return this->mainPart > v.get_main_part();
    }
};

class counter {
private:
    double main_part_1;
    double main_part_2;
    int power_1;
    int power_2;

    void set_parameters(value &first, value &second) {
        main_part_1 = first.get_main_part();
        main_part_2 = second.get_main_part();
        power_1 = first.get_power();
        power_2 = second.get_power();

    }

public:
    value multiplication(value first, value second) {
        set_parameters(first, second);

        double new_main_part = main_part_1 * main_part_2;
        int new_power = power_1 + power_2;
        if (new_main_part > 10) {
            new_main_part = new_main_part / 10;
            new_power += 1;
        }
        return value(new_main_part, new_power);
    }

    value division(value first, value second) {
        set_parameters(first, second);

        double new_main_part = main_part_1 / main_part_2;
        int new_power = power_1 - power_2;
        if(main_part_1==0) return value(0,0);
        if (new_main_part < 1) {
            new_main_part = new_main_part * 10;
            new_power -= 1;
        }


        return value(new_main_part, new_power);
    }
};



string value_to_string(value v){
    string res = to_string(v.get_main_part()) + "e" + to_string(v.get_power());
    return res;

}

value get_value(unique_ptr<ScMemoryContext>& context, ScAddr el, ScAddr nrel_parametr){
    ScIterator5Ptr iterator5Type =context->Iterator5(
            el,
            ScType::EdgeDCommon,
            ScType(0),
            ScType::EdgeAccess,
            nrel_parametr
    );
    if(!(iterator5Type->Next())) throw exception();
    ScAddr el_value = iterator5Type->Get(2);

    string valueSTR = context->HelperGetSystemIdtf(el_value);
    int x_pos=valueSTR.find('x');

    string main_partSTR= valueSTR.substr(0,x_pos);

    int e_pos = valueSTR.find('e');

    string powerSTR = valueSTR.substr(e_pos+1, valueSTR.length());
    return value(stod(main_partSTR), stoi(powerSTR));
}

bool check_is_weight_enough_to_become_black_hole(unique_ptr<ScMemoryContext>&context, ScAddr addr){
    value sun_weight_x15(2.985, 31);
    ScAddr nrel_weight = context->HelperResolveSystemIdtf("nrel_weight");
    value weight(0,0);
    weight = get_value(context, addr, nrel_weight);

    return weight.is_bigger_than(sun_weight_x15);
}

bool check_is_element_has_paramater(unique_ptr<ScMemoryContext> &context, ScAddr el, ScAddr nrel_paramater){
    ScIterator5Ptr iterator5Ptr = context->Iterator5(el,
            ScType::EdgeDCommon,
            ScType(0),
            ScType::EdgeAccessConstPosPerm,
            nrel_paramater);

    return iterator5Ptr->Next();
}

namespace astronomyModule {

    SC_AGENT_IMPLEMENTATION(FindRadiusOfBlackHole) {
        ScLog *logger = ScLog::GetInstance();
        if (!edgeAddr.IsValid())
            return SC_RESULT_ERROR;

        ScAddr questionNode1 = ms_context->GetEdgeTarget(edgeAddr);

        ScAddr templateStructNode = IteratorUtils::getFirstFromSet(ms_context.get(), questionNode1);

        if(!templateStructNode.IsValid()) return SC_RESULT_ERROR;

        ScAddr answer0 = ms_context->CreateNode(ScType::NodeConstStruct);

        ScAddr weight;
        try{
            weight = ms_context->HelperResolveSystemIdtf("nrel_weight");}
        catch (exception &e) {
            logger->Message(ScLog::Type::Error,"no weight parameter");
        }

        ScAddr concept_star = ms_context->HelperResolveSystemIdtf("concept_star");
        if(!templateStructNode.IsValid()) return SC_RESULT_ERROR;
        if(!check_is_element_of_concept(ms_context, templateStructNode, concept_star)){
            logger->Message(ScLog::Type::Error, "This object " +
            ms_context->HelperGetSystemIdtf(templateStructNode)
            +" is not a star!");
            return SC_RESULT_ERROR;
        }

        if(!check_is_element_has_paramater(ms_context, templateStructNode, weight)) {
            logger->Message(ScLog::Type::Info, "can't get weight parameter of " +
                                               ms_context->HelperGetSystemIdtf(templateStructNode)
            );
            return SC_RESULT_ERROR;
        }

        counter c;
        const value G(6.67, -11);
        const value Gx2 = c.multiplication(G, value(2, 0));
        const value c_e2 = value(9, 16);
        value weightEL(0,0);

        try {
            weightEL = get_value(ms_context, templateStructNode, weight);
        } catch (exception &e) {
            logger->Message(ScLog::Type::Error,"cant get "+
            ms_context->HelperGetSystemIdtf(weight)+
            " of "+
            ms_context->HelperGetSystemIdtf(templateStructNode));
            return SC_RESULT_ERROR;
        }

        if(!check_is_weight_enough_to_become_black_hole(ms_context, templateStructNode)){
            logger->Message(ScLog::Type::Error, "weight of "+ ms_context->HelperGetSystemIdtf(templateStructNode)+
                                                " is not enough to become black hole" );
            return SC_RESULT_ERROR;
        }

        value black_hole_radius = c.division(c.multiplication(Gx2, weightEL), c_e2);
        ScAddr radius = ms_context->CreateNode(ScType::NodeConst);
        if((ms_context->HelperSetSystemIdtf(value_to_string(black_hole_radius) + "m", radius))) {
            logger->Message(
            ScLog::Type::Info, "The radius of black hole for " + ms_context->HelperGetSystemIdtf(templateStructNode) + " is "+
            value_to_string(black_hole_radius) + "m"
            );
        } else{
            radius = ms_context->HelperResolveSystemIdtf(value_to_string(black_hole_radius)+"m");
        }

        ScAddr createdEdge = ms_context->CreateEdge(ScType::EdgeDCommonConst,
                                                    templateStructNode,
                                                    radius);
        {
            ScAddr nrel_black_hole_radius = ms_context->HelperResolveSystemIdtf("nrel_black_hole_radius");
            ScAddr edgy = ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, nrel_black_hole_radius, createdEdge);
            ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer0, templateStructNode);
            ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer0, createdEdge);
            ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer0, nrel_black_hole_radius);
            ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer0, edgy);
            ms_context->CreateEdge(ScType::EdgeAccessConstPosPerm, answer0, radius);
        }

        utils::AgentUtils::finishAgentWork(ms_context.get(), questionNode1, answer0);

        return SC_RESULT_OK;

    }
    // namespace exampleModule
}
