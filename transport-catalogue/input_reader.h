#pragma once

#include <vector>
#include <string>
#include "transport_catalogue.h"


namespace catalogue {

	namespace input {

		enum class InputType {
			STOP,
			BUS
		};

		struct IntputRequest {
			InputType type;
			std::string text;
		};

		namespace detail {

			std::string Ltrim(const std::string& s);

			std::string Rtrim(const std::string& s);

			std::string Trim(const std::string& s);


			std::pair<std::string, std::string> Split(std::string& line, char by);

		}

		/// <summary>
		/// Decide if request is BUS/STOP request and communicate the rest of the request.
		/// </summary>
		std::vector<IntputRequest> ReadInputRequests(std::istream& input);

		

		




		/// <summary>
		/// Parse Stop text to stop_name + coordinates + stops nearby
		/// </summary>
		StopInputData ParseStopData(const std::string& text);

		/// <summary>
		/// Parse Bus text to bus_name + full stop route
		/// </summary>
		std::pair<std::string, std::vector<std::string>> ParseBusData(const std::string& text);

		/// <summary>
		/// Add request to data base
		/// </summary>
		void AddInputRequest(const std::vector<IntputRequest>& request, TransportCatalogue& cat);

	}



}
