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

    GriddleStep.h
    Created: 7 Mar 2020 2:40:18am
    Author:  Kevin Frank

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*  
    This component contains all of the attributes and logic for a step in a 
    track of a Griddle sequence.

    A GriddleStep can hold a rest or a monophonic MIDI note with velocity and
    gate percent, all of which have graphical indicators.
*/
class GriddleStep : public Component
{
public:
    //==============================================================================
    GriddleStep(int stepIndex, int ownerTrackIndex);
    ~GriddleStep();
    //==============================================================================
    void paint(Graphics&) override;
    void resized() override;

    /** Handles mouse down events for selecting the step with a mouse click */
    void mouseDown(const MouseEvent& event) override;

    /** Sets the MIDI note number for the step

        @param midiNoteNumber    The MIDI note number to be played for the step.
                                 A value of -1 indicates this step is a rest.
    */
    void setNoteNumber(const int midiNoteNumber);

    /** Sets the MIDI velocity for the step

        @param velocity    The velocity value to use for the step.
    */
    void setVelocity(const int velocity);

    /** Sets the gate percent for the step

        @param velocity    The gate percent to use for the step.
    */
    void setGatePercent(const int gatePercent);

    /** Selects or deselects the step for editing

        @param selected    Pass true to select the step for editing or pass false to deselect the step.
    */
    void setStepSelected(const bool selected);

    /** Sets whether the step is selectable or not

        @param canSelect    Pass true to allow the step to be edited or pass false to disallow selection of the step.
    */
    void setCanSelect(const bool canSelect);

    /** Gets the current MIDI note number for the step

        @returns    The MIDI note number for the step (-1 if it's a rest).
    */
    int getNoteNumber() const;

    /** Gets the current MIDI velocity for the step

        @returns    The MIDI note number for the step (-1 if it's a rest).
    */
    int getVelocity() const;

    /** Gets the current gate percent for the step

        @returns    The gate percent for the step.
    */
    int getGatePercent() const;

    /** Gets the index of this step in its owner track's step list

        @returns    the index of this step in its owner track's step list
    */
    int getStepIndex() const;

    /** Gets the index of this step's owner track in the MainComponent's track list

        @returns    the index of this step's owner track in the MainComponent's track list
    */
    int getOwnerTrackIndex() const;

    /** Sets whether the step should be drawn flipped or not

        A step that is drawn flipped inverts the position of the gate line and shows the
        MIDI note number text backwards

        @param flipped    Pass true to draw the step flipped, otherwise pass false
    */
    void setFlipDrawState(const bool flipped);

    /** Sets whether the step should be drawn chopped or not

        A step that is drawn chopped has the background chopped in half diagonally
        with a slightly background color on one side of the diagonal "chop" line

        @param chopped    Pass true to draw the step chopped, otherwise pass false
    */
    void setChopDrawState(const bool chopped);

    //==============================================================================
/**
    A class for receiving events from a GriddleStep.

    @see GriddleStep::addListener, GriddleStep::removeListener
*/
    class JUCE_API  Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener() = default;

        /** Called when a GriddleStep has been selected. */
        virtual void stepSelected(GriddleStep* selectedStep) = 0;
    };

    /** Registers a listener that will be called when the step is selected. */
    void addListener(Listener* listener);

    /** Deregisters a previously-registered listener. */
    void removeListener(Listener* listener);
    
private:
    //==============================================================================
    // Core Member Variables
    int stepIndex_;
    int ownerTrackIndex_;
    bool canSelect_;
    int midiNoteNumber_;
    int gatePercent_;
    int velocity_;
    //==============================================================================

    //==============================================================================
    // Drawing Functionality Member Variables
    bool drawChopped_;
    bool drawFlipped_;
    String midiNoteString_;
    float velocityLinePosY_;
    float gateLinePosX_;
    Colour backgroundColor_;
    //==============================================================================

    // List of listeners registered to be notified when the step is selected
    ListenerList<Listener> listeners;

    /** Calls stepSelecetd method for all registered listeners */
    void callStepSelectedListeners();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GriddleStep)
};

inline int GriddleStep::getNoteNumber() const
{
    return midiNoteNumber_;
}

inline int GriddleStep::getVelocity() const
{
    return velocity_;
}

inline int GriddleStep::getGatePercent() const
{
    return gatePercent_;
}

inline int GriddleStep::getStepIndex() const
{
    return stepIndex_;
}

inline int GriddleStep::getOwnerTrackIndex() const
{
    return ownerTrackIndex_;
}

inline void GriddleStep::setCanSelect(const bool canSelect)
{
    canSelect_ = canSelect;
}

inline void GriddleStep::setFlipDrawState(const bool flipped)
{
    drawFlipped_ = flipped;
}

inline void GriddleStep::setChopDrawState(const bool chopped)
{
    drawChopped_ = chopped;
}