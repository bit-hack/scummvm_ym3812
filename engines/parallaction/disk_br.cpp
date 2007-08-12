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

#include "common/stdafx.h"
#include "parallaction/parallaction.h"


namespace Parallaction {


void DosDisk_br::errorFileNotFound(const char *s) {
	error("File '%s' not found", s);
}

Common::String DosDisk_br::selectArchive(const Common::String& name) {
	debugC(5, kDebugDisk, "DosDisk_br::selectArchive");

	Common::String oldPath(_partPath);
	strcpy(_partPath, name.c_str());

	return oldPath;
}

void DosDisk_br::setLanguage(uint16 language) {
	debugC(5, kDebugDisk, "DosDisk_br::setLanguage");

	return;
}

DosDisk_br::DosDisk_br(Parallaction* vm) : _vm(vm) {

}

DosDisk_br::~DosDisk_br() {
}

Cnv* DosDisk_br::loadTalk(const char *name) {
	debugC(5, kDebugDisk, "DosDisk_br::loadTalk");

	return 0;
}

Script* DosDisk_br::loadLocation(const char *name) {
	debugC(5, kDebugDisk, "DosDisk_br::loadLocation");
	return 0;
}

Script* DosDisk_br::loadScript(const char* name) {
	debugC(5, kDebugDisk, "DosDisk_br::loadScript");
	return 0;
}

//	there are no Head resources in Big Red Adventure
Graphics::Surface* DosDisk_br::loadHead(const char* name) {
	debugC(5, kDebugDisk, "DosDisk_br::loadHead");
	return 0;
}


Graphics::Surface* DosDisk_br::loadPointer(const char *name) {
	debugC(5, kDebugDisk, "DosDisk_br::loadPointer");

	char path[PATH_LEN];
	sprintf(path, "%s.ras", name);

	Common::File stream;
	if (!stream.open(path))
		errorFileNotFound(path);

	stream.skip(4);
	uint width = stream.readUint32BE();
	uint height = stream.readUint32BE();
	stream.skip(20);
	stream.skip(768);

	Graphics::Surface *surf = new Graphics::Surface;

	surf->create(width, height, 1);
	stream.read(surf->pixels, width * height);

	return surf;
}


Font* DosDisk_br::loadFont(const char* name) {
	debugC(5, kDebugDisk, "DosDisk_br::loadFont");

	char path[PATH_LEN];
	sprintf(path, "%s.fnt", name);

	Common::File stream;
	if (!stream.open(path))
		errorFileNotFound(path);

	return createFont(name, stream);
}


Cnv* DosDisk_br::loadObjects(const char *name) {
	debugC(5, kDebugDisk, "DosDisk_br::loadObjects");
	return 0;
}

void genSlidePath(char *path, const char* name) {
	sprintf(path, "%s.bmp", name);
}

Graphics::Surface* DosDisk_br::loadStatic(const char* name) {
	debugC(5, kDebugDisk, "DosDisk_br::loadStatic");
	return 0;
}

Cnv* DosDisk_br::loadFrames(const char* name) {
	debugC(5, kDebugDisk, "DosDisk_br::loadFrames");
	return 0;
}

// Slides in Nippon Safes are basically screen-sized pictures with valid
// palette data used for menu and for location switches. Big Red Adventure
// doesn't need slides in that sense, but it still has some special
// graphics resources with palette data, so those will be named slides.
//
BackgroundInfo* DosDisk_br::loadSlide(const char *name) {
	debugC(5, kDebugDisk, "DosDisk_br::loadSlide");

	char path[PATH_LEN];
	genSlidePath(path, name);

	Common::File stream;
	if (!stream.open(path))
		errorFileNotFound(path);

	BackgroundInfo* info = new BackgroundInfo;

	stream.skip(4);
	info->width = stream.readUint32BE();
	info->height = stream.readUint32BE();
	stream.skip(20);

	byte rgb[768];
	stream.read(rgb, 768);

	for (uint i = 0; i < 256; i++) {
		info->palette.setEntry(i, rgb[i] >> 2, rgb[i+256] >> 2, rgb[i+512] >> 2);
	}

	info->bg.create(info->width, info->height, 1);
	stream.read(info->bg.pixels, info->width * info->height);

	return info;
}

BackgroundInfo* DosDisk_br::loadScenery(const char *name, const char *mask, const char* path) {
	debugC(5, kDebugDisk, "DosDisk_br::loadScenery");
	return 0;
}

Table* DosDisk_br::loadTable(const char* name) {
	debugC(5, kDebugDisk, "DosDisk_br::loadTable");

	char path[PATH_LEN];
	sprintf(path, "%s/%s.tab", _partPath, name);

	Common::File	stream;
	if (!stream.open(path))
		errorFileNotFound(path);

	Table *t = new Table(100);

	fillBuffers(stream);
	while (scumm_stricmp(_tokens[0], "ENDTABLE")) {
		t->addData(_tokens[0]);
		fillBuffers(stream);
	}

	stream.close();

	return 0;
}

Common::SeekableReadStream* DosDisk_br::loadMusic(const char* name) {
	debugC(5, kDebugDisk, "DosDisk_br::loadMusic");
	return 0;
}


Common::ReadStream* DosDisk_br::loadSound(const char* name) {
	debugC(5, kDebugDisk, "DosDisk_br::loadSound");
	return 0;
}


} // namespace Parallaction
