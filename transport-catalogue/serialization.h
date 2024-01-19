#pragma once
#include <iostream>
#include <transport_catalogue.pb.h>
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "transport_router.h"



//serialization

namespace serialization{

	struct DeserializedStop {
		catalogue::Stop stop;
		std::vector<std::pair<std::string, double>> connected_stops;
	};

	struct DeserializedBus {
		std::string name;
		std::vector<std::string> stops;
		std::string end_stop;
	};



	catalogue_serialize::RendererParameters SerializeMapSettings(const catalogue::renderer::RendererParameters& params);


	catalogue::renderer::RendererParameters DeserializeMapSettings(catalogue_serialize::RendererParameters& object);

	void AddInfoFromDeserializedData(
		catalogue::TransportCatalogue& catalogue,
		const std::vector<DeserializedStop>& stop_data,
		const std::vector<DeserializedBus>& bus_data);

	catalogue_serialize::RouterParameters SerializeRouterSettings(RoutingSettings routing);


	RoutingSettings DeserializeRouterSettings(catalogue_serialize::RouterParameters& object);

	
	catalogue_serialize::TransportCatalogue SerializeTransportCatalogue(const catalogue::TransportCatalogue& catalogue);


	catalogue::TransportCatalogue DeserializeTransportCatalogue(catalogue_serialize::TransportCatalogue& object);

	void SerializeTransportSystem(
		const catalogue::TransportCatalogue& catalogue,
		const catalogue::renderer::RendererParameters& params,
		const RoutingSettings& routing,
		std::ostream& output);

	catalogue::TransportCatalogue DeserializeTransportSystem(
		catalogue::renderer::RendererParameters& params,
		RoutingSettings& routing,
		std::istream& input);
	
}