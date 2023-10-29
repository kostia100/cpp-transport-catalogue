#include "transport_router.h"





TransportGraphWrapper::TransportGraphWrapper(RoutingSettings settings, catalogue::TransportCatalogue& tc)
	: catalogue_(tc)
	, settings_(settings) {

}



void TransportGraphWrapper::BuildTransportGraph() {
	InitializeGraph();
	AddWaitingEdges();
	AddBusEdges();
	InitializeRouter();
}




TransportRouteInfo TransportGraphWrapper::FindRoute(RouteRequest request) const {
	if (stop_to_vertexid.count(request.fromStop) == 0 || stop_to_vertexid.count(request.toStop) == 0) {
		return PostProcessFindRoute(std::nullopt);
	}
	graph::VertexId fromVertex = stop_to_vertexid.at(request.fromStop) - 1;
	graph::VertexId toVertex = stop_to_vertexid.at(request.toStop) - 1;
	return PostProcessFindRoute(router_ptr_->BuildRoute(fromVertex, toVertex));

}


void TransportGraphWrapper::InitializeGraph() {
	std::vector<catalogue::Stop*> stops = catalogue_.GetStopsPtrInNetwork();
	size_t stop_nmb = stops.size();
	size_t vertex_nmb = 2 * stop_nmb;
	network_graph_ = graph::DirectedWeightedGraph<double>(vertex_nmb);;
}


void TransportGraphWrapper::AddWaitingEdges() {
	std::vector<catalogue::Stop*> stops = catalogue_.GetStopsPtrInNetwork();
	size_t stop_nmb = stops.size();
	size_t vertex_nmb = 2 * stop_nmb;
	size_t stop_index = 0;
	size_t vertex_index = 0;
	while (stop_index < stop_nmb && vertex_index < vertex_nmb) {

		double time = settings_.wait_time;
		graph::EdgeId id = network_graph_.AddEdge({ vertex_index, vertex_index + 1 , time });

		edgeid_to_data[id] = { EdgeType::WAIT, nullptr,stops[stop_index]  ,time, 0 };
		vertexid_to_data[vertex_index] = { VertexType::WAITINGVERTEX, stops[stop_index] };
		vertexid_to_data[vertex_index + 1] = { VertexType::REALSTOP, stops[stop_index] };
		stop_to_vertexid[stops[stop_index]] = vertex_index + 1;
		++stop_index;
		vertex_index += 2;
	}
}

void TransportGraphWrapper::AddBusEdges() {
	for (auto bus_ptr : catalogue_.GetAllBuses()) {
		//unique stops on this bus-line
		std::set<catalogue::Stop*> stops_on_line((bus_ptr->stops).begin(), (bus_ptr->stops).end());
		for (catalogue::Stop* source_stop : stops_on_line) {

			std::vector<catalogue::DirectDestination> destinations = catalogue_.GetDirectDestinations(bus_ptr, source_stop, settings_.velocity);

			for (const auto destination : destinations) {
				graph::VertexId source_id = stop_to_vertexid[source_stop];
				graph::VertexId destination_id = stop_to_vertexid[destination.destination_stop] - 1;

				graph::EdgeId id = network_graph_.AddEdge({ source_id, destination_id , destination.travel_time });
				edgeid_to_data[id] = { EdgeType::BUS, bus_ptr, nullptr , destination.travel_time, destination.span_count };
			}
		}
	}
}

void TransportGraphWrapper::InitializeRouter() {
	router_ptr_ = std::make_unique<graph::Router<double>>(network_graph_);
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

