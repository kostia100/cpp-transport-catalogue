#include "json_reader.h"
#include "json_builder.h"
#include <fstream>
#include <sstream>
#include <filesystem>


namespace catalogue {


	json::Document ReadJSON(std::istream& input) {
		std::stringstream data;
		std::string line;
		while (getline(input, line)) {
			data << line;
		}
		json::Document doc = json::LoadJSON(data.str());
		return doc;
	}


	std::vector<input::BusInputData> GetBusRequestsFromJSON(json::Node node) {
		json::Array data = node.AsArray();
		std::vector<input::BusInputData> result;
		std::string end_stop;
		for (auto elem : data) {
			json::Dict dic = elem.AsDict();

			if (dic.at("type").AsString() == "Bus") {
				std::string name = dic.at("name").AsString();
				std::vector<std::string> stops;
				std::vector<json::Node> stops_raw = dic.at("stops").AsArray();
				bool is_circular = dic.at("is_roundtrip").AsBool();
				if (is_circular) {
					end_stop = stops_raw[0].AsString();
					for (auto stp : stops_raw) {
						stops.push_back(stp.AsString());
					}
				}
				else {
					end_stop = stops_raw[stops_raw.size()-1].AsString();
					for (auto stp : stops_raw) {
						stops.push_back(stp.AsString());
					}

					int q = (int)(stops.size() - 2);
					{
						while (q >= 0) {
							std::string stp = stops[q];
							stops.push_back(stp);
							--q;
						}
					}

				}

				input::BusInputData info{ name, stops ,end_stop };
				result.push_back(info);
			}
		}

		return result;
	}



	std::vector<input::StopInputData> GetStopRequestsFromJSON(json::Node node) {
		json::Array data = node.AsArray();
		std::vector<input::StopInputData> result;
		for (auto elem : data ) {
			json::Dict dic = elem.AsDict();

			if (dic.at("type").AsString() == "Stop") {
				std::string name = dic.at("name").AsString();
				std::pair<double, double> coord{ dic.at("latitude").AsDouble() ,dic.at("longitude").AsDouble() };
				std::unordered_map<std::string, int> stops;
				for (auto stop : dic.at("road_distances").AsDict()) {
					stops[stop.first] = stop.second.AsInt();
				}
				input::StopInputData info{ name,coord,stops };
				result.push_back(info);
			}
		}

		return result;
	}

	std::vector<input::JsonOutputRequest> GetStatRequestsFromJSON(json::Node node) {
		using namespace std::literals;
		json::Array data = node.AsArray();
		std::vector<input::JsonOutputRequest> result;
		for (auto elem : data) {
			json::Dict dic = elem.AsDict();
			if (dic.at("type"s).AsString() == "Stop"s) {
				input::JsonOutputRequest output{ dic.at("id"s).AsInt(), input::OutputType::STOP , dic.at("name"s).AsString(), ""s ,""s };
				result.push_back(output);
			}
			else if (dic.at("type"s).AsString() == "Bus"s) {
				input::JsonOutputRequest output{ dic.at("id"s).AsInt(), input::OutputType::BUS , dic.at("name"s).AsString() , ""s ,""s };
				result.push_back(output);
			}
			else if (dic.at("type"s).AsString() == "Map"s) {
				input::JsonOutputRequest output{ dic.at("id").AsInt(), input::OutputType::MAP  , ""s , ""s ,""s };
				result.push_back(output);
			}
			else if (dic.at("type").AsString() == "Route") {
				input::JsonOutputRequest output{ dic.at("id").AsInt(), input::OutputType::ROUTE , ""s , dic.at("from").AsString(), dic.at("to").AsString() };
				result.push_back(output);
			}

		}

		return result;

	}

	RoutingSettings GetRoutingSettings(json::Node settings) {
		double velocity = settings.AsDict().at("bus_velocity").AsDouble();
		double wait_time = settings.AsDict().at("bus_wait_time").AsInt();
		return { velocity ,wait_time };
	}
	

	json::Node StopToNode(const StopInfo& stop, int index) {
		using namespace std::literals;
		if (!stop.stop_exists) {
			return json::Builder{}.StartDict()
								  .Key("request_id"s).Value(index)
								  .Key("error_message"s).Value("not found"s)
							   	  .EndDict().Build();
		}
		json::Array buses;
		for (auto bus : stop.buses) {
			buses.push_back(bus);
		}

		return json::Builder{}
						.StartDict()
						.Key("request_id"s).Value(index)
						.Key("buses"s).Value(buses)
						.EndDict().Build();
		
	}
	
	json::Node BusToNode(const BusInfo& bus, int index) {
		using namespace std::literals;
		if (!bus.bus_exists) {
			return json::Builder{}
							.StartDict()
							.Key("request_id"s).Value(index)
							.Key("error_message"s).Value("not found"s)
							.EndDict().Build();
		}
		
		return json::Builder{}
			.StartDict()
			.Key("request_id"s).Value(index)
			.Key("curvature"s).Value((double)bus.curvature)
			.Key("route_length"s).Value((int)bus.traffic_route_length)
			.Key("stop_count"s).Value((int)bus.all_stops)
			.Key("unique_stop_count"s).Value((int)bus.unique_stops)
			.EndDict().Build();
	}
	

	json::Node MapToNode(std::string network_map, int index) {
		using namespace std::literals;
		return json::Builder{}
						.StartDict()
						.Key("request_id"s).Value(index)
						.Key("map"s).Value(network_map)
						.EndDict().Build();
					
		
	}


	json::Array RouteItemsToNode(std::vector<std::variant<WaitItem, BusItem>> input) {
		using namespace std::literals;
		json::Array items;
		for (const auto elem : input) {
			if (std::holds_alternative<WaitItem>(elem)) {
				WaitItem val = std::get<WaitItem>(elem);
				items.push_back(json::Builder{}
					.StartDict()
					.Key("stop_name"s).Value(val.stop_name)
					.Key("time"s).Value(val.time)
					.Key("type"s).Value("Wait"s)
					.EndDict().Build());
			}
			else if (std::holds_alternative<BusItem>(elem)) {
				BusItem val = std::get<BusItem>(elem);
				items.push_back(json::Builder{}
					.StartDict()
					.Key("bus"s).Value(val.bus_name)
					.Key("span_count"s).Value(val.span_count)
					.Key("time"s).Value(val.time)
					.Key("type"s).Value("Bus"s)
					.EndDict().Build());
			}
		}
		return items;
	}

	json::Node RouteToNode(const TransportRouteInfo& route_info, int index) {
		using namespace std::literals;
		if (!route_info.rout_exists) {
			//route does not exist
			return json::Builder{}
				.StartDict()
				.Key("request_id"s).Value(index)
				.Key("error_message"s).Value("not found"s)
				.EndDict().Build();
		}
		//route exist
		return json::Builder{}
						.StartDict()
						.Key("request_id"s).Value(index)
						.Key("total_time"s).Value(route_info.total_time)
						.Key("items"s).Value(RouteItemsToNode(route_info.items))
						.EndDict().Build();
	}

	json::Node GetStatRequests(const std::vector<input::JsonOutputRequest>& requests, TransportCatalogue& catalogue) {
		json::Array result;
		for (auto r : requests) {
			if (r.type == input::OutputType::BUS) {
				BusInfo info =  catalogue.GetBusInfo(r.name);
				json::Node node_bus = BusToNode(info,r.index);
				result.push_back(node_bus);
			} 
			else if (r.type == input::OutputType::STOP) {
				StopInfo info = catalogue.GetStopInfo(r.name);
				json::Node node_stop = StopToNode(info, r.index);
				result.push_back(node_stop);
			}
		}
		return result;
	}




	json::Node GetStatWithMapRequests(
		const std::vector<input::JsonOutputRequest>& requests,
		renderer::NetworkDrawingData drawing_data,
		TransportCatalogue& catalogue,
		TransportGraphWrapper& graph_wrapper,
		graph::Router<double>& router
		) {
		json::Array result;
		for (auto r : requests) {
			if (r.type == input::OutputType::BUS) {
				BusInfo info = catalogue.GetBusInfo(r.name);
				json::Node node_bus = BusToNode(info, r.index);
				result.push_back(node_bus);
			}
			else if (r.type == input::OutputType::STOP) {
				StopInfo info = catalogue.GetStopInfo(r.name);
				json::Node node_stop = StopToNode(info, r.index);
				result.push_back(node_stop);
			}
			else if (r.type == input::OutputType::MAP) {
				std::ostringstream map_streamed;
				renderer::DrawNetworkMap(map_streamed, drawing_data);
				json::Node map_node = MapToNode(map_streamed.str(),r.index);
				result.push_back(map_node);
			}
			else if (r.type == input::OutputType::ROUTE) {
				TransportRouteInfo output = graph_wrapper.FindRoute(router,{ catalogue.FindStop(r.start),catalogue.FindStop(r.end) });
				//here compute with the Router the route
				json::Node route_node = RouteToNode(output,r.index);
				result.push_back(route_node);
			}
		}
		return result;
	}

	
	void JSONInfoRequest(std::istream& input, std::ostream& output, TransportCatalogue& catalogue) {
		json::Document doc = ReadJSON( input);
		json::Node nd = doc.GetRoot();
		std::map<std::string, json::Node> mp = nd.AsDict();
		json::Node base_requests_vector = mp.at("base_requests");
		json::Node stat_requests_vector = mp.at("stat_requests");

		std::vector<input::BusInputData> inputs_bus = GetBusRequestsFromJSON(base_requests_vector);
		std::vector<input::StopInputData> inputs_stops = GetStopRequestsFromJSON(base_requests_vector);
		std::vector<input::JsonOutputRequest> requests =  GetStatRequestsFromJSON(stat_requests_vector);

		input::AddBaseRequests(inputs_stops, inputs_bus, catalogue);

		json::Node output_node = GetStatRequests(requests,  catalogue);
		output << Print(output_node);
	}
	

	renderer::RendererParameters GetParametersFromNode(json::Node settings) {
		renderer::RendererParameters params;
		json::Dict rdr_settings = settings.AsDict();
		params.width = rdr_settings["width"].AsDouble();
		params.height = rdr_settings["height"].AsDouble();
		params.padding = rdr_settings["padding"].AsDouble();
		params.stop_radius  = rdr_settings["stop_radius"].AsDouble();
		params.line_width = rdr_settings["line_width"].AsDouble();

		params.bus_label_font_size = rdr_settings["bus_label_font_size"].AsDouble();
		params.bus_label_offset = std::move(GetVectorFromNode(rdr_settings["bus_label_offset"]));

		params.stop_label_font_size = rdr_settings["stop_label_font_size"].AsDouble();
		params.stop_label_offset = std::move(GetVectorFromNode(rdr_settings["stop_label_offset"]));
		
		params.underlayer_color = GetColorFromNode(rdr_settings["underlayer_color"]);
		params.underlayer_width = rdr_settings["underlayer_width"].AsDouble();

		std::vector<svg::Color> colors;
		for(const auto clr : rdr_settings["color_palette"].AsArray()){
			colors.push_back(GetColorFromNode(clr));
		}
		params.color_palette = std::move(colors);

		return params;
	}

	std::vector<double> GetVectorFromNode(json::Node nd) {
		std::vector<double> offset;
		json::Array arr = nd.AsArray();
		for (const auto elem : arr) {
			offset.push_back(elem.AsDouble());
		}

		return offset;
	}

	svg::Color GetColorFromNode(json::Node nd) {
		if (nd.IsString()) {
			return svg::Color(nd.AsString());
		}
		else {
			json::Array arr = nd.AsArray();
			if (arr.size() == 3) {
				uint8_t r = (uint8_t)arr[0].AsInt();
				uint8_t g = (uint8_t)arr[1].AsInt();
				uint8_t b = (uint8_t)arr[2].AsInt();
				return svg::Rgb(r,g,b);
			}
			else if(arr.size() == 4) {
				uint8_t r = (uint8_t)arr[0].AsInt();
				uint8_t g = (uint8_t)arr[1].AsInt();
				uint8_t b = (uint8_t)arr[2].AsInt();
				double opac = arr[3].AsDouble();
				return svg::Rgba(r, g, b, opac);
			}
			else {
				throw std::invalid_argument("Invalid argument in GetColorFromNode");
			}

		}

	}

	renderer::NetworkDrawingData JSONMapRequest(std::istream& input, TransportCatalogue& catalogue) {
		json::Document docJSON = ReadJSON(input);
		json::Node nd = docJSON.GetRoot();
		std::map<std::string, json::Node> mp = nd.AsDict();
		json::Node base_requests_vector = mp.at("base_requests");
		json::Node render_settings_map = mp.at("render_settings");

		std::vector<input::BusInputData> inputs_bus = GetBusRequestsFromJSON(base_requests_vector);
		std::vector<input::StopInputData> inputs_stops = GetStopRequestsFromJSON(base_requests_vector);
		
		renderer::RendererParameters params = GetParametersFromNode(render_settings_map);


		input::AddBaseRequests(inputs_stops, inputs_bus, catalogue);
		std::vector<Bus*> buses = catalogue.GetAllBuses();

		std::vector<Stop*> stopsPtr = catalogue.GetStopsPtrInNetwork();

		return renderer::NetworkDrawingData{ buses,stopsPtr,  params };
	}



	void JSONRequest(std::istream& input, std::ostream& output, TransportCatalogue& catalogue) {
		json::Document docJSON = ReadJSON(input);
		json::Node nd = docJSON.GetRoot();
		std::map<std::string, json::Node> mp = nd.AsDict();
		json::Node base_requests_vector = mp.at("base_requests");
		json::Node render_settings_map = mp.at("render_settings");
		json::Node routing_settings = mp.at("routing_settings");
		json::Node stat_requests_vector = mp.at("stat_requests");

		std::vector<input::BusInputData> inputs_bus = GetBusRequestsFromJSON(base_requests_vector);
		std::vector<input::StopInputData> inputs_stops = GetStopRequestsFromJSON(base_requests_vector);
		//next method GetStatRequestsFromJSON needs to be completed
		std::vector<input::JsonOutputRequest> requests = GetStatRequestsFromJSON(stat_requests_vector);

		renderer::RendererParameters params = GetParametersFromNode(render_settings_map);


		input::AddBaseRequests(inputs_stops, inputs_bus, catalogue);
		
		//construct graph with routing_settings + catalogue
		RoutingSettings rt_settings = GetRoutingSettings(routing_settings);
		std::vector<Bus*> buses = catalogue.GetAllBuses();
		std::vector<Stop*> stopsPtr = catalogue.GetStopsPtrInNetwork();
		renderer::NetworkDrawingData drawing_data{ buses,stopsPtr,  params };

		TransportGraphWrapper graph_wrapper(rt_settings, catalogue);
		graph::DirectedWeightedGraph<double> graph =  graph_wrapper.BuildTransportGraph();
		graph::Router router(graph);


		//complete also the next method GetStatWithMapRequests
		json::Node output_node = GetStatWithMapRequests(requests, drawing_data, catalogue, graph_wrapper,router);
		output << Print(output_node);

	}

}