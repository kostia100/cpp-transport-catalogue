#pragma once
#include <string>
#include <vector>
#include <set>
#include "geo.h"

namespace catalogue {



	/// <summary>
	/// Defines a stop.
	/// </summary>
	class Stop {
	public:
		std::string stop_name;
		geo::Coordinates location;
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

		//for round_trip: end=start, otherwise end !=start
		Stop* end_stop;

		double GetRouteGeoLength() const; 

		size_t CountUniqueStops() const; 

		bool IsRoundTrip() const;



	};


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

	/// <summary>
	/// Output of request "Stop"
	/// </summary>
	struct StopInfo {
		bool stop_exists;
		std::string stop_name;
		std::set<std::string> buses;
	};

}