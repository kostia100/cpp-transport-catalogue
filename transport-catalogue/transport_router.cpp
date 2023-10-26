#include "transport_router.h"





TransportGraphWrapper::TransportGraphWrapper(RoutingSettings settings, catalogue::TransportCatalogue& tc)
	: catalogue_(tc)
	, settings_(settings) {

}


graph::DirectedWeightedGraph<double> TransportGraphWrapper::BuildTransportGraph() {
	graph::DirectedWeightedGraph<double> grph = InitializeGraph();
	// add "Waiting" edges
	AddWaitingEdges(grph);
	// add Bus connections edges
	AddBusEdges(grph);

	return grph;
}



TransportRouteInfo TransportGraphWrapper::FindRoute(const graph::Router<double>& router, RouteRequest request) const {
	if (stop_to_VertexId.count(request.fromStop) == 0 || stop_to_VertexId.count(request.toStop) == 0) {
		return PostProcessFindRoute(std::nullopt);
	}
	graph::VertexId fromVertex = stop_to_VertexId.at(request.fromStop) - 1;
	graph::VertexId toVertex = stop_to_VertexId.at(request.toStop) - 1;
	return PostProcessFindRoute(router.BuildRoute(fromVertex, toVertex));

}

graph::DirectedWeightedGraph<double> TransportGraphWrapper::InitializeGraph() const {
	std::vector<catalogue::Stop*> stops = catalogue_.GetStopsPtrInNetwork();
	size_t stop_nmb = stops.size();
	size_t vertex_nmb = 2 * stop_nmb;
	graph::DirectedWeightedGraph<double> grph(vertex_nmb);
	return grph;
}


void TransportGraphWrapper::AddWaitingEdges(graph::DirectedWeightedGraph<double>& graph) {
	std::vector<catalogue::Stop*> stops = catalogue_.GetStopsPtrInNetwork();
	size_t stop_nmb = stops.size();
	size_t vertex_nmb = 2 * stop_nmb;
	size_t q = 0;
	size_t Vtx = 0;
	while (q < stop_nmb && Vtx < vertex_nmb) {

		double time = settings_.wait_time;
		graph::EdgeId id = graph.AddEdge({ Vtx, Vtx + 1 , time });

		edgeid_to_data[id] = { EdgeType::WAIT, nullptr,stops[q]  ,time, 0 };
		vertexid_to_data[Vtx] = { VertexType::WAITINGVERTEX, stops[q] };
		vertexid_to_data[Vtx + 1] = { VertexType::REALSTOP, stops[q] };
		stop_to_VertexId[stops[q]] = Vtx + 1;
		++q;
		Vtx += 2;
	}
}

void TransportGraphWrapper::AddBusEdges(graph::DirectedWeightedGraph<double>& graph) {
	for (auto bus_ptr : catalogue_.GetAllBuses()) {
		//unique stops on this bus-line
		std::set<catalogue::Stop*> stops_on_line((bus_ptr->stops).begin(), (bus_ptr->stops).end());
		for (catalogue::Stop* source_stop : stops_on_line) {
			
			std::vector<catalogue::DirectDestination> destinations = catalogue_.GetDirectDestinations(bus_ptr, source_stop, settings_.velocity);
			
			for (const auto destination : destinations) {
				graph::VertexId source_id = stop_to_VertexId[source_stop];
				graph::VertexId destination_id = stop_to_VertexId[destination.destination_stop] - 1;

				graph::EdgeId id = graph.AddEdge({ source_id, destination_id , destination.travel_time });
				edgeid_to_data[id] = { EdgeType::BUS, bus_ptr, nullptr , destination.travel_time, destination.span_count };
			}
		}
	}
}



TransportRouteInfo TransportGraphWrapper::PostProcessFindRoute(std::optional<graph::Router<double>::RouteInfo> route) const {
	if (route != std::nullopt) {
		double total_time = route->weight;
		std::vector<std::variant<WaitItem, BusItem>> items;

		for (const auto edge : route->edges) {
			EdgeData context = edgeid_to_data.at(edge);
			if (context.type == EdgeType::WAIT) {
				items.push_back(WaitItem{ context.waiting_stop->stop_name,context.time });
			}
			else if (context.type == EdgeType::BUS) {
				items.push_back(BusItem{ context.bus->bus_name,context.time,context.span_count });
			}

		}

		return { true,total_time , items };
	}

	return { false,0,{} };
}

