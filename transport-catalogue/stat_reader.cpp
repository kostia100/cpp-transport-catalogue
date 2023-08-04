#include "stat_reader.h"
#include <iostream>


namespace catalogue {

	namespace requests {


		std::vector<OutputRequest> ReadStatRequest(std::istream& input) {
			std::vector<OutputRequest> request;

			int request_number;
			input >> request_number;

			std::string line;
			getline(input, line);

			int r = 1;
			while (r <= request_number) {
				getline(input, line);

				std::string request_type = line.substr(0, 3);
				if (request_type == "Bus") {
					std::string name = line.substr(4, std::string::npos);
					request.push_back({ OutputType::BUS , catalogue::input::detail::Trim(name) });
				}
				else if (request_type == "Sto") {
					std::string name = line.substr(5, std::string::npos);
					request.push_back({ OutputType::STOP , catalogue::input::detail::Trim(name) });
				}
				else {
					//...
				}

				++r;
			}


			return request;
		}



		std::ostream& GetStats(std::ostream& out, std::vector<OutputRequest> request, TransportCatalogue& cat) {
			for (const auto& r : request) {
				if (r.type == OutputType::BUS) {
					out << cat.GetBusInfo(r.text) << std::endl;
				}
				else if (r.type == OutputType::STOP) {
					out << cat.GetStopInfo(r.text) << std::endl;
				}
				else {

				}
			}
			return out;
		}

	}


}