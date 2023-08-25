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


    TransportCatalogue catalogue;


    input::SetDataToCatalogue(std::cin, catalogue);
    request::GetDataFromCatalogue(std::cin, std::cout, catalogue);

}
