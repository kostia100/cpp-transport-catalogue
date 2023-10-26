#include "transport_catalogue.h"
#include <algorithm>
#include <set>
#include <iomanip>

namespace catalogue {


	void TransportCatalogue::AddStop(std::string stop_name, geo::Coordinates crd) {
		Stop stop = { stop_name, crd };
		stops_.push_back(stop);
		Stop* ptr_stop = &stops_.back();
		stopname_to_stop.insert({ stop.stop_name,ptr_stop });
		//Cross-data. First add empty set
		stopname_to_busnames.insert({ stop.stop_name , std::set<std::string>{} });
	}



	void TransportCatalogue::AddBus(std::string name_bus, const std::vector<std::string>& names_stops, std::string end_stop) {
		std::vector<Stop*> bus_stops;
		for (const auto stop : names_stops) {
			bus_stops.push_back(stopname_to_stop[stop]);
		}
		Stop* end_ptr = stopname_to_stop[end_stop];
		buses_.push_back({ name_bus , bus_stops , end_ptr });
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





	double TransportCatalogue::ConvertToTime(double distance, double velocity) {
		return distance / velocity * 60 / 1000;
	}

	

	std::vector<DirectDestination> TransportCatalogue::GetDirectDestinationsForRoundBus(const Bus* bus, const Stop* source, double velocity) const {
		std::vector<DirectDestination> destinations;
		std::vector<Stop*> line = bus->stops;
		auto final_it = line.end();
		--final_it;
		
		int occurence = count(line.begin(),line.end(),source);
		
		int departure = 1;

		auto search_from = line.begin();

		while(departure <= occurence){
			++departure;
			auto current = std::find(search_from, line.end(), source);
			std::vector<DirectDestination> trips = GetDestinationsForwards(current, final_it, velocity);
			std::copy(trips.begin(), trips.end(),back_inserter(destinations));
			search_from = current;
			++search_from;
		}
		
		return destinations;
	}

	
	

	std::vector<DirectDestination> TransportCatalogue::GetDirectDestinationsForStraightBus(const Bus* bus, const Stop* source, double velocity) const {
		std::vector<Stop*> line = bus->stops;
		Stop* starting_stop = bus->stops[0];
		auto start_it = std::find(line.begin(), line.end(), starting_stop);
		auto final_it = start_it + bus->stops.size();

		size_t mid = (bus->stops.size() - 1) / 2;

		//this the second terminal location
		auto mid_it = start_it + mid;
		auto past_mid_it =mid_it+1;
		auto current = std::find(line.begin(), line.end(), source);

		std::vector<DirectDestination> destinations = GetDirectDestinationsInRange(start_it, past_mid_it, *current,velocity);
		std::vector<DirectDestination> destinations_backward = GetDirectDestinationsInRange(mid_it, final_it, *current, velocity);
		for (auto elem : destinations_backward) {
			destinations.push_back(elem);
		}
		return destinations;
	}


	std::vector<DirectDestination> TransportCatalogue::GetDirectDestinations(const Bus* bus, const Stop* source , double velocity) const {
		if (bus->IsRoundTrip()) {
			return GetDirectDestinationsForRoundBus(bus,source,velocity);
		}
		else {
			return GetDirectDestinationsForStraightBus(bus, source, velocity);
		}
	}


	void TransportCatalogue::AddNearestStops(std::string stop_start, std::string stop_end, int distance) {
		Stop* stop_start_ptr = stopname_to_stop[stop_start];
		Stop* stop_end_ptr = stopname_to_stop[stop_end];
		stops_to_distances.insert({ {stop_start_ptr,stop_end_ptr}, distance });
	}


	int TransportCatalogue::GetTrafficRoute(Bus* bus_ptr) const {
		//Bus* bus_ptr = busname_to_bus.at(bus);
		int route = 0;
		for (size_t a = 0; a <= bus_ptr->stops.size() - 2; ++a) {
			route += GetDistanceBetweenStops(bus_ptr->stops[a], bus_ptr->stops[a + 1]);
		}
		return route;
	}

	double TransportCatalogue::GetCurvatureBus(Bus* bus_ptr) const {
		//Bus* bus_ptr = busname_to_bus.at(bus);
		return GetTrafficRoute(bus_ptr) / (bus_ptr->GetRouteGeoLength());
	}



	std::vector<Bus*> TransportCatalogue::GetAllBuses() const {
		std::vector<Bus*> buses;
		std::set<std::string> bus_names;
		for (auto elem : busname_to_bus) {
			bus_names.insert(elem.first);
		}
		for (auto name : bus_names) {
			buses.push_back(busname_to_bus.at(name));
		}
		return buses;
	}


	std::vector<Stop*> TransportCatalogue::GetStopsPtrInNetwork() const {
		std::vector<Stop*> stops;
		std::set<std::string> stop_names;

		for (auto stp : stops_) {
			if ((stopname_to_busnames.at(stp.stop_name)).size() > 0) {
				stop_names.insert(stp.stop_name);
			}

		}
		for (auto name : stop_names) {
			stops.push_back(stopname_to_stop.at(name));
		}
		return stops;
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




}



