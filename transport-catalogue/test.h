#pragma once

#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"
#include <cassert>
#include <vector>

namespace catalogue {

    namespace tests {

        using namespace input;
        using namespace requests;
        using namespace geography;

        void TestAddFindStop() {
            TransportCatalogue tc;

            tc.AddStop({ "A", { 11, 28 } });
            tc.AddStop({ "B", { 33, 45 } });
            tc.AddStop({ "C", { 52, 64 } });

            {
                Stop* st_ptr = tc.FindStop("A");
                assert(st_ptr != nullptr);
                assert(st_ptr->stop_name == "A");
                assert(st_ptr->location.lat == 11);
                assert(st_ptr->location.lng == 28);

                Stop* no_stop = tc.FindStop("AA");
                assert(no_stop == nullptr);
            }

            {
                Stop* st_ptr = tc.FindStop("B");
                assert(st_ptr != nullptr);
                assert(st_ptr->stop_name == "B");
                assert(st_ptr->location.lat == 33);
                assert(st_ptr->location.lng == 45);

                Stop* no_stop = tc.FindStop("BB");
                assert(no_stop == nullptr);
            }
        }


        void TestAddFindBus() {
            TransportCatalogue tc;

            tc.AddStop({ "A", { 11, 28 } });
            tc.AddStop({ "B", { 33, 45 } });
            tc.AddStop({ "C", { 52, 64 } });

            tc.AddBus({ "bus1", {"A","B","C","B","A"} });
            tc.AddBus({ "bus2", {"A","B","A"} });
            tc.AddBus({ "bus3", {"C","B","C"} });

            {
                Bus* bus_ptr1 = tc.FindBus("bus1");
                assert(bus_ptr1 != nullptr);
                assert(bus_ptr1->bus_name == "bus1");
                assert(bus_ptr1->CountUniqueStops() == 3);
                std::vector<Stop*> stops = bus_ptr1->stops;
                assert(stops.size() == 5);
                assert(stops[0]->stop_name == "A");
                assert(stops[1]->stop_name == "B");
                assert(stops[2]->stop_name == "C");
                assert(stops[3]->stop_name == "B");
                assert(stops[4]->stop_name == "A");
            }

            {
                Bus* bus_ptr1 = tc.FindBus("bus1000");
                assert(bus_ptr1 == nullptr);
            }
        }



        void TestParseStop() {
            {


                std::string input = " aabhd: 56.1125 , 56.9989 ";
                StopInputData stop = ParseStopData(input);
                assert(stop.name == "aabhd");
                assert(stop.coordinates.first == 56.1125);
                assert(stop.coordinates.second == 56.9989);
            }
            {
                std::string input = " avvaaw      : 96.1125         ,   -5.7789 ";
                StopInputData stop = ParseStopData(input);
                assert(stop.name == "avvaaw");
                assert(stop.coordinates.first == 96.1125);
                assert(stop.coordinates.second == -5.7789);
            }

            {
                std::string input = "ABC: 10.25 , 15.80, 100m to QZ, 526m to HP, 10236m to LUVF ";
                StopInputData stop = ParseStopData(input);
                assert(stop.name == "ABC");
                assert(stop.coordinates.first == 10.25);
                assert(stop.coordinates.second == 15.80);
                assert(stop.connected_stop.size() == 3);
                assert(stop.connected_stop.count("QZ") == 1);
                assert(stop.connected_stop["QZ"] == 100);
                assert(stop.connected_stop.count("HP") == 1);
                assert(stop.connected_stop["HP"] == 526);
                assert(stop.connected_stop.count("LUVF") == 1);
                assert(stop.connected_stop["LUVF"] == 10236);
            }

        }


        void TestParseBus() {
            {
                std::string input = " aaw887:  Afaw   >  Opmn >   fawI  > bbZ   ";
                std::pair<std::string, std::vector<std::string>> bus = ParseBusData(input);
                assert(bus.first == "aaw887");
                assert(bus.second.size() == 4);
                assert(bus.second[1] == "Opmn");
            }

            {
                std::string input = " Zua78:  ghP - Uzb - nha  ";
                std::pair<std::string, std::vector<std::string>> bus = ParseBusData(input);
                assert(bus.first == "Zua78");
                assert(bus.second.size() == 5);
                assert(bus.second[0] == "ghP");
                assert(bus.second[1] == "Uzb");
                assert(bus.second[2] == "nha");
                assert(bus.second[3] == "Uzb");
                assert(bus.second[4] == "ghP");
            }
        }

        void TestRequestBusData() {
            TransportCatalogue tc;

            tc.AddStop({ "A", { 0, 28 } });
            tc.AddStop({ "B", { 0, 45 } });
            tc.AddStop({ "C", { 52, 64 } });


            tc.AddBus({ "bus1", {"A","B","C","B","A"} });
            tc.AddBus({ "bus2", {"A","B","A"} });
            tc.AddBus({ "bus3", {"C","B","C"} });

            StopInputData data1{ "A",{0,0} ,{{"B",50}, {"C",100}} };
            StopInputData data2{ "B",{0,0} ,{ {"C",250} } };
            tc.AddNearestStops(data1);
            tc.AddNearestStops(data2);

            {
                std::vector<OutputRequest> request = { { OutputType::BUS , { "bus1" } } };
                std::ostringstream out;
                GetStats(out, request, tc);
                std::string output = out.str().substr(0, 43);
                std::string target = "Bus bus1: 5 stops on route, 3 unique stops,";
                assert(target == output);
            }
            {
                std::vector<OutputRequest> request = { { OutputType::BUS , { "bus2" } } };
                std::ostringstream out;
                GetStats(out, request, tc);
                std::string output = out.str().substr(0, 43);
                std::string target = "Bus bus2: 3 stops on route, 2 unique stops,";
                assert(target == output);
            }
            {
                std::vector<OutputRequest> request = { { OutputType::BUS , { "bus500" } } };
                std::ostringstream out;
                GetStats(out, request, tc);
                std::string output = out.str();
                std::string target = "Bus bus500: not found\n";
                assert(target == output);
            }

        }


        void TestRequestStopData() {
            TransportCatalogue tc;

            tc.AddStop({ "Aa", { 0, 28 } });
            tc.AddStop({ "Bb", { 0, 45 } });
            tc.AddStop({ "Cc C", { 52, 64 } });
            tc.AddStop({ "ZZZ", { 52, 64 } });



            tc.AddBus({ "ba125", {"Aa","Bb","Cc C","Bb","Aa"} });
            tc.AddBus({ "ba023", {"Aa","Bb","Aa"} });
            tc.AddBus({ "bb963", {"Cc C","Bb","Cc C"} });
            {
                std::vector<OutputRequest> request = { { OutputType::STOP , { "Aa" } } };
                std::ostringstream out;
                GetStats(out, request, tc);
                std::string output = out.str();
                std::string target = "Stop Aa: buses ba023 ba125\n";
                assert(target == output);
            }

            {
                std::vector<OutputRequest> request = { { OutputType::STOP , { "Bb" } } };
                std::ostringstream out;
                GetStats(out, request, tc);
                std::string output = out.str();
                std::string target = "Stop Bb: buses ba023 ba125 bb963\n";
                assert(target == output);
            }

            {
                std::vector<OutputRequest> request = { { OutputType::STOP , { "ZZZ" } } };
                std::ostringstream out;
                GetStats(out, request, tc);
                std::string output = out.str();
                std::string target = "Stop ZZZ: no buses\n";
                assert(target == output);
            }

            {
                std::vector<OutputRequest> request = { { OutputType::STOP , { "XYZ" } } };
                std::ostringstream out;
                GetStats(out, request, tc);
                std::string output = out.str();
                std::string target = "Stop XYZ: not found\n";
                assert(target == output);
            }


        }


        void TestParseOutputRequest() {
            std::istringstream data_input{
                "6\n"
                "Bus 256\n"
                "Bus 750\n"
                "Bus 751\n"
                "Stop Samara\n"
                "Stop Prazhskaya\n"
                "Stop Biryulyovo Zapadnoye\n" };

            std::vector<OutputRequest> request = ReadStatRequest(data_input);
            assert(request.size() == 6);
            assert(request[0].type == OutputType::BUS);
            assert(request[0].text == "256");
            assert(request[4].type == OutputType::STOP);
            assert(request[4].text == "Prazhskaya");
            assert(request[5].type == OutputType::STOP);
            assert(request[5].text == "Biryulyovo Zapadnoye");
        }


        void TestComputeTrafficDistance() {
            TransportCatalogue tc;

            tc.AddStop({ "A", { 0, 0 } });
            tc.AddStop({ "B", { 1, 0 } });
            tc.AddStop({ "C", { 2, 0 } });

            tc.AddBus({ "bus1", {"A","B","C","B","A"} });
            tc.AddBus({ "bus2", {"A","B","A"} });
            tc.AddBus({ "bus3", {"C","B","C"} });

            StopInputData stp1{ "A",{0,0} ,{{"B",50}, {"C",100}} };
            StopInputData stp2{ "B",{0,0} ,{ {"C",250} } };
            tc.AddNearestStops(stp1);
            tc.AddNearestStops(stp2);

            Bus* ptr1 = tc.FindBus("bus1");
            assert(ptr1 != nullptr);
            int resultBus1 = tc.GetTrafficRoute(ptr1);
            int targetBus1 = 600; // 50 + 250 +250 +50 = 600
            assert(targetBus1 == resultBus1);

            Bus* ptr2 = tc.FindBus("bus2");
            assert(ptr2 != nullptr);
            int resultBus2 = tc.GetTrafficRoute(ptr2);
            int targetBus2 = 100; // 50 + 50 = 100
            assert(targetBus2 == resultBus2);


            tc.AddStop({ "Z", { 2, 0 } });
            tc.AddBus({ "busZ", {"Z","Z"} });
            StopInputData stpZ{ "Z",{0,0} ,{ {"Z",123} } };
            tc.AddNearestStops(stpZ);
            Bus* ptrZ = tc.FindBus("busZ");
            int resultBusZ = tc.GetTrafficRoute(ptrZ);
            int targetBusZ = 123; // 123 !
            assert(targetBusZ == resultBusZ);
        }

        void TestComputeCurvature() {
            TransportCatalogue tc;

            Coordinates cA{ 10.5, 12.3 };
            Coordinates cB{ 14.2, 56.69 };
            Coordinates cC{ 23.89, 42.36 };

            tc.AddStop({ "A", { 10.5, 12.3 } });
            tc.AddStop({ "B", { 14.2, 56.69 } });
            tc.AddStop({ "C", { 23.89, 42.36 } });

            tc.AddBus({ "bus1", {"A","B","C","B","A"} });
            tc.AddBus({ "bus2", {"A","B","A"} });
            tc.AddBus({ "bus3", {"C","B","C"} });

            StopInputData stp1{ "A",{0,0} ,{{"B",5000}, {"C",10000}} };
            StopInputData stp2{ "B",{0,0} ,{ {"C",25000} } };
            tc.AddNearestStops(stp1);
            tc.AddNearestStops(stp2);

            Bus* ptr1 = tc.FindBus("bus1");
            assert(ptr1 != nullptr);
            double resultCruvature = tc.GetCurvatureBus(ptr1);
            int targetTrafficDistance = 60000; // 50 + 250 +250 +50 = 600
            double dAB = ComputeDistance(cA, cB);
            double dBC = ComputeDistance(cB, cC);
            double curvatureTarget = targetTrafficDistance / (dAB * 2 + dBC * 2);
            assert(curvatureTarget == resultCruvature);


        }


        void AllUnitTests() {
            TestAddFindStop();
            TestAddFindBus();
            TestParseStop();
            TestParseOutputRequest();
            TestRequestBusData();
            TestRequestStopData();
            TestComputeTrafficDistance();
            TestComputeCurvature();
            std::cout << "Unit Tests: OK" << std::endl;
        }

        void TestFullRequestBus() {
            std::istringstream data_input{
                   "13\n"
                   "Stop Tolstopaltsevo : 55.611087, 37.20829, 3900m to Marushkino\n"
                   "Stop Marushkino : 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino\n"
                   "Bus 256 : Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n"
                   "Bus 750: Tolstopaltsevo - Marushkino - Marushkino - Rasskazovka\n"
                   "Stop Rasskazovka : 55.632761, 37.333324, 9500m to Marushkino\n"
                   "Stop Biryulyovo Zapadnoye : 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam\n"
                   "Stop Biryusinka : 55.581065, 37.64839, 750m to Universam\n"
                   "Stop Universam : 55.587655, 37.645687, 5600m to Rossoshanskaya ulitsa, 900m to Biryulyovo Tovarnaya\n"
                   "Stop Biryulyovo Tovarnaya : 55.592028, 37.653656, 1300m to Biryulyovo Passazhirskaya\n"
                   "Stop Biryulyovo Passazhirskaya : 55.580999, 37.659164, 1200m to Biryulyovo Zapadnoye\n"
                   "Bus 828 : Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye\n"
                   "Stop Rossoshanskaya ulitsa : 55.595579, 37.605757\n"
                   "Stop Prazhskaya : 55.611678, 37.603831\n" };

            std::vector<IntputRequest> requests_input = ReadInputRequests(data_input);

            TransportCatalogue cat;
            AddInputRequest(requests_input, cat);

            std::istringstream output{
                "3\n"
                "Bus 256\n"
                "Bus 750\n"
                "Bus 751\n" };
            std::vector<OutputRequest> requests_output = ReadStatRequest(output);
            GetStats(std::cout, requests_output, cat);

            std::istringstream target_stream{
                "Bus 256: 6 stops on route, 5 unique stops, 5950 route length, 1.36124 curvature\n"
                "Bus 750: 7 stops on route, 3 unique stops, 27400 route length, 1.30853 curvature\n"
                "Bus 751: not found\n" };
            std::string target = target_stream.str();

            std::ostringstream out;
            GetStats(out, requests_output, cat);
            std::string result = out.str();
            assert(target == result);
        }





        /// <summary>
        /// Second version
        /// </summary>
        void TestFullRequestBusAndStop() {
            std::istringstream data_input{
                    "13\n"
                    "Stop Tolstopaltsevo : 55.611087, 37.20829, 3900m to Marushkino\n"
                    "Stop Marushkino : 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino\n"
                    "Bus 256 : Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n"
                    "Bus 750: Tolstopaltsevo - Marushkino - Marushkino - Rasskazovka\n"
                    "Stop Rasskazovka : 55.632761, 37.333324, 9500m to Marushkino\n"
                    "Stop Biryulyovo Zapadnoye : 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam\n"
                    "Stop Biryusinka : 55.581065, 37.64839, 750m to Universam\n"
                    "Stop Universam : 55.587655, 37.645687, 5600m to Rossoshanskaya ulitsa, 900m to Biryulyovo Tovarnaya\n"
                    "Stop Biryulyovo Tovarnaya : 55.592028, 37.653656, 1300m to Biryulyovo Passazhirskaya\n"
                    "Stop Biryulyovo Passazhirskaya : 55.580999, 37.659164, 1200m to Biryulyovo Zapadnoye\n"
                    "Bus 828 : Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye\n"
                    "Stop Rossoshanskaya ulitsa : 55.595579, 37.605757\n"
                    "Stop Prazhskaya : 55.611678, 37.603831\n" };

            std::vector<IntputRequest> requests_input = ReadInputRequests(data_input);

            TransportCatalogue cat;
            AddInputRequest(requests_input, cat);

            std::istringstream output{
                "6\n"
                "Bus 256\n"
                "Bus 750\n"
                "Bus 751\n"
                "Stop Samara\n"
                "Stop Prazhskaya\n"
                "Stop Biryulyovo Zapadnoye\n" };
            std::vector<OutputRequest> requests_output = ReadStatRequest(output);
            GetStats(std::cout, requests_output, cat);

            std::istringstream target_stream{
                "Bus 256: 6 stops on route, 5 unique stops, 5950 route length, 1.36124 curvature\n"
                "Bus 750: 7 stops on route, 3 unique stops, 27400 route length, 1.30853 curvature\n"
                "Bus 751: not found\n"
                "Stop Samara: not found\n"
                "Stop Prazhskaya: no buses\n"
                "Stop Biryulyovo Zapadnoye: buses 256 828\n" };
            std::string target = target_stream.str();

            std::ostringstream out;
            GetStats(out, requests_output, cat);
            std::string result = out.str();
            assert(target == result);

        }

    }


}


