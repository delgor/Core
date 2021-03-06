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

#ifndef NURIA_DEPEDENCYMANAGER_HPP
#define NURIA_DEPEDENCYMANAGER_HPP

#include "essentials.hpp"
#include <QObject>

namespace Nuria {
class DependencyManagerPrivate;

/**
 * \brief DepedencyManager enables easy to use dependency injection.
 * 
 * Dependency injection is interesting whenever a class has dependencies
 * to other utility classes. Those classes usually only have a single
 * application-wide instance, thus those are often implemented as singletons.
 * While using singletons is mostly easy to do, it has also its flaws:
 * 1. It requires additional methods to be implemented. Granted this is easy,
 *    but it's also really repetitive.
 * 2. It's really hard to test classes which rely on singletons.
 * 
 * \par Usage
 * First, you'll need to register all classes which you want to use as
 * dependencies to the Qt meta system:
 * \codeline Q_DECLARE_METATYPE(MyType*)
 * 
 * This line should come right after the class definition. Please note that
 * the macro itself \b must be invoked on the global scope.
 * 
 * After this, you can start using it right away with the NURIA_DEPENDENCY
 * macro:
 * \codeline MyType *myType = NURIA_DEPENDENCY(MyType)
 * 
 * \par Requirements for dependency classes
 * If DependencyManager should create instances on-demand, a constructor which
 * takes zero (Or only optional ones) arguments must be annotated using
 * \a Q_INVOKABLE. 
 * 
 * \par Advanced usage
 * For more fine-grained control, please see the methods this class offers.
 * If you're writing unit tests, then you're probably most interested in
 * storeObject(). Example:
 * \codeline Nuria::DependencyManager::instance ()->storeObject ("MyType", myType);
 * 
 * \note You have to supply a name here as the code won't be able to figure
 * out the name itself. You may want to consider writing a simple macro for
 * convenience:
 * \codeline #define STORE_DEP(T, Inst) Nuria::DependencyManager::instance ()->storeObject (#T, Inst);
 * 
 */
class NURIA_CORE_EXPORT DependencyManager : public QObject {
	Q_OBJECT
public:
	
	/**
	 * Behaviours for multi-threaded applications.
	 * \sa defaultBehaviour
	 */
	enum ThreadingPolicy {
		/**
		 * Maps to the current default policy.
		 */
		DefaultPolicy,
		
		/**
		 * One pool for all objects, but with a mutex guarding the
		 * internal structures.
		 */
		ApplicationGlobal,
		
		/**
		 * One pool for all objects, with \b no mutex guards. Use this
		 * setting for single-threaded applications.
		 */
		SingleThread,
		
		/**
		 * One pool per thread. Objects are freed when the thread gets
		 * destroyed, though some structures can only be freed upon
		 * application exit.
		 * \note This is the default behavious
		 */
		ThreadLocal
	};
	
	/** Destructor. */
	~DependencyManager ();
	
	/**
	 * Returns the global instance of the manager.
	 */
	static DependencyManager *instance ();
	
	/**
	 * Returns the current default threading policy.
	 * \sa setDefaultThreadingPolicy ThreadingPolicy
	 */
	ThreadingPolicy defaultThreadingPolicy () const;
	
	/**
	 * Sets the default threading policy.
	 * Passing DefaultPolicy has no effect.
	 * Changing the policy is \b not thread-safe.
	 * \sa ThreadingPolicy
	 */
	void setDefaultThreadingPolicy (ThreadingPolicy policy);
	
	/**
	 * Returns object \a name.
	 * If \a type is not \c -1 and \a name wasn't created yet, it will be
	 * created then, stored and returned.
	 * Else, \c nullptr is returned.
	 * 
	 * \note If \a type is not \c -1, it will be used as type check.
	 * If \a type and the type of object \a name don't match, \a nullptr
	 * is returned.
	 * 
	 * \sa getDependency NURIA_DEPENDENCY objectType
	 */
	void *objectByName (const QString &name, int type = -1, ThreadingPolicy policy = DefaultPolicy);
	
	/**
	 * Returns the meta type of object \a name or \c -1 if not found.
	 */
	int objectType (const QString &name, ThreadingPolicy policy = DefaultPolicy) const;
	
	/**
	 * Returns \c true if there is object \a name.
	 */
	inline bool hasObject (const QString &name, ThreadingPolicy policy = DefaultPolicy) const
	{ return objectType (name, policy) != -1; }
	
	/**
	 * Stores \a object of \a type as \a name. If there is already a object
	 * of the same name, it will be overwritten. \a object \b must be a
	 * registered type.
	 * \sa Q_DECLARE_METATYPE qRegisterMetaType
	 */
	void storeObject (const QString &name, void *object, int type,
			  ThreadingPolicy policy = DefaultPolicy);
	
	/**
	 * \overload
	 */
	template< typename T >
	void storeObject (const QString &name, T *object)
	{ storeObject (name, object, qMetaTypeId< T * > ()); }
	
	/**
	 * Tries to find object \a name of type \a T.
	 * On failure, \c nullptr is returned.
	 */
	template< typename T >
	inline static T *get (const QString &name, ThreadingPolicy policy = DefaultPolicy) {
		return static_cast< T * > (instance ()->objectByName (name, qMetaTypeId< T * > (), policy));
	}
	
private slots:
	
	void freeAllObjects ();
	
private:
	explicit DependencyManager (QObject *parent = 0);
	
	DependencyManagerPrivate *d_ptr;
};

}

#define NURIA_DEPENDENCY(T) Nuria::DependencyManager::get< T > (#T)

#endif // NURIA_DEPEDENCYMANAGER_HPP
