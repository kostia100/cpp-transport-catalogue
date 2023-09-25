#pragma once

#include "json.h"
#include <deque>

namespace json {

	class BaseContext;
	class DictItemContext;
	class ArrayItemContext;
	class KeyItemContext;

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
		std::vector<std::variant<nullptr_t, std::string>> keys_;
		Node root_;
		std::deque<Node> nodes_;

	};



	class BaseContext {
	public:
		BaseContext(Builder& builder);

		Node Build();

		KeyItemContext Key(std::string key);

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

		Builder& EndDict() = delete;

	};


	class KeyItemContext : public BaseContext {
	public:
		KeyItemContext(Builder& builder);

		Node Build() = delete;

		KeyItemContext Key(std::string key) = delete;

		DictItemContext Value(Node::Value value);

		Builder& EndDict() = delete;

		Builder& EndArray() = delete;
	};

}