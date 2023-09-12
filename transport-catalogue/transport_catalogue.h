#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <deque>
#include <string_view>
#include <unordered_map>
#include <map>
#include <set>
//#include "geo.h"
#include "domain.h"



namespace catalogue {

	
	/*
	/// <summary>
	/// Output of a request "Bus"
	/// </summary>
	struct BusInfo {
		bool bus_exists;
		std::string bus_name;
		size_t all_stops;
		size_t unique_stops;
		int traffic_route_length;
		double curvature;
	};
	*/

	/*
	/// <summary>
	/// Output of request "Stop"
	/// </summary>
	struct StopInfo {
		bool stop_exists;
		std::string stop_name;
		std::set<std::string> buses;
	};
	*/


	class TransportCatalogue {
	public:
		void AddStop(std::string stop_name, geo::Coordinates crd);

		/// <summary>
		/// Add Bus to TC. The end_stop is relevant only for DRAWING, not for calculations.
		/// </summary>
		/// <param name="name_bus"></param>
		/// <param name="names_stops"></param>
		/// <param name="end_stop"></param>
		void AddBus(std::string name_bus, const std::vector<std::string>& names_stops, std::string end_stop);

		void AddNearestStops(std::string stop_start, std::string stop_end, int distance);

		Stop* FindStop(const std::string& stop) const;
		Bus* FindBus(const std::string& bus) const;

		BusInfo GetBusInfo(const std::string& bus) const;
		StopInfo GetStopInfo(const std::string& stop) const;
		int GetDistanceBetweenStops(Stop* start, Stop* end) const;

		/// <summary>
		/// Return all Buses in alphabetical order.
		/// </summary>
		/// <returns></returns>
		std::vector<Bus*> GetAllBuses() const;


		/// <summary>
		/// Return coordinates of stops that are in the network (are on a bus line).
		/// </summary>
		/// <returns></returns>
		//std::vector<geo::Coordinates> GetStopsInNetwork() const;

		/// <summary>
		/// Return the Ptr on Stops in the Network, in ALPHABETICAL order
		/// </summary>
		/// <returns></returns>
		std::vector<Stop*> GetStopsPtrInNetwork() const;

	private:
		int GetTrafficRoute(Bus* bus) const;
		double GetCurvatureBus(Bus* bus) const;

		std::deque<Stop> stops_;

		std::unordered_map<std::string, Stop*> stopname_to_stop;

		std::deque<Bus> buses_;

		std::unordered_map<std::string, Bus*> busname_to_bus;

		std::unordered_map < std::string, std::set < std::string >> stopname_to_busnames;

		std::unordered_map<std::pair<Stop*, Stop*>, int, PairOfStopsPtrHasher> stops_to_distances;

	};

}

