#include "request_handler.h"

#include <iostream>
#include <sstream>
#include <unordered_map>
#include <iomanip>


namespace catalogue {

	namespace input {

		namespace detail {

			const std::string WHITESPACE = " \n\r\t\f\v";

			std::string Ltrim(const std::string& s)
			{
				size_t start = s.find_first_not_of(WHITESPACE);
				return (start == std::string::npos) ? "" : s.substr(start);
			}

			std::string Rtrim(const std::string& s)
			{
				size_t end = s.find_last_not_of(WHITESPACE);
				return (end == std::string::npos) ? "" : s.substr(0, end + 1);
			}

			std::string Trim(const std::string& s) {
				return Rtrim(Ltrim(s));
			}

			std::pair<std::string, std::string> Split(const std::string& line, char by) {
				size_t pos = line.find(by);
				std::string left = line.substr(0, pos);

				if (pos < line.size() && pos + 1 < line.size()) {
					return { Trim(left), Trim(line.substr(pos + 1)) };
				}
				else {
					return { Trim(left), std::string() };
				}
			}

		}


		std::vector<InputRequest> ReadInputRequests(std::istream& input) {
			int request_number;
			input >> request_number;

			std::vector<InputRequest> requests;
			std::string line;
			getline(input, line);
			int r = 1;
			while (r <= request_number) {
				getline(input, line);
				detail::Trim(line);
				auto pos = line.find(" ");
				std::string request_type = line.substr(0, pos);
				std::string  request_text = line.substr(pos);
				if (request_type == "Bus") {
					requests.push_back({ InputType::BUS , request_text });
				}
				else if (request_type == "Stop") {
					requests.push_back({ InputType::STOP , request_text });
				}
				++r;
			}


			return requests;
		}




		StopInputData ParseStopData(const std::string& text) {
			std::pair<std::string, std::string> stop_data = detail::Split(text, ':');

			std::string stp_name = detail::Trim(stop_data.first);

			std::string content = stop_data.second;
			std::stringstream strstream(content);
			std::string data;

			std::unordered_map<std::string, int> connected_stops;


			getline(strstream, data, ',');
			double X = std::stod(detail::Trim(data));
			getline(strstream, data, ',');
			double Y = std::stod(detail::Trim(data));


			while (getline(strstream, data, ',')) {
				size_t pos = data.find('m');
				int dist = std::stoi(detail::Trim(data.substr(0, pos)));
				std::string stop_name = detail::Trim(data.substr(pos + 4, std::string::npos));
				connected_stops[stop_name] = dist;
			}


			return StopInputData{ stp_name , {X,Y} , connected_stops };
		}


		std::pair<std::string, std::vector<std::string>> ParseBusData(const std::string& text) {
			std::pair<std::string, std::string> bus_data = detail::Split(text, ':');

			std::string bus_name = detail::Trim(bus_data.first);
			std::string stops_data = detail::Trim(bus_data.second);

			std::vector<std::string> stops;

			size_t pos = stops_data.find('>');
			bool is_cirular = pos != std::string::npos;
			std::stringstream strstream(stops_data);
			std::string segment;
			if (is_cirular) {
				while (getline(strstream, segment, '>')) {
					stops.push_back(detail::Trim(segment));
				}
			}
			else {
				while (getline(strstream, segment, '-')) {
					stops.push_back(detail::Trim(segment));
				}

				int q = (int)(stops.size() - 2);
				{
					while (q >= 0) {
						std::string stp = stops[q];
						stops.push_back(stp);
						--q;
					}
				}


			}


			return { bus_name , stops };
		}


		void AddInputRequest(const std::vector<input::InputRequest>& requests, TransportCatalogue& catalogue) {

			for (const auto& r : requests) {
				if (r.type == input::InputType::STOP) {
					input::StopInputData stop = input::ParseStopData(r.text);
					catalogue.AddStop(stop.name, { stop.coordinates.first, stop.coordinates.second });
				}
			}

			for (const auto& r : requests) {
				if (r.type == input::InputType::STOP) {
					input::StopInputData stop = input::ParseStopData(r.text);
					//catalogue.AddNearestStops(stop.name,stop.connected_stop);
					for (auto stop_end : stop.connected_stop) {
						catalogue.AddNearestStops(stop.name, stop_end.first, stop_end.second);
					}
				}
			}



			for (const auto& r : requests) {
				if (r.type == input::InputType::BUS) {
					auto bus = input::ParseBusData(r.text);
					//the next line is in Principle !!!FALSE!!!
					catalogue.AddBus(bus.first, bus.second,bus.second[0]);
				}
			}

		}

		void SetDataToCatalogue(std::istream& input, TransportCatalogue& catalogue) {
			std::vector<input::InputRequest> requests_input = input::ReadInputRequests(input);
			AddInputRequest(requests_input, catalogue);
		}



		//----JSON----
		void AddBaseRequests(
			std::vector<input::StopInputData>stop_data,
			std::vector<input::BusInputData> bus_data,
			TransportCatalogue& catalogue) {

			for (const auto& stop : stop_data) {
				catalogue.AddStop(stop.name, { stop.coordinates.first, stop.coordinates.second });
			}

			for (const auto& stop : stop_data) {
				for (auto stop_end : stop.connected_stop) {
					catalogue.AddNearestStops(stop.name, stop_end.first, stop_end.second);
				}
			}

			for (const auto& bus : bus_data) {
				catalogue.AddBus(bus.name,bus.stops, bus.end_stop);
			}
		}



		//-----------------------------------------------------------

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


		

		//--------------------


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