#pragma once
#include "svg.h"
#include "domain.h"
#include <iostream>

namespace catalogue {


	namespace renderer {

		/// <summary>
		/// settings for SVG items
		/// </summary>
		struct RendererParameters {
			double width;
			double height;
			double padding;
			double stop_radius;
			double line_width;

			int bus_label_font_size;
			std::vector<double> bus_label_offset;
			
			int stop_label_font_size;
			std::vector<double> stop_label_offset;
			

			svg::Color underlayer_color;

			double underlayer_width;

			std::vector<svg::Color> color_palette;
			

		};


		/// <summary>
		/// Data containing info on what to draw (bus-lines+coords) and how.
		/// </summary>
		struct NetworkDrawingData {
			std::vector<Bus*> buses;
			std::vector<Stop*> stops;
			RendererParameters params;
		};

		


		class BusShape : public svg::Drawable {
		public:

			BusShape(std::vector<svg::Point> stops, svg::Color color, double stroke_width);

			void Draw(svg::ObjectContainer& container)  const override; 

		private:

			std::vector<svg::Point> stops_;
			svg::Color color_;
			double stroke_width_;

		};


		class LabelBusShape : public svg::Drawable {
		public: 

			LabelBusShape( std::vector<svg::Point> locations,
							std::vector<double> offset,
							int font_size,
							std::string name_bus,
							svg::Color background_color,
							double stroke_width,
							svg::Color bus_name_color);

			void Draw(svg::ObjectContainer& container) const override;

		private:
			std::vector<svg::Point> locations_;
			std::vector<double> offset_;
			int font_size_;
			// font-family: "Verdana"
			// font-weigth: "bold"
			std::string name_bus_;

			//background:
			svg::Color background_color_;
			double stroke_width_;
			//stroke-linecap = "round"
			//stroke-linejoin = "round

			//label
			svg::Color bus_name_color_;

		};

		class StopShape : public svg::Drawable {
		public:
			StopShape(svg::Point location,double radius);
			void Draw(svg::ObjectContainer& container) const override;

		private:
			svg::Point location_;
			double r_;
		};

		class StopLabelShape : public svg::Drawable {
		public:
			StopLabelShape(svg::Point location,
						  std::vector<double> offset,
						  int font_size ,
						  std::string stop_name, 
						  svg::Color background_color,	
						  double stroke_width);

			void Draw(svg::ObjectContainer& container) const override;
		
		private:
			svg::Point location_;
			std::vector<double> offset_;
			int font_size_;
			// font-family: "Verdana"
			// font-weigth: NO!!!
			std::string stop_name_;

			//background:
			svg::Color background_color_;
			double stroke_width_;
			//stroke-linecap = "round"
			//stroke-linejoin = "round

			//label-name
			//fill : "black" by default
		};


		BusShape GetBusShape(Bus* bus, geo::SphereProjector& proj, svg::Color Color , double stroke_width);

		LabelBusShape GetLabelBusShape(Bus* bus, geo::SphereProjector& proj, svg::Color color, RendererParameters& params);

		StopShape GetStopShape(Stop* stop, geo::SphereProjector& proj, RendererParameters& params);

		StopLabelShape GetStopLabelShape(Stop* stop, geo::SphereProjector& proj, RendererParameters& params);


		void DrawBusLines(std::vector<std::unique_ptr<svg::Drawable>>& picture,
						  std::vector<Bus*> buses,
						  geo::SphereProjector& projector,
						  RendererParameters params);

		void DrawBusLabels(std::vector<std::unique_ptr<svg::Drawable>>& picture,
						   std::vector<Bus*> buses,
						   geo::SphereProjector& projector,
						   RendererParameters params);

		void DrawStops(std::vector<std::unique_ptr<svg::Drawable>>& picture,
					   std::vector<Stop*> stops,
					   geo::SphereProjector& projector,
					   RendererParameters params);

		void DrawStopLabels(std::vector<std::unique_ptr<svg::Drawable>>& picture,
							std::vector<Stop*> stops,
							geo::SphereProjector& projector,
							RendererParameters params);

		/// <summary>
		/// Draw a SVG-Map with bus and stops lines.
		/// </summary>
		/// <param name="out"></param>
		/// <param name="buses"></param>
		/// <param name="params"></param>
		void DrawNetworkMap(std::ostream& out, renderer::NetworkDrawingData data);


	}

}