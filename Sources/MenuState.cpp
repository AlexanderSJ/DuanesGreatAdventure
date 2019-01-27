#include "Frog.h"
#include "MenuState.h"
#include "MainUpdate.h"

using namespace Webfoot;

/// Name of the transition to play when initializing
#define INIT_TRANSITION_NAME "Enter"
/// Name of the transition to play when deinitializing
#define DEINIT_TRANSITION_NAME "Exit"

//-----------------------------------------------------------------------------

void MenuState::Init()
{
   waitingForExitTransition = false;
   waitingForFadeOut = false;
   shouldFadeOut = false;

   // Load the menu.
   theGUI->Push(GUILayerNameGet(), _OnGUILayerInit, (void*)this);

   // Fade in, if needed.
   if(!theFades->FadeActiveCheck() && theFades->FadedCheck())
      theFades->FadeIn();

   // Tell the widgets to play their enter transition.
   theGUI->TransitionBegin(INIT_TRANSITION_NAME);
}

//-----------------------------------------------------------------------------

void MenuState::Deinit()
{
   theGUI->Pop();
}

//-----------------------------------------------------------------------------

void MenuState::Update()
{
   if(waitingForExitTransition && !theGUI->TransitioningCheck())
   {
      // When the gui transition is complete, fade out.
      waitingForExitTransition = false;
      if(shouldFadeOut)
         theFades->FadeToBlack();
      waitingForFadeOut = true;
   }

   if(waitingForFadeOut && !theFades->FadeActiveCheck())
   {
      // When the fade-out is complete, change states.
      waitingForFadeOut = false;
      theStates->StateChangeContinue();
   }
}

//-----------------------------------------------------------------------------

void MenuState::StateChangeTransitionBegin(bool fadeOut)
{
   if(waitingForExitTransition)
      return;
   waitingForExitTransition = true;
   shouldFadeOut = fadeOut;

   // Don't actually change state until the transitions are over.
   theStates->StateChangeWait();

   // Tell the widgets to play their exit transition.
   theGUI->TransitionBegin(DEINIT_TRANSITION_NAME);
}

//-----------------------------------------------------------------------------

void MenuState::_OnGUILayerInit(LayerWidget* layer, void* userData)
{
   MenuState* menuState = (MenuState*)userData;
   if(menuState)
      menuState->OnGUILayerInit(layer);
}

//-----------------------------------------------------------------------------
