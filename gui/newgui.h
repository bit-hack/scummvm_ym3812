/* ScummVM - Scumm Interpreter
 * Copyright (C) 2002-2006 The ScummVM project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $Header$
 */

#ifndef NEWGUI_H
#define NEWGUI_H

#include "common/scummsys.h"
#include "common/singleton.h"
#include "common/stack.h"
#include "common/str.h"
#include "graphics/fontman.h"
#include "gui/theme.h"

class OSystem;

namespace GUI {

class Dialog;

#define g_gui	(GUI::NewGui::instance())


// Height of a single text line
#define kLineHeight	(g_gui.getFontHeight() + 2)


using Graphics::TextAlignment;
using Graphics::kTextAlignCenter;
using Graphics::kTextAlignLeft;
using Graphics::kTextAlignRight;


// Simple dialog stack class
// Anybody nesting dialogs deeper than 4 is mad anyway
typedef Common::FixedStack<Dialog *> DialogStack;


/**
 * GUI manager singleton.
 */
class NewGui : public Common::Singleton<NewGui> {
	typedef Common::String String;
	friend class Dialog;
	friend class Common::Singleton<SingletonBaseType>;
	NewGui();
public:

	// Main entry for the GUI: this will start an event loop that keeps running
	// until no dialogs are active anymore.
	void runLoop();

	bool isActive() const	{ return ! _dialogStack.empty(); }

	Theme *theme() { return _theme; }

	const Graphics::Font &getFont() const { return *(_theme->getFont()); }
	int getFontHeight() const { return _theme->getFontHeight(); }
	int getStringWidth(const Common::String &str) const { return _theme->getStringWidth(str); }
	int getCharWidth(byte c) const { return _theme->getCharWidth(c); }

protected:
	OSystem			*_system;

	Theme		*_theme;

	bool		_needRedraw;
	DialogStack	_dialogStack;

	bool		_stateIsSaved;

	// for continuous events (keyDown)
	struct {
		uint16 ascii;
		byte flags;
		int keycode;
	} _currentKeyDown;
	uint32		_keyRepeatTime;

	// position and time of last mouse click (used to detect double clicks)
	struct {
		int16 x, y;	// Position of mouse when the click occured
		uint32 time;	// Time
		int count;	// How often was it already pressed?
	} _lastClick;

	// mouse cursor state
	bool		_oldCursorMode;
	int		_cursorAnimateCounter;
	int		_cursorAnimateTimer;
	byte		_cursor[2048];

	void saveState();
	void restoreState();

	void openDialog(Dialog *dialog);
	void closeTopDialog();

	void loop();

	void animateCursor();
};

} // End of namespace GUI

#endif
