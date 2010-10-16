/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL$
 * $Id$
 *
 */

#include "gob/console.h"
#include "gob/gob.h"
#include "gob/inter.h"

namespace Gob {

GobConsole::GobConsole(GobEngine *vm) : GUI::Debugger(), _vm(vm) {
	DCmd_Register("varSize",   WRAP_METHOD(GobConsole, cmd_varSize));
	DCmd_Register("var8",      WRAP_METHOD(GobConsole, cmd_var8));
	DCmd_Register("var16",     WRAP_METHOD(GobConsole, cmd_var16));
	DCmd_Register("var32",     WRAP_METHOD(GobConsole, cmd_var32));
	DCmd_Register("varString", WRAP_METHOD(GobConsole, cmd_varString));
}

GobConsole::~GobConsole() {
}

void GobConsole::preEnter() {
}

void GobConsole::postEnter() {
}

bool GobConsole::cmd_varSize(int argc, const char **argv) {
	DebugPrintf("Size of the variable space: %d bytes\n", _vm->_inter->_variables->getSize());
	return true;
}

bool GobConsole::cmd_var8(int argc, const char **argv) {
	if (argc == 1) {
		DebugPrintf("Usage: var8 <var offset> (<value>)\n");
		return true;
	}

	uint32 varNum = atoi(argv[1]);

	if (varNum >= _vm->_inter->_variables->getSize()) {
		DebugPrintf("Variable offset out of range\n");
		return true;
	}

	if (argc > 2) {
		uint32 varVal = atoi(argv[2]);
		_vm->_inter->_variables->writeOff8(varNum, varVal);
	}

	DebugPrintf("var8_%d = %d\n", varNum, _vm->_inter->_variables->readOff8(varNum));

	return true;
}

bool GobConsole::cmd_var16(int argc, const char **argv) {
	if (argc == 1) {
		DebugPrintf("Usage: var16 <var offset> (<value>)\n");
		return true;
	}

	uint32 varNum = atoi(argv[1]);

	if ((varNum + 1) >= _vm->_inter->_variables->getSize()) {
		DebugPrintf("Variable offset out of range\n");
		return true;
	}

	if (argc > 2) {
		uint32 varVal = atoi(argv[2]);
		_vm->_inter->_variables->writeOff16(varNum, varVal);
	}

	DebugPrintf("var16_%d = %d\n", varNum, _vm->_inter->_variables->readOff16(varNum));

	return true;
}

bool GobConsole::cmd_var32(int argc, const char **argv) {
	if (argc == 1) {
		DebugPrintf("Usage: var32 <var offset> (<value>)\n");
		return true;
	}

	uint32 varNum = atoi(argv[1]);

	if ((varNum + 3) >= _vm->_inter->_variables->getSize()) {
		DebugPrintf("Variable offset out of range\n");
		return true;
	}

	if (argc > 2) {
		uint32 varVal = atoi(argv[2]);
		_vm->_inter->_variables->writeOff32(varNum, varVal);
	}

	DebugPrintf("var8_%d = %d\n", varNum, _vm->_inter->_variables->readOff32(varNum));

	return true;
}

bool GobConsole::cmd_varString(int argc, const char **argv) {
	if (argc == 1) {
		DebugPrintf("Usage: varString <var offset> (<value>)\n");
		return true;
	}

	uint32 varNum = atoi(argv[1]);

	if (varNum >= _vm->_inter->_variables->getSize()) {
		DebugPrintf("Variable offset out of range\n");
		return true;
	}

	if (argc > 2) {
		uint32 maxLength = _vm->_inter->_variables->getSize() - varNum;

		Common::strlcpy(_vm->_inter->_variables->getAddressOffString(varNum), argv[2], maxLength);
	}

	DebugPrintf("varString_%d = \"%s\"\n", varNum, _vm->_inter->_variables->getAddressOffString(varNum));

	return true;
}

} // End of namespace Gob
