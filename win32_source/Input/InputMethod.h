/*
 * Copyright 2010 by Seth N. Hetu
 *
 * Please refer to the end of the file for licensing information
 */

#ifndef _INPUT_METHOD
#define _INPUT_METHOD

#include <string>
#include <vector>
#include "MyWin32Window.h"
#include "Hotkeys.h"
#include "OnscreenKeyboard.h"
#include "Settings/Types.h"
#include "NGram/wz_utilities.h"
#include "Transform/Transformation.h"
#include "Input/VirtKey.h"


//Hilite styles
enum HILITE_FLAGS {HF_NOTHING=0, HF_PATSINT=1, HF_CURRSELECTION=2, HF_LABELTILDE=4};



//Expected interface: "Input Method"
class InputMethod {
public:

	//Cascading init
	virtual void init(MyWin32Window* mainWindow, MyWin32Window* sentenceWindow, MyWin32Window* helpWindow, MyWin32Window* memoryWindow, const std::vector< std::pair <int, unsigned short> > &systemWordLookup, OnscreenKeyboard *helpKeyboard, std::wstring systemDefinedWords, const std::wstring& encoding, CONTROL_KEY_TYPE controlKeyStyle, bool typeBurmeseNumbers, bool typeNumeralConglomerates, bool suppressUppercase);


public:

	//Useful functionality
	virtual void treatAsHelpKeyboard(InputMethod* providingHelpFor, std::function<void (const std::wstring& fromEnc, const std::wstring& toEnc, std::wstring& src)> ConfigGetAndTransformSrc = std::function<void (const std::wstring& fromEnc, const std::wstring& toEnc, std::wstring& src)>()) = 0;
	bool isHelpInput();
	void forceViewChanged();
	bool getAndClearViewChanged();
	bool getAndClearRequestToTypeSentence();
	std::pair <std::string, std::wstring> getAndClearMostRecentRomanizationCheck();

	//Central dispatch
	virtual bool handleVKey(VirtKey& vkey);

	//Keypress handlers (abstract virtual)
	virtual void handleEsc() = 0;
	virtual void handleBackspace(VirtKey& vkey) = 0;
	virtual void handleDelete() = 0;
	virtual void handleLeftRight(bool isRight, bool loopToZero) = 0;
	virtual void handleUpDown(bool isDown) = 0;
	virtual void handleCommit(bool strongCommit) = 0;
	virtual void handleNumber(VirtKey& vkey) = 0;
	virtual void handleStop(bool isFull, VirtKey& vkey) = 0;
	virtual void handleTab() = 0;

	virtual void handleKeyPress(VirtKey& vkey);
	virtual void typeZWS();


	//Need to move later...
	virtual void typeHelpWord(std::string roman, std::wstring myanmar, int currStrDictID) = 0;

	//Returns its id (-1 for failure) and its romanization
	virtual std::pair<int, std::string> lookupWord(std::wstring typedWord) = 0;

protected:
	std::wstring systemDefinedWords;
	std::vector< std::pair <int, unsigned short> > systemWordLookup;
	std::vector<std::wstring> userDefinedWords;

	//Additional character to print
	wchar_t typedStopChar;

	//Additional entry
	std::pair <std::string, std::wstring> mostRecentRomanizationCheck;


//Todo: Fix redundancy...
public:
	const std::wstring& getEncoding() { return encoding; }


protected:
	//Window control
	MyWin32Window* mainWindow;
	MyWin32Window* sentenceWindow;
	MyWin32Window* helpWindow;
	MyWin32Window* memoryWindow;

	//For now... may move later
	OnscreenKeyboard *helpKeyboard;

	//Parent property
	std::wstring encoding;

	//Helper typing control
	InputMethod* providingHelpFor;
	std::function<void (const std::wstring& fromEnc, const std::wstring& toEnc, std::wstring& src)> ConfigGetAndTransformText;
	/*const Transformation* myenc2Uni;
	const Transformation* uni2Myenc;
	const Transformation* uni2Romanenc;
	const Transformation* romanenc2Uni;*/

	//Repaint after this?
	bool viewChanged;
	bool requestToTypeSentence;

	bool suppressUppercase;

	//Helpful placeholders (in case we need to change them)
	wchar_t zwsAlpha;
	int zwsID;

	//Must be maintained by the subclass
	std::wstringstream typedRomanStr;


public:  //Abstract methods

	//Is this class a placeholder, or is it a real IM?
	//   There are other ways to do this, but it's nice to 
	//   have a way of double-checking.
	//virtual bool isPlaceholder() = 0; 

	//Get strings to print, always in unicode
	//The current typed string (sentence string). 
	//This will be typed to the output program if "Enter" is pressed.
	//All sub-classes must ensure that this remains valid outside of any function call, and
	//  before any calls to the base class's methods.
	//Returns 3 strings:
	//  1) The string before the highlight.
	//  2) The string before the cursor (highlighted string).
	//  3) The string after the cursor.
	// In addition, a fourth string is returned containing the entire typed string.
	//TODO: Eventually redo with tuples
	virtual std::vector< std::wstring > getTypedSentenceStrings() = 0;
	virtual void appendToSentence(wchar_t letter, int id) = 0; //Used for system letters only, for perf. reasons. (id is optional)

	//The current "candidate" string, which will be displayed in the top
	//  window. It will not be entered until it also appears in the sentence string.
	//The same warnings apply as to the typedSentenceString.
	//NOTE: for now, the "int" part is just the highlight level: 1 for red and 2 for green (3 for both, which means green)
	//      TODO: Make this cleaner.
	virtual std::vector< std::pair<std::wstring, unsigned int> > getTypedCandidateStrings() = 0;


	//Get the typed romanized string. This consists ONLY of all typed valid letters
	//Not abstract
	virtual std::wstring getTypedRomanString(bool asLowercase);

	//Get the status of paging
	// Returns <currIndex, maxPages>
	virtual std::pair<int, int> getPagingInfo() const;

	//Called periodically
	virtual void reset(bool resetCandidates, bool resetRoman, bool resetSentence, bool performFullReset) = 0;

};


#endif //_INPUT_METHOD

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

