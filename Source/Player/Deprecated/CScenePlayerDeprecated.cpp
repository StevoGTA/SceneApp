//
//  CScenePlayerDeprecated.cpp
//  StevoBrock-Core
//
//  Created by Stevo on 9/20/19.
//

#include "CSceneAppX.h"
#include "CSceneAppPlayerX.h"
#include "CSceneAppMoviePlayer.h"


CURLX eGetURLForResourceFilename(const CString& resourceFilename) { return CURLX(CString::mEmpty); }
void CSceneAppPlayerX::removePeriodic(void** periodicReference) {}
void CSceneAppPlayerX::installPeriodic(void** periodicReference) {}

S2DSize32 eSceneAppViewportPixelSize = S2DSize32::mZero;

CSceneAppMoviePlayer::CSceneAppMoviePlayer(const CURLX& URL) {}
CSceneAppMoviePlayer::~CSceneAppMoviePlayer() {}
void CSceneAppMoviePlayer::setControlMode(ESceneAppMoviePlayerControlMode controlMode) {}
void CSceneAppMoviePlayer::play() {}
void CSceneAppMoviePlayer::stop() {}

void CSceneAppMoviePlayer::setScreenRect(const S2DRect32& screenRect) {}
void CSceneAppMoviePlayer::setBackgroundInImageURL(const CURLX& inImageURL) {}
void CSceneAppMoviePlayer::setSceneAppMoviePlayerProcsInfo(const SSceneAppMoviePlayerProcsInfo& sceneAppMoviePlayerProcsInfo) {}
UError CSceneAppMoviePlayer::handleEvent(CEvent& event) { return kNoError; }
