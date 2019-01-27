#ifndef __MAINMENU_H__
#define __MAINMENU_H__

#include "Frog.h"
#include "MenuState.h"

namespace Webfoot {

//==============================================================================

/// First menu that the player typically sees.
class MainMenu : public MenuState
{
public:
   typedef MenuState Inherited;

   virtual ~MainMenu() {}
   
   virtual void Init();
   virtual void OnGUILayerInit(LayerWidget* layer);
   virtual void Deinit();

   static MainMenu instance;


protected:
   virtual const char* GUILayerNameGet();

   /// Called when the Play button is clicked.
   static void OnPlayClick(PressButtonWidget*, void*);
   /// Called when the Exit button is clicked.
   static void OnExitClick(PressButtonWidget*, void*);

   /// True if the Exit button was clicked.
   bool exitingGame;
};

MainMenu* const theMainMenu = &MainMenu::instance;

//==============================================================================

} //namespace Webfoot {

#endif //#ifndef __MAINMENU_H__
