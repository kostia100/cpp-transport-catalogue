#pragma once

#include "json.h"
#include "request_handler.h"
#include "map_renderer.h"
#include <iostream>
#include "transport_router.h"


namespace catalogue {


	/// <summary>
	/// Parse JSON
	/// </summary>
	/// <param name="input"></param>
	/// <returns></returns>
	json::Document ReadJSON(std::istream& input);



	std::vector<input::BusInputData> GetBusRequestsFromJSON(json::Node node);

	std::vector<input::StopInputData> GetStopRequestsFromJSON(json::Node node);

	std::vector<input::JsonOutputRequest> GetStatRequestsFromJSON(json::Node node);

	std::string GetSerializationSettingFromJSON(json::Node node);

	/// <summary>
	/// Parse routing settings from a JSON.
	/// </summary>
	RoutingSettings GetRoutingSettings(json::Node node);

	




	json::Node StopToNode(const StopInfo& stop, int index);

	json::Node BusToNode(const BusInfo& bus, int index);

	json::Node MapToNode(std::string network_map, int index);

	/// <summary>
	/// Transform a route-times (bus/wait) to std::vector of json::Node.
	/// </summary>
	json::Array RouteItemsToNode(std::vector<std::variant<WaitItem, BusItem>> input);

	/// <summary>
	/// transform a RouteInfo object into a json::Node
	/// </summary>
	json::Node RouteToNode(const TransportRouteInfo& route_info, int index);



	json::Node GetStatRequests(const std::vector<input::JsonOutputRequest>& requests ,TransportCatalogue& catalogue);


	json::Node GetStatWithMapRequests(
		const std::vector<input::JsonOutputRequest>& requests,
		renderer::NetworkDrawingData drawing_data,
		TransportCatalogue& catalogue,
		TransportGraphWrapper& graph_wrapper);


	/// <summary>
	/// Parse JSON node to std::vector<int>.
	/// </summary>
	/// <param name="nd"></param>
	/// <returns></returns>
	std::vector<double> GetVectorFromNode(json::Node nd);

	/// <summary>
	/// Parse JSON node a svg::Color.
	/// </summary>
	/// <param name="nd"></param>
	/// <returns></returns>
	svg::Color GetColorFromNode(json::Node nd);

	/// <summary>
	/// Parse settings for SVG drawing from a JSON node.
	/// </summary>
	/// <param name="settings"></param>
	/// <returns></returns>
	renderer::RendererParameters GetParametersFromNode(json::Node settings);

	/// <summary>
	/// Read JSON input = (base_request+stat_request), write+ask TC, return output info in JSON format.
	/// </summary>
	/// <param name="input"></param>
	/// <param name="output"></param>
	/// <param name="catalogue"></param>
	void JSONInfoRequest(std::istream& input, std::ostream& output, TransportCatalogue& catalogue);


	/// <summary>
	/// Read JSON input = (base_request+render_settings+stat_request), write to TC, return data for map drawing.
	/// </summary>
	renderer::NetworkDrawingData JSONMapRequest(std::istream& input,TransportCatalogue& catalogue);



	/// <summary>
	/// Read JSON full input = (base_request+render_settings+stat_request), write to TC, return response to
	/// stat_request including map drawing.
	/// </summary>
	/// <param name="input"></param>
	/// <param name="output"></param>
	/// <param name="catalogue"></param>
	void JSONRequest(std::istream& input, std::ostream& output, TransportCatalogue& catalogue);

	/// <summary>
	/// Read JSON, transform to TC and serialize to a .db file.
	/// </summary>
	/// <param name="input"></param>
	/// <returns></returns>
	void MakeBase(std::istream& input);



	/// <summary>
	/// Deserialize from .db file to TC and process requests.
	/// </summary>
	/// <param name="input"></param>
	/// <param name="output"></param>
	/// <param name="catalogue"></param>
	void ProcessRequests(std::istream& input, std::ostream& output);

}