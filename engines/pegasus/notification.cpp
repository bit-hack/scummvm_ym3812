/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * Additional copyright for this file:
 * Copyright (C) 1995-1997 Presto Studios, Inc.
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
 */

#include "pegasus/constants.h"
#include "pegasus/notification.h"

namespace Pegasus {

typedef tReceiverList::iterator tReceiverIterator;

Notification::Notification(const tNotificationID id, NotificationManager *owner) : IDObject(id) {
	_owner = owner;
	_currentFlags = kNoNotificationFlags;
	if (_owner)
		_owner->addNotification(this);
}

Notification::~Notification() {
	for (tReceiverIterator it = _receivers.begin(); it != _receivers.end(); it++)
		it->receiver->newNotification(NULL);

	if (_owner)
		_owner->removeNotification(this);
}

//	Selectively set or clear notificiation bits.
//	Wherever mask is 0, leave existing bits untouched.
//	Wherever mask is 1, set bit equivalent to flags.
void Notification::notifyMe(NotificationReceiver *receiver, tNotificationFlags flags, tNotificationFlags mask) {
	for (tReceiverIterator it = _receivers.begin(); it != _receivers.end(); it++) {
		if (it->receiver == receiver) {
			it->mask = (it->mask & ~mask) | (flags & mask);
			receiver->newNotification(this);
			return;
		}
	}

	tReceiverEntry newEntry;
	newEntry.receiver = receiver;
	newEntry.mask = flags;
	_receivers.push_back(newEntry);

	receiver->newNotification(this);
}

void Notification::cancelNotification(NotificationReceiver *receiver) {
	for (tReceiverIterator it = _receivers.begin(); it != _receivers.end();) {
		if (it->receiver == receiver)
			it = _receivers.erase(it);
		else
			it++;
	}
}

void Notification::setNotificationFlags(tNotificationFlags flags, tNotificationFlags mask) {
	_currentFlags = (_currentFlags & ~mask) | flags;
}

void Notification::checkReceivers() {	
	tNotificationFlags currentFlags = _currentFlags;
	_currentFlags = kNoNotificationFlags;

	for (tReceiverIterator it = _receivers.begin(); it != _receivers.end(); it++)
		if (it->mask & currentFlags)
			it->receiver->receiveNotification(this, currentFlags);
}

//	Receiver entries are equal if their receivers are equal.

int operator==(const tReceiverEntry &entry1, const tReceiverEntry &entry2) {
	return	entry1.receiver == entry2.receiver;
}

int operator!=(const tReceiverEntry &entry1, const tReceiverEntry &entry2) {
	return	entry1.receiver != entry2.receiver;
}

NotificationReceiver::NotificationReceiver() {
	_notification = NULL;
}

NotificationReceiver::~NotificationReceiver() {
	if (_notification)
		_notification->cancelNotification(this);
}

void NotificationReceiver::receiveNotification(Notification *, const tNotificationFlags) {
}

void NotificationReceiver::newNotification(Notification *notification) {
	_notification = notification;
}

typedef tNotificationList::iterator tNotificationIterator;

NotificationManager::NotificationManager() {
}

NotificationManager::~NotificationManager() {
	detachNotifications();
}

void NotificationManager::addNotification(Notification *notification) {
	_notifications.push_back(notification);
}

void NotificationManager::removeNotification(Notification *notification) {
	for (tNotificationIterator it = _notifications.begin(); it != _notifications.end();) {
		if ((*it) == notification)
			it = _notifications.erase(it);
		else
			it++;
	}
}

void NotificationManager::detachNotifications() {
	for (tNotificationIterator it = _notifications.begin(); it != _notifications.end(); it++)
		(*it)->_owner = 0;
}

void NotificationManager::checkNotifications() {
	for (tNotificationIterator it = _notifications.begin(); it != _notifications.end(); it++)
		if ((*it)->_currentFlags != kNoNotificationFlags)
			(*it)->checkReceivers();
}

} // End of namespace Pegasus
