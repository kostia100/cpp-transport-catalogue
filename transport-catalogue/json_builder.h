#pragma once

#include "json.h"
#include <deque>

namespace json {

	class BaseContext;
	class DictItemContext;
	class ArrayItemContext;
	class KeyItemContext;

	class ValueDictContext;
	class ValueArrayContext;

	Node BuildNode(Node::Value value);



	class Builder {
	public:

		Builder();

		KeyItemContext Key(std::string key);

		Builder& Value(Node::Value value);

		DictItemContext StartDict();

		ArrayItemContext StartArray();

		Builder& EndDict();

		Builder& EndArray();

		Node Build();

	private:
		Node root_;
		std::vector<Node*> nodes_stack_;
		std::vector<std::variant<nullptr_t, std::string>> keys_;
		std::deque<Node> all_nodes_;

	};



	class BaseContext {
	public:
		BaseContext(Builder& builder);

		Node Build();

		KeyItemContext Key(std::string key);

		//Builder& Value(Node::Value value);

		DictItemContext StartDict();

		ArrayItemContext StartArray();

		Builder& EndDict();

		Builder& EndArray();

	protected:
		Builder& builder_;
	};

	class DictItemContext : public BaseContext {
	public:
		DictItemContext(Builder& builder);

		Node Build() = delete;

		Builder& Value(Node::Value value) = delete;

		DictItemContext StartDict() = delete;

		ArrayItemContext StartArray() = delete;

		Builder& EndArray() = delete;

	};

	class ArrayItemContext : public BaseContext {
	public:
		ArrayItemContext(Builder& builder);

		Node Build() = delete;

		KeyItemContext Key(std::string key) = delete;

		ArrayItemContext Value(Node::Value value);

		//DictItemContext StartDict();

		//Builder& StartArray();

		Builder& EndDict() = delete;

		//Builder& EndArray();

	};


	class KeyItemContext : public BaseContext {
	public:
		KeyItemContext(Builder& builder);

		Node Build() = delete;

		KeyItemContext Key(std::string key) = delete;

		//Builder& Value(Node::Value value);
		DictItemContext Value(Node::Value value);

		//DictItemContext StartDict();

		//ArrayItemContext StartArray();

		Builder& EndDict() = delete;

		Builder& EndArray() = delete;
	};

}