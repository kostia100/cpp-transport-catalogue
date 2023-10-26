#include "domain.h"


namespace catalogue {

	double Bus::GetRouteGeoLength() const {
		double route = 0.0;
		for (size_t a = 0; a <= stops.size() - 2; ++a) {
			geo::Coordinates start = (*stops[a]).location;
			geo::Coordinates finish = (*stops[a + 1]).location;
			route += ComputeDistance(start, finish);
		}
		return route;
	}
	
	size_t Bus::CountUniqueStops() const {
		return std::set<Stop*>(stops.begin(), stops.end()).size();
	}

	bool Bus::IsRoundTrip() const {
		return end_stop == *stops.begin();
	}

}