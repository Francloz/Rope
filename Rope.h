#pragma once
#pragma warning(disable:4996)

#include <string>
#include <stdexcept>
#include <algorithm>
#include <memory>

// Rope data implementation.
// Keeps a string stored in pieces in the leaves of a tree.
// This allows appends in O(1), but has search in O(log(n)).
class Rope {

	// This structure represent a node of the rope.
	// Only leaves contain the strings.
	struct Node {
		// Size of the string stored in the leaves of this tree.
		size_t size;
		// Height of the tree.
		size_t lvl;
		// String.
		char *str;
		// Left child.
		std::shared_ptr<Node> l;
		// Right child.
		std::shared_ptr<Node> r;

		// Constructs a node whose children are r and l.
		// @param l left child
		// @param r right child
		Node(const std::shared_ptr<Node>& l, const std::shared_ptr<Node>& r) : size(l->size + r->size), lvl(0), str(nullptr), l(l), r(r) {}
		
		// Constructs a tree containing the string.
		// @param max_size maximum size of the sub-string
		// @param str string
		// @param begin beginning of the string
		// @param end end of the string
		Node(const size_t& max_size, const char* str, const size_t& begin, const size_t& end) : size(end-begin), str(nullptr), l(nullptr), r(nullptr)
		{
			size_t size = end - begin;
			if (size <= max_size)
			{
				this->str = new char[max_size];
				std::strncpy(this->str, str + begin, size);
				lvl = 0;
			}
			else if (size <= 2 * max_size) 
			{
				l = std::make_shared<Node>(max_size, str, begin, begin + max_size);
				r = std::make_shared<Node>(max_size, str, begin + max_size, end);
				lvl = std::max(l->lvl, r->lvl);
			}
			else 
			{
				const size_t middle = size / max_size / 2 * max_size;
				l = std::make_shared<Node>(max_size, str, begin, begin + middle);
				r = std::make_shared<Node>(max_size, str, begin + middle, end);
				lvl = std::max(l->lvl, r->lvl);
			}
		}

		// Deletes the string stored if necessary.
		~Node()
		{
			if (str != nullptr)
			{
				delete[] str;
			}
		}
	};

	// Maximum size of the string stored in the leaves.
	const size_t MAX_STR;
	// Root of the rope.
	std::shared_ptr<Node> root;

	// Rotates the node rightward.
	// @param node node to rotate
	void rotate_right(std::shared_ptr<Node>& node) const
	{
		std::shared_ptr<Node> aux = node->l;

		node->l = aux->r;
		aux->r = node;

		node->size = node->l->size + node->r->size;
		node->lvl = std::max(node->l->size,node->r->size);

		aux->size = aux->l->size + aux->r->size;
		aux->lvl = std::max(aux->l->size, aux->r->size);

		node = aux;
	}

	// Rotates the node leftward.
	// @param node node to rotate
	void rotate_left(std::shared_ptr<Node>& node) const
	{
		std::shared_ptr<Node> aux = node->r;

		node->r = aux->l;
		aux->l = node;

		node->size = node->l->size + node->r->size;
		node->lvl = std::max(node->l->size, node->r->size);

		aux->size = aux->l->size + aux->r->size;
		aux->lvl = std::max(aux->l->size, aux->r->size);

		node = aux;
	}
	// Returns the string that is stored inside the tree.
	// @return string
	std::string string(const std::shared_ptr<Node>& node) const
	{
		std::string str;
		if (node->str != nullptr)
			str = std::string(node->str, node->size);
		else
			str = string(node->l) + string(node->r);
		return str;
	}
public:
	// Constructs a rope setting the maximum size of the string in each node to max_size.
	// @param max_size maximum size of the sub-strings 
	Rope(const size_t& max_size = 20) : MAX_STR(max_size), root() {}

	// Initializes the rope with the given string.
	// @param str string array
	// @param begin first position
	// @param end final position
	// @param max_size maximum size of the sub-strings
	Rope(const char* str, const size_t& begin, const size_t& end, const size_t& max_size = 20) : MAX_STR(max_size), root(std::shared_ptr<Node>(new Node(MAX_STR, str, begin, end))) {}
	
	// Merges two ropes into one and results the merged rope. This method is equivalent to
	// appending the second rope to the first one.
	// @param l left rope
	// @param r right rope
	// @return merged rope. 
	static Rope merge(Rope& l, Rope& r)
	{
		if (l.MAX_STR != r.MAX_STR)
			throw std::invalid_argument("Size surpassed the given maximum.");

		Rope rope(r.MAX_STR);
		rope.root = std::make_shared<Node>(l.root, r.root);
		return rope;
	}

	// Appends the passed rope at the end.
	// @param rope appended rope
	inline void append(Rope& rope) 
	{
		root = std::make_shared<Node>(root, rope.root);
	}
	// Returns the size of the rope.
	inline size_t size() const
	{
		return root->size;
	}
	
	// Appends the string to the end of the rope.
	// @param str string
	// @param begin initial position
	// @param end final position
	inline void append(const char* str, const size_t& begin, const size_t& end) 
	{
		auto aux = std::make_shared<Node>(MAX_STR, str, begin, end);
		root = std::make_shared<Node>(root, aux);
		
		bool is_balanced = false;

		while(!is_balanced){			
			const int diff = root->l->lvl - root->r->lvl;
			if (diff > 2)
			{
				rotate_right(root);
			}
			else if (diff < -2)
			{
				rotate_left(root);
			}
			else 
			{
				is_balanced = true;
			}
		} 
	}

	// Returns the character at the given position.
	// @param index position
	// @return character at 'index'
	char operator[](size_t index) const
	{
		std::shared_ptr<Node> curr = root;
		while (root->size >= index) {
			if (root->l->size >= index)
			{
				curr = root->l;
			}
			else
			{
				index -= root->l->size;
				curr = root->r;
			}
		}
		return curr->str[index];
	}

	// Returns the string that is stored inside the rope.
	// @return string
	inline std::string string() const
	{
		return string(root);
	}
};
