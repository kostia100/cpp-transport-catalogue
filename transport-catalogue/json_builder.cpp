#include "json_builder.h"

namespace json {

	Node BuildNode(Node::Value value) {

		if (std::holds_alternative<int>(value)) {
			int& val = std::get<int>(value);
			return  Node{ val };
		}

		if (std::holds_alternative<double>(value)) {
			double& val = std::get<double>(value);
			return  Node{ val };
		}

		if (std::holds_alternative<std::string>(value)) {
			std::string& val = std::get<std::string>(value);
			return  Node{ val };
		}

		if (std::holds_alternative<bool>(value)) {
			bool& val = std::get<bool>(value);
			return  Node{ val };
		}

		if (std::holds_alternative<nullptr_t>(value)) {
			nullptr_t& val = std::get<nullptr_t>(value);
			return  Node{ val };
		}

		if (std::holds_alternative<Array>(value)) {
			Array& val = std::get<Array>(value);
			return  Node{ val };
		}

		if (std::holds_alternative<Dict>(value)) {
			Dict& val = std::get<Dict>(value);
			return  Node{ val };
		}
		return  Node{ nullptr };
	}

	Builder::Builder() : nodes_{ root_ } {

	}


	KeyItemContext Builder::Key(std::string key) {

		if (nodes_.size() == 0) {
			throw std::logic_error("Object already build");
		}

		if (!std::holds_alternative<Dict>(nodes_.back().GetValue())) {
			throw std::logic_error("Applying Key outside Dict");
		}

		if (!std::holds_alternative<nullptr_t>(keys_.back())) {
			throw std::logic_error("Applying Key several times");
		}

		keys_.back() = key;
		Node& last_node = nodes_.back();
		const Dict& cst_dict = last_node.AsDict();
		Dict& q = const_cast<Dict&>(cst_dict);
		q[key];

		return KeyItemContext{ *this };
	}



	Builder& Builder::Value(Node::Value value) {
		if (nodes_.size() == 0) {
			throw std::logic_error("Applying Value several times");
		}

		//Array => add element
		if (std::holds_alternative<Array>(nodes_.back().GetValue())) {

			Node nd = BuildNode(value);
			Node& last_node = nodes_.back();
			const Array& cst_array = last_node.AsArray();
			Array& q = const_cast<Array&>(cst_array);
			q.push_back(move(nd));

			return *this;
		}
		//Dict =>  add Value to the last pair
		else if (std::holds_alternative<Dict>(nodes_.back().GetValue())) {
			Node nd = BuildNode(value);
			Node& last_node = nodes_.back();
			const Dict& cst_dict = last_node.AsDict();
			Dict& q = const_cast<Dict&>(cst_dict);

			std::string* ptr = std::get_if<std::string>(&keys_.back());
			if (ptr == nullptr) {
				throw std::logic_error("Applying Value without Key");
			}

			q[*ptr] = nd;

			//once the key is used set it to nullptr
			keys_.back() = nullptr;

			return *this;
		}
		//Node => Simple case oder Terminal case
		else if (std::holds_alternative<nullptr_t>(nodes_.back().GetValue())) {
			Node nd = BuildNode(value);
			root_ = nd;
			nodes_.pop_back();

			return *this;

		}
		else {
			throw std::logic_error("");
		}

	}

	
	
	DictItemContext Builder::StartDict() {
		nodes_.emplace_back(Dict{});
		keys_.push_back(nullptr);

		return DictItemContext{ *this };
	}

	ArrayItemContext Builder::StartArray() {
		nodes_.emplace_back(Array{});
		return ArrayItemContext{ *this };
	}
	

	Builder& Builder::EndDict() {
		if (nodes_.size() == 0) {
			throw std::logic_error("Object already build");
		}
		if (!std::holds_alternative<Dict>(nodes_.back().GetValue())) {
			throw std::logic_error("Applying EndDict in wrong context");
		}
		Node dict = nodes_.back();
		nodes_.pop_back();
		keys_.pop_back();
		Value(dict.AsDict());

		return *this;
	}
	

	Builder& Builder::EndArray() {
		if (nodes_.size() == 0) {
			throw std::logic_error("Object already build");
		}
		if (!std::holds_alternative<Array>(nodes_.back().GetValue())) {
			throw std::logic_error("Applying EndArray in wrong context");
		}
		Node cur_array = nodes_.back();
		nodes_.pop_back();
		Value(cur_array.AsArray());

		return *this;
	}
	

	Node Builder::Build() {
		if (nodes_.size() > 0) {
			throw std::logic_error("Object is not build yet");
		}

		return root_;
	}



	//-------------------------------------------
	//------------ CONTEXTS ---------------------

	BaseContext::BaseContext(Builder& builder) : builder_(builder) {

	}

	Node BaseContext::Build() {
		return builder_.Build();
	}

	KeyItemContext BaseContext::Key(std::string key) {
		return builder_.Key(key);
	}


	DictItemContext BaseContext::StartDict() {
		return builder_.StartDict();
	}

	ArrayItemContext BaseContext::StartArray() {
		return builder_.StartArray();
	}

	Builder& BaseContext::EndDict() {
		return builder_.EndDict();
	}

	Builder& BaseContext::EndArray() {
		return builder_.EndArray();
	}


	DictItemContext::DictItemContext(Builder& builder) : BaseContext(builder) {

	}


	ArrayItemContext::ArrayItemContext(Builder& builder) : BaseContext(builder) {

	}

	ArrayItemContext ArrayItemContext::Value(Node::Value value) {
		return ArrayItemContext{ builder_.Value(value) };
	}

	KeyItemContext::KeyItemContext(Builder& builder) : BaseContext(builder) {

	}

	DictItemContext KeyItemContext::Value(Node::Value value) {
		return DictItemContext{ builder_.Value(value) };
	}
	

	//-----------------------------------------------

}