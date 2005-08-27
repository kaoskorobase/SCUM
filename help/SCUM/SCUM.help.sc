// ---------------------------------------------------------------------
// SCUM : Object
// ---------------------------------------------------------------------
//
// namespace class without instances. defines constants used in SCUM
// classes.
//
// see also: SCUMObject, SCUMView
//
// ---------------------------------------------------------------------
// examples
// ---------------------------------------------------------------------

(
SCUM.do {
	SCUMWindow.make {
		~title = "scoobadoo";
		~initialSize = 300;
		~bgColor = Color.rand;
	}.show
}
)

// ---------------------------------------------------------------------
// class methods
// ---------------------------------------------------------------------
//
// *mod...
//
//    modifier bit masks for input events
//
// *key...
//
//    unicode key codes
//
// *isConnected
//
//    return true when the connection to the SCUM server is
//    established.
//
// *connect(completionFunc)
//
//    connect to the server and execute completionFunc on success.
//
// *disconnect
//
//    disconnect from the server.
//
//
// *do(function)
//
//    make sure there's a connection to the server and execute
//    function.
//
// *sendMsg(...msg)
//
//    send an OSC message to the server (see NetAddr).
//
// *sendMsg(time...args)
//
//    send an OSC bundle to the server (see NetAddr).
//
// ---------------------------------------------------------------------
// $Id$
// ---------------------------------------------------------------------
