/* Copyright (c) 2014, The Nuria Project
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *    1. The origin of this software must not be misrepresented; you must not
 *       claim that you wrote the original software. If you use this software
 *       in a product, an acknowledgment in the product documentation would be
 *       appreciated but is not required.
 *    2. Altered source versions must be plainly marked as such, and must not be
 *       misrepresented as being the original software.
 *    3. This notice may not be removed or altered from any source
 *       distribution.
 */

#include "sessionmanager.hpp"


class Nuria::SessionManagerPrivate {
public:
	QMap< QByteArray, Session > sessions;
};

Nuria::SessionManager::SessionManager (QObject *parent)
	: Nuria::AbstractSessionManager (parent), d_ptr (new SessionManagerPrivate)
{
}

Nuria::Session Nuria::SessionManager::get (const QByteArray &id) {
	if (d_ptr->sessions.contains (id)) {
		return d_ptr->sessions.value (id);
	} else {
		return createSession (id);
	}
}

void Nuria::SessionManager::removeSession (const QByteArray &id) {
	d_ptr->sessions.remove (id);
}

Nuria::Session Nuria::SessionManager::createSession (const QByteArray &id) {
	Session session (id, this);
	d_ptr->sessions.insert (id, session);
	return session;
}
