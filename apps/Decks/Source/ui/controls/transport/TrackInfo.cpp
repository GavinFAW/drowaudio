/*
 *  TrackInfo.cpp
 *  resampling_test
 *
 *  Created by David Rowland on 26/03/2009.
 *  Copyright 2009 UWE. All rights reserved.
 *
 */

#include "TrackInfo.h"

TrackInfo::TrackInfo(int deckNo_, FilteringAudioFilePlayer* filePlayer_)
	:	deckNo(deckNo_),
		filePlayer(filePlayer_),
		time(0.0),
		fileLength(0.0)
{
	filePlayer->addChangeListener(this);
	
	currentTime = timeToTimecodeStringLowRes(time);
	remainingTime = timeToTimecodeStringLowRes( fileLength - time );
	
	addAndMakeVisible(suggestButton = new TextButton("i"));
	suggestButton->addListener(this);
}

TrackInfo::~TrackInfo()
{
	filePlayer->removeChangeListener(this);

	String message("TrackInfo ");
	message << deckNo << " deleted";
	DBG(message);
	
	deleteAllChildren();
}

//=================================================
void TrackInfo::paint(Graphics &g)
{
	const int h = getHeight();
	const int w = getWidth();
	const int m = 1;
	const int timeWidth = 72;
//	const int fontSize = getHeight() * 0.5;//28;
	const int fontSize = (h * 0.5) - (2*m);

	g.fillAll(Colours::black);
	g.setColour(Colours::white);
	
	g.setFont(fontSize);
	g.drawText(String(currentTime),
			   w-timeWidth, m, timeWidth-(2*m), fontSize,
			   Justification::centredRight, true);
	
	String remainString("-");
	remainString << String(remainingTime);
	g.drawText(remainString,
			   w-timeWidth, fontSize + (2*m) +(m), timeWidth-(2*m), fontSize,
			   Justification::centredRight, true);

	g.setFont(fontSize);
//	g.drawRect(0, 0, 20, timeHeight+1, 1);
	g.drawFittedText(String(deckNo),
					 m, m, fontSize+(2*m), (h * 0.5) - (2*m),
					 Justification::centred, 2, 1);	
//	g.drawFittedText(fileName,
//					 fontSize + (4*m), m, w-timeWidth-fontSize-(8*m), h,
//					 Justification::topLeft, 2, 1);	
	g.drawFittedText(trackName,
					 fontSize + (4*m), m, w-timeWidth-fontSize-(8*m), h*0.5,
					 Justification::topLeft, 1, 1);	
	g.drawFittedText(artistName,
					 fontSize + (4*m), 2*m + h*0.5, w-timeWidth-fontSize-(8*m), h*0.5,
					 Justification::topLeft, 1, 1);	
	
	g.setColour(DecksColours::getInstance()->getColour(DecksColours::panelLineColour));
	g.drawRect(0, 0, w, h, 1);
	g.drawHorizontalLine(fontSize+(2*m), 0, w);
	g.drawVerticalLine(fontSize+(2*m), 0, h);
	g.drawVerticalLine(w-timeWidth-(2*m), 0, h);
}

void TrackInfo::resized()
{
	const int h = getHeight();
//	const int w = getWidth();
	const int m = 1;

	suggestButton->setBounds(2*m, h - ((h * 0.5) - (2*m)), (h * 0.5) - (3*m), (h * 0.5) - (3*m));
}

void TrackInfo::buttonClicked (Button* button)
{
	if(button == suggestButton)
	{
//		if (suggestButton->getToggleState()) {
		TrackSuggestions content(filePlayer->getLibraryEntry(), ITunesLibrary::getInstance()->getLibraryTree());
			content.setSize (300, 300);
			
			CallOutBox callOut (content, *suggestButton, getTopLevelComponent());
			callOut.runModalLoop();
//		}
//		else {
//			DBG("hide suggestions");
//		}
	}
}

void TrackInfo::changeListenerCallback(ChangeBroadcaster* changedObject)
{
	if (changedObject == filePlayer)
	{
		AudioFormatReaderSource* readerSource = filePlayer->getAudioFormatReaderSource();
		double sampleRate = 44100.0;
		if ( readerSource != 0)
			sampleRate = readerSource->getAudioFormatReader()->sampleRate;
		
		fileLength = filePlayer->getTotalLength() / sampleRate;
		fileName = filePlayer->getFileName();
		
		trackName = filePlayer->getLibraryEntry().getProperty(Columns::columnNames[Columns::Song]);
		artistName = filePlayer->getLibraryEntry().getProperty(Columns::columnNames[Columns::Artist]);

		if (trackName.isEmpty() && artistName.isEmpty())
			trackName = fileName;
		
		if (filePlayer->isPlaying())
			startTimer(100);
		else {
			stopTimer();
			timerCallback();
		}
	}
}

void TrackInfo::timerCallback()
{
	time = filePlayer->getCurrentPosition();
	
	currentTime = timeToTimecodeStringLowRes(time);
	remainingTime = timeToTimecodeStringLowRes( fileLength - time );
	
	repaint();
}