#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <optional>
#include <variant>
#include <cmath>

namespace svg {

    struct Rgb {
        
        Rgb()
            : red(0)
            , green(0)
            , blue(0) {

        }


        Rgb(uint8_t r , uint8_t g , uint8_t b )
        : red(r)
        , green(g)
        , blue(b) {

        }
        
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;


    };

    struct Rgba {

        Rgba()
            : red(0)
            , green(0)
            , blue(0)
            , opacity(1) {

        }

        Rgba(uint8_t r, uint8_t g, uint8_t b , double o)
            : red(r)
            , green(g)
            , blue(b)
            , opacity(o){

        }

        uint8_t red = 0 ;
        uint8_t green = 0;
        uint8_t blue = 0;
        double opacity = 1;
    };



    //using Color = std::string;

    //inline const Color NoneColor{ "none" };

    using Color = std::variant < std::monostate, std::string, svg::Rgb, svg::Rgba > ;

    struct OstreamSolutionPrinter {

        

        std::ostream& out;

        void operator()(std::monostate) const {
            using namespace std::literals;
            out << "none"sv;
        }

        void operator()(std::string color) const {
            out << color;
        }

        void operator()(Rgb rgb) const {
            out << "rgb(";
            out << (int)rgb.red;
            out << ",";
            out << (int)rgb.green;
            out << ",";
            out << (int)rgb.blue << ")";
        }

        void operator()(Rgba rgba) const {
            out << "rgba(";
            out << (int)rgba.red;
            out << ",";
            out << (int)rgba.green;
            out << ",";
            out << (int)rgba.blue;
            out << ",";
            out << rgba.opacity << ")";

        }
    };

    std::ostream& operator<<(std::ostream& os, const Color& value);

    enum class StrokeLineCap {
        BUTT,
        ROUND,
        SQUARE,
    };

    enum class StrokeLineJoin {
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND,
    };

    std::ostream& operator<<(std::ostream& os, const StrokeLineCap& value);
    std::ostream& operator<<(std::ostream& os, const StrokeLineJoin& value);



    template <typename Owner>
    class PathProps {
    public:
        Owner& SetFillColor(Color color) {
            fill_color_ = std::move(color);
            return AsOwner();
        }
        Owner& SetStrokeColor(Color color) {
            stroke_color_ = std::move(color);
            return AsOwner();
        }

        Owner& SetStrokeWidth(double width) {
            stroke_width_ = width;
            return AsOwner();
        }

        Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
            stroke_line_cap_ = line_cap;
            return AsOwner();
        }

        Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
            stroke_line_join_ = line_join;
            return AsOwner();
        }

    protected:
        ~PathProps() = default;

        void RenderAttrs(std::ostream& out) const {
            using namespace std::literals;

            if (fill_color_) {
                out << " fill=\""sv << *fill_color_ << "\""sv;
            }
            if (stroke_color_) {
                out << " stroke=\""sv << *stroke_color_ << "\""sv;
            }

            if (stroke_width_) {
                out << " stroke-width=\""sv << *stroke_width_ << "\""sv;
            }

            if (stroke_line_cap_) {
                out << " stroke-linecap=\""sv << *stroke_line_cap_ << "\""sv;
            }

            if (stroke_line_join_) {
                out << " stroke-linejoin=\""sv << *stroke_line_join_ << "\""sv;
            }
        }

    private:
        Owner& AsOwner() {
            // static_cast безопасно преобразует *this к Owner&,
            // если класс Owner — наследник PathProps
            return static_cast<Owner&>(*this);
        }

        std::optional<Color> fill_color_;
        std::optional<Color> stroke_color_;
        std::optional<double> stroke_width_;
        std::optional<StrokeLineCap> stroke_line_cap_;
        std::optional<StrokeLineJoin> stroke_line_join_;
    };
        

    struct Point {
        Point() = default;
        Point(double x, double y)
            : x(x)
            , y(y) {
        }
        double x = 0;
        double y = 0;
    };

    /*
     * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
     * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
     */
    struct RenderContext {
        RenderContext(std::ostream& out)
            : out(out) {
        }

        RenderContext(std::ostream& out, int indent_step, int indent = 0)
            : out(out)
            , indent_step(indent_step)
            , indent(indent) {
        }

        RenderContext Indented() const {
            return { out, indent_step, indent + indent_step };
        }

        void RenderIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        std::ostream& out;
        int indent_step = 0;
        int indent = 0;
    };

    /*
     * Абстрактный базовый класс Object служит для унифицированного хранения
     * конкретных тегов SVG-документа
     * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
     */
    class Object {
    public:
        void Render(const RenderContext& context) const;

        virtual ~Object() = default;

    private:
        virtual void RenderObject(const RenderContext& context) const = 0;

    };

    //---------------------- Interfaces ----------------------
    class ObjectContainer {

    public:
        virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;

        template <typename Obj>
        void Add(Obj obj) {
            AddPtr(std::make_unique<Obj>(std::move(obj)));
        }

    protected:
        ~ObjectContainer() {};

    };

    class Drawable {

    public:
        virtual void Draw(ObjectContainer& container) const = 0;
        virtual ~Drawable() {};
    };

    //---------------------- ----------- ----------------------



    /*
     * Класс Circle моделирует элемент <circle> для отображения круга
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
     */
    class Circle final : public Object, public PathProps<Circle> {
    public:
        Circle& SetCenter(Point center);
        Circle& SetRadius(double radius);

    private:
        void RenderObject(const RenderContext& context) const override;

        Point center_;
        double radius_ = 1.0;
    };

    /*
     * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
     */
    class Polyline : public Object, public PathProps<Polyline> {
    public:
        // Добавляет очередную вершину к ломаной линии
        Polyline& AddPoint(Point point);

        /*
         * Прочие методы и данные, необходимые для реализации элемента <polyline>
         */

    private:
        void RenderObject(const RenderContext& context) const override;

        std::vector<Point> points_;
    };

    /*
     * Класс Text моделирует элемент <text> для отображения текста
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
     */
    class Text : public Object, public PathProps<Text> {
    public:

        // Задаёт координаты опорной точки (атрибуты x и y)
        Text& SetPosition(Point pos);

        // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
        Text& SetOffset(Point offset);

        // Задаёт размеры шрифта (атрибут font-size)
        Text& SetFontSize(uint32_t size);

        // Задаёт название шрифта (атрибут font-family)
        Text& SetFontFamily(std::string font_family);

        // Задаёт толщину шрифта (атрибут font-weight)
        Text& SetFontWeight(std::string font_weight);

        // Задаёт текстовое содержимое объекта (отображается внутри тега text)
        Text& SetData(std::string data);

        // Прочие данные и методы, необходимые для реализации элемента <text>

    private:
        void RenderObject(const RenderContext& context) const override;

        std::string ScreenData() const;

        std::string data_;

        std::string font_family_;
        bool font_fam_set_ = false;

        std::string font_weight_;
        bool font_weight_set_ = false;

        Point position_;
        Point offset_;

        size_t font_size_ = 1;
    };

    class Document : public ObjectContainer {
    public:
        /*
         Метод Add добавляет в svg-документ любой объект-наследник svg::Object.
         Пример использования:
         Document doc;
         doc.Add(Circle().SetCenter({20, 30}).SetRadius(15));
        */

        /*
        template <typename Obj>
        void Add(Obj obj);
        */

        // Добавляет в svg-документ объект-наследник svg::Object
        void AddPtr(std::unique_ptr<Object>&& obj);

        // Выводит в ostream svg-представление документа
        void Render(std::ostream& out) const;

        // Прочие методы и данные, необходимые для реализации класса Document

    private:
        std::vector<std::unique_ptr<Object>> objects_;
    };

    /*
    template <typename Obj>
    void Document::Add(Obj obj) {
        objects_.emplace_back(std::make_unique<Obj>(std::move(obj)));
    }
    */


    template <typename DrawableIterator>
    void DrawPicture(DrawableIterator begin, DrawableIterator end, svg::ObjectContainer& target) {
        for (auto it = begin; it != end; ++it) {
            (*it)->Draw(target);
        }
    }

    template <typename Container>
    void DrawPicture(const Container& container, svg::ObjectContainer& target) {
        using namespace std;
        DrawPicture(begin(container), end(container), target);
    }

}  // namespace svg





