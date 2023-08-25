#include "stat_reader.h"
#include <iostream>
#include <iomanip>


namespace catalogue {

	namespace request {

		std::ostream& operator<<(std::ostream& out, const StopInfo& info) {
			out << "Stop " << info.stop_name << ": ";
			if (!info.stop_exists) {
				out << "not found";
			}
			else if (info.buses.size() == 0) {
				out << "no buses";
			}
			else {
				out << "buses";
				for (auto& b : info.buses) {
					out << " " << b;
				}
			}
			return out;
		}

		std::ostream& operator<<(std::ostream& out, const BusInfo& info) {
			out << "Bus " << info.bus_name << ": ";
			if (info.bus_exists) {
				out << info.all_stops << " stops on route, ";
				out << info.unique_stops << " unique stops, ";
				out << std::setprecision(6) << info.traffic_route_length << " route length, ";
				out << std::setprecision(6) << info.curvature << " curvature";
				return out;
			}
			out << "not found";
			return out;
		}


		void GetDataFromCatalogue(std::istream& input, std::ostream& out, TransportCatalogue& catalogue) {

			int request_number;
			input >> request_number;

			std::string line;
			getline(input, line);

			int r = 1;
			while (r <= request_number) {
				getline(input, line);

				std::string request_type = line.substr(0, 3);
				if (request_type == "Bus") {
					std::string request_data = line.substr(4, std::string::npos);
					std::string bus_name = catalogue::input::detail::Trim(request_data);
					out << catalogue.GetBusInfo(bus_name) << std::endl;
				}
				else if (request_type == "Sto") {
					std::string request_data = line.substr(5, std::string::npos);
					std::string stop_name = catalogue::input::detail::Trim(request_data);
					out << catalogue.GetStopInfo(stop_name) << std::endl;
				}

				++r;
			}
		}


		std::ostream& GetStats(std::ostream& out, std::vector<OutputRequest> request, TransportCatalogue& catalogue) {
			for (const auto& r : request) {
				if (r.type == OutputType::BUS) {
					out << catalogue.GetBusInfo(r.text) << std::endl;
				}
				else if (r.type == OutputType::STOP) {
					out << catalogue.GetStopInfo(r.text) << std::endl;
				}
				else {

				}
			}
			return out;
		}
		

	}


}