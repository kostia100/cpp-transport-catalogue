#include "transport_catalogue.h"
#include <algorithm>
#include <set>
#include <iomanip>

namespace catalogue {

	using namespace geography;

	double Bus::GetRouteGeoLength() const {
		double route = 0.0;
		for (int a = 0; a <= stops.size() - 2; ++a) {
			Coordinates start = (*stops[a]).location;
			Coordinates finish = (*stops[a + 1]).location;
			route += ComputeDistance(start, finish);
		}
		return route;
	}

	size_t Bus::CountUniqueStops() const {
		return std::set<Stop*>(stops.begin(), stops.end()).size();
	}




	void TransportCatalogue::AddStop(std::string stop_name, double coordX, double coordY) {
		Coordinates crd = { coordX,coordY };
		Stop stop = { stop_name, crd };
		stops_.push_back(stop);
		Stop* ptr_stop = &stops_.back();
		stopname_to_stop.insert({ stop.stop_name,ptr_stop });
		//Cross-data. First add empty set
		stopname_to_busnames.insert({ stop.stop_name , std::set<std::string>{} });
	}



	void TransportCatalogue::AddBus(std::string name_bus, std::vector<std::string> names_stops) {
		std::vector<Stop*> bus_stops;
		for (const auto stop : names_stops) {
			bus_stops.push_back(stopname_to_stop[stop]);
		}

		buses_.push_back({ name_bus , bus_stops });
		Bus* ptr_bus = &buses_.back();
		busname_to_bus.insert({ name_bus,  ptr_bus });

		//Cross-data.
		for (Stop* stop_ptr : ptr_bus->stops) {
			stopname_to_busnames[stop_ptr->stop_name].insert(ptr_bus->bus_name);
		}
	}


	Stop* TransportCatalogue::FindStop(const std::string& stop) const {

		auto pos = stopname_to_stop.find(stop);
		if (pos != stopname_to_stop.end()) {
			return stopname_to_stop.at(stop);
		}


		return nullptr;
	}


	Bus* TransportCatalogue::FindBus(const std::string& bus) const {
		auto pos = busname_to_bus.find(bus);
		if (pos != busname_to_bus.end()) {
			return busname_to_bus.at(bus);
		}
		return nullptr;
	}

	int TransportCatalogue::GetDistanceBetweenStops(Stop* start, Stop* end) const {
		if (stops_to_distances.find({ start,end }) != stops_to_distances.end()) {
			// the distance start->end is defined 
			return stops_to_distances.at({ start,end });
		}
		else {
			// if not, we assume that it is equal to end->start
			return stops_to_distances.at({ end,start });
		}
	}

	void TransportCatalogue::AddNearestStops(input::StopInputData data) {
		Stop* base_stop = stopname_to_stop[data.name];
		for (const auto& stp : data.connected_stop) {
			Stop* nearby_stop = stopname_to_stop.at(stp.first);
			int distance = stp.second;
			stops_to_distances.insert({ {base_stop,nearby_stop}, distance });
		}

	}


	int TransportCatalogue::GetTrafficRoute(Bus* bus_ptr) const {
		//Bus* bus_ptr = busname_to_bus.at(bus);
		int route = 0;
		for (int a = 0; a <= bus_ptr->stops.size() - 2; ++a) {
			route += GetDistanceBetweenStops(bus_ptr->stops[a], bus_ptr->stops[a + 1]);
		}
		return route;
	}

	double TransportCatalogue::GetCurvatureBus(Bus* bus_ptr) const {
		//Bus* bus_ptr = busname_to_bus.at(bus);
		return GetTrafficRoute(bus_ptr) / (bus_ptr->GetRouteGeoLength());
	}



	BusInfo TransportCatalogue::GetBusInfo(const std::string& bus) const {
		Bus* bus_ptr = FindBus(bus);
		if (bus_ptr == nullptr) {
			return BusInfo{ false,bus,0,0,0,0 };
		}
		return BusInfo{
			true,
			bus_ptr->bus_name,
			bus_ptr->stops.size(),
			bus_ptr->CountUniqueStops(),
			GetTrafficRoute(bus_ptr),
			GetCurvatureBus(bus_ptr)

		};
	}



	//------------------------

	StopInfo TransportCatalogue::GetStopInfo(const std::string& stop) const {
		Stop* stop_ptr = FindStop(stop);
		if (stop_ptr == nullptr) {
			return StopInfo{ false, stop, {} };
		}
		return StopInfo{ true,stop, stopname_to_busnames.at(stop_ptr->stop_name) };
	}


	void AddInputRequest(const std::vector<input::IntputRequest>& requests, TransportCatalogue& catalogue) {

		for (const auto& r : requests) {
			if (r.type == input::InputType::STOP) {
				input::StopInputData stop = input::ParseStopData(r.text);
				catalogue.AddStop(stop.name, stop.coordinates.first, stop.coordinates.second);
			}
		}

		for (const auto& r : requests) {
			if (r.type == input::InputType::STOP) {
				input::StopInputData stop = input::ParseStopData(r.text);
				catalogue.AddNearestStops(stop);
			}
		}



		for (const auto& r : requests) {
			if (r.type == input::InputType::BUS) {
				auto bus = input::ParseBusData(r.text);
				catalogue.AddBus(bus.first, bus.second);
			}
		}

	}

	void WriteInputToCatalogue(std::istream& input, TransportCatalogue& catalogue) {
		std::vector<input::IntputRequest> requests_input = input::ReadInputRequests(std::cin);
		AddInputRequest(requests_input, catalogue);
	}

}



