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

    GriddleStep.cpp
    Created: 7 Mar 2020 2:40:18am
    Author:  Kevin Frank

  ==============================================================================
*/

#include <JuceHeader.h>
#include "GriddleStep.h"

//==============================================================================
GriddleStep::GriddleStep(int stepIndex, int ownerTrackIndex)
    : stepIndex_(stepIndex)
    , ownerTrackIndex_(ownerTrackIndex)
    , velocityLinePosY_(10.0f)
    , gateLinePosX_(30.0f)
    , backgroundColor_(Colours::lightslategrey)
    , midiNoteNumber_(-1)
    , gatePercent_(100)
    , velocity_(127)
    , midiNoteString_("")
    , drawChopped_(false)
    , drawFlipped_(false)
    , canSelect_(true)
{
    // The default size for a step is 40x95px
    setSize(40, 95);

    // Call the mutator methods to initialize the displayed indicators
    setNoteNumber(midiNoteNumber_);
    setVelocity(velocity_);
    setGatePercent(gatePercent_);
}

GriddleStep::~GriddleStep()
{
}

void GriddleStep::mouseDown(const MouseEvent& event)
{
    // Set the step as selected if the left mouse button was clicked on the step
    if (event.mouseWasClicked())
    {
        if (event.mods.isLeftButtonDown())
        {
            setStepSelected(true);
        }
    }
}

void GriddleStep::setStepSelected(const bool selected)
{
    // Only select the step if it can be selected
    if (selected && canSelect_)
    {
        // Highlight the step with a different background color since it's selected
        backgroundColor_ = Colours::lightskyblue.darker(0.3f);

        // Notify the registered ilsteners that the step has been selected
        callStepSelectedListeners();
    }
    else
    {
        // If the step is deselected, just revert the background color to the standard step color
        backgroundColor_ = Colours::lightslategrey;
    }
}

void GriddleStep::setNoteNumber(const int midiNoteNumber)
{
    // Update the midiNoteNumber_ member variable and the note number display string
    midiNoteNumber_ = midiNoteNumber;

    if (midiNoteNumber >= 0)
    {
        midiNoteString_ = MidiMessage::getMidiNoteName(midiNoteNumber_, true, true, 4);
    }
    else
    {
        // Nothing is displayed for a rest
        midiNoteString_ = "";
    }
}

void GriddleStep::setVelocity(const int velocity)
{
    // Update the velocity_ member variable and velocity display line position
    velocity_ = velocity;
    velocityLinePosY_ = static_cast<float>((getHeight() - ((velocity_/127.0) * getHeight())));
}

void GriddleStep::setGatePercent(const int gatePercent)
{
    // Update the gatePercent_ member variable and gate percent display line position
    gatePercent_ = gatePercent;
    gateLinePosX_ = static_cast<float>(((gatePercent_/100.0) * getWidth()));
}

void GriddleStep::addListener(GriddleStep::Listener* l) 
{
    listeners.add(l); 
}

void GriddleStep::removeListener(GriddleStep::Listener* l) 
{ 
    listeners.remove(l); 
}

void GriddleStep::callStepSelectedListeners()
{
    Component::BailOutChecker checker(this);
    listeners.callChecked(checker, [this](Listener& l) { l.stepSelected(this); });
}

void GriddleStep::paint(Graphics& g)
{
    g.fillAll(backgroundColor_);

    // Draw the chopped indicator if applicable
    if (drawChopped_)
    {
        g.setColour(backgroundColor_.darker(0.2f));
        Path chopTriangle;
        if (! drawFlipped_)
            chopTriangle.addTriangle(0.0f, static_cast<float>(getHeight()), static_cast<float>(getWidth()), 0.0f, static_cast<float>(getWidth()), static_cast<float>(getHeight()));
        else
            chopTriangle.addTriangle(0.0f, static_cast<float>(getHeight()), static_cast<float>(getWidth()), 0.0f, 0.0f, 0.0f);
        g.fillPath(chopTriangle);
    }

    // Draw the velocity Line
    g.setColour(Colours::darkslategrey);
    g.setOpacity(0.5f);

    Line<float> vLine(Point<float>(0, velocityLinePosY_),
        Point<float>(static_cast<float>(getWidth()), velocityLinePosY_));

    g.drawLine(vLine, 3.0f);

    // Draw the gate Line
    g.setColour(Colours::darkslategrey);
    g.setOpacity(0.5f);

    auto gateLineDrawPosX = gateLinePosX_;
    if (drawFlipped_)
        gateLineDrawPosX = static_cast<float>(getWidth()) - gateLinePosX_;

    Line<float> gLine(Point<float>(gateLineDrawPosX, 0),
        Point<float>(gateLineDrawPosX, static_cast<float>(getHeight())));

    g.drawLine(gLine, 3.0f);
    
    // Draw the note string
    g.setFont(Font(20.0, Font::bold | Font::italic));
    g.setColour(Colours::lightgrey);
    g.setOpacity(1.0f);

    // Rotate and invert the text if the step should be drawn flipped
    Justification textJustify = Justification::bottomLeft;
    if (drawFlipped_)
    {
        g.addTransform(AffineTransform::rotation(MathConstants<float>::pi, static_cast<float>(getWidth()/2), static_cast<float>(getHeight()/2)));
        g.addTransform(AffineTransform::verticalFlip(static_cast<float>(getHeight())));
    }
    g.drawFittedText(midiNoteString_, Rectangle<int>(2, 2, getWidth() - 6, getHeight() - 1), textJustify, 1);
}

void GriddleStep::resized()
{
    // GriddleStep component can currently only be resized automatically when then number of steps for the owner track is changed

    // Scale the velocity and gate lines to the new size
    velocityLinePosY_ = static_cast<float>((getHeight() - ((velocity_ / 127.0) * getHeight())));
    gateLinePosX_ = static_cast<float>(((gatePercent_ / 100.0) * getWidth()));

}
