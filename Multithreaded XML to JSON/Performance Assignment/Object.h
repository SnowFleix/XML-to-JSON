#pragma once
#include <string>
#include <list>
#include <algorithm>

class Object
{
public:
	Object() {}
	~Object() { std::for_each(_child.begin(), _child.end(), [](Object* o) {delete o; }); }

	// setters
	void setName(std::string name) { _name = name; }
	void setAttribute(std::string attribute) { _attribute = attribute; }
	void setParent(Object* parentpointer) { _parent = parentpointer; }
	void addChild(Object* child) { _child.push_back(child); }

	// getters
	const std::string getName() { return _name; }
	const std::string getAttribute() { return _attribute; }
	Object* getParent() { return _parent; }
	std::list<Object*>& getChildren() { return _child; }

private:
	Object* _parent;
	std::string _name, _attribute;
	std::list<Object*> _child;
};

