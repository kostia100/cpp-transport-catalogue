
#include <iostream>
#include <cassert>
#include <sstream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "transport_catalogue.h"
#include "request_handler.h"
#include "testTC.h"
#include "testNode.h"
#include "testReadJSON.h"
#include "testSVG.h"
#include "json_reader.h"
#include "svg.h"
#include "map_renderer.h"

using namespace catalogue;


void TestDrawBusMap() {

    std::istringstream input{
    "{\n"
    "\"base_requests\": [\n"
    "{\n"
    "\"type\": \"Bus\",\n"
    "\"name\": \"14\",\n"
    "\"stops\": [\n"
    "\"Ulitsa Lizy Chaikinoi\",\n"
    "\"Elektroseti\",\n"
    "\"Ulitsa Dokuchaeva\",\n"
    "\"Ulitsa Lizy Chaikinoi\" \n"
    "], \n"
    "\"is_roundtrip\": true \n"
    "}, \n"
    "{\n"
    "\"type\": \"Bus\",\n"
    "\"name\": \"114\",\n"
    "\"stops\": [\n"
    "\"Morskoy vokzal\",\n"
    "\"Rivierskiy most\" \n"
    "], \n"
    "\"is_roundtrip\": false \n"
    "}, \n"
    "{\n"
    "\"type\": \"Stop\",\n"
    "\"name\": \"Rivierskiy most\",\n"
    "\"latitude\": 43.587795,\n"
    "\"longitude\": 39.716901,\n"
    "\"road_distances\": {\n"
    "\"Morskoy vokzal\": 850 \n"
    "} \n"
    "}, \n"
    "{\n"
    "\"type\": \"Stop\",\n"
    "\"name\": \"Morskoy vokzal\",\n"
    "\"latitude\": 43.581969,\n"
    "\"longitude\": 39.719848,\n"
    "\"road_distances\": {\n"
    "\"Rivierskiy most\": 850 \n"
    "} \n"
    "}, \n"
    "{\n"
    "\"type\": \"Stop\",\n"
    "\"name\": \"Elektroseti\",\n"
    "\"latitude\": 43.598701,\n"
    "\"longitude\": 39.730623,\n"
    "\"road_distances\": {\n"
    "\"Ulitsa Dokuchaeva\": 3000,\n"
    "\"Ulitsa Lizy Chaikinoi\": 4300 \n"
    "} \n"
    "}, \n"
    "{\n"
    "\"type\": \"Stop\",\n"
    "\"name\": \"Ulitsa Dokuchaeva\",\n"
    "\"latitude\": 43.585586,\n"
    "\"longitude\": 39.733879,\n"
    "\"road_distances\": {\n"
    "\"Ulitsa Lizy Chaikinoi\": 2000,\n"
    "\"Elektroseti\": 3000 \n"
    " } \n"
    " }, \n"
    "{\n"
    "\"type\": \"Stop\",\n"
    "\"name\": \"Ulitsa Lizy Chaikinoi\",\n"
    "\"latitude\": 43.590317,\n"
    "\"longitude\": 39.746833,\n"
    "\"road_distances\": {\n"
    "\"Elektroseti\": 4300,\n"
    "\"Ulitsa Dokuchaeva\": 2000 \n"
    "} \n"
    "} \n"
    "], \n"
    "\"render_settings\": {\n"
    "\"width\": 600,\n"
    "\"height\": 400,\n"
    "\"padding\": 50,\n"
    "\"stop_radius\": 5,\n"
    "\"line_width\": 14,\n"
    "\"bus_label_font_size\": 20,\n"
    "\"bus_label_offset\": [\n"
    "7, \n"
    "15 \n"
    "], \n"
    "\"stop_label_font_size\": 20,\n"
    "\"stop_label_offset\": [\n"
    "7, \n"
    "-3 \n"
    "], \n"
    "\"underlayer_color\": [\n"
    "255, \n"
    "255, \n"
    "255, \n"
    "0.85 \n"
    "], \n"
    "\"underlayer_width\": 3,\n"
    "\"color_palette\": [\n"
    "\"green\",\n"
    "[255, 160,  0],\n"
    "\"red\" \n"
    "] \n"
    "}, \n"
    "\"stat_requests\": [\n"
    "] \n"
    "}\n" };

    TransportCatalogue catalogue;

    renderer::NetworkDrawingData data = JSONMapRequest(input, catalogue);

    DrawNetworkMap(std::cout, data);

    std::cout << "\n";



}


void TestDrawRoundBusMap() {

    std::istringstream input{
        "{ \n"

        " \"base_requests\": [ \n"
        "{ \n"
      "\"type\": \"Bus\", \n"
      "\"name\" : \"123\", \n"
      "\"stops\" : [\n"
      "  \"Aa\", \n"
      "  \"Bb\" \n"
        //      "  \"Cc\" \n"
        //      "  \"Aa\" \n"
              "] , \n"
             " \"is_roundtrip\" : false \n"

                " }, \n"
                "{ \n"
            "  \"type\": \"Stop\", \n"
            "  \"name\" : \"Bb\", \n"
            "  \"latitude\" : 43.591969, \n"
            "  \"longitude\" : 39.719848, \n"
            "  \"road_distances\" : { \n"
            "  } \n"
            "}, \n"
           " { \n"
            "  \"type\": \"Stop\", \n"
            "  \"name\" : \"Aa\", \n"
            "  \"latitude\" : 43.587795, \n"
            "  \"longitude\" : 39.716901, \n"
            "  \"road_distances\" : { \n"
            "    \"Aa\": 850 \n"
            "  }\n"
            "}, \n"
            " { \n"
            "  \"type\": \"Stop\", \n"
            "  \"name\" : \"Cc\", \n"
            "  \"latitude\" : 43.577795, \n"
            "  \"longitude\" : 39.716901, \n"
            "  \"road_distances\" : { \n"
            "    \"Aa\": 850 \n"
            "  }\n"
            "} \n"
          "], \n"
            "    \"render_settings\": { \n"
          "  \"width\": 600, \n"
          "  \"height\" : 400, \n"
          "  \"padding\" : 50, \n"
          "  \"stop_radius\" : 5, \n"
          "  \"line_width\" : 14, \n"
          " \"bus_label_font_size\" : 20, \n"
          "  \"bus_label_offset\" : [\n"
          "    7, \n"
          "    15 \n"
          "  ] , \n"

          "  \"stop_label_font_size\" : 20, \n"
          "  \"stop_label_offset\" : [\n"
            "  7, \n"
            "  -3 \n"
            " ] , \n"
             "  \"underlayer_color\" : [\n"
            "  255, \n"
           "   255, \n"
            "  255, \n"
            "  0.85 \n"
           " ] , \n"

          "   \"underlayer_width\": 3, \n"
          "  \"color_palette\" : [\n"
          "    \"green\", \n"
          "    [255, 160, 0], \n"
          "    \"red\" \n"
          "  ] \n"
         " }, \n"
          "\"stat_requests\": [\n"
         " ] \n"
          "  }  \n"
    };

    TransportCatalogue catalogue;

    renderer::NetworkDrawingData data = JSONMapRequest(input, catalogue);

    DrawNetworkMap(std::cout, data);

    std::cout << "\n";



}


std::string ReadFromFile(std::string file_path) {

    std::ifstream file;
    file.open(file_path);
    std::stringstream input;
    if (file.is_open()) {
        while (file) {
            std::string data;
            std::getline(file, data);
            input << data;
        }
    }
    return input.str();

}




int main()
{

    json::test::TestsJSON();
    std::cout << "++++" << std::endl;
    svg::test::TestSVG();
    std::cout << "++++" << std::endl;
    test::AllUnitTests();
    std::cout << "++++" << std::endl;
    test::TestFullRequestBus();
    std::cout << "++++" << std::endl;
    test::TestFullRequestBusAndStop();
    std::cout << "++++" << std::endl;
    test::TestLoadJSONToTC();
    std::cout << "++++" << std::endl;
    TestDrawBusMap();
    std::cout << "++++" << std::endl;
    TestDrawRoundBusMap();
    std::cout << "++++" << std::endl;
 
    

   

    {
        
        //Yandex entry/exit points
        TransportCatalogue catalogue;
        JSONRequest(std::cin, std::cout, catalogue);
        
    }


}


