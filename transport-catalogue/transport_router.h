#pragma once

#include <algorithm>
#include <variant>
#include <vector>
#include "graph.h"
#include "router.h"
#include "transport_catalogue.h"



struct WaitItem {
	std::string stop_name;
	double time;
};

struct BusItem {
	std::string bus_name;
	double time;
	int span_count;
};


struct TransportRouteInfo {
	bool rout_exists;
	double total_time;
	std::vector<std::variant<WaitItem, BusItem>> items;
};


struct RouteRequest {
	catalogue::Stop* fromStop;
	catalogue::Stop* toStop;
};

struct RoutingSettings {
	double velocity;
	double wait_time;
};



enum class EdgeType {
	WAIT,
	BUS
};

struct EdgeData {
	EdgeType type;
	catalogue::Bus* bus; // nullptr if WAIT
	catalogue::Stop* waiting_stop; // nullptr if BUS 
	double time;
	int span_count; // 0 if WAIT
};

enum class VertexType {
	WAITINGVERTEX,
	REALSTOP
};


struct VertexData {
	VertexType type;
	catalogue::Stop* stop;
};

class TransportGraphWrapper {
public:
	
	TransportGraphWrapper(RoutingSettings settings, catalogue::TransportCatalogue& tc);


	/// <summary>
	/// One stop => 2 vertex.
	/// Even indexed vertex: "waiting time stops".
	/// Odd indexed vertex: "bus connection stops".
	/// </summary>
	void BuildTransportGraph();

	TransportRouteInfo FindRoute(RouteRequest request) const;

private:
	/// <summary>
	/// Create graph with vertexes.
	/// </summary>
	void InitializeGraph();

	/// <summary>
	/// Add "Waiting" edges.
	/// </summary>
	void AddWaitingEdges();

	/// <summary>
	/// Add Bus connections edges.
	/// </summary>
	void AddBusEdges();

	/// <summary>
	/// Create router from graph.
	/// </summary>
	void InitializeRouter();


	/// <summary>
	/// Transform "raw" result from Router class
	/// </summary>
	/// <param name="route"></param>
	/// <returns></returns>
	TransportRouteInfo PostProcessFindRoute(std::optional<graph::Router<double>::RouteInfo> route) const;


	/// <summary>
	/// Store the information about the given Edge: bus_name/time needed/...
	/// </summary>
	std::map<graph::EdgeId, EdgeData> edgeid_to_data;

	std::map<graph::VertexId, VertexData> vertexid_to_data;

	/// <summary>
	/// Store info to retrieve VertexId from Stop*.
	/// </summary>
	std::map<catalogue::Stop*, graph::VertexId> stop_to_vertexid;


	/// <summary>
	/// Underyling Transport Catalogue
	/// </summary>
	catalogue::TransportCatalogue& catalogue_;
	
	/// <summary>
	/// Settings to compute the weights of the edges.
	/// </summary>
	RoutingSettings settings_ ;

	
	graph::DirectedWeightedGraph<double> network_graph_;

	std::unique_ptr<graph::Router<double>> router_ptr_;


};