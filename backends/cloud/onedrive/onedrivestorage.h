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
*/

#ifndef BACKENDS_CLOUD_ONEDRIVE_ONEDRIVESTORAGE_H
#define BACKENDS_CLOUD_ONEDRIVE_ONEDRIVESTORAGE_H

#include "backends/cloud/storage.h"
#include "common/callback.h"

namespace Cloud {
namespace OneDrive {

class OneDriveStorage: public Cloud::Storage {
	static Common::String KEY, SECRET;

	Common::String _token, _uid;

	/** This private constructor is called from loadFromConfig(). */
	OneDriveStorage(Common::String token, Common::String uid);

	static void getAccessToken(Common::String code);	

public:	
	virtual ~OneDriveStorage();

	/**
	* Storage methods, which are used by CloudManager to save
	* storage in configuration file.
	*/

	/**
	* Save storage data using ConfMan.
	* @param keyPrefix all saved keys must start with this prefix.
	* @note every Storage must write keyPrefix + "type" key
	*       with common value (e.g. "Dropbox").
	*/

	virtual void saveConfig(Common::String keyPrefix);

	/** Public Cloud API comes down there. */

	/** Returns Common::Array<StorageFile>. */
	virtual void listDirectory(Common::String path, FileArrayCallback callback, bool recursive = false) {} //TODO

	/** Calls the callback when finished. */
	virtual void upload(Common::String path, Common::ReadStream *contents, BoolCallback callback) {} //TODO

	/** Returns pointer to Networking::NetworkReadStream. */
	virtual Networking::NetworkReadStream *streamFile(Common::String path) { return 0; } //TODO

	/** Calls the callback when finished. */
	virtual void download(Common::String remotePath, Common::String localPath, BoolCallback callback) {} //TODO

	/** Calls the callback when finished. */
	virtual void remove(Common::String path, BoolCallback callback) {} //TODO

	/** Calls the callback when finished. */
	virtual void syncSaves(BoolCallback callback);

	/** Calls the callback when finished. */
	virtual void createDirectory(Common::String path, BoolCallback callback) {} //TODO

	/** Calls the callback when finished. */
	virtual void touch(Common::String path, BoolCallback callback) {} //TODO

	/** Returns the StorageInfo struct. */
	virtual void info(StorageInfoCallback callback) {} //TODO

	/** Returns whether saves sync process is running. */
	virtual bool isSyncing() { return false; } //TODO

	/** Returns whether there are any requests running. */
	virtual bool isWorking() { return false; } //TODO

	/**
	* Add OneDriveStorage with given token and uid into Cloud::Manager.	
	*/
	static void addStorage(Common::String token, Common::String uid);

	/**
	* Load token and user id from configs and return OneDriveStorage for those.	
	* @return pointer to the newly created OneDriveStorage or 0 if some problem occured.
	*/
	static OneDriveStorage *loadFromConfig(Common::String keyPrefix);

	/**
	* Returns OneDrive auth link.
	*/
	static Common::String getAuthLink();

	/**
	* Show message with OneDrive auth instructions. (Temporary)
	*/
	static void authThroughConsole();
};

} //end of namespace OneDrive
} //end of namespace Cloud

#endif
