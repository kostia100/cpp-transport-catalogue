#include <iostream>
#include <cassert>
#include <sstream>
#include <fstream>
#include <string_view>
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
#include "json_builder.h"

using namespace catalogue;


void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

void UnitTests() {
    json::test::TestsJSON();
    svg::test::TestSVG();
    test::AllUnitTests();
    test::TestFullRequestBus();
    test::TestFullRequestBusAndStop();
    test::TestLoadJSONToTC();
}


int main(int argc, char* argv[])
{
    //UnitTests();
    
    //input/output
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    if (mode == "make_base"sv) {
        MakeBase(std::cin);
        // make base here

    }
    else if (mode == "process_requests"sv) {
        // process requests here
        ProcessRequests(std::cin, std::cout);

    }
    else {
        PrintUsage();
        return 1;
    }
}


