#include "serialization.h"
#include "geo.h"


namespace serialization {

	//-----------------------------------------------------------------------------------------------------
	//----COLOR---
	//-----------------------------------------------------------------------------------------------------

	catalogue_serialize::Color SerializeColor(svg::Color color) {
		catalogue_serialize::Color object;

		if (std::holds_alternative<std::monostate>(color)) {
			catalogue_serialize::Monostate mono;
			mono.set_exists(true);
			*object.mutable_monostate() = mono;
		}
		else if (std::holds_alternative<std::string>(color)) {
			const std::string& col = std::get<std::string>(color);
			catalogue_serialize::ColorString string_col;
			string_col.set_name(col);
			*object.mutable_colstring() = string_col;
		}
		else if (std::holds_alternative<svg::Rgb>(color)) {
			const svg::Rgb& col = std::get<svg::Rgb>(color);
			catalogue_serialize::Rgb rgb_col;
			rgb_col.set_red(col.red);
			rgb_col.set_green(col.green);
			rgb_col.set_blue(col.blue);
			*object.mutable_rgb() = rgb_col;
		}
		else if (std::holds_alternative<svg::Rgba>(color)) {
			const svg::Rgba& col = std::get<svg::Rgba>(color);
			catalogue_serialize::Rgba rgba_col;
			rgba_col.set_red(col.red);
			rgba_col.set_green(col.green);
			rgba_col.set_blue(col.blue);
			rgba_col.set_opacity(col.opacity);
			*object.mutable_rgba() = rgba_col;
			
		}
		return object;
	}

	svg::Color DeserializeColor(catalogue_serialize::Color color) {
		if (color.has_colstring()) {
			
			catalogue_serialize::ColorString object_string = *color.mutable_colstring();

			return svg::Color(*object_string.mutable_name());
		}
		else if (color.has_rgb()) {
			catalogue_serialize::Rgb object_rgb = *color.mutable_rgb();
			svg::Rgb color_rgb;
			color_rgb.red = object_rgb.red();
			color_rgb.green = object_rgb.green();
			color_rgb.blue = object_rgb.blue();

			return color_rgb;
		}
		else if (color.has_rgba()) {
			catalogue_serialize::Rgba object_rgba = *color.mutable_rgba();
			svg::Rgba color_rgba;
			color_rgba.red = object_rgba.red();
			color_rgba.green = object_rgba.green();
			color_rgba.blue = object_rgba.blue();
			color_rgba.opacity = object_rgba.opacity();
			return color_rgba;
		}
		else {
			return std::monostate();
		}
	}

	//-----------------------------------------------------------------------------------------------------
	//----MAP SETTINGS---
	//-----------------------------------------------------------------------------------------------------


	catalogue_serialize::RendererParameters SerializeMapSettings(const catalogue::renderer::RendererParameters& params) {
		catalogue_serialize::RendererParameters object;
		object.set_width(params.width);
		object.set_height(params.height);
		object.set_padding(params.padding);
		object.set_stop_radius(params.stop_radius);
		object.set_line_width(params.line_width);

		object.set_bus_label_font_size(params.bus_label_font_size);
		for (double bus_offset : params.bus_label_offset) {
			object.add_bus_label_offset(bus_offset);
		}

		object.set_stop_label_font_size(params.stop_label_font_size);
		for (double stop_offset : params.stop_label_offset) {
			object.add_stop_label_offset(stop_offset);
		}

		*object.mutable_underlayer_color() = SerializeColor(params.underlayer_color);
		object.set_underlayer_width(params.underlayer_width);
		for (auto color : params.color_palette) {
			catalogue_serialize::Color serialized_col = SerializeColor(color);
			object.add_color_palette()->CopyFrom(serialized_col);
		}

		return object;
	}


	catalogue::renderer::RendererParameters DeserializeMapSettings(catalogue_serialize::RendererParameters& object) {
		catalogue::renderer::RendererParameters params;

		params.width = object.width();
		params.height = object.height();
		params.padding = object.padding();
		params.stop_radius = object.stop_radius();
		params.line_width = object.line_width();

		params.bus_label_font_size = object.bus_label_font_size();
		std::vector<double> bus_offset;
		for (int j = 0; j < object.bus_label_offset_size(); ++j) {
			bus_offset.push_back(object.bus_label_offset(j));
		}

		params.bus_label_offset = std::move(bus_offset);

		params.stop_label_font_size = object.stop_label_font_size();
		std::vector<double> stop_offset;
		for (int j = 0; j < object.stop_label_offset_size(); ++j) {
			stop_offset.push_back(object.stop_label_offset(j));
		}

		params.stop_label_offset = std::move(stop_offset);

		catalogue_serialize::Color underlayer_col = *object.mutable_underlayer_color();
		params.underlayer_color = DeserializeColor(underlayer_col);
		params.underlayer_width = object.underlayer_width();

		std::vector<svg::Color> palette;
		for (int j = 0; j < object.color_palette_size(); ++j) {
			palette.push_back(DeserializeColor(*object.mutable_color_palette(j)));
		}

		params.color_palette = std::move(palette);
		return params;
	}

	//-----------------------------------------------------------------------------------------------------
	//----ROUTERSETTINGS---
	//-----------------------------------------------------------------------------------------------------


	catalogue_serialize::RouterParameters SerializeRouterSettings(RoutingSettings routing) {
		catalogue_serialize::RouterParameters object;
		object.set_velocity(routing.velocity);
		object.set_wait_time(routing.wait_time);
		return object;
	}


	RoutingSettings DeserializeRouterSettings(catalogue_serialize::RouterParameters& object) {
		RoutingSettings routing{ object.velocity(),object.wait_time()};
		return routing;
	}

	//-----------------------------------------------------------------------------------------------------
	//----CATALOGUE---
	//-----------------------------------------------------------------------------------------------------


	catalogue_serialize::TransportCatalogue SerializeTransportCatalogue(const catalogue::TransportCatalogue& catalogue) {
		catalogue_serialize::TransportCatalogue object;
		//Serialize Stops
		for (const catalogue::Stop& stp : catalogue.GetStops()) {
			//Serialize name
			catalogue_serialize::Stop current_stop;
			current_stop.set_name(stp.stop_name);

			//Serialize coordinates
			catalogue_serialize::Coordinates coord;
			coord.set_lat(stp.location.lat);
			coord.set_lng(stp.location.lng);

			*current_stop.mutable_location() = coord;

			//Serialize next stops
			for (auto& [name, distance] : catalogue.GetConnectedStops(stp.stop_name)) {
				catalogue_serialize::NextStop nextstop;
				nextstop.set_name(name);
				nextstop.set_distance(distance);
				current_stop.add_next()->CopyFrom(nextstop);
			}

			object.add_stop()->CopyFrom(current_stop);
		}

		//Serialize Bus
		for (const catalogue::Bus& bus : catalogue.GetBuses()) {
			catalogue_serialize::Bus current_bus;
			current_bus.set_name(bus.bus_name);
			current_bus.set_end_stop(bus.end_stop->stop_name);

			for (auto& stop_ptr : bus.stops) {
				current_bus.add_stop(stop_ptr->stop_name);
			}

			object.add_bus()->CopyFrom(current_bus);
		}

		//Serialize TransportCatalogue
		return object;

	}

	void AddInfoFromDeserializedData(
		catalogue::TransportCatalogue& catalogue,
		const std::vector<DeserializedStop>& stop_data,
		const std::vector<DeserializedBus>& bus_data) {

		for (const auto& elem : stop_data) {
			catalogue.AddStop(elem.stop.stop_name, { elem.stop.location.lat, elem.stop.location.lng });
		}

		for (const auto& elem : stop_data) {
			for (auto stop_end : elem.connected_stops) {
				catalogue.AddNearestStops(elem.stop.stop_name, stop_end.first, stop_end.second);
			}
		}

		for (const auto& elem : bus_data) {
			catalogue.AddBus(elem.name, elem.stops, elem.end_stop);
		}

	}



	catalogue::TransportCatalogue DeserializeTransportCatalogue(catalogue_serialize::TransportCatalogue& object) {

		catalogue::TransportCatalogue tc;
		std::vector<DeserializedStop> stops;
		for (size_t q = 0; q < object.stop_size();++q) {
			catalogue_serialize::Stop current = *object.mutable_stop(q);
			catalogue_serialize::Coordinates coord = *current.mutable_location();
			std::string stop_name = *current.mutable_name();
			catalogue::geo::Coordinates location{ coord.lat(),coord.lng() };

			std::vector<std::pair<std::string, double>> connections;
			for (size_t y = 0; y < current.next_size(); ++y) {
				catalogue_serialize::NextStop next = *current.mutable_next(y);
				connections.push_back({ *next.mutable_name() , next.distance() });
			}
			DeserializedStop deserialzed_stop{ { stop_name ,{location} } ,connections };
			stops.push_back(deserialzed_stop);
		}
		//

		//Deserialize Buses and add to TC

		std::vector<DeserializedBus> buses;
		for (size_t q = 0; q < object.bus_size();++q) {
			catalogue_serialize::Bus current_bus = *object.mutable_bus(q);
			std::string bus_name = *current_bus.mutable_name();

			std::vector<std::string> stops;

			for (size_t r = 0; r < current_bus.stop_size(); ++r) {
				stops.push_back(*current_bus.mutable_stop(r));
			}

			std::string end_stop = *current_bus.mutable_end_stop();;
			buses.push_back({ bus_name ,stops,end_stop });
		}

		AddInfoFromDeserializedData(tc, stops, buses);

		return tc;
	
	}

	//-----------------------------------------------------------------------------------------------------
	//----SYSTEM---
	//-----------------------------------------------------------------------------------------------------

	void SerializeTransportSystem(
		const catalogue::TransportCatalogue& catalogue,
		const catalogue::renderer::RendererParameters& params,
		const RoutingSettings& routing,
		std::ostream& output) {

		catalogue_serialize::TransportSystem object;

		*object.mutable_parameters() = SerializeMapSettings(params);
		*object.mutable_catalogue() = SerializeTransportCatalogue(catalogue);
		*object.mutable_routing() = SerializeRouterSettings(routing);


		object.SerializeToOstream(&output);

	}

	catalogue::TransportCatalogue DeserializeTransportSystem(
		catalogue::renderer::RendererParameters& params,
		RoutingSettings& routing,
		std::istream& input) {

		catalogue_serialize::TransportSystem object;
		if (!object.ParseFromIstream(&input)) {
			return catalogue::TransportCatalogue {};
		}

		params = DeserializeMapSettings(*object.mutable_parameters());
		routing = DeserializeRouterSettings(*object.mutable_routing());
		catalogue::TransportCatalogue tc = DeserializeTransportCatalogue(*object.mutable_catalogue());


		return tc;

	}
	
	

}