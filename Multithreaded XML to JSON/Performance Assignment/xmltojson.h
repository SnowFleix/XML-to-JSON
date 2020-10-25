#pragma once

#include <string>
#include <vector>
#include "Object.h"
#include "utils.h"

const char OPEN = '<';
const char CLOSE = '>';
const char SLASH = '/';
const char SPACE = ' ';
const char QUOTE = '\'';
const char DQUOTE = '\"';
const char EQUAL = '=';
const char NEWLINE = '\n';
const char TAB = '\t';
const char EXCLAMATION = '!';
const char QUATIONMARK = '?';
const char MINUS = '-';
const char SQRFRONT = '[';
const char SQREND = ']';
// deal with CDATA later
const char CDATA[9] = "![CDATA["; 
// for a carriage return
const char CARRET = 13; 

std::string buildJSONstringFromObject(std::list<Object*>& objects, int currentNoIndents) {
	if (objects.size() < 1)
		return "";

	std::string retStr = "";
	for (Object* o : objects) {
		for (int i = 0; i < currentNoIndents; i++) retStr += "\t";
		retStr = retStr + "\"" + o->getName() + "\": ";
		if (isOnlyWhitespace(o->getAttribute())) {
			// I think recursion is pretty inefficient 
			retStr = retStr + "[\n" + buildJSONstringFromObject(o->getChildren(), currentNoIndents + 1); 
			for (int i = 1; i < currentNoIndents; i++) 
				retStr += "\t"; retStr += "],\n";
		}
		else {
			// maybe do something else with the comma cause it'll still add it even at the end
			retStr = retStr + "\"" + o->getAttribute() + "\",\n"; 
		}
	}
	return retStr;
}

std::string ConvertXMLToJSON(std::vector<char> xml) {

	int i(0);
	char curChar = xml[i];
	std::string builder = "";
	std::string name, data;
	std::list<Object*> objects;
	// current object
	Object* obj = new Object(); 

	while (i < xml.size()) {
		curChar = xml[i];
#pragma region Dealing with new lines, tabs and carriage returns
		// if it's a newline, tab or carriage return there's no need to do anything
		if (curChar == NEWLINE || curChar == TAB || curChar == CARRET) { 
			i++; continue;
		}
#pragma endregion

		if (curChar == OPEN) {

#pragma region Deals with the object finishing
			// if it's the end of the object
			if (xml[i + 1] == SLASH) {
				if (obj->getParent() != nullptr)
					obj = obj->getParent();
				else {
					objects.push_back(obj);
					obj = new Object();
				}
				i++;

				// loop past until the name close is found
				while (xml[i] != CLOSE) {
					i++;
				}
				i++;
				continue;
			}
#pragma endregion

			builder = "";
			curChar = xml[++i];

#pragma region Deal with creating a new object and adding attribute
			while (curChar != CLOSE) {
				if (curChar != NEWLINE && curChar != TAB) {
					builder += curChar;
				}
				curChar = xml[++i];
			}

			if (!isOnlyWhitespace(obj->getName()) && !isOnlyWhitespace(builder)) {
				//pointer goes out of scope but the object still exists - probably really bad
				Object* newObj = new Object(); 
				newObj->setParent(obj);
				obj->addChild(newObj);
				obj = newObj;
			}
			if (isOnlyWhitespace(obj->getName())) {
				// set the name of the object
				obj->setName(builder); 
			}
#pragma endregion

#pragma region Deals with getting the attribute from the xml object
			if (xml[i + 1] != OPEN && xml[i + 1] != CLOSE)
			{
				builder = "";
				curChar = xml[i + 1];
				while (curChar != OPEN) {
					if (curChar != NEWLINE && curChar != TAB) {
						builder += curChar;
					}
					curChar = xml[++i];
				}
				obj->setAttribute(builder);
				continue;
			}
#pragma endregion

#pragma region Dealing with xml comments
			// if it has <!-- it means it's a comment
			if (xml[i + 1] == EXCLAMATION && xml[i + 2] == MINUS && xml[i + 3] == MINUS) {
				i += 4; curChar = xml[i];
				while (!(xml[i] == MINUS && xml[i + 1] == MINUS && xml[i + 2] == CLOSE)) {
					/// doesn't do anything with the comments yet
					curChar = xml[++i];
				}
				i += 3;
				continue;
			}
#pragma endregion
		}
		i++;
	}

	int z = 0;

	// need to loop through the objects and turn them into JSON
	std::string stringBuilder(buildJSONstringFromObject(objects, 1));

	// Pretty sure the objects weren't being delted from memory before
	std::for_each(objects.begin(), objects.end(), [](Object* o) {
		delete o; 
	});

	return stringBuilder;
}