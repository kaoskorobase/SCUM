// ---------------------------------------------------------------------
// SCUMDesktop : SCUMObject
// ---------------------------------------------------------------------
//
// singleton class that represents the desktop. holds references to
// allocated toplevel objects like windows and menus and has
// properties that are used as default values in various view classes.
//
// ---------------------------------------------------------------------
// properties
// ---------------------------------------------------------------------
//
// <  size -- Size
//
//    size of the screen in pixels
//
// <> fgColor -- Color
//
//    default foreground color
//
// <> bgColor -- Color
//
//    default background color
//
// <> focusColor -- Color
//
//    color used for focus indication
//
// <> font -- Font
//
//    default font
//
// ---------------------------------------------------------------------
// class methods
// ---------------------------------------------------------------------
//
// *keyDownAction
// *keyDownAction_
// *keyUpAction
// *keyUpAction_
//
//    get/set global default key handlers.
//
// *windows
//
//    return a list of all GUI windows.
//
// *closeAllWindows
//
//    close all GUI windows.
//
// ---------------------------------------------------------------------
// $Id$
// ---------------------------------------------------------------------
