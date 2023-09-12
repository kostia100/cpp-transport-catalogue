#include "map_renderer.h"
#include <vector>

namespace catalogue {

	namespace renderer {

		BusShape::BusShape(std::vector<svg::Point> stops,svg::Color color , double stroke_width) 
			: stops_(stops) 
			, color_(color)
			, stroke_width_(stroke_width){
		}

		void BusShape::Draw(svg::ObjectContainer& container) const  {
			svg::Polyline line;
			for (const auto pt : stops_) {
				line.AddPoint(pt);
			}
			line.SetFillColor({})
				.SetStrokeLineCap(svg::StrokeLineCap::ROUND)
				.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
				.SetStrokeColor(color_)
				.SetStrokeWidth(stroke_width_);
			
			//here we need to add more stuff

			container.Add(line);
		}

		LabelBusShape::LabelBusShape(	std::vector<svg::Point> locations,
										std::vector<double> offset,
										int font_size,
										std::string name_bus,
										svg::Color background_color,
										double stroke_width,
										svg::Color bus_name_color)
			: locations_(locations)
			, offset_(offset)
			, font_size_(font_size)
			, name_bus_(name_bus)
			, background_color_(background_color)
			, stroke_width_(stroke_width)
			, bus_name_color_(bus_name_color){

		}



		void LabelBusShape::Draw(svg::ObjectContainer& container) const {
			svg::Text background1;
			background1
				.SetFillColor(background_color_)
				.SetStrokeColor(background_color_)
				.SetStrokeWidth(stroke_width_)
				.SetStrokeLineCap(svg::StrokeLineCap::ROUND)
				.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
				.SetPosition(locations_[0])
				.SetOffset({ offset_[0],offset_[1] })
				.SetFontSize(font_size_)
				.SetFontFamily("Verdana")
				.SetFontWeight("bold")
				.SetData(name_bus_);

			
			/*
			background1.SetPosition(locations_[0])
					  .SetOffset({ offset_[0],offset_[1] })
					  .SetFontSize(font_size_)
				      .SetFontFamily("Verdana")
					  .SetFontWeight("bold")
					  .SetData(name_bus_);
			
			background1.SetFillColor(background_color_)
					  .SetStrokeColor(background_color_)
				      .SetStrokeWidth(stroke_width_)
					  .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
					  .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
			*/

			svg::Text busname1;
			busname1.SetPosition(locations_[0])
				.SetOffset({ offset_[0],offset_[1] })
				.SetFontSize(font_size_)
				.SetFontFamily("Verdana")
				.SetFontWeight("bold")
				.SetData(name_bus_);
			busname1.SetFillColor(bus_name_color_);

			container.Add(background1);
			container.Add(busname1);
			
			//in case end_stop exists (i.e. the bus is NOT roundtrip)
			if (locations_.size()>1) {
				svg::Text background2;
				background2
					.SetFillColor(background_color_)
					.SetStrokeColor(background_color_)
					.SetStrokeWidth(stroke_width_)
					.SetStrokeLineCap(svg::StrokeLineCap::ROUND)
					.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
					.SetPosition(locations_[1])
					.SetOffset({ offset_[0],offset_[1] })
					.SetFontSize(font_size_)
					.SetFontFamily("Verdana")
					.SetFontWeight("bold")
					.SetData(name_bus_);


				svg::Text busname2;
				busname2.SetPosition(locations_[1])
					.SetOffset({ offset_[0],offset_[1] })
					.SetFontSize(font_size_)
					.SetFontFamily("Verdana")
					.SetFontWeight("bold")
					.SetData(name_bus_);
				busname2.SetFillColor(bus_name_color_);
				container.Add(background2);
				container.Add(busname2);
			}


		}

		//----STOP SHAPE----
		StopShape::StopShape(svg::Point location, double radius) 
			:location_(location)
			,r_(radius){

		}

		void StopShape::Draw(svg::ObjectContainer& container) const {
			svg::Circle circle;
			circle.SetCenter(location_).SetRadius(r_).SetFillColor("white");
			container.Add(circle);

		}
		//------------------ 
		//----STOP LABEL SHAPE----

		StopLabelShape::StopLabelShape(
						svg::Point location,
						std::vector<double> offset,
						int font_size,
						std::string stop_name,
						svg::Color background_color,
						double stroke_width)
						: location_(location)
						, offset_(offset)
						, font_size_(font_size)
						, stop_name_(stop_name)
					    , background_color_(background_color)
						, stroke_width_(stroke_width){

		}

		void StopLabelShape::Draw(svg::ObjectContainer& container) const {
			svg::Text background;
			background.SetPosition(location_)
				.SetOffset({ offset_[0],offset_[1] })
				.SetFontSize(font_size_)
				.SetFontFamily("Verdana")
				//.SetFontWeight("bold")
				.SetData(stop_name_);

			background.SetFillColor(background_color_)
				.SetStrokeColor(background_color_)
				.SetStrokeWidth(stroke_width_)
				.SetStrokeLineCap(svg::StrokeLineCap::ROUND)
				.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);


			svg::Text stopname;
			stopname.SetPosition(location_)
				.SetOffset({ offset_[0],offset_[1] })
				.SetFontSize(font_size_)
				.SetFontFamily("Verdana")
				.SetData(stop_name_);

			stopname.SetFillColor("black");

			container.Add(background);
			container.Add(stopname);
		}

		//------------------


		BusShape GetBusShape(Bus* bus, geo::SphereProjector& proj, svg::Color color , double stroke_width) {
			std::vector<Stop*> stops = bus->stops;
			std::vector<svg::Point> pts;
			for (const auto stp : stops) {
				//here call the conversion function
				geo::PointOnPlane geoPt = proj(stp->location);
				pts.push_back({ geoPt.X,geoPt.Y});
			}
			return BusShape(pts, color, stroke_width);
		}

		LabelBusShape GetLabelBusShape(Bus* bus, geo::SphereProjector& proj, svg::Color bus_color, RendererParameters& params) {

			std::vector<svg::Point> locations;
			Stop* start_stop = (bus->stops)[0];
			geo::PointOnPlane geoStart = proj(start_stop->location);
			locations.push_back({ geoStart.X , geoStart.Y });
			
			Stop* end_stop = bus->end_stop;
			if (start_stop != end_stop) {
				//no round_trip case => need to add end_stop
				geo::PointOnPlane geoEnd = proj(end_stop->location);
				locations.push_back({ geoEnd.X , geoEnd.Y });
			}

			return LabelBusShape(locations,
				params.bus_label_offset,
				params.bus_label_font_size,
				bus->bus_name,
				params.underlayer_color,
				params.underlayer_width,
				bus_color);
		}


		StopShape GetStopShape(Stop* stop, geo::SphereProjector& proj,  RendererParameters& params) {
			geo::PointOnPlane geoStop = proj(stop->location);
			svg::Point location{geoStop.X,geoStop.Y};
			return StopShape(location,params.stop_radius);
		}

		StopLabelShape GetStopLabelShape(Stop* stop, geo::SphereProjector& proj, RendererParameters& params) {
			geo::PointOnPlane geoStop = proj(stop->location);
			svg::Point location{ geoStop.X,geoStop.Y };
			return StopLabelShape(
				location,
				params.stop_label_offset,
				params.stop_label_font_size,
				stop->stop_name,
				params.underlayer_color,
				params.underlayer_width
				);
		}



		void DrawBusLines(std::vector<std::unique_ptr<svg::Drawable>>& picture,
			std::vector<Bus*> buses,
			geo::SphereProjector& projector,
			RendererParameters params) {

			std::vector<svg::Color> clrs = params.color_palette;
			size_t color_count = 0;
			//add buses to Document
			for (const auto b : buses) {
				///only draw if the bus line has stops
				if ((b->stops).size() != 0) {
					svg::Color clr_bus = clrs[color_count];
					picture.emplace_back(std::make_unique<renderer::BusShape>(renderer::GetBusShape(b, projector, clr_bus, params.line_width)));
					if (color_count == clrs.size() - 1) {
						//if we just used the last color in the list, go back to the begining
						color_count = 0;
					}
					else {
						++color_count;
					}
				}
			}

		}

		void DrawBusLabels(std::vector<std::unique_ptr<svg::Drawable>>& picture,
			std::vector<Bus*> buses,
			geo::SphereProjector& projector,
			RendererParameters params) {

			std::vector<svg::Color> clrs = params.color_palette;
			size_t clr_bus_label = 0;
			for (const auto b : buses) {
				///only draw if the bus line has stops
				if ((b->stops).size() != 0) {
					svg::Color clr_bus = clrs[clr_bus_label];
					picture.emplace_back(std::make_unique<renderer::LabelBusShape>(renderer::GetLabelBusShape(b, projector, clr_bus, params)));
					if (clr_bus_label == clrs.size() - 1) {
						//if we just used the last color in the list, go back to the begining
						clr_bus_label = 0;
					}
					else {
						++clr_bus_label;
					}
				}
			}

		}

		void DrawStops(std::vector<std::unique_ptr<svg::Drawable>>& picture,
			std::vector<Stop*> stops,
			geo::SphereProjector& projector,
			RendererParameters params) {
			for (const auto stp : stops) {
				picture.emplace_back(std::make_unique<renderer::StopShape>(renderer::GetStopShape(stp, projector, params)));
			}
		}

		void DrawStopLabels(std::vector<std::unique_ptr<svg::Drawable>>& picture,
			std::vector<Stop*> stops,
			geo::SphereProjector& projector,
			RendererParameters params) {

			for (const auto stp : stops) {
				picture.emplace_back(std::make_unique<renderer::StopLabelShape>(renderer::GetStopLabelShape(stp, projector, params)));
			}

		}

		void DrawNetworkMap(std::ostream& out, renderer::NetworkDrawingData data) {
			//initialize projector
			std::vector< geo::Coordinates> geo_coords;
			for (const auto stp : data.stops) {
				geo_coords.push_back(stp->location);
			}

			geo::SphereProjector projector(geo_coords.begin(), geo_coords.end(), data.params.width, data.params.height, data.params.padding);
			
			svg::Document doc;
			std::vector<svg::Color> clrs = data.params.color_palette;
			std::vector<std::unique_ptr<svg::Drawable>> picture;
			//add buses to Picture
			DrawBusLines(picture,data.buses,projector,data.params);

			//add bus labels to Picture
			DrawBusLabels(picture, data.buses, projector, data.params);

			//add stops to Picture
			DrawStops(picture, data.stops, projector, data.params);

			//add stop labels to Picture
			DrawStopLabels(picture, data.stops, projector, data.params);

			//draw picture
			DrawPicture(picture, doc);
			doc.Render(out);
		}


	}


}