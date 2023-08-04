#include <iostream>
#include <cassert>
#include <sstream>

#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"
#include "test.h"



using namespace catalogue;

int main()
{
    tests::AllUnitTests();
    std::cout << "++++" << std::endl;
    tests::TestFullRequestBus();
    std::cout << "++++" << std::endl;
    tests::TestFullRequestBusAndStop();
    std::cout << "++++" << std::endl;

    
    TransportCatalogue cat;
    std::vector<input::IntputRequest> requests_input = input::ReadInputRequests(std::cin);
    input::AddInputRequest(requests_input, cat);
    std::vector<requests::OutputRequest> requests_output = requests::ReadStatRequest(std::cin);
    requests::GetStats(std::cout, requests_output, cat);

}

