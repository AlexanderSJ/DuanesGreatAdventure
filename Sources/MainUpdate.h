#ifndef __MAINUPDATE_H__
#define __MAINUPDATE_H__

#include "Frog.h"

namespace Webfoot {

/// Misc constants
extern JSONValue* theConsts;

/// Key for the text to use for the window title and taskbar icon.
#define WINDOW_TITLE_KEY "WindowTitle"

/// Misc constants
extern JSONValue* theConsts;

//==============================================================================

/// MainUpdate is the lowest platform-independent point that is called for
/// initialization, deinitialization, and updates.  Each call to Update
/// should correspond to one frame.
class MainUpdate
{
public:
   /// Called before Init to load constants from JSON that do not depend on
   /// the graphics path.
   void ConstsInit();
   /// Called after Deinit to clean up constants loaded from JSON.
   void ConstsDeinit();

   MainUpdate();
   
   /// Lowest platform-independent point to be called for initialization.
   void Init();
   /// Lowest platform-independent point to be called for cleanup.
   void Deinit();
   /// Update is the lowest platform-independent point that is called on every
   /// iteration of the main loop.  The true main loop is platform-dependent.
   void Update();
   
   /// Call this to signal that the main loop should be terminated.
   void Exit() { isExiting = true; }
   /// The platform-specific main loop should check this to see if it should stop looping. 
   bool ExitingCheck() { return isExiting; }

   static MainUpdate instance;

protected:
   /// True if the main loop should stop looping.
   bool isExiting;
};

static MainUpdate * const theMainUpdate = &MainUpdate::instance;

//==============================================================================

} //namespace Webfoot {

#endif //#ifndef __MAINUPDATE_H__
