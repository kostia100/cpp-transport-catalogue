#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "transport_catalogue.h"



namespace catalogue {


	namespace input {

		enum class InputType {
			STOP,
			BUS
		};

		struct InputRequest {
			InputType type;
			std::string text;
		};

		namespace detail {

			std::string Ltrim(const std::string& s);

			std::string Rtrim(const std::string& s);

			std::string Trim(const std::string& s);


			std::pair<std::string, std::string> Split(std::string& line, char by);

		}

		struct StopInputData {
			std::string name;
			std::pair<double, double> coordinates;
			std::unordered_map<std::string, int> connected_stop;
		};

		struct BusInputData {
			std::string name;
			std::vector<std::string> stops;
			std::string end_stop;
		};



		/// <summary>
		/// Decide if request is BUS/STOP request and communicate the rest of the request.
		/// </summary>
		std::vector<InputRequest> ReadInputRequests(std::istream& input);


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
		void AddInputRequest(const std::vector<input::InputRequest>& request, TransportCatalogue& catalogue);


		/// <summary>
		/// Add requests coming from JSON
		/// </summary>
		/// <param name="bus_data"></param>
		/// <param name="stop_data"></param>
		/// <param name="catalogue"></param>
		void AddBaseRequests(
			std::vector<input::StopInputData>stop_data,
			std::vector<input::BusInputData> bus_data,
			TransportCatalogue& catalogue);

		/// <summary>
		/// Reads input from istream, parse, write to catalogue.
		/// </summary>
		/// <param name="input"></param>
		/// <param name="catalogue"></param>
		void SetDataToCatalogue(std::istream& input, TransportCatalogue& catalogue);

		//-------Requests 

		/// <summary>
		/// Type of output the use can request: Stop/Bus.
		/// </summary>
		enum class OutputType {
			STOP,
			BUS,
			MAP
		};


		/// <summary>
		/// A output request: type+content.
		/// </summary>
		struct OutputRequest {
			OutputType type;
			std::string text;
		};

		/// <summary>
		/// A JSON output request: same but with index
		/// </summary>
		struct JsonOutputRequest {
			int index;
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
