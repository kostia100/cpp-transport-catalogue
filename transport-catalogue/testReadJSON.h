#pragma once

#include <iostream>
#include <cassert>
#include <sstream>

#include "transport_catalogue.h"
#include "json_reader.h"

namespace catalogue {

    namespace test {

        void TestOutputJSON() {


            std::istringstream input{
                    "{\n"
                    "\"base_requests\": [ \n"
                    "{ \n"
                    "\"type\": \"Bus\", \n"
                    "\"name\" : \"114\",  \n"
                    "        \"stops\" : [\"Sea Station\", \"Riv Bridge\"] , \n"
                    "        \"is_roundtrip\" : false \n"
                    "} , \n"
                    "{\n"
                    " \"type\": \"Stop\", \n"
                    "  \"name\" : \"Riv Bridge\", \n"
                    "  \"latitude\" : 43.587795, \n"
                    "  \"longitude\" : 39.716901, \n"
                    "  \"road_distances\" : {\"Sea Station\": 850} \n"
                    "},\n"
                    "{ \n"
                    "\"type\": \"Stop\", \n"
                    "\"name\" : \"Sea Station\", \n"
                    "\"latitude\" : 43.581969, \n"
                    "\"longitude\" : 39.719848, \n"
                    "\"road_distances\" : {\"Riv Bridge\": 850} \n"
                    "}\n"
                    "], \n"
                    "\"stat_requests\": [ \n"
                    "{ \"id\": 1, \"type\" : \"Stop\", \"name\" : \"Riv Bridge\" }, \n"
                    "{ \"id\": 2, \"type\" : \"Bus\", \"name\" : \"114\" } \n"
                    "]\n"
                    "}\n"
            };
            TransportCatalogue catalogue;
            JSONInfoRequest(input, std::cout, catalogue);
            std::cout << std::endl;

        }

        void TestLoadJSONAndGetResponse()
        {
            {
                std::istringstream input{
                    "{\n"
                    "\"base_requests\": [ \n"
                    "{ \n"
                    "\"type\": \"Bus\", \n"
                    "\"name\" : \"114\",  \n"
                    "        \"stops\" : [\"Sea Station\", \"Riv Bridge\"] , \n"
                    "        \"is_roundtrip\" : false \n"
                    "} , \n"
                    "{\n"
                    " \"type\": \"Stop\", \n"
                    "  \"name\" : \"Riv Bridge\", \n"
                    "  \"latitude\" : 43.587795, \n"
                    "  \"longitude\" : 39.716901, \n"
                    "  \"road_distances\" : {\"Sea Station\": 850} \n"
                    "},\n"
                    "{ \n"
                    "\"type\": \"Stop\", \n"
                    "\"name\" : \"Sea Station\", \n"
                    "\"latitude\" : 43.581969, \n"
                    "\"longitude\" : 39.719848, \n"
                    "\"road_distances\" : {\"Riv Bridge\": 850} \n"
                    "}\n"
                    "], \n"
                    "\"stat_requests\": [ \n"
                    "{ \"id\": 1, \"type\" : \"Stop\", \"name\" : \"Riv Bridge\" }, \n"
                    "{ \"id\": 2, \"type\" : \"Bus\", \"name\" : \"114\" } \n"
                    "]\n"
                    "}\n"
                };

                std::ostringstream out;
                TransportCatalogue catalogue;
                JSONInfoRequest(input, out, catalogue);
                json::Node output_node = json::LoadJSON(out.str()).GetRoot();

                std::istringstream target_stream{
                                    "[ \n"
                                    "{ \n"
                                    "    \"buses\": [  \n"
                                    "        \"114\"   \n"
                                    "    ] , \n"
                                    "    \"request_id\" : 1 \n"
                                    "}, \n"
                                    "{  \n"
                                    "    \"curvature\": 1.23199, \n"
                                    "    \"request_id\" : 2, \n"
                                    "    \"route_length\" : 1700,  \n"
                                    "    \"stop_count\" : 3,  \n"
                                    "    \"unique_stop_count\" : 2  \n"
                                    " } \n"
                                    "] \n" };
                json::Node target_node = ReadJSON(target_stream).GetRoot();

                //assert(arrTarget[1] == arrOutput[1]);
                assert(target_node == output_node);
            }
        }


        void TestLoadJSONAndAskNonExistingBus()
        {

            std::istringstream input{
                "{\n"
                "\"base_requests\": [ \n"
                "{ \n"
                "\"type\": \"Bus\", \n"
                "\"name\" : \"114\",  \n"
                "        \"stops\" : [\"AAAA\", \"BBB\"] , \n"
                "        \"is_roundtrip\" : false \n"
                "} , \n"
                "{\n"
                " \"type\": \"Stop\", \n"
                "  \"name\" : \"AAAA\", \n"
                "  \"latitude\" : 43.587795, \n"
                "  \"longitude\" : 39.716901, \n"
                "  \"road_distances\" : {\"BBB\": 850} \n"
                "},\n"
                "{ \n"
                "\"type\": \"Stop\", \n"
                "\"name\" : \"BBB\", \n"
                "\"latitude\" : 43.581969, \n"
                "\"longitude\" : 39.719848, \n"
                "\"road_distances\" : {\"AAAA\": 850} \n"
                "}\n"
                "], \n"
                "\"stat_requests\": [ \n"
                "{ \"id\": 123456, \"type\" : \"Stop\", \"name\" : \"CCC\" } \n"
                "]\n"
                "}\n"
            };

            std::ostringstream out;
            TransportCatalogue catalogue;
            JSONInfoRequest(input, out, catalogue);
            json::Node output_node = json::LoadJSON(out.str()).GetRoot();

            std::istringstream target_stream{
                                "[ \n"
                                "{ \n"
                                "\"request_id\": 123456, \n"
                                "\"error_message\" : \"not found\" \n"
                                "}  \n"
                                "] \n" };
            json::Node target_node = ReadJSON(target_stream).GetRoot();

            assert(target_node == output_node);

        }



        void TestLoadJSONToTC() {
            std::cout << "Start testing loading JSON to TC" << std::endl;
            TestOutputJSON();
            TestLoadJSONAndGetResponse();
            TestLoadJSONAndAskNonExistingBus();
            std::cout << "Read/Write JSON from/to TC Tests: OK" << std::endl;
        }

    }

}