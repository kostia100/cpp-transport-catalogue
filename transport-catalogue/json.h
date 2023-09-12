#pragma once


#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <sstream>
#include <iomanip>

namespace json {


    class Node;
    // Сохраните объявления Dict и Array без изменения
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;



    // Эта ошибка должна выбрасываться при ошибках парсинга JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node {
    public:

        using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

        /* Реализуйте Node, используя std::variant */

        //Node(Value value);
        Node();
        Node(nullptr_t);
        Node(int value);
        Node(std::string value);
        Node(double value);
        Node(bool value);
        Node(Array value);
        Node(Dict value);

        bool IsInt() const;
        bool IsDouble() const;
        bool IsPureDouble() const;
        bool IsBool() const;
        bool IsString() const;
        bool IsNull() const;
        bool IsArray() const;
        bool IsMap() const;

        int AsInt() const;
        bool AsBool() const;
        double AsDouble() const;

        const std::string& AsString() const;
        const Array& AsArray() const;
        const Dict& AsMap() const;


        const Value& GetValue() const {
            return value_;
        }



    private:


        Value value_;
    };

    bool operator== (const Node& lhs, const Node& rhs);
    bool operator!= (const Node& lhs, const Node& rhs);

    void PrintNode(const Node& node, std::ostream& out);

    struct OstreamNodePrinter {
        std::ostream& out;

        void operator()(int value) const {
            out << value;
        }

        void operator()(double value) const {
            out  << value;
            //out << std::setprecision(6) << value;
        }

        void operator()(bool value) const {
            using namespace std::literals;
            if (value == true) {
                out << "true"sv;

            }
            else {
                out << "false"sv;
            }
        }

        void operator()(std::string value) const {
            out << '\"';
            for (char c : value) {
                if (c == '\"') {
                    out << "\\\"";
                }
                else if (c == '\n') {
                    out << "\\n";
                }
                else if (c == '\r') {
                    out << "\\r";
                }
                else if (c == '\\') {
                    out << "\\\\";
                }
                else {
                    out << c;
                }
            }
            out << "\"";
        }

        void operator()(nullptr_t) const {
            using namespace std::literals;
            out << "null"sv;
        }

        void operator()(Array arr) const {
            using namespace std::literals;
            out << "[";
            //out << std::endl;
            bool is_first = true;
            for (auto& elem : arr) {
                if (is_first) {
                    PrintNode(elem, out);
                    is_first = false;
                }
                else {
                    out << ",";
                    PrintNode(elem, out);
                }
            }
            //out << std::endl;
            out << "]";
        }

        void operator()(Dict dic) const {
            using namespace std::literals;
            out << "{";
            //out << std::endl;
            bool is_first = true;
            for (auto& elem : dic) {
                if (is_first) {
                    out << "\"";
                    out << elem.first;
                    out << "\"";
                    out << ":";
                    PrintNode(elem.second, out);
                    is_first = false;
                }
                else {
                    out << ",";
                    out << "\"";
                    out << elem.first;
                    out << "\"";
                    out << ":";
                    PrintNode(elem.second, out);
                }
            }
            //out << std::endl;
            out << "}";
        }
    };



    class Document {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;

    private:
        Node root_;
    };

    bool operator== (const Document& lhs, const Document& rhs);
    bool operator!= (const Document& lhs, const Document& rhs);

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);

    json::Document LoadJSON(const std::string& s);

    std::string Print(const Node& node);

}  // namespace json
