#include "json_reader.h"
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
			json::Dict dic = elem.AsMap();

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
			json::Dict dic = elem.AsMap();

			if (dic.at("type").AsString() == "Stop") {
				std::string name = dic.at("name").AsString();
				std::pair<double, double> coord{ dic.at("latitude").AsDouble() ,dic.at("longitude").AsDouble() };
				std::unordered_map<std::string, int> stops;
				for (auto stop : dic.at("road_distances").AsMap()) {
					stops[stop.first] = stop.second.AsInt();
				}
				input::StopInputData info{ name,coord,stops };
				result.push_back(info);
			}
		}

		return result;
	}

	std::vector<input::JsonOutputRequest> GetStatRequestsFromJSON(json::Node node) {
		json::Array data = node.AsArray();
		std::vector<input::JsonOutputRequest> result;
		for (auto elem : data) {
			json::Dict dic = elem.AsMap();
			if (dic.at("type").AsString() == "Stop") {
				input::JsonOutputRequest output{ dic.at("id").AsInt(), input::OutputType::STOP , dic.at("name").AsString() };
				result.push_back(output);
			}
			else if (dic.at("type").AsString() == "Bus") {
				input::JsonOutputRequest output{ dic.at("id").AsInt(), input::OutputType::BUS , dic.at("name").AsString() };
				result.push_back(output);
			}
			else if (dic.at("type").AsString() == "Map") {
				input::JsonOutputRequest output{ dic.at("id").AsInt(), input::OutputType::MAP , "" };
				result.push_back(output);
			}

		}

		return result;

	}

	json::Node StopToNode(const StopInfo& stop, int index) {
		json::Dict result;
		result["request_id"] = index ;
		if (!stop.stop_exists) {
			std::string message = "not found";
			result["error_message"] = message;
			return result;
		}
		json::Array buses;
		for (auto bus : stop.buses) {
			buses.push_back(bus);
		}
		result["buses"] = buses;
		return result;
	}

	json::Node BusToNode(const BusInfo& bus, int index) {
		json::Dict result;
		result["request_id"] = index;
		if (!bus.bus_exists) {
			std::string message = "not found";
			result["error_message"] = message;
			return result;
		}
		result["curvature"] = (double)bus.curvature;
		result["route_length"] = (int)bus.traffic_route_length;
		result["stop_count"] = (int)bus.all_stops;
		result["unique_stop_count"] = (int)bus.unique_stops;
		return result;
	}

	json::Node MapToNode(std::string network_map,  int index) {
		json::Dict result;
		result["request_id"] = index;
		result["map"] = json::Node(network_map);
		return result;
	}

	json::Node GetStatRequests(const std::vector<input::JsonOutputRequest>& requests, TransportCatalogue& catalogue) {
		json::Array result;
		for (auto r : requests) {
			if (r.type == input::OutputType::BUS) {
				BusInfo info =  catalogue.GetBusInfo(r.text);
				json::Node node_bus = BusToNode(info,r.index);
				result.push_back(node_bus);
			} 
			else if (r.type == input::OutputType::STOP) {
				StopInfo info = catalogue.GetStopInfo(r.text);
				json::Node node_stop = StopToNode(info, r.index);
				result.push_back(node_stop);
			}
		}
		return result;
	}


	json::Node GetStatWithMapRequests(const std::vector<input::JsonOutputRequest>& requests, renderer::NetworkDrawingData drawing_data, TransportCatalogue& catalogue) {
		json::Array result;
		for (auto r : requests) {
			if (r.type == input::OutputType::BUS) {
				BusInfo info = catalogue.GetBusInfo(r.text);
				json::Node node_bus = BusToNode(info, r.index);
				result.push_back(node_bus);
			}
			else if (r.type == input::OutputType::STOP) {
				StopInfo info = catalogue.GetStopInfo(r.text);
				json::Node node_stop = StopToNode(info, r.index);
				result.push_back(node_stop);
			}
			else if (r.type == input::OutputType::MAP) {
				std::ostringstream map_streamed;
				renderer::DrawNetworkMap(map_streamed, drawing_data);
				json::Node map_node = MapToNode(map_streamed.str(),r.index);
				result.push_back(map_node);
			}
		}
		return result;
	}

	
	void JSONInfoRequest(std::istream& input, std::ostream& output, TransportCatalogue& catalogue) {
		json::Document doc = ReadJSON( input);
		json::Node nd = doc.GetRoot();
		std::map<std::string, json::Node> mp = nd.AsMap();
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
		json::Dict rdr_settings = settings.AsMap();
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
		std::map<std::string, json::Node> mp = nd.AsMap();
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
		std::map<std::string, json::Node> mp = nd.AsMap();
		json::Node base_requests_vector = mp.at("base_requests");
		json::Node render_settings_map = mp.at("render_settings");
		json::Node stat_requests_vector = mp.at("stat_requests");

		std::vector<input::BusInputData> inputs_bus = GetBusRequestsFromJSON(base_requests_vector);
		std::vector<input::StopInputData> inputs_stops = GetStopRequestsFromJSON(base_requests_vector);
		std::vector<input::JsonOutputRequest> requests = GetStatRequestsFromJSON(stat_requests_vector);
		renderer::RendererParameters params = GetParametersFromNode(render_settings_map);


		input::AddBaseRequests(inputs_stops, inputs_bus, catalogue);
		
		std::vector<Bus*> buses = catalogue.GetAllBuses();
		std::vector<Stop*> stopsPtr = catalogue.GetStopsPtrInNetwork();
		renderer::NetworkDrawingData drawing_data{ buses,stopsPtr,  params };

		json::Node output_node = GetStatWithMapRequests(requests, drawing_data, catalogue);
		output << Print(output_node);

	}

}