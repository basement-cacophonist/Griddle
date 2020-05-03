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

    GriddleTrack.cpp
    Created: 7 Mar 2020 2:40:00am
    Author:  Kevin Frank

  ==============================================================================
*/

#include <JuceHeader.h>
#include "GriddleTrack.h"

//==============================================================================
GriddleTrack::GriddleTrack(int trackIndex)
    : steps_{ { std::shared_ptr<GriddleStep>(new GriddleStep(0,trackIndex)),
                std::shared_ptr<GriddleStep>(new GriddleStep(1,trackIndex)),
                std::shared_ptr<GriddleStep>(new GriddleStep(2,trackIndex)),
                std::shared_ptr<GriddleStep>(new GriddleStep(3,trackIndex)),
                std::shared_ptr<GriddleStep>(new GriddleStep(4,trackIndex)),
                std::shared_ptr<GriddleStep>(new GriddleStep(5,trackIndex)),
                std::shared_ptr<GriddleStep>(new GriddleStep(6,trackIndex)),
                std::shared_ptr<GriddleStep>(new GriddleStep(7,trackIndex)),
                std::shared_ptr<GriddleStep>(new GriddleStep(8,trackIndex)),
                std::shared_ptr<GriddleStep>(new GriddleStep(9,trackIndex)),
                std::shared_ptr<GriddleStep>(new GriddleStep(10,trackIndex)),
                std::shared_ptr<GriddleStep>(new GriddleStep(11,trackIndex)),
                std::shared_ptr<GriddleStep>(new GriddleStep(12,trackIndex)),
                std::shared_ptr<GriddleStep>(new GriddleStep(13,trackIndex)),
                std::shared_ptr<GriddleStep>(new GriddleStep(14,trackIndex)),
                std::shared_ptr<GriddleStep>(new GriddleStep(15,trackIndex))
              }}
    , trackIndex_(trackIndex)
    , isPlaying_(false)
    , numStepsToDraw_(16)
    , activeStateToDraw_(true)
    , flippedStateToDraw_(false)
    , choppedStateToDraw_(false)
    , burntStateToDraw_(false)
    , flipToggle_("FLIP")
    , chopToggle_("CHOP")
    , burnToggle_("BURN")
    , activeToggle_("ACTIVE")
    , projectTrackDataVar_(new DynamicObject())
{
    setSize(1200, 95);

    // Track Title Label
    addAndMakeVisible(trackTitleLabel_);
    trackTitleLabel_.setTopLeftPosition(20, 5);
    trackTitleLabel_.setSize(120, 80);
    trackTitleLabel_.setJustificationType(Justification::centredLeft);
    trackTitleLabel_.setText("A", dontSendNotification);
    trackTitleLabel_.setAlpha(0.5f);
    trackTitleLabel_.setFont(Font(110.0f, Font::italic | Font::bold));

    // Track Active Toggle
    addAndMakeVisible(activeToggle_);
    activeToggle_.setSize(30, 30);
    activeToggle_.setTopLeftPosition(10, 30);
    activeToggle_.setImages(false, true, true, ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey.darker(0.2f),
        ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey,
        ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey);
    activeToggle_.setClickingTogglesState(true);
    activeToggle_.onClick = [this] { updateTrackActiveState(); };
    activeToggle_.setToggleState(true, dontSendNotification);

    // GriddleSteps
    for (auto i = 0; i < steps_.size(); ++i)
    {
        auto rightMargin = 5;
        steps_[i]->setTopLeftPosition(240 + (i * steps_[i]->getWidth()) + (i * rightMargin), 0);
        addAndMakeVisible(*steps_[i]);
    }

    // Flip Toggle
    addAndMakeVisible(flipToggle_);
    flipToggle_.setTopLeftPosition(980, 23);
    flipToggle_.setSize(40, 40);
    flipToggle_.setImages(false, true, true, ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey.darker(0.2f),
        ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey,
        ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey);
    flipToggle_.setClickingTogglesState(true);
    flipToggle_.onClick = [this] { updateFlippedState(); };

    // Flip Label
    addAndMakeVisible(flipLabel_);
    flipLabel_.setSize(80, 20);
    flipLabel_.setTopLeftPosition(960, 70);
    flipLabel_.setText("FLIP", dontSendNotification);
    flipLabel_.setJustificationType(Justification::centred);
    flipLabel_.setFont(Font(16.0f, Font::italic | Font::bold));

    // Chop Toggle
    addAndMakeVisible(chopToggle_);
    chopToggle_.setTopLeftPosition(1060, 23);
    chopToggle_.setSize(40, 40);
    chopToggle_.setImages(false, true, true, ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey.darker(0.2f),
        ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey,
        ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey);
    chopToggle_.setClickingTogglesState(true);
    chopToggle_.onClick = [this] { updateChoppedState(); };

    // Chop Label
    addAndMakeVisible(chopLabel_);
    chopLabel_.setSize(80, 20);
    chopLabel_.setTopLeftPosition(1040, 70);
    chopLabel_.setText("CHOP", dontSendNotification);
    chopLabel_.setJustificationType(Justification::centred);
    chopLabel_.setFont(Font(16.0f, Font::italic | Font::bold));

    // Burn Toggle
    addAndMakeVisible(burnToggle_);
    burnToggle_.setTopLeftPosition(1140, 23);
    burnToggle_.setSize(40, 40);
    burnToggle_.setImages(false, true, true, ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey.darker(0.2f),
        ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey,
        ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey);
    burnToggle_.setClickingTogglesState(true);
    burnToggle_.onClick = [this] { updateBurntState(); };

    // Burn Label
    burnLabel_.setSize(80, 20);
    burnLabel_.setTopLeftPosition(1120, 70);
    addAndMakeVisible(burnLabel_);
    burnLabel_.setText("BURN", dontSendNotification);
    burnLabel_.setJustificationType(Justification::centred);
    burnLabel_.setFont(Font(16.0f, Font::italic | Font::bold));

    // MIDI Channel ComboBox and Label
    addAndMakeVisible(midiChannelComboBox_);
    midiChannelComboBox_.setTopLeftPosition(170, 15);
    midiChannelComboBox_.setSize(60, 25);
    for (auto i = 1; i <= 16; ++i)
    {
        midiChannelComboBox_.addItem(std::to_string(i), i);
    }
    midiChannelComboBox_.onChange = [this] { updateMidiChannel(); };
    midiChannelComboBox_.setSelectedId(1, dontSendNotification);

    addAndMakeVisible(midiChannelLabel_);
    midiChannelLabel_.attachToComponent(&midiChannelComboBox_, true);
    midiChannelLabel_.setFont(Font(16.0f, Font::italic | Font::bold));
    midiChannelLabel_.setText("MIDI CH", dontSendNotification);

    // # Steps ComboBox and Label
    addAndMakeVisible(numStepsComboBox_);
    numStepsComboBox_.setTopLeftPosition(170, 55);
    numStepsComboBox_.setSize(60, 25);
    for (auto i = 1; i <= steps_.size(); ++i)
    {
        numStepsComboBox_.addItem(std::to_string(i), i);
    }
    numStepsComboBox_.onChange = [this] { updateNumSteps(); };
    numStepsComboBox_.setSelectedId(16, dontSendNotification);

    addAndMakeVisible(numStepsLabel_);
    numStepsLabel_.attachToComponent(&numStepsComboBox_, true);
    numStepsLabel_.setFont(Font(16.0f, Font::italic | Font::bold));
    numStepsLabel_.setText("# STEPS", dontSendNotification);


    trackTitleLabel_.toBack();
}

GriddleTrack::~GriddleTrack()
{
}

const String GriddleTrack::loadProjectTrackData(const var& projectTrackData)
{
    // Initialize an error string to populate if any properties are missing or invalid
    String errorString("");

    // Creating Identifier objects can be slow, so just initialize one to reuse for each property
    Identifier propertyId;

    // Initialize a default return var to be reused for each getProperty call
    var defaultReturn;
    
    // ******************************************************************************************
    // Load each setting from the properties in the file
    // ******************************************************************************************
    // NOTE: When the various components are set, the notification setting is always
    // dontSendNotification, so that the notifier callbacks don't get asynchronously called after 
    // the processing is over resulting in a the unsaved changes flag incorrectly getting set.
    // Since notifications aren't sent, any processing that needs to happen when the values are
    // set is called expliclitly.
    // *******************************************************************************************
    
    propertyId = "is_active";
    if (projectTrackData.hasProperty(propertyId))
    {
        auto isActive = projectTrackData.getProperty(propertyId, defaultReturn);
        activeToggle_.setToggleState(isActive, dontSendNotification);
        updateTrackActiveState(false);
    }
    else
    {
        errorString += ("PROPERTY MISSING - " + propertyId.toString() + " property not found in track settings for track " + trackTitleLabel_.getText() + String(NewLine::getDefault()));
    }

    propertyId = "midi_ch";
    if (projectTrackData.hasProperty(propertyId))
    {
        auto midiChannel = projectTrackData.getProperty(propertyId, defaultReturn);
        midiChannelComboBox_.setSelectedId(midiChannel, dontSendNotification);
    }
    else
    {
        errorString += ("PROPERTY MISSING - " + propertyId.toString() + " property not found in track settings for track " + trackTitleLabel_.getText() + String(NewLine::getDefault()));
    }

    propertyId = "num_steps";
    if (projectTrackData.hasProperty(propertyId))
    {
        auto numSteps = projectTrackData.getProperty(propertyId, defaultReturn);
        numStepsComboBox_.setSelectedId(numSteps, dontSendNotification);
        updateNumSteps(false);
    }
    else
    {
        errorString += ("PROPERTY MISSING - " + propertyId.toString() + " property not found in track settings for track " + trackTitleLabel_.getText() + String(NewLine::getDefault()));
    }

    propertyId = "is_flipped";
    if (projectTrackData.hasProperty(propertyId))
    {
        auto isFlipped = projectTrackData.getProperty(propertyId, defaultReturn);
        flipToggle_.setToggleState(isFlipped, dontSendNotification);
        updateFlippedState(false);
    }
    else
    {
        errorString += ("PROPERTY MISSING - " + propertyId.toString() + " property not found in track settings for track " + trackTitleLabel_.getText() + String(NewLine::getDefault()));
    }

    propertyId = "is_chopped";
    if (projectTrackData.hasProperty(propertyId))
    {
        auto isChopped = projectTrackData.getProperty(propertyId, defaultReturn);
        chopToggle_.setToggleState(isChopped, dontSendNotification);
        updateChoppedState(false);
    }
    else
    {
        errorString += ("PROPERTY MISSING - " + propertyId.toString() + " property not found in track settings for track " + trackTitleLabel_.getText() + String(NewLine::getDefault()));
    }

    propertyId = "is_burnt";
    if (projectTrackData.hasProperty(propertyId))
    {
        auto isBurnt = projectTrackData.getProperty(propertyId, defaultReturn);
        burnToggle_.setToggleState(isBurnt, dontSendNotification);
        updateBurntState(false);
    }
    else
    {
        errorString += ("PROPERTY MISSING - " + propertyId.toString() + " property not found in track settings for track " + trackTitleLabel_.getText() + String(NewLine::getDefault()));
    }

    propertyId = "steps";
    if (projectTrackData.hasProperty(propertyId))
    {
        auto stepsArray = projectTrackData.getProperty(propertyId, defaultReturn);

        int numStepsToRead = stepsArray.size();
        if (numStepsToRead > steps_.size())
        {
            numStepsToRead = static_cast<int>(steps_.size());
            errorString += ("INVALID STEPS LIST - track " + trackTitleLabel_.getText() + " has more than " + String(steps_.size()) + " step entries" + String(NewLine::getDefault()));
        }
        else if (numStepsToRead < steps_.size())
        {
            errorString += ("INVALID STEPS LIST - track " + trackTitleLabel_.getText() + " has fewer than " + String(steps_.size()) + " step entries" + String(NewLine::getDefault()));
        }

        for (auto sI = 0; sI < numStepsToRead; ++sI)
        {
            propertyId = "note_number";
            if (stepsArray[sI].hasProperty(propertyId))
            {
                auto noteNumber = stepsArray[sI].getProperty(propertyId, defaultReturn);
                steps_[sI]->setNoteNumber(noteNumber);
            }
            else
            {
                errorString += ("PROPERTY MISSING - " + propertyId.toString() + " property not found for step " + String(sI + 1) + " in track settings for track " + trackTitleLabel_.getText() + String(NewLine::getDefault()));
            }

            propertyId = "velocity";
            if (stepsArray[sI].hasProperty(propertyId))
            {
                auto velocity = stepsArray[sI].getProperty(propertyId, defaultReturn);
                steps_[sI]->setVelocity(velocity);
            }
            else
            {
                errorString += ("PROPERTY MISSING - " + propertyId.toString() + " property not found for step " + String(sI + 1) + " in track settings for track " + trackTitleLabel_.getText() + String(NewLine::getDefault()));
            }

            propertyId = "gate_percent";
            if (stepsArray[sI].hasProperty(propertyId))
            {
                auto gatePercent = stepsArray[sI].getProperty(propertyId, defaultReturn);
                steps_[sI]->setGatePercent(gatePercent);
            }
            else
            {
                errorString += ("PROPERTY MISSING - " + propertyId.toString() + " property not found for step " + String(sI + 1) + " in track settings for track " + trackTitleLabel_.getText() + String(NewLine::getDefault()));
            }
        }
    }
    else
    {
        errorString += ("PROPERTY MISSING - " + propertyId.toString() + " property not found in track settings for track " + trackTitleLabel_.getText() + String(NewLine::getDefault()));
    }

    return errorString;
}

const var& GriddleTrack::getProjectTrackData()
{
    // Creating Identifier objects can be slow, so just initialize one to reuse for each property
    Identifier propertyId;

    // Set the properties to write to the project file as JSON
    // ********************************************************
    propertyId = "name";
    projectTrackDataVar_.getDynamicObject()->setProperty(propertyId, trackTitleLabel_.getText());

    propertyId = "is_active";
    projectTrackDataVar_.getDynamicObject()->setProperty(propertyId, activeToggle_.getToggleState());

    propertyId = "midi_ch";
    projectTrackDataVar_.getDynamicObject()->setProperty(propertyId, midiChannelComboBox_.getSelectedId());

    propertyId = "num_steps";
    projectTrackDataVar_.getDynamicObject()->setProperty(propertyId, numStepsComboBox_.getSelectedId());

    propertyId = "is_flipped";
    projectTrackDataVar_.getDynamicObject()->setProperty(propertyId, flipToggle_.getToggleState());

    propertyId = "is_chopped";
    projectTrackDataVar_.getDynamicObject()->setProperty(propertyId, chopToggle_.getToggleState());

    propertyId = "is_burnt";
    projectTrackDataVar_.getDynamicObject()->setProperty(propertyId, burnToggle_.getToggleState());

    Array<var> stepsArray;

    for (auto stepI = 0; stepI < steps_.size(); ++stepI)
    {
        var tempStepVar(new DynamicObject());

        propertyId = "note_number";
        tempStepVar.getDynamicObject()->setProperty(propertyId, steps_[stepI]->getNoteNumber());

        propertyId = "velocity";
        tempStepVar.getDynamicObject()->setProperty(propertyId, steps_[stepI]->getVelocity());

        propertyId = "gate_percent";
        tempStepVar.getDynamicObject()->setProperty(propertyId, steps_[stepI]->getGatePercent());

        stepsArray.add(tempStepVar);
    }

    propertyId = "steps";
    projectTrackDataVar_.getDynamicObject()->setProperty(propertyId, stepsArray);

    return projectTrackDataVar_;
}

bool GriddleTrack::isActive(const bool toDrawValue) const
{
    bool active = activeToggle_.getToggleState();

    if (toDrawValue)
        active = activeStateToDraw_;

    return active;
}

bool GriddleTrack::isFlipped(const bool toDrawValue) const
{
    bool flipped = flipToggle_.getToggleState();
    if (toDrawValue)
        flipped = flippedStateToDraw_;
    return flipped;
}

bool GriddleTrack::isChopped(const bool toDrawValue) const
{
    bool chopped = chopToggle_.getToggleState();
    if (toDrawValue)
        chopped = choppedStateToDraw_;
    return chopped;
}

bool GriddleTrack::isBurnt(const bool toDrawValue) const
{
    bool burnt = burnToggle_.getToggleState();
    if (toDrawValue)
        burnt = burntStateToDraw_;
    return burnt;
}

int GriddleTrack::getTempoMultiplier() const
{
    return (isBurnt() ? 2 : 1);
}

void GriddleTrack::addStepsListener(GriddleStep::Listener* listener)
{
    for (auto i = 0; i < 16; ++i)
    {
        steps_[i]->addListener(listener);
    }
}

void GriddleTrack::updateTrackActiveState(const bool notifyTrackChanged)
{
    if (! isPlaying_)
    {
        activeStateToDraw_ = activeToggle_.getToggleState();
        trackTitleLabel_.setEnabled(activeToggle_.getToggleState());
    }

    if (notifyTrackChanged)
        callTrackCharacteristicsChangedCallbacks();
}

void GriddleTrack::updateFlippedState(const bool notifyTrackChanged)
{
    // The flipped state to draw should change immediately if the sequence isn't playing,
    // or if it is, but the active state of the current sequence pass for this track is inactive
    if (! isPlaying_ || (isActive(true) == false))
        flippedStateToDraw_ = flipToggle_.getToggleState();

    if (notifyTrackChanged)
        callTrackCharacteristicsChangedCallbacks();
}

void GriddleTrack::updateChoppedState(const bool notifyTrackChanged)
{
    // The chopped state to draw should change immediately if the sequence isn't playing,
    // or if it is, but the active state of the current sequence pass for this track is inactive
    if (! isPlaying_ || (isActive(true) == false))
        choppedStateToDraw_ = chopToggle_.getToggleState();

    if (notifyTrackChanged)
        callTrackCharacteristicsChangedCallbacks();
}

void GriddleTrack::updateBurntState(const bool notifyTrackChanged)
{
    // The burnt state to draw should change immediately if the sequence isn't playing,
    // or if it is, but the active state of the current sequence pass for this track is inactive
    if (! isPlaying_ || (isActive(true) == false))
        burntStateToDraw_ = burnToggle_.getToggleState();

    if (notifyTrackChanged)
        callTrackCharacteristicsChangedCallbacks();
}

void GriddleTrack::updateMidiChannel()
{
    callTrackCharacteristicsChangedCallbacks();
}

void GriddleTrack::updateNumSteps(const bool notifyTrackChanged)
{
    // The number of steps to draw should change immediately if the sequence isn't playing,
    // or if it is, but the active state of the current sequence pass for this track is inactive
    if (! isPlaying_ || (isActive(true) == false))
        numStepsToDraw_ = numStepsComboBox_.getSelectedId();

    if (notifyTrackChanged)
        callTrackCharacteristicsChangedCallbacks();
}

void GriddleTrack::setStepSelected(const int index)
{
    if (index < steps_.size())
    {
        steps_[index]->setStepSelected(true);
    }
}

void GriddleTrack::applyPendingChanges(const bool isPlaying)
{
    // Update the isPlaying_ flag
    isPlaying_ = isPlaying;

    // Update the enabled state of the title label
    trackTitleLabel_.setEnabled(activeToggle_.getToggleState());

    // Update the "to draw" variables for the GUI elements
    numStepsToDraw_ = (numStepsComboBox_.getSelectedItemIndex() + 1);
    activeStateToDraw_ = activeToggle_.getToggleState();    
    flippedStateToDraw_ = flipToggle_.getToggleState();
    choppedStateToDraw_ = chopToggle_.getToggleState();
    burntStateToDraw_ = burnToggle_.getToggleState();

    // Only make this track's steps selectable if the sequence isn't playing
    // or the track isn't currently active
    for (auto sI = 0; sI < steps_.size(); ++sI)
    {
        steps_[sI]->setCanSelect(! isPlaying_ || ! isActive());
    }
}

void GriddleTrack::callTrackCharacteristicsChangedCallbacks()
{
    Component::BailOutChecker checker(this);

    if (checker.shouldBailOut())
        return;

    if (onTrackCharacteristicsChanged != nullptr)
        onTrackCharacteristicsChanged();
}

void GriddleTrack::paint(Graphics& g)
{
    // Set the background color of the track based on the active state to draw
    auto bgColor = Colour::fromRGB(25, 25, 25);
    if (!activeStateToDraw_)
        bgColor = bgColor.darker(0.4f);

    // Draw the burnt gradient on the track background if applicable
    if (burntStateToDraw_)
    {
        auto burnOpacity = 0.5f;

        if (!activeStateToDraw_)
            burnOpacity = 0.35f;

        g.setGradientFill(ColourGradient(bgColor, 0.0f, 0.0f, Colour::fromFloatRGBA(0.34f, 0.02f, 0.0f, burnOpacity), 0.0f, static_cast<float>(getHeight()), false));
        g.fillAll();
    }
    else
    {
        g.fillAll(bgColor);
    }

    // Set the images for the various toggles based on their toggle states
    // *******************************************************************
    if (activeToggle_.getToggleState())
    {
        activeToggle_.setImages(false, true, true, ImageCache::getFromMemory(BinaryData::RoundCheckboxChecked_png, BinaryData::RoundCheckboxChecked_pngSize), 1.0f, Colours::lightslategrey.darker(0.2f),
            ImageCache::getFromMemory(BinaryData::RoundCheckboxChecked_png, BinaryData::RoundCheckboxChecked_pngSize), 1.0f, Colours::lightslategrey,
            ImageCache::getFromMemory(BinaryData::RoundCheckboxChecked_png, BinaryData::RoundCheckboxChecked_pngSize), 1.0f, Colours::lightslategrey);
    }
    else
    {
        activeToggle_.setImages(false, true, true, ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey.darker(0.2f),
            ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey,
            ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey);
    }

    if (flipToggle_.getToggleState())
    {
        flipToggle_.setImages(false, true, true, ImageCache::getFromMemory(BinaryData::RoundCheckboxChecked_png, BinaryData::RoundCheckboxChecked_pngSize), 1.0f, Colours::lightslategrey.darker(0.2f),
            ImageCache::getFromMemory(BinaryData::RoundCheckboxChecked_png, BinaryData::RoundCheckboxChecked_pngSize), 1.0f, Colours::lightslategrey,
            ImageCache::getFromMemory(BinaryData::RoundCheckboxChecked_png, BinaryData::RoundCheckboxChecked_pngSize), 1.0f, Colours::lightslategrey);
    }
    else
    {
        flipToggle_.setImages(false, true, true, ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey.darker(0.2f),
            ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey,
            ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey);
    }

    if (chopToggle_.getToggleState())
    {
        chopToggle_.setImages(false, true, true, ImageCache::getFromMemory(BinaryData::RoundCheckboxChecked_png, BinaryData::RoundCheckboxChecked_pngSize), 1.0f, Colours::lightslategrey.darker(0.2f),
            ImageCache::getFromMemory(BinaryData::RoundCheckboxChecked_png, BinaryData::RoundCheckboxChecked_pngSize), 1.0f, Colours::lightslategrey,
            ImageCache::getFromMemory(BinaryData::RoundCheckboxChecked_png, BinaryData::RoundCheckboxChecked_pngSize), 1.0f, Colours::lightslategrey);
    }
    else
    {
        chopToggle_.setImages(false, true, true, ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey.darker(0.2f),
            ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey,
            ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey);
    }

    if (burnToggle_.getToggleState())
    {
        burnToggle_.setImages(false, true, true, ImageCache::getFromMemory(BinaryData::RoundCheckboxChecked_png, BinaryData::RoundCheckboxChecked_pngSize), 1.0f, Colours::lightslategrey.darker(0.2f),
            ImageCache::getFromMemory(BinaryData::RoundCheckboxChecked_png, BinaryData::RoundCheckboxChecked_pngSize), 1.0f, Colours::lightslategrey,
            ImageCache::getFromMemory(BinaryData::RoundCheckboxChecked_png, BinaryData::RoundCheckboxChecked_pngSize), 1.0f, Colours::lightslategrey);
    }
    else
    {
        burnToggle_.setImages(false, true, true, ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey.darker(0.2f),
            ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey,
            ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey);
    }

    // Draw the steps in the fixed-width steps area
    // ********************************************
    auto rightMargin = 5;
    auto totalStepsAreaWidth = 720;

    auto extraWidthToDistribute = static_cast<int>(totalStepsAreaWidth) % numStepsToDraw_;
    auto width = static_cast<int>(std::floor(totalStepsAreaWidth / numStepsToDraw_) - rightMargin);
    auto xAxisAddend = 0;

    for (auto i = 0; i < steps_.size(); ++i)
    {
        auto widthAddend = 0;
        if (extraWidthToDistribute > 0)
        {
            widthAddend = 1;
            --extraWidthToDistribute;
        }

        steps_[i]->setSize(width + widthAddend, steps_[i]->getHeight());

        steps_[i]->setTopLeftPosition(240 + xAxisAddend, 0);

        steps_[i]->setFlipDrawState(flippedStateToDraw_);
        steps_[i]->setChopDrawState(choppedStateToDraw_);

        if ((i + 1) > numStepsToDraw_)
        {
            steps_[i]->setVisible(false);
        }
        else
        {
            steps_[i]->setVisible(true);

            xAxisAddend += (steps_[i]->getWidth() + rightMargin);
        }
    }
}

void GriddleTrack::resized()
{
    // GriddleTrack component does not currently support resizing
}

