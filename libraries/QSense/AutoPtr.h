/*
 * AutoPtr.h
 *
 * Definition of the AutoPtr template class.
 *
 * Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
 * and Contributors.
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef QSENSE_AUTOPTR
#define QSENSE_AUTOPTR

#if defined( ARDUINO )
#include "QSense.h"
#include "../StandardCplusplus/algorithm"
#else
#include <QSense.h>
#include <algorithm>
#endif

namespace qsense
{
/**
 * @brief AutoPtr is a "smart" pointer for classes implementing reference counting based garbage collection.
 *
 * To be usable with the AutoPtr template, a class must
 * implement the following behaviour:
 *
 * - A class must maintain a reference count.
 * - The constructors of the object initialize the reference count to one.
 * - The class must implement a public duplicate() method:
 *     void duplicate();
 * that increments the reference count by one.
 * - The class must implement a public release() method:
 *     void release()
 * that decrements the reference count by one, and,
 * if the reference count reaches zero, deletes the
 * object.
 *
 * AutoPtr works in the following way:
 * - If an AutoPtr is assigned an ordinary pointer to
 * an object (via the constructor or the assignment operator),
 * it takes ownership of the object and the object's reference
 * count remains unchanged.
 * - If the AutoPtr is assigned another AutoPtr, the
 * object's reference count is incremented by one by
 * calling duplicate() on its object.
 * - The destructor of AutoPtr calls release() on its
 * object.
 * - AutoPtr supports dereferencing with both the ->
 * and the * operator. An attempt to dereference a null
 * AutoPtr results in a error that will cause application termination.
 * AutoPtr also implements all relational operators.
 * Note that AutoPtr allows casting of its encapsulated data types.
*/
template <class C>
class AutoPtr
{
public:
  /// Default constructor.  Creates a new instance that points to nothing
  AutoPtr() : _ptr( NULL ) {}

  /// Create an auto pointer that takes ownership of the specified pointer
  AutoPtr( C* ptr ) : _ptr( ptr ) {}

  /**
   * @brief AutoPtr Create an auto pointer that takes ownership of the specified pointer
   * @param ptr The pointer to take ownership of
   * @param shared If \c true then increment the reference count for the
   *   specified pointer.
   */
  AutoPtr( C* ptr, bool shared ) : _ptr( ptr )
  {
    if ( shared && _ptr ) _ptr->duplicate();
  }

  /// Copy constructor.  Increases the reference count for the owned object.
  AutoPtr( const AutoPtr& ptr ) : _ptr( ptr._ptr )
  {
    if ( _ptr ) _ptr->duplicate();
  }

  /// Copy constructor for taking ownership of another type of object
  template <class Other> 
  AutoPtr( const AutoPtr<Other>& ptr ) : _ptr( const_cast<Other*>( ptr.get() ) )
  {
    if ( _ptr ) _ptr->duplicate();
  }

  /// Destructor.  Invokes \c release on the owned object.
  ~AutoPtr() { if ( _ptr ) _ptr->release(); }
  
  /// Use to (re)set the owned object.  If current owned object is not
  /// \c null, invokes \c release on the object.
  AutoPtr& assign( C* ptr )
  {
    if ( _ptr != ptr )
    {
      if ( _ptr ) _ptr->release();
      _ptr = ptr;
    }

    return *this;
  }

  /**
   * @brief Reset the owned object.  If current owned object is not
   * \c null, invokes \c release on the object.  Will invoke \c duplicate
   * on the new instance if \c shared is specified
   * @param ptr The new object to take ownership of.
   * @param shared If \c true, \c duplicate() the owned object.
   * @return Reference to this instance for convenience.
   */
  AutoPtr& assign( C* ptr, bool shared )
  {
    if ( _ptr != ptr )
    {
      if ( _ptr ) _ptr->release();
      _ptr = ptr;
      if (shared && _ptr) _ptr->duplicate();
    }
    return *this;
  }
  
  /// Share the pointer owned by the specified auto pointer instance.
  AutoPtr& assign( const AutoPtr& ptr )
  {
    if ( &ptr != this )
    {
      if ( _ptr ) _ptr->release();
      _ptr = ptr._ptr;
      if ( _ptr ) _ptr->duplicate();
    }

    return *this;
  }
  
  /// Share the pointer owned by the specified auto pointer of different type.
  template <class Other> 
  AutoPtr& assign( const AutoPtr<Other>& ptr )
  {
    if ( ptr.get() != _ptr )
    {
      if ( _ptr ) _ptr->release();
      _ptr = const_cast<Other*>( ptr.get() );
      if ( _ptr ) _ptr->duplicate();
    }

    return *this;
  }

  /// Copy assignment operator.  Delegates to {@link #assign}.
  AutoPtr& operator = ( C* ptr )
  {
    return assign( ptr );
  }

  /// Copy assignment operator.  Delegates to {@link #assign}.
  AutoPtr& operator = ( const AutoPtr& ptr )
  {
    return assign( ptr );
  }
  
  /// Copy assignment operator.  Delegates to {@link #assign}.
  template <class Other>
  AutoPtr& operator = ( const AutoPtr<Other>& ptr )
  {
    return assign<Other>(ptr);
  }

  /// Swap the pointers of this instance with the specified instance.
  void swap( AutoPtr& ptr )
  {
    std::swap( _ptr, ptr._ptr );
  }
  
  /// Casts the AutoPtr via a dynamic cast to the given type.
  /// Returns an AutoPtr containing NULL if the cast fails.
  /// Example: (assume class Sub: public Super)
  ///    AutoPtr<Super> super(new Sub());
  ///    AutoPtr<Sub> sub = super.cast<Sub>();
  ///    poco_assert (sub.get());
  template <class Other>
  AutoPtr<Other> cast() const
  {
    Other* pOther = dynamic_cast<Other*>( _ptr );
    return AutoPtr<Other>( pOther, true );
  }

  /// Casts the AutoPtr via a static cast to the given type.
  /// Example: (assume class Sub: public Super)
  ///    AutoPtr<Super> super(new Sub());
  ///    AutoPtr<Sub> sub = super.unsafeCast<Sub>();
  ///    poco_assert (sub.get());
  template <class Other>
  AutoPtr<Other> unsafeCast() const
  {
    Other* pOther = static_cast<Other*>(_ptr);
    return AutoPtr<Other>(pOther, true);
  }

  /// Pointer acess operator for the owned object.  Returns \c NULL if invalid
  C* operator -> () { return _ptr; }

  /// Pointer acess operator for the owned object.  Returns \c NULL if invalid
  const C* operator -> () const { return _ptr; }

  /// Dereference operator for the owned object.  Will lead to program
  /// termination if the owned object is not valid
  C& operator * () { return *_ptr; }

  /// Dereference operator for the owned object.  Will lead to program
  /// termination if the owned object is not valid
  const C& operator * () const { return *_ptr; }

  /// Return the owned pointer.  Callers must not \c delete
  C* get() { return _ptr; }

  /// Return the owned pointer.  Callers must not \c delete
  const C* get() const { return _ptr; }

  /// Function operator.  Return the owned pointer
  operator C* () { return _ptr; }
  
  /// Function operator.  Return the owned pointer
  operator const C* () const { return _ptr; }
  
  /// Negative check of make sure the owned pointer is not valid.
  bool operator ! () const { return _ptr == 0; }

  /// Negative check of make sure the owned pointer is not valid.
  bool isNull() const { return _ptr == 0; }
  
  /// Invokes \c duplicate() on the owned pointer if valid.
  C* duplicate()
  {
    if ( _ptr ) _ptr->duplicate();
    return _ptr;
  }

  /// Compare the owned objects for equality
  bool operator == ( const AutoPtr& ptr ) const
  {
    return _ptr == ptr._ptr;
  }

  /// Compare the owned object against the specified object for equality
  bool operator == ( const C* ptr ) const { return _ptr == ptr; }

  /// Compare the owned object against the specified object for equality
  bool operator == (C* ptr) const { return _ptr == ptr; }

  /// Compare the owned objects for inequality
  bool operator != (const AutoPtr& ptr) const
  {
    return _ptr != ptr._ptr;
  }

  /// Compare the owned object against the specified object for inequality
  bool operator != ( const C* ptr ) const { return _ptr != ptr; }

  /// Compare the owned object against the specified object for inequality
  bool operator != ( C* ptr ) const { return _ptr != ptr; }

  /// Compare the owned objects for ordering
  bool operator < ( const AutoPtr& ptr ) const
  {
    return _ptr < ptr._ptr;
  }

  /// Compare the owned object against the specified object for ordering
  bool operator < ( const C* ptr ) const { return _ptr < ptr; }

  /// Compare the owned object against the specified object for ordering
  bool operator < ( C* ptr ) const { return _ptr < ptr; }

  bool operator <= ( const AutoPtr& ptr ) const
  {
    return _ptr <= ptr._ptr;
  }

  bool operator <= ( const C* ptr ) const { return _ptr <= ptr; }

  bool operator <= ( C* ptr ) const { return _ptr <= ptr; }

  bool operator > ( const AutoPtr& ptr ) const
  {
    return _ptr > ptr._ptr;
  }

  bool operator > ( const C* ptr ) const { return _ptr > ptr; }

  bool operator > ( C* ptr ) const { return _ptr > ptr; }

  bool operator >= ( const AutoPtr& ptr ) const
  {
    return _ptr >= ptr._ptr;
  }

  bool operator >= ( const C* ptr ) const { return _ptr >= ptr; }

  bool operator >= ( C* ptr ) const { return _ptr >= ptr; }

private:
  C* _ptr;
};


template <class C>
inline void swap( AutoPtr<C>& p1, AutoPtr<C>& p2 )
{
  p1.swap( p2 );
}

} // namespace qsense


#endif // QSENSE_AUTOPTR
