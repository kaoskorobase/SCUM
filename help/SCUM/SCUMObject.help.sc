// ---------------------------------------------------------------------
// SCUMObject : SCUM
// ---------------------------------------------------------------------
//
// base class for classes whose instances allocate operating system
// resources in SCUM.
//
// instances are either destroyed explicitly or implicitly when the UI
// shuts down. each object can be attached resources that are sent a
// 'free' message when the object is destroyed.
//
// properties of the underlying primitive object are accessed through
// the 'getProperty' and 'setProperty' methods. property keys are
// instances of Symbol.
//
// see also: SCUM, SCUMDesktop, SCUMView, SCUMMenu.
//
// ---------------------------------------------------------------------
// instance variables
// ---------------------------------------------------------------------
//
// dataPtr -- RawPointer
//
//   pointer to the primitive object
//
// resources -- Array
//
//   list of resources that are free'd when the object is destroyed
//
// ---------------------------------------------------------------------
// change notification keys
// ---------------------------------------------------------------------
//
// destroyed
//
//   the object has been destroyed
//
// ---------------------------------------------------------------------
// class methods
// ---------------------------------------------------------------------
//
// *propertyKeys
//
//   return a list of the settable property keys of instances of this
//   class. override in subclasses to account for additional properties.
//
// ---------------------------------------------------------------------
// instance methods
// ---------------------------------------------------------------------
//
// use ( function )
//
//   evaluate function with the receiver as an argument (OBSOLETE).
//
// isValid
// notValid
//
//   return true, false respectively when the object has not been
//   destroyed yet.
//
// destroy
//
//   destroy the underlying primitive object and free any attached
//   resources. after calling this method isValid will return false.
//
// releaseActionMap
//
//   break links to any change notification listeners.
//
// properties
//
//   return a dictionary of key -> value mappings for settable
//   properties. storing this information along with the class name
//   allows persistency schemes to restore the full state of the object.
//
// properties_ ( dict )
//
//   set the receiver's properties from the key -> value pairs in
//   dict. it is not an error if a mapping is not a valid property for
//   the receiver.
//
// getProperty ( key, value )
//
//   return the value associated with the property key. aggregate type
//   return values (like Rect, Point, Size, Font, etc.) must be allocated
//   before calling this method and passed as the value argument.
//
// setProperty ( key, value )
//
//   set the property key to value. returns true for some property
//   keys when the value has changed.
//
// setPropertyChanged ( key, value, function )
//
//   set the property key to value and execute function if the value
//   has changed.
//
// addResource ( resource )
//
//   add a resource handle to the receiver's list of attached
//   resources. resources added with this method are sent the 'free'
//   message when the object is destroyed.
//
// sched ( delta, function )
//
//   convenience method that only schedules function on the AppClock
//   as long as its primitive object has not been destroyed yet.
//
// play ( delta, function )
//
//   convenience method that only plays function on the AppClock
//   as long as its primitive object has not been destroyed yet.
//
// ------------------------------------------------------------------------
// $Id: SCUMObject.help.sc,v 1.1 2004/07/30 16:20:14 steve Exp $
// ------------------------------------------------------------------------
