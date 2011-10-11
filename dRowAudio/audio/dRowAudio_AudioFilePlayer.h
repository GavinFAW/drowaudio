/*
 *  AudioFilePlayer.h
 *  dRowAudio
 *
 *	Derived from code by Martin Robinson 2008
 *  Created by David Rowland on 10/01/2009.
 *  Copyright 2009 dRowAudio. All rights reserved.
 *
 */

#ifndef _DROWAUDIO_AUDIOFILEPLAYER__H_
#define _DROWAUDIO_AUDIOFILEPLAYER__H_

#include "dRowAudio_SoundTouchAudioSource.h"
#include "dRowAudio_ReversibleAudioSource.h"
#include "dRowAudio_LoopingAudioSource.h"
class FilteringAudioSource;

//==============================================================================
/**
 This class can be used to load and play an audio file from disk.
 
 This combines the functionality of an AudioTransportSource, 
 AudioFormatReader and AudioFormatReaderSource.
 
 @see AudioTransportSource
 @see AudioFormatReader
 @see AudioFormatReaderSource
 */
class AudioFilePlayer	:	public PositionableAudioSource
{
public:
    //==============================================================================
	/** Creates an empty AudioFilePlayer.
     */
	AudioFilePlayer();
    
	/** Destructor.
     */
	~AudioFilePlayer();
	
    /** Sets the current library entry.
     */
    void setLibraryEntry (ValueTree newEntry)               {   libraryEntry = newEntry;    }

    /** Returns the currents library entry.
     */
    ValueTree getLibraryEntry()                             {   return  libraryEntry;       }
    
	/** Open and get ready to play a given audio file from an absolute path.
     */
	bool setFile (const String& path);
    
    /** Open and get ready to play a given audio file from a stream.
     */
	bool setInputStream (InputStream* inputStream);
    
	/** Returns the absolute path of the current audio file.
     */
	String getPath()                                        {	return filePath;                }
    
    //==============================================================================
    /** Changes the current playback position in the source stream. */
    void setPosition (double newPosition)       { audioTransportSource->setPosition (newPosition);  }
    
    /** Returns the position that the next data block will be read from in seconds. */
    double getCurrentPosition() const           { return audioTransportSource->getCurrentPosition();}
    
    /** Returns the stream's length in seconds. */
    double getLengthInSeconds() const           { return audioTransportSource->getLengthInSeconds();}
    
    /** Returns true if the player has stopped because its input stream ran out of data.
     */
    bool hasStreamFinished() const noexcept     { return audioTransportSource->hasStreamFinished(); }
    
    //==============================================================================
    /** Starts playing (if a source has been selected). */
    void start()                        {   audioTransportSource->start();          }
    
    /** Stops playing. */
    void stop()                         {   audioTransportSource->stop();           }
    
    /** Returns true if it's currently playing. */
    bool isPlaying() const noexcept     { return audioTransportSource->isPlaying(); }
    
    /** Play the audio file from the start. */
	void startFromZero();
	
	/** Pauses or plays the audio file. */
	void pause();
	    
    //==============================================================================
    /** Sets the resampling ratio to use also notifying any listeners. */
    void setResamplingRatio (const double samplesInPerOutputSample);
    
    /** Sets whether the source should play forwards or backwards.
     */
	void setPlayDirection (bool shouldPlayForwards)	{	soundTouchAudioSource->setPlayDirection (shouldPlayForwards);	}
    
    /** Returns true if the source is playing forwards.
     */
	bool getPlayDirection ()						{	return soundTouchAudioSource->getPlayDirection();	}
    
    //==============================================================================
    /** Sets the start and end times of the loop.
        This doesn't actually activate the loop, use setLoopBetweenTimes() to toggle this.
     */
	void setLoopTimes (double startTime, double endTime);
	
    /** Enables the loop point set.
     */
    void setLoopBetweenTimes (bool shouldLoop);
    
    /** Returns true if the loop is activated.
     */
    bool getLoopBetweenTimes()                      {   return loopingAudioSource->getLoopBetweenTimes();       }
    
    //==============================================================================
	/// Returns the AudioFormatReaderSource currently being used
	inline AudioFormatReaderSource* getAudioFormatReaderSource()   {   return audioFormatReaderSource; }
	
    /** Sets the AudioFormatManager to use.
     */
    void setAudioFormatManager (AudioFormatManager* newManager,  bool deleteWhenNotNeeded = true);
    
	/** Returns the AudioFormatManager being used.
     */
	inline AudioFormatManager* getAudioFormatManager()             {   return formatManager;           }
        
    /** Returns the AudioTransportSource being used.
     */
    inline AudioTransportSource* getAudioTransportSource()         {   return audioTransportSource;    }

    /** Returns the SoundTouchAudioSource being used.
     */
    inline SoundTouchAudioSource* getSoundTouchAudioSource()       {   return soundTouchAudioSource;   }

	/** Returns the FilteringAudioSource being used.
     */
    inline FilteringAudioSource* getFilteringAudioSource()         {   return filteringAudioSource;    }

    //==============================================================================
    /** A class for receiving callbacks from a FilteringAudioFilePlayer.
	 
	 To be told when a player's file changes, you can register a FilteringAudioFilePlayer::Listener
	 object using FilteringAudioFilePlayer::addListener().
	 
	 @see AudioFilePlayer::addListener, AudioFilePlayer::removeListener
	 */
    class  Listener
    {
    public:
        //==============================================================================
        /** Destructor. */
        virtual ~Listener() {}
		
        //==============================================================================
        /** Called when the player's file is changed.
         
            You can find out the new file path using AudioFilePlayer::getFilePath().
		 */
        virtual void fileChanged (AudioFilePlayer* player) = 0;
		
		/** Called when the player's resampling ratio is changed.
		 
            You can find out the new ratio using AudioFilePlayer::getSoundTouchAudioSource().
		 */
		virtual void resamplingRatioChanged (AudioFilePlayer* player) {};

        /** Called when the player's tempo ratio is changed.
		 
            You can find out the new ratio using AudioFilePlayer::getSoundTouchAudioSource().
		 */
		virtual void tempoRatioChanged (AudioFilePlayer* player) {};

        /** Called when the player's pitch ratio is changed.
		 
            You can find out the new ratio using AudioFilePlayer::getSoundTouchAudioSource().
		 */
		virtual void pitchRatioChanged (AudioFilePlayer* player) {};

        //==============================================================================
        /** Called when the the player is stopped or started.
         You can find out if it is currently stopped with FilteringAudioFilePlayer::isPlaying().
		 */
        //        virtual void playerStoppedOrStarted (FilteringAudioFilePlayer *player);
        
        virtual void loopBetweenTimesChanged (AudioFilePlayer* player) {};
    };
	
    /** Adds a listener to be called when this slider's value changes. */
    void addListener (Listener* listener);
	
    /** Removes a previously-registered listener. */
    void removeListener (Listener* listener);
	
    //==============================================================================
    /** Implementation of the AudioSource method. */
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate);
    
    /** Implementation of the AudioSource method. */
    void releaseResources();
    
    /** Implementation of the AudioSource method. */
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill);
    
    //==============================================================================
    /** Sets the next read position in samples.
     */
    void setNextReadPosition (int64 newPosition)
    {
        soundTouchAudioSource->setNextReadPosition (newPosition);
    }
    
    /** Returns the position from which the next block will be returned.
     */
    int64 getNextReadPosition() const   {   return soundTouchAudioSource->getNextReadPosition();    }
    
    /** Returns the total length of the stream (in samples). */
    int64 getTotalLength() const        {   return soundTouchAudioSource->getTotalLength(); }
    
    /** Returns true if this source is actually playing in a loop. */
    bool isLooping() const              {   return soundTouchAudioSource->isLooping();      }
    
    /** Tells the source whether you'd like it to play in a loop. */
    void setLooping (bool shouldLoop)   {   soundTouchAudioSource->setLooping (shouldLoop); }
    
private:	
    //==============================================================================
	TimeSliceThread bufferingTimeSliceThread;

	OptionalScopedPointer<AudioFormatManager> formatManager;

    AudioSource* masterSource;
    ScopedPointer<AudioFormatReaderSource> audioFormatReaderSource;
	ScopedPointer<AudioTransportSource> audioTransportSource;
    ScopedPointer<LoopingAudioSource> loopingAudioSource;
    ScopedPointer<SoundTouchAudioSource> soundTouchAudioSource;
    ScopedPointer<FilteringAudioSource> filteringAudioSource;

	String filePath;
    ValueTree libraryEntry;
    
    ListenerList <Listener> listeners;

	bool setSourceWithReader (AudioFormatReader* reader);
    
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioFilePlayer);
};

#endif //_DROWAUDIO_AUDIOFILEPLAYER__H_