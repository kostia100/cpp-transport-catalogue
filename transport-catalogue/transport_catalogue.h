#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <deque>
#include <string_view>
#include <unordered_map>
#include <map>
#include <set>
#include "geo.h"
#include "input_reader.h"



namespace catalogue {

	/// <summary>
	/// Defines a stop.
	/// </summary>
	class Stop {
	public:
		std::string stop_name;
		geography::Coordinates location;
	};


	/// <summary>
	/// Hasher for a pair of Stop*
	/// </summary>
	struct PairOfStopsPtrHasher {
		size_t operator()(std::pair<Stop*, Stop*> pair_ptr) const {
			std::hash<const void*> ptr_hasher;
			return ptr_hasher(pair_ptr.first) + 41 * ptr_hasher(pair_ptr.second);
		}
	};


	/// <summary>
	/// Defines a Bus
	/// </summary>
	class Bus {
	public:
		std::string bus_name;
		std::vector<Stop*> stops;

		double GetRouteGeoLength() const;

		size_t CountUniqueStops() const;


	};


	/// <summary>
	/// Info for request Bus
	/// </summary>
	struct BusInfo {
		bool bus_exists;
		std::string bus_name;
		size_t all_stops;
		size_t unique_stops;
		int traffic_route_length;
		double curvature;
	};


	struct StopInfo {
		bool stop_exists;
		std::string stop_name;
		std::set<std::string> buses;
	};

	

	class TransportCatalogue {
	public:
		void AddStop(std::string stop_name, double coordX , double coordY);
		void AddBus(std::string name_bus, std::vector<std::string> names_stops);

		void AddNearestStops(input::StopInputData data);

		Stop* FindStop(const std::string& stop) const;
		Bus* FindBus(const std::string& bus) const;

		BusInfo GetBusInfo(const std::string& bus) const;
		StopInfo GetStopInfo(const std::string& stop) const;


	private:
		int GetDistanceBetweenStops(Stop* start, Stop* end) const;
		int GetTrafficRoute(Bus* bus) const;
		double GetCurvatureBus(Bus* bus) const;

		std::deque<Stop> stops_;

		std::unordered_map<std::string, Stop*> stopname_to_stop;

		std::deque<Bus> buses_;

		std::unordered_map<std::string, Bus*> busname_to_bus;

		std::unordered_map < std::string, std::set < std::string >> stopname_to_busnames;

		std::unordered_map<std::pair<Stop*, Stop*>, int, PairOfStopsPtrHasher> stops_to_distances;

	};

	/// <summary>
	/// Add request to data base
	/// </summary>
	void AddInputRequest(const std::vector<input::IntputRequest>& request, TransportCatalogue& catalogue);

	/// <summary>
	/// Get input from stream, parse, write to catalogue.
	/// </summary>
	/// <param name="input"></param>
	/// <param name="catalogue"></param>
	void WriteInputToCatalogue(std::istream& input, TransportCatalogue& catalogue);

}

