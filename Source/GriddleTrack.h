/*
==============================================================================

Copyright 2020 Kevin Frank

This file is part of Griddle.

Griddle is free software : you can redistribute it and /or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Griddle is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Griddle. If not, see < https://www.gnu.org/licenses/>.

==============================================================================
*/

/*
  ==============================================================================

    GriddleTrack.h
    Created: 7 Mar 2020 2:40:00am
    Author:  Kevin Frank

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include <array>
#include "GriddleStep.h"

//==============================================================================
/*
    This component contains all of the attributes and logic for a track in
    a Griddle sequence.

    A GriddleTrack has a series of monophonic steps and additional attributes
    detailing how the steps in the track should be played back.
*/
class GriddleTrack : public Component
{
public:
    //==============================================================================
    GriddleTrack(int trackIndex);
    ~GriddleTrack();
    //==============================================================================
    void paint(Graphics&) override;
    void resized() override;
    
    /** Passes through registration of a Listener for each of the GriddleSteps in the track

        @param listener    Pointer to the listener object

    */
    void addStepsListener(GriddleStep::Listener* listener);

    /** Sets the title string for the track

        @param title    The string to use for the track title

    */
    void setTitle(const juce::String& title);

    /** Sets the step at the passed-in index to be the currently-selected step for editing

        @param index    Index of the step to be selected

    */
    void setStepSelected(const int index);

    /** Gets a reference object for the step at the passed-in index

        @param index    Index of the step to return
        @returns        A reference object for the specified GriddleStep

    */
    const GriddleStep& getStep(const int index) const;

    /** Gets the current number of steps for the track

        @returns    The current number of steps for the track

    */
    int getNumSteps() const;

    /** Gets the current MIDI channel for the track

        @returns    The current MIDI channel for the track

    */
    int getMidiChannel() const;

    /** Gets the boolean indicator for whether the track is currently active or not

        A track only plays back if it is active, and can have its steps edited when it is inactive
        while the sequence is playing.

        @param toDrawValue    Pass true to get back the active state to be drawn for the track,
                              or pass false to get back the current absolute active state
        @returns              Active flag for the track, per the toDrawValue parameter

    */
    bool isActive(const bool toDrawValue = false) const;

    /** Gets the boolean indicator for whether the track is currently flipped or not

        A flipped track has its steps played back in reverse order.

        @param toDrawValue    Pass true to get back the flipped state to be drawn for the track,
                              or pass false to get back the current absolute flipped state
        @returns              Flipped flag for the track, per the toDrawValue parameter

    */
    bool isFlipped(const bool toDrawValue = false) const;

    /** Gets the boolean indicator for whether the track is currently chopped or not

        A chopped track has the gate percent for all of its steps overridden to 5% during playback.

        @param toDrawValue    Pass true to get back the chopped state to be drawn for the track,
                              or pass false to get back the current absolute chopped state
        @returns              Chopped flag for the track, per the toDrawValue parameter

    */
    bool isChopped(const bool toDrawValue = false) const;

    /** Gets the boolean indicator for whether the track is currently burnt or not

        A burnt track has its steps played back at double the master tempo.

        @param toDrawValue    Pass true to get back the burnt state to be drawn for the track,
                              or pass false to get back the current absolute burnt state
        @returns              Burnt flag for the track, per the toDrawValue parameter

    */
    bool isBurnt(const bool toDrawValue = false) const;

    /** Gets the tempo multiplier for the track (currently based only on the burnt state)

        @returns    2 if the track is currently burnt, otherwise 1

    */
    int getTempoMultiplier() const;
    
    /** Applies any pending track characteristic changes, updating the draw state variables and other elements

        @param isPlaying    Pass true if the sequence is currently playing, otherwise pass false. 

    */
    void applyPendingChanges(const bool isPlaying);

    /** Loads all of the track characteristics into the track from JSON data

        @params projectTrackData    The var containing the JSON track settings from a project file

        @returns                    A string containing any error conditions encountered when unpacking the settings

    */
    const String loadProjectTrackData(const var& projectTrackData);

    /** Gets the track data in a var that can be written in JSON format to a project file

        @returns A var containing the track settings that can be writte in JSON format to a project file

    */
    const var& getProjectTrackData();

    /** A lambda can be assigned to this callback object to have it called when the characteristics of the track change */
    std::function<void()> onTrackCharacteristicsChanged;

private:
    
    //==============================================================================
    // Core Member Variables
    std::array<std::shared_ptr<GriddleStep>, 16> steps_;
    bool isPlaying_;
    int trackIndex_;
    var projectTrackDataVar_;
    //==============================================================================

    //==============================================================================
    // Draw State Variables
    //
    // Since during playback, track attributes can be changed, but the associated 
    // indicator doesn't change until the start of the next measure, these variables
    // hold the attribute states to be drawn and the respective controls hold the 
    // current/pending settings
    int numStepsToDraw_;
    bool activeStateToDraw_;
    bool flippedStateToDraw_;
    bool choppedStateToDraw_;
    bool burntStateToDraw_;
    //==============================================================================

    //==============================================================================
    // GUI Components
    Label trackTitleLabel_;
    ImageButton activeToggle_;
    ComboBox midiChannelComboBox_;
    Label midiChannelLabel_;
    ComboBox numStepsComboBox_;
    Label numStepsLabel_;
    ImageButton flipToggle_;
    ImageButton chopToggle_;
    ImageButton burnToggle_;
    Label flipLabel_;
    Label chopLabel_;
    Label burnLabel_;
    //==============================================================================
    
    //==============================================================================
    // Private Member Methods

    /** Callback to handle processing when the selected item in the MIDI Channel ComboBox changes */
    void updateMidiChannel();

    /** Handles processing to change the current number of steps for the track 

        @param notifyTrackChanged    Pass true to have the method notify listeners of
                                     changes to the track or pass false to prohibit
                                     notification
    
    */
    void updateNumSteps(const bool notifyTrackChanged = true);

    /** Handles processing to change the active state of the track

        @param notifyTrackChanged    Pass true to have the method notify listeners of
                                     changes to the track or pass false to prohibit
                                     notification

    */
    void updateTrackActiveState(const bool notifyTrackChanged = true);

    /** Handles processing to change the flipped state for the track

        @param notifyTrackChanged    Pass true to have the method notify listeners of
                                     changes to the track or pass false to prohibit
                                     notification

    */
    void updateFlippedState(const bool notifyTrackChanged = true);

    /** Handles processing to change the chopped state for the track

        @param notifyTrackChanged    Pass true to have the method notify listeners of
                                     changes to the track or pass false to prohibit
                                     notification

    */
    void updateChoppedState(const bool notifyTrackChanged = true);

    /** Handles processing to change the burnt state for the track

        @param notifyTrackChanged    Pass true to have the method notify listeners of
                                     changes to the track or pass false to prohibit
                                     notification

    */
    void updateBurntState(const bool notifyTrackChanged = true);

    /** Calls lambda functions registered for onTrackCharacteristicsChanged  */
    void callTrackCharacteristicsChangedCallbacks();

    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GriddleTrack)
};

inline void GriddleTrack::setTitle(const juce::String& title)
{
    trackTitleLabel_.setText(title, dontSendNotification);
};

inline int GriddleTrack::getNumSteps() const
{
    return (numStepsComboBox_.getSelectedItemIndex() + 1);
}

inline int GriddleTrack::getMidiChannel() const
{
    return (midiChannelComboBox_.getSelectedItemIndex() + 1);
}

inline const GriddleStep& GriddleTrack::getStep(int index) const
{
    return (*steps_[index]);
}