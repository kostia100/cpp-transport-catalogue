#include "svg.h"

namespace svg {

    using namespace std::literals;


    std::ostream& operator<<(std::ostream& os, const Color& color) {

        std::visit(OstreamSolutionPrinter{ os }, color);
        return os;
    }

    std::ostream& operator<<(std::ostream& out, const StrokeLineCap& input) {
        switch (input)
        {
        case StrokeLineCap::BUTT:
            out << "butt"sv;
            break;
        case StrokeLineCap::ROUND:
            out << "round"sv;
            break;
        case StrokeLineCap::SQUARE:
            out << "square"sv;
        default:
            break;
        }

        return out;
    }

    std::ostream& operator<<(std::ostream& out, const StrokeLineJoin& input) {
        switch (input)
        {
        case StrokeLineJoin::ARCS:
            out << "arcs";
            break;
        case StrokeLineJoin::BEVEL:
            out << "bevel";
            break;
        case StrokeLineJoin::MITER:
            out << "miter";
            break;
        case StrokeLineJoin::MITER_CLIP:
            out << "miter-clip";
            break;
        case StrokeLineJoin::ROUND:
            out << "round";
            break;
        default:
            break;
        }
        return out;

    }

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }

    // ---------- Circle ------------------

    Circle& Circle::SetCenter(Point center) {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius) {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\""sv;
        RenderAttrs(out);
        out << "/>"sv;
    }

    // ---------- Polyline ------------------
    Polyline& Polyline::AddPoint(Point pt) {
        points_.push_back(pt);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<polyline points=\""sv;
        bool is_first = true;
        for (const Point& pt : points_) {
            if (!is_first) {
                out << " "sv;
            }
            is_first = false;
            out << pt.x << ","sv << pt.y;
        }
        //out << "\" />"sv;
        out << "\""sv;
        RenderAttrs(out);
        out << "/>"sv;
    }

    // ---------- Text ------------------


    Text& Text::SetPosition(Point pos) {
        position_ = pos;
        return *this;
    }

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& Text::SetOffset(Point offset) {
        offset_ = offset;
        return *this;
    }

    // Задаёт размеры шрифта (атрибут font-size)
    Text& Text::SetFontSize(uint32_t size) {
        font_size_ = size;
        return *this;
    }

    // Задаёт название шрифта (атрибут font-family)
    Text& Text::SetFontFamily(std::string font_family) {
        font_family_ = font_family;
        font_fam_set_ = true;
        return *this;
    }

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& Text::SetFontWeight(std::string font_weight) {
        font_weight_ = font_weight;
        font_weight_set_ = true;
        return *this;
    }

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& Text::SetData(std::string data) {
        data_ = data;
        return *this;
    }

    std::string Text::ScreenData() const {
        std::stringstream stream;
        for (const char c : data_) {
            switch (c) {
            case '"':
                stream << "&quot;";
                break;
            case '\'':
                stream << "&apos;";
                break;
            case '<':
                stream << "&lt;";
                break;
            case '>':
                stream << "&gt;";
                break;
            case '&':
                stream << "&amp;";
                break;
            default:
                stream << c;
            }

        }
        return stream.str();
    }

    /*
    OLD version 
    void Text::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<text ";
        out << "x=\""sv << position_.x << "\" y=\""sv << position_.y << "\" "sv;
        out << "dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\" "sv;

        out << "font-size=\""sv << font_size_ << "\""sv;

        if (font_fam_set_) {
            out << " font-family=\""sv << font_family_ << "\""sv;
        }

        if (font_weight_set_) {
            out << " font-weight=\""sv << font_weight_ << "\""sv;
        }
        RenderAttrs(out);
        out << ">"sv;
        out << ScreenData();
        out << "</text>";
    }
    */

    void Text::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        //out << "<text ";
        out << "<text";
        RenderAttrs(out);
        out << " x=\""sv << position_.x << "\" y=\""sv << position_.y << "\" "sv;
        out << "dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\" "sv;

        out << "font-size=\""sv << font_size_ << "\""sv;

        if (font_fam_set_) {
            out << " font-family=\""sv << font_family_ << "\""sv;
        }

        if (font_weight_set_) {
            out << " font-weight=\""sv << font_weight_ << "\""sv;
        }
        //RenderAttrs(out);
        out << ">"sv;
        out << ScreenData();
        out << "</text>";

    }


    // ----------Document------------------
    void Document::AddPtr(std::unique_ptr<Object>&& obj) {
        objects_.push_back(std::move(obj));
    }

    // Выводит в ostream svg-представление документа
    void Document::Render(std::ostream& out) const {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
        RenderContext ctx(out, 2, 2);
        for (size_t a = 0; a < objects_.size(); ++a) {
            objects_[a]->Render(ctx);
        }


        out << "</svg>"sv;
    }

}  // namespace svg