/*
 * Copyright 2011 by Seth N. Hetu
 *
 * Please refer to the end of the file for licensing information
 */

#pragma once


#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <stdexcept>

#include "NGram/wz_utilities.h"


/**
 * A "node" in our javascript tree contains either a
 *   String value or a Map to other nodes via string keys.
 * (The String value is stored in a stack to allow multiple overrides)
 * Note that this class is only included in ConfigManager.h, so we define the
 *   entire thing in the header. (Include guards are there just in case).
 */
class StringNode {
public:
	//Constructors
	StringNode() {
		this->parent = NULL;
		this->dirty = true;
	}
	StringNode(const std::wstring& val) {
		this->parent = NULL;
		this->str(val);
		this->dirty = true;
	}
	StringNode(const wchar_t* val) {
		this->parent = NULL;
		this->str(val);
		this->dirty = true;
	}

	//Check properties about this node
	bool isLeaf() const {
		return !textValues.empty();
	}
	bool isEmpty() const {
		return textValues.empty() && childList.empty();
	}

	//Get/Set string value
	const std::vector<std::wstring>& getStringStack() const {
		if (textValues.empty())
			throw std::runtime_error("Cannot call \"getStringStack\" on an empty node.");
		return textValues;
	}
	const std::wstring& str() const {
		if (textValues.empty())
			throw std::runtime_error("Cannot call \"getString\" on an empty node.");
		return textValues.back();
	}
	void str(const std::wstring& val) {
		if (val.empty())
			throw std::runtime_error("Cannot call \"setString\" on an empty string.");
		textValues.push_back(val);
		assertValid();
	}

	//Get/Set children
	//NOTE: I'd like to make this const, but that would require making "dirty" mutable,
	//      which seems like an abuse of the keyword.
	std::map<std::wstring, StringNode>& getChildNodes() {
		return childList;
	}
	StringNode& getOrAddChild(const std::wstring& rkey) {
		std::wstring key = waitzar::sanitize_id(rkey);
		if (childList.count(key)==0) {
			childList[key] = StringNode();
			childList[key].parent = this;
			childList[key].parentKey = key;
			assertValid();
			childList[key].setAndPropagateDirty(true);
		}
		return childList[key];
	}

	//Used to access child elements
	StringNode& operator[] (const std::wstring& rkey) {
		std::wstring key = waitzar::sanitize_id(rkey);
		if (childList.count(key)>0)
			return childList[key];
		throw std::runtime_error((std::string("Node contains no key: ")+waitzar::escape_wstr(key)).c_str());
    }


	//Helper: Get a fully-qualified key name for this node
	std::wstring getFullyQualifiedKeyName() const {
		std::wstring ret;
		for (const StringNode* considering=this; considering->parent!=NULL; considering=considering->parent) {
			ret = considering->parentKey + (!ret.empty()?L".":L"") + ret;
		}
		return ret;
	}

	std::wstring getKeyInParentMap() const {
		return parentKey;
	}


	bool isDirty() const {
		return dirty;
	}
	void setAndPropagateDirty(bool val) {
		//Nothing
		//if (val==dirty)
		//	return;

		//"Dirty" always propagates up
		if (val) {
			dirty = val;
			if (parent!=NULL && !parent->dirty)
				parent->setAndPropagateDirty(val);
			return;
		}

		//"Clean" propagates up if all other children are clean
		if (!val) {
			dirty = val;
			if (parent!=NULL) {
				bool cleanParent = true;
				auto candidates = parent->getChildNodes();
				for (auto it=candidates.begin(); it!=candidates.end(); it++) {
					if (it->second.isDirty()) {
						cleanParent = false;
						break;
					}
				}
				if (cleanParent)
					parent->setAndPropagateDirty(val);
			}
			return;
		}
	}


private:
	//Data
	std::map<std::wstring, StringNode> childList;
	std::vector<std::wstring> textValues;

	//For fast reverse-lookup of a fully-qualified key name
	StringNode* parent;
	std::wstring parentKey;

	//Partial loading requires this
	bool dirty;

	//Helper: throw an exception if we're in an invalid state.
	void assertValid() const {
		if (!textValues.empty() && !childList.empty())
			throw std::runtime_error("Non-leaf node contains value.");
	}
};




/*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
