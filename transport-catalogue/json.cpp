
#include "json.h"

using namespace std;

namespace json {

    namespace {

        using Number = std::variant<int, double>;
        using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

        Number LoadNumber(std::istream& input) {
            using namespace std::literals;

            std::string parsed_num;

            // Считывает в parsed_num очередной символ из input
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
            };

            // Считывает одну или более цифр в parsed_num из input
            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
            };

            if (input.peek() == '-') {
                read_char();
            }
            // Парсим целую часть числа
            if (input.peek() == '0') {
                read_char();
                // После 0 в JSON не могут идти другие цифры
            }
            else {
                read_digits();
            }

            bool is_int = true;
            // Парсим дробную часть числа
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            // Парсим экспоненциальную часть числа
            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    // Сначала пробуем преобразовать строку в int
                    try {
                        return std::stoi(parsed_num);
                    }
                    catch (...) {
                        // В случае неудачи, например, при переполнении,
                        // код ниже попробует преобразовать строку в double
                    }
                }
                return std::stod(parsed_num);
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        // Считывает содержимое строкового литерала JSON-документа
        // Функцию следует использовать после считывания открывающего символа ":
        std::string LoadBasicString(std::istream& input) {
            using namespace std::literals;

            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    // Поток закончился до того, как встретили закрывающую кавычку?
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') {
                    // Встретили закрывающую кавычку
                    ++it;
                    break;
                }
                else if (ch == '\\') {
                    // Встретили начало escape-последовательности
                    ++it;
                    if (it == end) {
                        // Поток завершился сразу после символа обратной косой черты
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
                    switch (escaped_char) {
                    case 'n':
                        s.push_back('\n');
                        break;
                    case 't':
                        s.push_back('\t');
                        break;
                    case 'r':
                        s.push_back('\r');
                        break;
                    case '"':
                        s.push_back('"');
                        break;
                    case '\\':
                        s.push_back('\\');
                        break;
                    default:
                        // Встретили неизвестную escape-последовательность
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else if (ch == '\n' || ch == '\r') {
                    // Строковый литерал внутри- JSON не может прерываться символами \r или \n
                    throw ParsingError("Unexpected end of line"s);
                }
                else {
                    // Просто считываем очередной символ и помещаем его в результирующую строку
                    s.push_back(ch);
                }
                ++it;
            }

            return s;
        }



        Node LoadNode(istream& input);

        Node LoadArray(istream& input) {
            Array result;

            char c;
            for (; input >> c && c != ']';) {
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }
            if (c != ']') {
                throw ParsingError("Could not parse Array");
            }


            return Node(move(result));
        }


        Node LoadString(istream& input) {
            string line;
            getline(input, line, '"');
            return Node(move(line));
        }

        Node LoadBool(istream& input) {
            char a, b, c, d, e;
            input >> a;
            if (a == 't') {
                input >> b;
                input >> c;
                input >> d;
                if (b == 'r' && c == 'u' && d == 'e') {
                    return Node(true);
                }
                else {
                    throw ParsingError("Could not parse bool");
                }
            }
            else if (a == 'f') {
                input >> b;
                input >> c;
                input >> d;
                input >> e;
                if (b == 'a' && c == 'l' && d == 's' && e == 'e') {
                    return Node(false);
                }
                else {
                    throw ParsingError("Could not parse bool");
                }
            }
            else {
                throw ParsingError("Could not parse bool");
            }
            /*
            std::string tf;
            input >> tf;
            if (tf == "true") {
                return Node(true);
            }
            else if (tf == "false") {
                return Node(false);
            }
            else {
                throw ParsingError("Could not parse bool");
            }
            */
        }

        Node LoadNull(istream& input) {
            char a, b, c, d;
            input >> a;
            if (a == 'n') {
                input >> b;
                input >> c;
                input >> d;
                if (b == 'u' && c == 'l' && d == 'l') {
                    return Node(nullptr);
                }
                else {
                    throw ParsingError("Could not parse null");
                }
            }
            else {
                throw ParsingError("Could not parse null");
            }
            /*
            std::string nl;
            input >> nl;
            if (nl != "null") {
                throw ParsingError("Could not parse null");
            }
            return Node();
            */
        }



        Node LoadDict(istream& input) {
            Dict result;

            char c;
            for (; input >> c && c != '}';) {
                if (c == ',') {
                    input >> c;
                }

                string key = LoadString(input).AsString();
                input >> c;
                result.insert({ move(key), LoadNode(input) });
            }

            if (c != '}') {
                throw ParsingError("Could not parse Array");
            }

            return Node(move(result));
        }

        Node LoadNode(istream& input) {
            char c;
            input >> c;
            if (c == '}') {
                throw ParsingError("Wrong input");
            }
            if (c == ']') {
                throw ParsingError("Wrong input");
            }

            if (c == '[') {
                return LoadArray(input);
            }
            else if (c == '{') {
                return LoadDict(input);
            }
            else if (c == '"') {
                std::string str = LoadBasicString(input);
                return Node(str);
            }
            else if (c == 'n') {
                input.putback(c);
                return LoadNull(input);
            }
            else if (c == 't' || c == 'f') {
                input.putback(c);
                return LoadBool(input);
            }
            else {
                input.putback(c);
                Number nmb = LoadNumber(input);
                if (const auto ptrInt = std::get_if<int>(&nmb)) {
                    return Node(*ptrInt);
                }
                else {
                    const auto ptrDbl = std::get_if<double>(&nmb);
                    return Node(*ptrDbl);
                }

            }
        }

    }  // namespace

    //------------Constructors-------------------

    /*
    Node::Node(Value value)
        :value_(value){
    }
    */

    Node::Node()
        : value_(nullptr) {
    }

    Node::Node(int x)
        : value_(x) {
    }

    Node::Node(std::string x)
        : value_(x) {
    }

    Node::Node(nullptr_t)
        : value_(nullptr) {
    }

    Node::Node(double x)
        : value_(x) {
    }

    Node::Node(bool x)
        : value_(x) {
    }

    Node::Node(Array value)
        : value_(value) {
    }

    Node::Node(Dict value)
        : value_(value) {
    }

    //------------IsType-------------------
    bool Node::IsInt() const {
        if (std::holds_alternative<int>(value_)) {
            return true;
        }
        return false;
    }

    bool Node::IsDouble() const {
        if (std::holds_alternative<int>(value_)) {
            return true;
        }
        if (std::holds_alternative<double>(value_)) {
            return true;
        }
        return false;
    }

    bool Node::IsPureDouble() const {
        if (std::holds_alternative<int>(value_)) {
            return false;
        }
        else if (std::holds_alternative<double>(value_)) {
            return true;
        }
        return false;
    }

    bool Node::IsBool() const {
        if (std::holds_alternative<bool>(value_)) {
            return true;
        }
        return false;
    }

    bool Node::IsString() const {
        if (std::holds_alternative<string>(value_)) {
            return true;
        }
        return false;
    }

    bool Node::IsNull() const {
        if (std::holds_alternative<nullptr_t>(value_)) {
            return true;
        }
        return false;
    }

    bool Node::IsArray() const {
        if (std::holds_alternative<Array>(value_)) {
            return true;
        }
        return false;
    }

    bool Node::IsMap() const {
        if (std::holds_alternative<Dict>(value_)) {
            return true;
        }
        return false;
    }

    //------------AsType-------------------
    int Node::AsInt() const {
        if (const auto ptr = std::get_if<int>(&value_)) {
            return *ptr;
        }
        throw std::logic_error("int");
    }

    bool Node::AsBool() const {
        if (const auto ptr = std::get_if<bool>(&value_)) {
            return *ptr;
        }
        throw std::logic_error("bool");
    }

    double Node::AsDouble() const {
        if (const auto ptr = std::get_if<int>(&value_)) {
            return (double)*ptr;
        }
        if (const auto ptr = std::get_if<double>(&value_)) {
            return *ptr;
        }
        throw std::logic_error("double");
    }

    const string& Node::AsString() const {
        if (const auto ptr = std::get_if<std::string>(&value_)) {
            return *ptr;
        }
        throw std::logic_error("string");
    }

    const Array& Node::AsArray() const {
        if (const auto ptr = std::get_if<Array>(&value_)) {
            return *ptr;
        }
        throw std::logic_error("array");
    }

    const Dict& Node::AsMap() const {
        if (const auto ptr = std::get_if<Dict>(&value_)) {
            return *ptr;
        }
        throw std::logic_error("dict");
    }


    bool operator== (const Node& lhs, const Node& rhs) {
        if (lhs.IsInt() && rhs.IsInt()) {
            return lhs.AsInt() == rhs.AsInt();
        }
        else if (lhs.IsPureDouble() && rhs.IsPureDouble()) {
            return lhs.AsDouble() == rhs.AsDouble();
        }
        else if (lhs.IsBool() && rhs.IsBool()) {
            return lhs.AsBool() == rhs.AsBool();
        }
        else if (lhs.IsString() && rhs.IsString()) {
            return lhs.AsString() == rhs.AsString();
        }
        else if (lhs.IsNull() && rhs.IsNull()) {
            return true;
        }
        else if (lhs.IsArray() && rhs.IsArray()) {
            return lhs.AsArray() == rhs.AsArray();
        }
        else if (lhs.IsMap() && rhs.IsMap()) {
            return lhs.AsMap() == rhs.AsMap();
        }
        else {
            return false;
        }
    }


    bool operator!= (const Node& lhs, const Node& rhs) {
        return !(lhs == rhs);
    }



    void PrintNode(const Node& node, std::ostream& out) {
        OstreamNodePrinter printer{ out };
        std::visit(printer, node.GetValue());
    }


    //------------Document-------------------


    Document::Document(Node root)
        : root_(move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    bool operator== (const Document& lhs, const Document& rhs) {
        return lhs.GetRoot() == rhs.GetRoot();
    }
    bool operator!= (const Document& lhs, const Document& rhs) {
        return !(lhs == rhs);
    }

    Document Load(istream& input) {
        return Document{ LoadNode(input) };
    }

    void Print(const Document& doc, std::ostream& output) {
        PrintNode(doc.GetRoot(), output);
        // Реализуйте функцию самостоятельно
    }

    //------
    json::Document LoadJSON(const std::string& s) {
        std::istringstream strm(s);
        return json::Load(strm);
    }
    

    std::string Print(const Node& node)  {
        std::ostringstream out;
        Print(Document{ node }, out);
        return out.str();
    }
    


}  // namespace json