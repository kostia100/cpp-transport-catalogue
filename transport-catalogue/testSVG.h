#pragma once

#define _USE_MATH_DEFINES

#include <iostream>
#include <cassert>
#include "math.h"
#include "svg.h"




namespace shapes {

    svg::Polyline CreateStar(svg::Point center, double outer_rad, double inner_rad, int num_rays) {
        using namespace svg;
        Polyline polyline;
        for (int i = 0; i <= num_rays; ++i) {
            double angle = 2 * M_PI * (i % num_rays) / num_rays;
            polyline.AddPoint({ center.x + outer_rad * sin(angle), center.y - outer_rad * cos(angle) });
            if (i == num_rays) {
                break;
            }
            angle += M_PI / num_rays;
            polyline.AddPoint({ center.x + inner_rad * sin(angle), center.y - inner_rad * cos(angle) });
        }
        return polyline;
    }

    class Triangle : public svg::Drawable {
    public:
        Triangle(svg::Point p1, svg::Point p2, svg::Point p3)
            : p1_(p1)
            , p2_(p2)
            , p3_(p3) {
        }

        // Реализует метод Draw интерфейса svg::Drawable
        void Draw(svg::ObjectContainer& container) const override {
            container.Add(svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
        }

    private:
        svg::Point p1_, p2_, p3_;
    };


    class Star : public svg::Drawable {
    public:
        Star(svg::Point center, double outer_rad, double inner_rad, int num_rays)
            : center_(center)
            , outer_rad_(outer_rad)
            , inner_rad_(inner_rad)
            , num_rays_(num_rays) {

        }

        void Draw(svg::ObjectContainer& container) const override {
            container.Add(CreateStar(center_, outer_rad_, inner_rad_, num_rays_).SetFillColor("red").SetStrokeColor("black"));
        }

    private:
        svg::Point center_;
        double outer_rad_, inner_rad_;
        int num_rays_;


    };


    class Snowman : public svg::Drawable {
    public:
        Snowman(svg::Point head, double head_rad)
            : head_(head)
            , head_rad_(head_rad) {

        }

        void Draw(svg::ObjectContainer& container) const override {
            svg::Point bottom{ head_.x , head_.y + 5 * head_rad_ };
            svg::Point mid{ head_.x , head_.y + 2 * head_rad_ };;
            //container.Add(svg::Circle().SetCenter(bottom).SetRadius(2 * head_rad_));
            container.Add(svg::Circle().SetCenter(bottom).SetRadius(2 * head_rad_).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
            container.Add(svg::Circle().SetCenter(mid).SetRadius(1.5 * head_rad_).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
            container.Add(svg::Circle().SetCenter(head_).SetRadius(head_rad_).SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
        }

    private:
        svg::Point head_;
        double head_rad_;


    };

} // namespace shapes 



namespace svg {

    namespace test {

        Polyline CreateStarTest(Point center, double outer_rad, double inner_rad, int num_rays) {
            Polyline polyline;
            for (int i = 0; i <= num_rays; ++i) {
                double angle = 2 * M_PI * (i % num_rays) / num_rays;
                polyline.AddPoint({ center.x + outer_rad * sin(angle), center.y - outer_rad * cos(angle) });
                if (i == num_rays) {
                    break;
                }
                angle += M_PI / num_rays;
                polyline.AddPoint({ center.x + inner_rad * sin(angle), center.y - inner_rad * cos(angle) });
            }
            return polyline;
        }


        void DrawCircleTest() {
            std::cout << "Circle" << std::endl;
            Document doc;
            doc.Add(Circle().SetCenter({ 20, 20 }).SetRadius(10));
            doc.Render(std::cout);
        }



        // Выводит приветствие, круг и звезду
        void DrawPictureTest() {
            using namespace std::literals;
            std::cout << "Picture:text+star+circle" << std::endl;
            Document doc;
            doc.Add(Circle().SetCenter({ 20, 20 }).SetRadius(10));
            doc.Add(Text()
                .SetFontFamily("Verdana"s)
                .SetPosition({ 35, 20 })
                .SetOffset({ 0, 6 })
                .SetFontSize(12)
                .SetFontWeight("bold"s)
                .SetData("Hello C++"s));
            doc.Add(CreateStarTest({ 20, 50 }, 10, 5, 5));
            doc.Render(std::cout);
        }


        void DrawShapesTest() {
            using namespace svg;
            using namespace shapes;
            using namespace std;
            cout << "Draw shapes" << endl;
            vector<unique_ptr<svg::Drawable>> picture;

            picture.emplace_back(make_unique<Triangle>(Point{ 100, 20 }, Point{ 120, 50 }, Point{ 80, 40 }));

            // 5-лучевая звезда с центром {50, 20}, длиной лучей 10 и внутренним радиусом 4
            picture.emplace_back(make_unique<Star>(Point{ 50.0, 20.0 }, 10.0, 4.0, 5));
            // Снеговик с "головой" радиусом 10, имеющей центр в точке {30, 20}
            picture.emplace_back(make_unique<Snowman>(Point{ 30, 20 }, 10.0));

            svg::Document doc;
            // Так как документ реализует интерфейс ObjectContainer,
            // его можно передать в DrawPicture в качестве цели для рисования
            DrawPicture(picture, doc);

            // Выводим полученный документ в stdout
            doc.Render(cout);
        }

        void DrawShapesWithColorTest() {

            using namespace svg;
            using namespace shapes;
            using namespace std;

            cout << "Draw shapes colors" << endl;
            std::vector<std::unique_ptr<svg::Drawable>> picture;
            picture.emplace_back(std::make_unique<Triangle>(Point{ 100, 20 }, Point{ 120, 50 }, Point{ 80, 40 }));
            picture.emplace_back(std::make_unique<Star>(Point{ 50.0, 20.0 }, 10.0, 4.0, 5));
            picture.emplace_back(std::make_unique<Snowman>(Point{ 30, 20 }, 10.0));

            svg::Document doc;
            DrawPicture(picture, doc);

            const Text base_text =  //
                Text()
                .SetFontFamily("Verdana"s)
                .SetFontSize(12)
                .SetPosition({ 10, 100 })
                .SetData("Happy New Year!"s);
            doc.Add(Text{ base_text }
                .SetStrokeColor("yellow"s)
                .SetFillColor("yellow"s)
                .SetStrokeLineJoin(StrokeLineJoin::ROUND)
                .SetStrokeLineCap(StrokeLineCap::ROUND)
                .SetStrokeWidth(3));
            doc.Add(Text{ base_text }.SetFillColor("red"s));

            doc.Render(cout);
        }

        void TestRgb() {
            std::cout << "Test Rgb" << std::endl;
            svg::Rgb rgb{ 255, 0, 100 };
            assert(rgb.red == 255);
            assert(rgb.green == 0);
            assert(rgb.blue == 100);

            // Задаёт цвет по умолчанию: red=0, green=0, blue=0
            svg::Rgb color;
            assert(color.red == 0 && color.green == 0 && color.blue == 0);
            std::cout << "Test Rgb OK" << std::endl;
        }

        void TestRgba() {
            std::cout << "Test Rgba" << std::endl;
            // Задаёт цвет в виде четырёх компонент: red, green, blue, opacity
            svg::Rgba rgba{ 100, 20, 50, 0.3 };
            assert(rgba.red == 100);
            assert(rgba.green == 20);
            assert(rgba.blue == 50);
            assert(rgba.opacity == 0.3);

            // Чёрный непрозрачный цвет: red=0, green=0, blue=0, alpha=1.0
            svg::Rgba color;
            assert(color.red == 0 && color.green == 0 && color.blue == 0 && color.opacity == 1.0);
            std::cout << "Test Rgba OK" << std::endl;
        }

        void TestOutputVariantColor() {
            using namespace std::literals;
            Color none_color;
            std::cout << none_color << std::endl; // none

            Color purple{ "purple"s };
            std::cout << purple << std::endl; // purple

            Color rgb = Rgb{ 100, 200, 255 };
            std::cout << rgb << std::endl; // rgb(100,200,255)

            Color rgba = Rgba{ 100, 200, 255, 0.5 };
            std::cout << rgba << std::endl; // rgba(100,200,255,0.5)

            Color rgba2 = Rgba{ 100, 200, 255, 0.5013 };
            std::cout << rgba2 << std::endl; // rgba(100,200,255,0.5013)
        }


        void TestSVG() {
            std::cout << "Start SVG Test" << std::endl;
            std::cout << "++++++" << std::endl;
            test::DrawCircleTest();
            std::cout << std::endl << "++++++" << std::endl;
            test::DrawPictureTest();
            std::cout << std::endl << "++++++" << std::endl;
            test::DrawShapesTest();
            std::cout << std::endl << "++++++" << std::endl;
            test::DrawShapesWithColorTest();
            std::cout << std::endl << "++++++" << std::endl;
            test::TestRgb();
            std::cout << "++++++" << std::endl;
            test::TestRgba();
            std::cout << "++++++" << std::endl;
            test::TestOutputVariantColor();
            std::cout << "All SVG Tests: OK" << std::endl;
        }


    }


}