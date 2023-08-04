#pragma once

#include "input_reader.h"
#include <vector>
#include <string>
#include <iostream>



namespace catalogue {

	namespace requests {

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


		/// <summary>
		/// Read output requests from stream and parse.
		/// </summary>
		/// <param name="input"></param>
		/// <returns></returns>
		std::vector<OutputRequest> ReadStatRequest(std::istream& input);


		/// <summary>
		/// Send parsed request to catalogue and write result in ostream.
		/// </summary>
		/// <param name="out"></param>
		/// <param name="request"></param>
		/// <param name="cat"></param>
		/// <returns></returns>
		std::ostream& GetStats(std::ostream& out, std::vector<OutputRequest> request, TransportCatalogue& cat);

	}

}

