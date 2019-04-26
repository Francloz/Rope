#pragma once

#include <string>
#include <stdexcept>

class Rope {
	struct Node {
		size_t size, lvl;
		char *str;
		Node *l, *r;

		Node(Node* l, Node* r) : size(l->size + r->size), str(nullptr), l(l), r(r) {}
		Node(const size_t& max_size, const char* str, const size_t& begin, const size_t& end) : size(end-begin), str(new char[max_size]), l(nullptr), r(nullptr)
		{
			if (size > max_size)
				throw std::invalid_argument("Size surpassed the given maximum.");

			strncpy(this->str, str + begin, size);
		}
	};

	const size_t MAX_STR;
	Node* root;

public:
	Rope(const size_t& max_size) : MAX_STR(max_size), root(nullptr) {}

	void append(Rope& rope) 
	{
		Node* node = new Node(root, rope.root);
		root = node;
	}
};