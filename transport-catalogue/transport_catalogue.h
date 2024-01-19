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

	
	


	/// <summary>
	/// Direct (without Waiting on a stop) destionation from a source.
	/// </summary>
	struct DirectDestination {
		Stop* destination_stop;
		double travel_time;
		int span_count;
	};



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
		//std::vector<geo::Coordinates> GetStopsInNetwork() const;

		/// <summary>
		/// Return the Ptr on Stops in the Network, in ALPHABETICAL order
		/// </summary>
		std::vector<Stop*> GetStopsPtrInNetwork() const;


		/// <summary>
		/// Compute the direct directions on the given bus-line from the source
		/// </summary>
		std::vector<DirectDestination> GetDirectDestinations(const Bus* bus, const Stop* source, double bus_speed) const;


		const std::deque<Bus> GetBuses() const;
		const std::deque<Stop> GetStops() const;

		/// <summary>
		/// Give the stops that are directly reachable from start stop.
		/// </summary>
		/// <param name="start"></param>
		/// <returns></returns>
		std::unordered_map<std::string, int> GetConnectedStops(const std::string& start) const;

	private:

		std::vector<DirectDestination> GetDirectDestinationsForRoundBus(const Bus* bus, const Stop* source, double velocity) const;


		std::vector<DirectDestination> GetDirectDestinationsForStraightBus(const Bus* bus, const Stop* source, double velocity) const;

		template <typename StopIt>
		std::vector<DirectDestination> GetDestinationsForwards(StopIt source, StopIt destination, double velocity) const;

		template <typename StopIt>
		std::vector<DirectDestination> GetDestinationsBackwards(const StopIt source, const StopIt destination, double velocity) const;

		template <typename StopIt>
		std::vector<DirectDestination> GetDirectDestinationsInRange(const StopIt start, const StopIt finish, const Stop* source, double velocity) const;


		static double ConvertToTime(double distance, double velocity);

		int GetTrafficRoute(Bus* bus) const;
		double GetCurvatureBus(Bus* bus) const;



		std::deque<Stop> stops_;

		std::unordered_map<std::string, Stop*> stopname_to_stop;

		std::deque<Bus> buses_;

		std::unordered_map<std::string, Bus*> busname_to_bus;

		std::unordered_map < std::string, std::set < std::string >> stopname_to_busnames;

		std::unordered_map<std::pair<Stop*, Stop*>, int, PairOfStopsPtrHasher> stops_to_distances;

	};



	template <typename StopIt>
	std::vector<DirectDestination> TransportCatalogue::GetDestinationsForwards(const StopIt source, const StopIt destination, double velocity) const {
		std::vector<DirectDestination> destinations;
		double travel_time = 0;
		int span_count = 0;
		StopIt current = source;
		StopIt next = source;
		while (++next <= destination) {
			double distance = GetDistanceBetweenStops(*current, *next);
			travel_time += ConvertToTime(distance, velocity);
			++span_count;
			DirectDestination destination{ *next , travel_time , span_count };
			destinations.push_back(destination);
			++current;
		}
		return destinations;
	}


	template <typename StopIt>
	std::vector<DirectDestination> TransportCatalogue::GetDestinationsBackwards(const StopIt source, const StopIt destination, double velocity) const {
		std::vector<DirectDestination> destinations;
		double travel_time = 0;
		int span_count = 0;
		StopIt current = source;
		StopIt previous = source;
		while (current != destination && --previous >= destination) {
			double distance = GetDistanceBetweenStops(*previous, *current);
			travel_time += ConvertToTime(distance, velocity);
			++span_count;
			DirectDestination destination{ *previous , travel_time , span_count };
			destinations.push_back(destination);
			--current;

		}
		return destinations;
	}

	template <typename StopIt>
	std::vector<DirectDestination> TransportCatalogue::GetDirectDestinationsInRange(const StopIt start, const StopIt end, const Stop* source, double velocity) const {
		std::vector<DirectDestination> destinations;
		auto final_it = end;
		--final_it;

		int occurence = count(start, end, source);

		int departure = 1;

		auto search_from = start;

		while (departure <= occurence) {
			++departure;
			auto current = std::find(search_from, end, source);
			std::vector<DirectDestination> trips = GetDestinationsForwards(current, final_it, velocity);
			std::copy(trips.begin(), trips.end(), back_inserter(destinations));
			search_from = current;
			++search_from;
		}

		return destinations;
	}

}

