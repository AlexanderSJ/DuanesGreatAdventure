#include "Frog.h"
#include "MainMenu.h"
#include "MainGame.h"
#include "MainUpdate.h"

using namespace Webfoot;

/// Which interface should be shown for this state.
#define GUI_LAYER_NAME "MainMenu"

MainMenu MainMenu::instance;

//-----------------------------------------------------------------------------

void MainMenu::Init()
{
   Inherited::Init();
   exitingGame = false;
}

//-----------------------------------------------------------------------------

void MainMenu::OnGUILayerInit(LayerWidget*)
{
   // Set up the GUI callbacks.
   PressButtonWidget::OnClickRegister(GUI_LAYER_NAME ".Play", OnPlayClick);
   PressButtonWidget::OnClickRegister(GUI_LAYER_NAME ".Exit", OnExitClick);
}

//-----------------------------------------------------------------------------

void MainMenu::Deinit()
{
   if(exitingGame)
      theMainUpdate->Exit();

   Inherited::Deinit();
}

//-----------------------------------------------------------------------------

const char* MainMenu::GUILayerNameGet()
{
   return GUI_LAYER_NAME;
}

//-----------------------------------------------------------------------------

void MainMenu::OnPlayClick(PressButtonWidget*, void*)
{
   if(!theStates->StateChangeCheck() && !theMainUpdate->ExitingCheck())
   {
      theMainMenu->StateChangeTransitionBegin(true);
      theStates->Push(theMainGame);
   }
}

//-----------------------------------------------------------------------------

void MainMenu::OnExitClick(PressButtonWidget*, void*)
{
   if(!theStates->StateChangeCheck() && !theMainUpdate->ExitingCheck())
   {
      theMainMenu->StateChangeTransitionBegin(true);
      theMainMenu->exitingGame = true;
      theStates->Pop();
   }
}

//-----------------------------------------------------------------------------
