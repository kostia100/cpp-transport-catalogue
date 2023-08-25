#pragma once

#include "input_reader.h"
#include "transport_catalogue.h"
#include <vector>
#include <string>
#include <iostream>



namespace catalogue {

	namespace request {

		/// <summary>
		/// Type of output the use can request: Stop/Bus.
		/// </summary>
		enum class OutputType {
			STOP,
			BUS
		};


		/// <summary>
		/// A output request: type+content.
		/// </summary>
		struct OutputRequest {
			OutputType type;
			std::string text;
		};


		std::ostream& operator<<(std::ostream& out, const StopInfo& stop);

		std::ostream& operator<<(std::ostream& out, const BusInfo& bus);

		
		/// <summary>
		/// Read request from std::istream, send request to catalogue, write result in std::ostream.
		/// </summary>
		/// <param name="input"></param>
		/// <param name="out"></param>
		/// <param name="catalogue"></param>
		void GetDataFromCatalogue(std::istream& input, std::ostream& out, TransportCatalogue& catalogue);


		/// <summary>
		/// Send parsed request to catalogue and write result in ostream. Used in tests.
		/// </summary>
		/// <param name="out"></param>
		/// <param name="request"></param>
		/// <param name="catalogue"></param>
		/// <returns></returns>
		std::ostream& GetStats(std::ostream& out, std::vector<OutputRequest> request, TransportCatalogue& catalogue);


	}

}

