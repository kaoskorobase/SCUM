// ---------------------------------------------------------------------
// SCUM layout management
// ---------------------------------------------------------------------
//
// layout management in SCUM is the process of container views
// assigning positions and space to their child views when resizing
// the toplevel window.
//
// laying out views involves two distinct steps, size requisition and
// size allocation.
//
// during size requisition, each container, starting with the toplevel
// window, asks its children for their preferred size, assembles the
// result into a single size needed for the container and passes it on
// to its parent container. the size reported by the window's child is
// the smallest reasonable size its contents can have and thus also
// the minimum size the window can be resized to.
//
// size allocation takes place when the user or a program resizes the
// window, or when a property-change in a view makes a window resize
// necessary.
//
// there are a number of view properties that affect how the view
// behaves when it is assigned more space during size allocation than
// it requested during size requisition.
//
// xExpand,
// yExpand:
//
//    a Point that specifies weights in its x and y coordinates. only
//    views that have an expand value greater than zero are assigned
//    more space than their preferred size suggests if there is
//    available extra space.
//
//    expand values greater than zero are weights, i.e. they specify
//    resize behavior in relation to other views in the same
//    container.
//
// xFill,
// yFill:
//
//    a Point that specifies a ratio of how much of available extra
//    space the view should actually fill in each dimension. a value
//    of 1 fills available space completely, a value of 0 leaves the
//    view at its preferred size. the smallest possible fill value is
//    bounded by the view's preferred size.
//
// alignment:
//
//    an integer that specifies how the view should be aligned in
//    available space when it doesn't fill packing space completely.
//
// possible values are:
//
//             +-------------+
//             |  1   2   3  |
//             |             |
//             |  4   5   6  |
//             |             |
//             |  7   8   9  |
//             +-------------+
//
// minSize:
//
//    an artificial minimum size the view should take. the value is
//    bounded to its minimum by the view's preferred size.
//
// see also: SCUMBin, SCUMBox, SCUMHBox, SCUMVBox, SCUMGrid,
// SCUMHGrid, SCUMVGrid
//
// ---------------------------------------------------------------------
// $Id$
// ---------------------------------------------------------------------
