// ---------------------------------------------------------------------
// SCUMContainer : SCUMView
// ---------------------------------------------------------------------
//
// base class for views holding child views. subclasses implement
// layout management or provide an interface to an operating system
// window.
//
// see also: SCUMBin, SCUMBox, SCUMHBox, SCUMVBox, SCUMGrid, SCUMHGrid,
// SCUMVGrid, SCUMWindow.
//
// ---------------------------------------------------------------------
// instance variables
// ---------------------------------------------------------------------
//
// children -- Array
//
//   list of the receiver's child views in the order they where added
//
// named -- Dictionary
//
//   mapping from identifier strings to child views
//
// ---------------------------------------------------------------------
// properties
// ---------------------------------------------------------------------
//
// <>xPadding -- Float
// <>yPadding -- Float
//
//   the containers internal padding in x and y direction
//
// ---------------------------------------------------------------------
// instance methods
// ---------------------------------------------------------------------
//
// children
//
//   return the receiver's child views.
//
// at ( key )
//
//   return the child at index key, if key is an Integer, otherwise
//   the child named key.
//
// ---------------------------------------------------------------------
// $Id$
// ---------------------------------------------------------------------
