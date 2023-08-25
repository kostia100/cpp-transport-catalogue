#include "input_reader.h"
#include <iostream>
#include <sstream>
#include <unordered_map>


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

		
		std::vector<IntputRequest> ReadInputRequests(std::istream& input) {
			int request_number;
			input >> request_number;

			std::vector<IntputRequest> requests;
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
		

		void AddInputRequest(const std::vector<input::IntputRequest>& requests, TransportCatalogue& catalogue) {

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
						catalogue.AddNearestStops( stop.name , stop_end.first,  stop_end.second);
					}
				}
			}



			for (const auto& r : requests) {
				if (r.type == input::InputType::BUS) {
					auto bus = input::ParseBusData(r.text);
					catalogue.AddBus(bus.first, bus.second);
				}
			}

		}

		void SetDataToCatalogue(std::istream& input, TransportCatalogue& catalogue) {
			std::vector<input::IntputRequest> requests_input = input::ReadInputRequests(std::cin);
			AddInputRequest(requests_input, catalogue);
		}



	}


}
