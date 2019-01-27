#include "Frog.h"
#include "MainUpdate.h"
#include "MainMenu.h"

using namespace Webfoot;

MainUpdate MainUpdate::instance;
Cursor* cursor = NULL;
Font* font = NULL;

/// Game setup constants
#define GAME_CONSTS_FILE "Scripts/Consts"

JSONValue* Webfoot::theConsts;

//------------------------------------------------------------------------------
MainUpdate::MainUpdate()
{
   isExiting = false;
}
//------------------------------------------------------------------------------

void MainUpdate::ConstsInit()
{
   // Load constants that do not depend on the graphics path.
   JSONParser parser;
   theConsts = parser.Load(GAME_CONSTS_FILE);
}

//------------------------------------------------------------------------------

void MainUpdate::ConstsDeinit()
{
   SmartDeinitDelete(theConsts);
}

//------------------------------------------------------------------------------

void MainUpdate::Init()
{
   isExiting = false;
   theClock->LongLoopNotify();
   theText->Init();
   
   // Seed the random number generator.
   FrogMath::Seed1 = theClock->RandomSeedGet();
   FrogMath::Seed2 = FrogMath::Seed1;

   theSprites->Init();

   // Fade in from black
   theFades->Init();
   theFades->ColorSet(COLOR_RGBA8_BLACK);
   theFades->FadeIn();

   theAnimatedBackgrounds->Init();

#if PLATFORM_IS_WINDOWS || PLATFORM_IS_MACOSX
   // Only have a cursor on the PC and Mac
   cursor = frog_new Cursor();
   cursor->Init();
   cursor->ImageSet("Cursor");
   //cursor->VisibleSet(false);
#endif

   font = theFonts->Get("Arial");

   theGUI->Init();

   theStates->Init(theMainMenu);
}

//------------------------------------------------------------------------------

void MainUpdate::Deinit()
{
   theStates->Deinit();
   theGUI->Deinit();
   theSounds->MusicStop();
   theAnimatedBackgrounds->Deinit();
   SmartDeinitDelete(cursor);
   if(font)
   {
      theFonts->Unload(font);
      font = NULL;
   }
   theSprites->Deinit();
   theText->Deinit();
}

//------------------------------------------------------------------------------
// ALL PROGRAMMERS START HERE!
//
// Update is the lowest level platform-independent thing that's getting called 
// on every frame.  All the game-specific things are called from here.
// Ultimately, it's just a function that's called every frame.
// What a frame is, is yet to be defined...
//
// We can't guarantee we'll call it on every v-blank, it's just called for every 
// iteration of the main loop
//------------------------------------------------------------------------------
void MainUpdate::Update()
{
   unsigned int dt = theClock->LoopDurationGet();
   theStates->Update();

   theFades->Update(dt);
   theAnimatedBackgrounds->Update(dt);

   theGUI->Update(dt);
   theStates->StateUpdate();

   theScreen->PreDraw();

   // Draw background
   theAnimatedBackgrounds->Draw();
   theStates->StateDraw();
   theGUI->Draw();

   if(cursor)
      cursor->Draw();

   theFades->Draw();

   theScreen->PostDraw();
}

//------------------------------------------------------------------------------
