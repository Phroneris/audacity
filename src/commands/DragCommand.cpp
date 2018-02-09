/**********************************************************************

   Audacity - A Digital Audio Editor
   Copyright 1999-2018 Audacity Team
   License: wxwidgets

   James Crook

******************************************************************//**

\file DragCommand.cpp
\brief Definitions for DragCommand

\class DragCommand
\brief Command that sets clip information

*//*******************************************************************/

#include "../Audacity.h"
#include "DragCommand.h"
#include "../Project.h"
#include "../Track.h"
#include "../TrackPanel.h"
#include "../WaveTrack.h"
#include "../ShuttleGui.h"
#include "CommandContext.h"

DragCommand::DragCommand()
{
}

enum kCoordTypes
{
   kPanel,
   kApp,
   kTrack0,
   kTrack1,
   nCoordTypes
};

static const wxString kCoordTypeStrings[nCoordTypes] =
{
   XO("Panel"),
   XO("App"),
   XO("Track0"),
   XO("Track1"),
};


bool DragCommand::DefineParams( ShuttleParams & S ){ 
   wxArrayString coords( nCoordTypes, kCoordTypeStrings );
   S.OptionalN( bHasId         ).Define(     mId,          wxT("Id"),         11000.0, -100000.0, 1000000.0);
   S.OptionalY( bHasWinName    ).Define(     mWinName,     wxT("Window"),     "Timeline");
   S.OptionalY( bHasFromX      ).Define(     mFromX,       wxT("FromX"),      200.0, 0.0, 1000000.0);
   S.OptionalY( bHasFromY      ).Define(     mFromY,       wxT("FromY"),      10.0,  0.0, 1000000.0);
   S.OptionalN( bHasToX        ).Define(     mToX,         wxT("ToX"),        400.0, 0.0, 1000000.0);
   S.OptionalN( bHasToY        ).Define(     mToY,         wxT("ToY"),        10.0,  0.0, 1000000.0);
   S.OptionalN( bHasRelativeTo ).DefineEnum( mRelativeTo,  wxT("RelativeTo"), kPanel, coords );
   return true;
};

void DragCommand::PopulateOrExchange(ShuttleGui & S)
{
   wxArrayString coords( nCoordTypes, kCoordTypeStrings );

   S.AddSpace(0, 5);

   S.StartMultiColumn(3, wxALIGN_CENTER);
   {
      S.Optional( bHasId         ).TieNumericTextBox(  _("Id:"),          mId );
      S.Optional( bHasWinName    ).TieTextBox(         _("Window Name:"), mWinName );
      S.Optional( bHasFromX      ).TieNumericTextBox(  _("From X:"),      mFromX );
      S.Optional( bHasFromY      ).TieNumericTextBox(  _("From Y:"),      mFromY );
      S.Optional( bHasToX        ).TieNumericTextBox(  _("To X:"),        mToX );
      S.Optional( bHasToY        ).TieNumericTextBox(  _("To Y:"),        mToY );
      S.Optional( bHasRelativeTo ).TieChoice(          _("Relative To:"), mRelativeTo, &coords );
   }
   S.EndMultiColumn();
}

bool DragCommand::Apply(const CommandContext & context)
{
   wxWindow * pWin = context.GetProject();
   wxWindow * pWin1 = nullptr;
   wxMouseEvent Evt( wxEVT_MOTION );
   Evt.m_x = mFromX;
   Evt.m_y = mFromY;
   if( bHasId )
      pWin1 = pWin->FindWindowById( mId );
   if( bHasWinName )
      pWin1 = pWin->FindWindowByName( mWinName );
   if( pWin1 )
      pWin = pWin1;
   // Process twice - possible bug in Audacity being worked around
   // where we need an event to enter AND an event to move.
   // AdornedRuler Quick-Play bug.
   pWin->GetEventHandler()->ProcessEvent( Evt );
   pWin->GetEventHandler()->ProcessEvent( Evt );
   if( bHasToX ){
      wxMouseEvent Evt2( wxEVT_LEFT_DOWN );
      Evt2.m_leftDown = true;
      Evt2.m_x = mFromX;
      Evt2.m_y = mFromY;
      Evt2.m_aux2Down = true;
      pWin->GetEventHandler()->ProcessEvent( Evt2 );
      wxMouseEvent Evt3( wxEVT_MOTION );
      Evt3.m_leftDown = true;
      Evt2.m_aux2Down = true;
      Evt3.m_x = mToX;
      Evt3.m_y = mToY;
      // AdornedRuler Quick-Play bug again.
      pWin->GetEventHandler()->ProcessEvent( Evt3 );
      pWin->GetEventHandler()->ProcessEvent( Evt3 );
      wxMouseEvent Evt4( wxEVT_LEFT_UP );
      Evt2.m_aux2Down = true;
      Evt4.m_x = mToX;
      Evt4.m_y = mToY;
      pWin->GetEventHandler()->ProcessEvent( Evt4 );
   }
   return true;
}