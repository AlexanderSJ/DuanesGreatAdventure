#ifndef __MENUSTATE_H__
#define __MENUSTATE_H__

#include "Frog.h"

namespace Webfoot {

//==============================================================================

/// Base class to use for general menus that use the Gui system.
class MenuState : public GameState
{
public:
   virtual ~MenuState() {}
   
   virtual void Init();
   /// Called automatically after the menu's GUILayer is created or recreated.
   /// Use this to set up event handlers, populate lists, etc.
   virtual void OnGUILayerInit(LayerWidget* layer) {}
   virtual void Deinit();

   virtual void Update();

protected:
   /// Begin the transition that precedes a state change.
   void StateChangeTransitionBegin(bool fadeOut);

   /// Return the name of the Gui interface for this menu.
   virtual const char* GUILayerNameGet() = 0;
   /// Callback for the GUILayer's OnInit event.
   static void _OnGUILayerInit(LayerWidget* layer, void* userData);

   bool waitingForExitTransition;
   bool waitingForFadeOut;
   bool shouldFadeOut;
};

//==============================================================================

} //namespace Webfoot {

#endif //#ifndef __MENUSTATE_H__
