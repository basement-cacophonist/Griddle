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

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
    : tracks_{ {std::shared_ptr<GriddleTrack>(new GriddleTrack(0)), 
                std::shared_ptr<GriddleTrack>(new GriddleTrack(1)), 
                std::shared_ptr<GriddleTrack>(new GriddleTrack(2)), 
                std::shared_ptr<GriddleTrack>(new GriddleTrack(3))} }
    , previousSampleNumber_(0)
    , bufferSampleRate_(44100.0)
    , playbackSampleNumber_(0)
    , seqStartTime_(0.0)
    , nextStartTime_(0.0)
    , tempoBPM_(120.0)
    , thisPassBPM_(tempoBPM_)
    , isPlaying_(false)
    , keyboardComponent_(keyboardState_, MidiKeyboardComponent::horizontalKeyboard)
    , restButton_("REST")
    , playButton_("PLAY")
    , stopButton_("STOP")
    , stepSelectPreviousButton_("PREVIOUS STEP")
    , stepSelectNextButton_("NEXT STEP")
    , stepSelectPreviousTrackButton_("PREVIOUS TRACK")
    , stepSelectNextTrackButton_("NEXT TRACK")
    , autoAdvanceSelectionToggle_("AUTO ADVANCE SELECTION")
    , playLineX_Offset_(0.0f)
    , selectedStepPtr_(nullptr)
    , startOfMeasurePassed_(false)
    , unsavedProjectChanges_(false)
    , REST_NOTE_VALUE(-1)
    , STEPS_DISPLAY_PIXEL_WIDTH(715)
    , GriddleLightGray(Colour::fromRGB(175, 175, 175))
    , GriddleDarkGray(Colour::fromRGB(50, 50, 50))
    , GriddleSuperDarkGray(Colour::fromRGB(25, 25, 25))
{
    //==============================================================================
    // Window Size and Colour Scheme Initialization
    setSize(1200, 800);
    getLookAndFeel().setColour(ResizableWindow::backgroundColourId, Colours::black);
    getLookAndFeel().setColour(Slider::thumbColourId, Colours::lightslategrey);
    getLookAndFeel().setColour(Slider::rotarySliderFillColourId, GriddleDarkGray);
    getLookAndFeel().setColour(Slider::rotarySliderOutlineColourId, GriddleDarkGray.darker(0.4f));
    getLookAndFeel().setColour(Slider::trackColourId, GriddleDarkGray);
    getLookAndFeel().setColour(Slider::ColourIds::backgroundColourId, GriddleDarkGray.darker(0.4f));
    getLookAndFeel().setColour(Label::textColourId, GriddleLightGray);
    getLookAndFeel().setColour(TextButton::buttonColourId, GriddleSuperDarkGray);
    getLookAndFeel().setColour(FileBrowserComponent::filenameBoxBackgroundColourId, GriddleDarkGray);
    getLookAndFeel().setColour(FileBrowserComponent::currentPathBoxBackgroundColourId, GriddleDarkGray);
    getLookAndFeel().setColour(PopupMenu::backgroundColourId, GriddleSuperDarkGray);
    getLookAndFeel().setColour(PopupMenu::highlightedBackgroundColourId, GriddleDarkGray);
    getLookAndFeel().setColour(ComboBox::backgroundColourId, GriddleSuperDarkGray);
    getLookAndFeel().setColour(AlertWindow::backgroundColourId, GriddleSuperDarkGray);
    getLookAndFeel().setColour(FileListComponent::backgroundColourId, GriddleDarkGray);
    //==============================================================================
    
    // Title Image
    addAndMakeVisible(titleImage_);
    titleImage_.setImage(ImageCache::getFromMemory(BinaryData::Title_png, BinaryData::Title_pngSize));
    titleImage_.setTopLeftPosition(40, 31);
    titleImage_.setSize(370, 101);

    // Subtitle Image
    addAndMakeVisible(subtitleImage_);
    subtitleImage_.setImage(ImageCache::getFromMemory(BinaryData::Subtitle_png, BinaryData::Subtitle_pngSize));
    subtitleImage_.setTopLeftPosition(40, 135);
    subtitleImage_.setSize(370, 36);
        
    // About Button
    addAndMakeVisible(aboutButton_);
    aboutButton_.setTopLeftPosition(910, 130);
    aboutButton_.setSize(120, 30);
    aboutButton_.setButtonText("About Griddle");
    aboutButton_.onClick = [this] { showAboutDialog();  };

    // Tutorial Video Link Button
    addAndMakeVisible(tutorialVideoButton_);
    tutorialVideoButton_.setTopLeftPosition(1040, 130);
    tutorialVideoButton_.setSize(120, 30);
    tutorialVideoButton_.setButtonText("Tutorial Video");
    tutorialVideoButton_.onClick = [this] { openTutorialVideoLink();  };

    // Project Button and Label
    addAndMakeVisible(projectButton_);
    projectButton_.setTopLeftPosition(910, 40);
    projectButton_.setSize(250, 30);
    projectButton_.setButtonText("(new - click here for options)");
    projectButton_.onClick = [this] { handleProjectButtonClick(); };

    addAndMakeVisible(projectLabel_);
    projectLabel_.setText("PROJECT", dontSendNotification);
    projectLabel_.setJustificationType(Justification::centred);
    projectLabel_.setFont(Font(16.0f, Font::italic | Font::bold));
    projectLabel_.attachToComponent(&projectButton_, true);

    // Project Menu
    projectMenu_.addItem(1, "New");
    projectMenu_.addSeparator();
    projectMenu_.addItem(2, "Load");
    projectMenu_.addSeparator();
    projectMenu_.addItem(3, "Save As");
    projectMenu_.addItem(4, "Save");

    // Tempo Slider
    addAndMakeVisible(tempoSlider_);
    tempoSlider_.setTopLeftPosition(520, 20);
    tempoSlider_.setSize(160, 160);
    tempoSlider_.setSliderStyle(Slider::SliderStyle::Rotary);
    tempoSlider_.setTextBoxStyle(Slider::TextBoxBelow, false, tempoSlider_.getTextBoxWidth(), tempoSlider_.getTextBoxHeight());
    tempoSlider_.setRange(10.0, 180.0, 0.5);
    tempoSlider_.setTextValueSuffix(" BPM");
    tempoSlider_.addListener(this);
    tempoSlider_.setValue(tempoBPM_, dontSendNotification);

    // Tempo Dial Image
    addAndMakeVisible(tempoDialImage_);
    tempoDialImage_.setImage(ImageCache::getFromMemory(BinaryData::TempoDial_png, BinaryData::TempoDial_pngSize));    
    tempoDialImage_.setTopLeftPosition(560, 50);
    tempoDialImage_.setSize(80, 80);
    rotateTempoDialImage();    

    // Play Button
    addAndMakeVisible(playButton_);
    playButton_.setTopLeftPosition(450, 60);
    playButton_.setSize(70, 70);
    playButton_.setImages(false, true, true, ImageCache::getFromMemory(BinaryData::PlayButton_png, BinaryData::PlayButton_pngSize), 1.0f, Colours::transparentBlack,
        ImageCache::getFromMemory(BinaryData::PlayButton_png, BinaryData::PlayButton_pngSize), 0.8f, Colours::transparentBlack,
        ImageCache::getFromMemory(BinaryData::PlayButton_png, BinaryData::PlayButton_pngSize), 0.6f, Colours::transparentBlack);
    playButton_.onClick = [this] { handlePlayButtonClick(); };

    // Stop Button
    Component::addAndMakeVisible(stopButton_);
    stopButton_.setTopLeftPosition(680, 60);
    stopButton_.setSize(70, 70);
    stopButton_.setImages(false, true, true, ImageCache::getFromMemory(BinaryData::StopButton_png, BinaryData::StopButton_pngSize), 1.0f, Colours::transparentBlack,
        ImageCache::getFromMemory(BinaryData::StopButton_png, BinaryData::StopButton_pngSize), 0.8f, Colours::transparentBlack,
        ImageCache::getFromMemory(BinaryData::StopButton_png, BinaryData::StopButton_pngSize), 0.6f, Colours::transparentBlack);
    stopButton_.onClick = [this] { handleStopButtonClick(); };
    stopButton_.setEnabled(false);

    // MIDI Output ComboBox and Label
    addAndMakeVisible(midiOutputList_);
    midiOutputList_.setTopLeftPosition(910, 85);
    midiOutputList_.setSize(250, 30);
    midiOutputList_.setTextWhenNoChoicesAvailable("No MIDI Outputs Enabled");
    auto midiOutputs = MidiOutput::getAvailableDevices();
    juce::StringArray midiOutIdentifiers;
    for (auto i = 0; i < midiOutputs.size(); ++i)
    {
        midiOutIdentifiers.add(midiOutputs[i].name);
    }
    midiOutputList_.addItemList(midiOutIdentifiers, 1);
    midiOutputList_.onChange = [this] { setMidiOutput(midiOutputList_.getItemText(midiOutputList_.getSelectedItemIndex())); };
    midiOutputList_.setSelectedId(1, dontSendNotification);
    midiOutputs = MidiOutput::getAvailableDevices();
    for (auto i = 0; i < midiOutputs.size(); ++i)
    {
        if (midiOutputs[i].name == midiOutputList_.getItemText(midiOutputList_.getSelectedItemIndex()))
        {
            midiOutPtr_ = MidiOutput::openDevice(midiOutputs[i].identifier);
        }
    }

    addAndMakeVisible(midiOutputListLabel_); 
    midiOutputListLabel_.setText("MIDI OUTPUT", dontSendNotification);
    midiOutputListLabel_.setJustificationType(Justification::centred);
    midiOutputListLabel_.setFont(Font(16.0f, Font::italic | Font::bold));
    midiOutputListLabel_.attachToComponent(&midiOutputList_, true);

    // GriddleTracks
    for (auto i = 0; i < tracks_.size(); ++i)
    {
        addAndMakeVisible(*tracks_[i]);

        auto bottomMargin = 5;
        tracks_[i]->setTopLeftPosition(0, 200 + (i * tracks_[i]->getHeight()) + (i * bottomMargin));
        tracks_[i]->addStepsListener(this);
        tracks_[i]->onTrackCharacteristicsChanged = [this] { handleTrackCharacteristicsChanged(); };

        switch (i)
        {
        case 0:
            tracks_[i]->setTitle("A");
            break;
        case 1:
            tracks_[i]->setTitle("B");
            break;
        case 2:
            tracks_[i]->setTitle("C");
            break;
        case 3:
            tracks_[i]->setTitle("D");
            break;
        default:
            break;
        }
    }

    // Step Edit Section Label
    addAndMakeVisible(stepEditSectionLabel_);
    stepEditSectionLabel_.setTopLeftPosition(10, 610);
    stepEditSectionLabel_.setSize(120, 180);
    stepEditSectionLabel_.setJustificationType(Justification::topLeft);
    stepEditSectionLabel_.setText("STEP EDIT", dontSendNotification);
    stepEditSectionLabel_.setAlpha(0.5f);
    stepEditSectionLabel_.setColour(Label::textColourId, GriddleLightGray);
    stepEditSectionLabel_.setFont(Font(35.0f, Font::italic | Font::bold));
    
    // Select Previous Step Button
    addAndMakeVisible(stepSelectPreviousButton_);
    stepSelectPreviousButton_.setTopLeftPosition(620, 610);
    stepSelectPreviousButton_.setSize(70, 70);
    stepSelectPreviousButton_.setImages(false, true, true, ImageCache::getFromMemory(BinaryData::ArrowLeft_png, BinaryData::ArrowLeft_pngSize), 1.0f, Colours::transparentBlack,
        ImageCache::getFromMemory(BinaryData::ArrowLeft_png, BinaryData::ArrowLeft_pngSize), 0.8f, Colours::transparentBlack,
        ImageCache::getFromMemory(BinaryData::ArrowLeft_png, BinaryData::ArrowLeft_pngSize), 0.6f, Colours::transparentBlack);
    stepSelectPreviousButton_.onClick = [this] { handleStepSelectPreviousButtonClick(); };

    // Select Next Step Button
    addAndMakeVisible(stepSelectNextButton_);
    stepSelectNextButton_.setTopLeftPosition(710, 610);
    stepSelectNextButton_.setSize(70, 70);
    stepSelectNextButton_.setImages(false, true, true, ImageCache::getFromMemory(BinaryData::ArrowRight_png, BinaryData::ArrowRight_pngSize), 1.0f, Colours::transparentBlack,
        ImageCache::getFromMemory(BinaryData::ArrowRight_png, BinaryData::ArrowRight_pngSize), 0.8f, Colours::transparentBlack,
        ImageCache::getFromMemory(BinaryData::ArrowRight_png, BinaryData::ArrowRight_pngSize), 0.6f, Colours::transparentBlack);
    stepSelectNextButton_.onClick = [this] { handleStepSelectNextButtonClick(); };

    // Auto-advance Selection Toggle
    addAndMakeVisible(autoAdvanceSelectionToggle_);
    autoAdvanceSelectionToggle_.setTopLeftPosition(840, 615);
    autoAdvanceSelectionToggle_.setSize(40, 40);
    autoAdvanceSelectionToggle_.setImages(false, true, true, ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey.darker(0.2f),
        ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey,
        ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey);
    autoAdvanceSelectionToggle_.onClick = [this] { updateAutoAdvanceSelectionState(); };
    updateAutoAdvanceSelectionState();

    // Auto-advance Selection Label
    addAndMakeVisible(autoAdvanceSelectionLabel_);
    autoAdvanceSelectionLabel_.setTopLeftPosition(800, 650);
    autoAdvanceSelectionLabel_.setSize(120, 40);
    autoAdvanceSelectionLabel_.setJustificationType(Justification::centred);
    autoAdvanceSelectionLabel_.setFont(Font(16.0f, Font::italic | Font::bold));
    autoAdvanceSelectionLabel_.setText("AUTO-ADVANCE", dontSendNotification);

    // Select Step on Next Track Button
    addAndMakeVisible(stepSelectPreviousTrackButton_);
    stepSelectPreviousTrackButton_.setTopLeftPosition(940, 610);
    stepSelectPreviousTrackButton_.setSize(70, 70);
    stepSelectPreviousTrackButton_.setImages(false, true, true, ImageCache::getFromMemory(BinaryData::ArrowUp_png, BinaryData::ArrowUp_pngSize), 1.0f, Colours::transparentBlack,
        ImageCache::getFromMemory(BinaryData::ArrowUp_png, BinaryData::ArrowUp_pngSize), 0.8f, Colours::transparentBlack,
        ImageCache::getFromMemory(BinaryData::ArrowUp_png, BinaryData::ArrowUp_pngSize), 0.6f, Colours::transparentBlack);
    stepSelectPreviousTrackButton_.onClick = [this] { handleStepSelectPreviousTrackButtonClick(); };

    // Select Step on Previous Track Button
    addAndMakeVisible(stepSelectNextTrackButton_);
    stepSelectNextTrackButton_.setTopLeftPosition(1030, 610);
    stepSelectNextTrackButton_.setSize(70, 70);
    stepSelectNextTrackButton_.setImages(false, true, true, ImageCache::getFromMemory(BinaryData::ArrowDown_png, BinaryData::ArrowDown_pngSize), 1.0f, Colours::transparentBlack,
        ImageCache::getFromMemory(BinaryData::ArrowDown_png, BinaryData::ArrowDown_pngSize), 0.8f, Colours::transparentBlack,
        ImageCache::getFromMemory(BinaryData::ArrowDown_png, BinaryData::ArrowDown_pngSize), 0.6f, Colours::transparentBlack);
    stepSelectNextTrackButton_.onClick = [this] { handleStepSelectNextTrackButtonClick(); };

    // MIDI Keyboard
    addAndMakeVisible(keyboardComponent_);
    keyboardComponent_.setTopLeftPosition(610, 690);
    keyboardComponent_.setSize(500, 95);
    keyboardComponent_.setKeyWidth(32);
    keyboardComponent_.setColour(MidiKeyboardComponent::mouseOverKeyOverlayColourId, Colour::fromRGB(148, 162, 175));
    keyboardComponent_.setOctaveForMiddleC(4);
    keyboardState_.addListener(this);

    // Rest Button
    addAndMakeVisible(restButton_);
    restButton_.setTopLeftPosition(1120, 705);
    restButton_.setSize(70, 70);
    restButton_.setImages(false, true, true, ImageCache::getFromMemory(BinaryData::RestButton_png, BinaryData::RestButton_pngSize), 1.0f, Colours::transparentBlack,
        ImageCache::getFromMemory(BinaryData::RestButton_png, BinaryData::RestButton_pngSize), 0.8f, Colours::transparentBlack,
        ImageCache::getFromMemory(BinaryData::RestButton_png, BinaryData::RestButton_pngSize), 0.6f, Colours::transparentBlack);
    restButton_.onClick = [this] { handleRestButtonClick(); };

    // Step Edit Velocity Slider/Label
    addAndMakeVisible(stepEditVelocitySlider_);
    stepEditVelocitySlider_.setTopLeftPosition(190, 700);
    stepEditVelocitySlider_.setSize(400, 25);
    stepEditVelocitySlider_.setTextBoxStyle(Slider::TextBoxLeft, false, stepEditVelocitySlider_.getTextBoxWidth(), stepEditVelocitySlider_.getTextBoxHeight());
    stepEditVelocitySlider_.setRange(1.0, 127.0, 1.0);
    stepEditVelocitySlider_.setTextValueSuffix("");
    stepEditVelocitySlider_.addListener(this);
    stepEditVelocitySlider_.setValue(100, dontSendNotification);
    
    addAndMakeVisible(stepEditVelocityLabel_);
    stepEditVelocityLabel_.setText("VELOCITY", dontSendNotification);
    stepEditVelocityLabel_.setJustificationType(Justification::centred);
    stepEditVelocityLabel_.setFont(Font(16.0f, Font::italic | Font::bold));
    stepEditVelocityLabel_.attachToComponent(&stepEditVelocitySlider_, true);

    // Step Edit Gate Slider/Label
    addAndMakeVisible(stepEditGateSlider_);
    stepEditGateSlider_.setTopLeftPosition(190, 750);
    stepEditGateSlider_.setSize(400, 25);
    stepEditGateSlider_.setTextBoxStyle(Slider::TextBoxLeft, false, stepEditGateSlider_.getTextBoxWidth(), stepEditGateSlider_.getTextBoxHeight());
    stepEditGateSlider_.setRange(1.0, 100.0, 1.0);
    stepEditGateSlider_.setTextValueSuffix("%");
    stepEditGateSlider_.addListener(this);
    stepEditGateSlider_.setValue(100, dontSendNotification);

    addAndMakeVisible(stepEditGateLabel_);
    stepEditGateLabel_.setText("GATE", dontSendNotification);
    stepEditGateLabel_.setJustificationType(Justification::centred);
    stepEditGateLabel_.setFont(Font(16.0f, Font::italic | Font::bold));
    stepEditGateLabel_.attachToComponent(&stepEditGateSlider_, true);

    // Step Edit Note Label
    addAndMakeVisible(stepEditNoteLabel_);
    stepEditNoteLabel_.setTopLeftPosition(190, 620);
    stepEditNoteLabel_.setSize(400, 55);
    stepEditNoteLabel_.setText("", dontSendNotification);
    stepEditNoteLabel_.setJustificationType(Justification::centred);
    stepEditNoteLabel_.setFont(Font(70.0, Font::italic | Font::bold));
    stepEditNoteLabel_.setColour(Label::backgroundColourId, GriddleSuperDarkGray);
    stepEditNoteLabel_.setColour(Label::outlineColourId, GriddleDarkGray);

    // Step Edit Note Title Label
    addAndMakeVisible(stepEditNoteTitleLabel_);
    stepEditNoteTitleLabel_.setText("NOTE", dontSendNotification);
    stepEditNoteTitleLabel_.setJustificationType(Justification::centred);
    stepEditNoteTitleLabel_.setFont(Font(16.0f, Font::italic | Font::bold));
    stepEditNoteTitleLabel_.attachToComponent(&stepEditNoteLabel_, true);

    // Set FPS for animating the play lines to 30FPS
    setFramesPerSecond(30);

    // Play Lines for each track
    for (auto plI = 0; plI < playLines_.size(); ++plI)
    {
        addAndMakeVisible(playLines_[plI]);
        Rectangle<float> rect(240.0f + playLineX_Offset_, static_cast<float>(tracks_[plI]->getY()), 3.0f, static_cast<float>(tracks_[plI]->getHeight()));
        playLines_[plI].setRectangle(Parallelogram<float>(rect));
        playLines_[plI].setFill(Colours::lightgrey);
        playLines_[plI].setAlwaysOnTop(true);
    }

    // All tacks are populated with default data now, so clone the default track data into a var for
    // when the user chooses to initialize a new project from the project menu
    trackDefaultData_ = tracks_[0]->getProjectTrackData().clone();

    // Listen for computer keyboard events
    addKeyListener(this);

    // Start a new project
    startNewProject();
}

MainComponent::~MainComponent()
{
}

void MainComponent::showAboutDialog()
{
    // Populate the About Window content
    String aboutMsg = "Griddle is a free application developed by Kevin Frank using the JUCE framework";
    aboutMsg.append(String(NewLine::getDefault()) + String(NewLine::getDefault()) + "Griddle is released under the GNU GPLv3 License", SIZE_MAX);
    aboutMsg.append(String(NewLine::getDefault()) + "See the accompanying LICENSE.GPL file for detailed license information", SIZE_MAX);
    aboutMsg.append(String(NewLine::getDefault()) + String(NewLine::getDefault()) + "Griddle uses the following fonts under the SIL Open Font license:", SIZE_MAX);
    aboutMsg.append(String(NewLine::getDefault()) + "Alex Brush by Robert E. Leuschke", SIZE_MAX);
    aboutMsg.append(String(NewLine::getDefault()) + "Genome by Alfredo Marco Pradil", SIZE_MAX);
    aboutMsg.append(String(NewLine::getDefault()) + String(NewLine::getDefault()) + "Griddle uses the following icons under the Creative Commons Attribution (CC BY) license:", SIZE_MAX);
    aboutMsg.append(String(NewLine::getDefault()) + "Switch by Lee Mette from thenounproject.com", SIZE_MAX);
    aboutMsg.append(String(NewLine::getDefault()) + "Checkbox by regina from thenounproject.com", SIZE_MAX);
    aboutMsg.append(String(NewLine::getDefault()) + "Play by Arthur Shlain from thenounproject.com", SIZE_MAX);
    aboutMsg.append(String(NewLine::getDefault()) + "Stop by Arthur Shlain from thenounproject.com", SIZE_MAX);
    aboutMsg.append(String(NewLine::getDefault()) + "Circle by Arthur Shlain from thenounproject.com", SIZE_MAX);
    aboutMsg.append(String(NewLine::getDefault()) + "Arrow by Arthur Shlain from thenounproject.com", SIZE_MAX);

    // Show the About window
    AlertWindow::showMessageBox(AlertWindow::NoIcon, "ABOUT GRIDDLE v" + String(ProjectInfo::versionString), aboutMsg);
}

void MainComponent::openTutorialVideoLink()
{
    URL tutorialVideoLink("https://vimeo.com/414305938");

    tutorialVideoLink.launchInDefaultBrowser();
}

void MainComponent::startNewProject()
{
    // Load each track with default track data
    for (auto trackI = 0; trackI < tracks_.size(); ++trackI)
    {
        auto errors = tracks_[trackI]->loadProjectTrackData(trackDefaultData_);
    }

    // Reset the selected step, force-clearing the current step selection
    resetSelectedStep(true);

    // Set the project button text to indicate a new/unsaved project
    projectButton_.setButtonText("(new - click here for options)");

    // Clear the current project file
    currentProjectFile_ = File();

    // Reset the source buffer
    updateSourceMidiBuffer();

    // Clear the unsaved changes flag
    setUnsavedChangesFlag(false);
}

void MainComponent::hiResTimerCallback()
{
    // Determine the current sample number in the MidiBuffer from the time
    auto clockTime = Time::getMillisecondCounterHiRes() * 0.001;
    auto currentTime = clockTime - seqStartTime_;
    auto currentSampleNumber = static_cast<int>(currentTime * bufferSampleRate_);

    // Send all MIDI messages that are at or before the current sample number
    MidiBuffer::Iterator iterator(playbackBuffer_);

    while (iterator.getNextEvent(bufferOutputMessage_, playbackSampleNumber_))
    {
        if (playbackSampleNumber_ >= currentSampleNumber)
            break;

        midiOutPtr_->sendMessageNow(bufferOutputMessage_);
    }

    // Clear MIDI events from the playbackBuffer_ that were sent
    playbackBuffer_.clear(previousSampleNumber_, currentSampleNumber - previousSampleNumber_);

    // Store off the sample number for the next pass
    previousSampleNumber_ = currentSampleNumber;

    // Handle the end of the measure when it is reached
    if (clockTime >= nextStartTime_)
    {
        // If there are still some messages that didn't get sent for this measure, send them now.
        // This should just be any NOTE OFFs that didn't get caught because of imprecise timing.
        if (!playbackBuffer_.isEmpty())
        {
            while (iterator.getNextEvent(bufferOutputMessage_, playbackSampleNumber_))
            {
                midiOutPtr_->sendMessageNow(bufferOutputMessage_);
            }
        }

        // Populate the playbackBuffer_ with the MIDI events from the sourceBuffer_
        playbackBuffer_.clear();
        playbackBuffer_ = sourceBuffer_;

        // Update the sample number and time variables
        previousSampleNumber_ = 0;
        seqStartTime_ = clockTime;
        nextStartTime_ = seqStartTime_ + (1 / (tempoBPM_ / 60.0) * 4);  

        // Update the BPM to use for this measure
        thisPassBPM_ = tempoBPM_;

        // Set the startOfMeasurePassed_ flag so that GUI elements can update accordingly in the update method
        startOfMeasurePassed_ = true;

        // Reset the play line offset for the start of the measure
        playLineX_Offset_ = 0.0;

    }
}

void MainComponent::handleNoteOn(MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
    // Ensure NOTE ON events from the MIDI keyboard are only processed if there is a selected step
    if (selectedStepPtr_ != nullptr)
    {
        // Update the note number, display of the note for the step, and the source buffer
        selectedStepPtr_->setNoteNumber(midiNoteNumber);
        stepEditNoteLabel_.setText(MidiMessage::getMidiNoteName(midiNoteNumber, true, true, 4), dontSendNotification);
        updateSourceMidiBuffer();

        // Send a NOTE ON message to preview the note, only if the sequence is not currently being played
        if (! isPlaying_)
        {
            auto message = MidiMessage::noteOn(tracks_[selectedStepPtr_->getOwnerTrackIndex()]->getMidiChannel(), midiNoteNumber, static_cast<uint8>(stepEditVelocitySlider_.getValue()));

            midiOutPtr_->sendMessageNow(message);
        }

        // Advance the step selection if auto-advance is set
        if (autoAdvanceSelectionToggle_.getToggleState())
        {
            stepSelectionGoToNext();
        }

        setUnsavedChangesFlag(true);
    }
}

void MainComponent::handleNoteOff(MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
    // Ensure NOTE OFF events from the MIDI keyboard are only processed if there is a selected step
    if (selectedStepPtr_ != nullptr)
    {
        // Send a NOTE OFF message only if the sequence is not currently being played
        if (! isPlaying_)
        {
            auto message = MidiMessage::noteOff(tracks_[selectedStepPtr_->getOwnerTrackIndex()]->getMidiChannel(), midiNoteNumber, static_cast<uint8>(stepEditVelocitySlider_.getValue()));
            midiOutPtr_->sendMessageNow(message);
        }
    }
}

void MainComponent::handleRestButtonClick()
{
    // When REST is only processed if there is a selected step
    if (selectedStepPtr_ != nullptr)
    {
        // Set the step to a rest and update the source buffer
        selectedStepPtr_->setNoteNumber(REST_NOTE_VALUE);    
        stepEditNoteLabel_.setText("", dontSendNotification);
        updateSourceMidiBuffer();

        // Advance the step selection if auto-advance is set
        if (autoAdvanceSelectionToggle_.getToggleState())
        {
            stepSelectionGoToNext();
        }

        setUnsavedChangesFlag(true);
    }
}

void MainComponent::handleProjectButtonClick()
{
    // Show the Project menu when the project button is clicked
    const int menuResult = projectMenu_.showAt(&projectButton_);

    if (menuResult == 1)
    {
        // ** NEW **

        // Ensure the user has a chance to save any unsaved changes before starting a new project
        if (promptForProjectSave() == CancellationEnum::NOT_CANCELLED)
        {
            startNewProject();
        }
    }
    else if (menuResult == 2)
    {
        // ** LOAD **

        // Ensure the user has a chance to save any unsaved changes before loading another project
        if (promptForProjectSave() == CancellationEnum::NOT_CANCELLED)
        {
            loadProject();
        }

    }
    else if (menuResult == 3)
    {
        // ** SAVE AS **
        saveProjectAs();
    }
    else if (menuResult == 4)
    {
        // ** SAVE **

        // If the current project is new, give the user the save-as window, otherwise just save the changes to the current project file
        if (projectButton_.getButtonText().startsWith("(new"))
        {
            saveProjectAs();
        }
        else
        {
            saveProject(currentProjectFile_);
        }
    }
}

void MainComponent::loadProject()
{
    // Show a FileChooserDialogBox to open .griddle files
    WildcardFileFilter wildcardFilter("*.griddle", String(), "Griddle Project files");
       
    // Set the initial directory for the FileBrowserComponent to the parent directory of current project file by default
    File initialDir(currentProjectFile_.getParentDirectory());

    // If there isn't a current project file, set the initial directory for the FileBrowser component to the Projects directory
    if (initialDir.getFullPathName().isEmpty())
    {
        initialDir = File::getSpecialLocation(File::currentApplicationFile).getParentDirectory().getFullPathName() + String("/Projects/");

        // If for some reason the Projects directory doesn't exist, fall back to the location of the Griddle executable
        if (!initialDir.exists())
            initialDir = File::getSpecialLocation(File::currentApplicationFile).getParentDirectory().getFullPathName();
    }

    FileBrowserComponent browser(FileBrowserComponent::canSelectFiles | FileBrowserComponent::openMode,
        initialDir,
        &wildcardFilter,
        nullptr);

    FileChooserDialogBox dialogBox("Open Griddle Project File",
        "Please choose a Griddle Project file that you want to open...",
        browser,
        false,
        GriddleSuperDarkGray);

    if (dialogBox.show())
    {
        // Parse the file as JSON
        File selectedFile = browser.getSelectedFile(0);
        var projectData = JSON::parse(selectedFile);

        // If the file was parsed successfully, set the project data from the file
        if (projectData.isVoid() == false)
        {
            
            loadedProject_ = projectData;

            // Creating Identifier objects can be slow, so just initialize one to reuse for each property
            Identifier propertyId;

            // Initialize a default return var to be reused for each getProperty call
            var defaultReturn;


            String invalidMidiOutput("");

            // Initialize an error string to populate if any properties are missing or invalid
            String errorString("");
            
            // ******************************************************************************************
            // Load each setting from the properties in the file
            // ******************************************************************************************
            // NOTE: When the various components are set, the notification setting is always
            // dontSendNotification, so that the notifier callbacks don't get asynchronously called after 
            // the processing is over resulting in a the unsaved changes flag incorrectly getting set.
            // Since notifications aren't sent, any processing that needs to happen when the values are
            // set is called expliclitly.
            // *******************************************************************************************
            propertyId = "master_settings";            
            if (loadedProject_.hasProperty(propertyId))
            {
                var masterSettings = loadedProject_.getProperty(propertyId, defaultReturn);

                propertyId = "tempo";                
                if (masterSettings.hasProperty(propertyId))
                {
                    var tempo = masterSettings.getProperty(propertyId, defaultReturn);
                    tempoSlider_.setValue(tempo, dontSendNotification);
                    rotateTempoDialImage();
                    tempoBPM_ = tempoSlider_.getValue();
                }
                else
                {
                    errorString += ("PROPERTY MISSING - " + propertyId.toString() + " property not found in master_settings" + String(NewLine::getDefault()));
                }

                propertyId = "midi_output";
                if (masterSettings.hasProperty(propertyId))
                {
                    var midiOutput = masterSettings.getProperty(propertyId, defaultReturn);
                    invalidMidiOutput = midiOutput.toString();
                    for (auto moI = 0; moI < midiOutputList_.getNumItems(); ++moI)
                    {
                        if (midiOutputList_.getItemText(moI) == midiOutput.toString())
                        {
                            midiOutputList_.setSelectedId(midiOutputList_.getItemId(moI), dontSendNotification);
                            setMidiOutput(midiOutputList_.getItemText(midiOutputList_.getSelectedItemIndex()));
                            invalidMidiOutput = "";
                            break;
                        }
                    }
                }
                else
                {
                    errorString += ("PROPERTY MISSING - " + propertyId.toString() + " property not found in master_settings" + String(NewLine::getDefault()));
                }
            }
            else
            {
                errorString += ("PROPERTY MISSING - " + propertyId.toString() + " not found in the project file" + String(NewLine::getDefault()));
            }

            propertyId = "sequence";
            if (loadedProject_.hasProperty(propertyId))
            {
                var sequenceData = loadedProject_.getProperty(propertyId, defaultReturn);

                propertyId = "tracks";
                if (sequenceData.hasProperty(propertyId))
                {
                    var tracksList = sequenceData.getProperty(propertyId, defaultReturn);

                    int numTracksToRead = tracksList.size();
                    if (numTracksToRead > tracks_.size())
                    {
                        numTracksToRead = static_cast<int>(tracks_.size());

                        errorString += ("INVALID TRACKS LIST - sequence has more than " + String(tracks_.size()) + " track entries" + String(NewLine::getDefault()));
                    }
                    else if (numTracksToRead < tracks_.size())
                    {
                        errorString += ("INVALID TRACKS LIST - sequence has fewer than " + String(tracks_.size()) + " track entries" + String(NewLine::getDefault()));
                    }

                    for (auto tracksI = 0; tracksI < numTracksToRead; ++tracksI)
                    {
                        errorString += tracks_[tracksI]->loadProjectTrackData(tracksList[tracksI]);
                    }
                }
                else
                {
                    errorString += ("PROPERTY MISSING - " + propertyId.toString() + " property not found in sequence" + String(NewLine::getDefault()));
                }
            }
            else
            {
                errorString += ("PROPERTY MISSING - " + propertyId.toString() + " property not found in the project file" + String(NewLine::getDefault()));
            }
                        
            // After loading all of the project settings, update the currentProjectFile_ and the project button to display the loaded filename
            currentProjectFile_ = selectedFile;
            projectButton_.setButtonText(currentProjectFile_.getFileName());
            
            // Reset the step selection, force-clearing the current selection
            resetSelectedStep(true);

            // Reset the source buffer
            updateSourceMidiBuffer();

            // Clear the unsaved changes flag
            setUnsavedChangesFlag(false);

            // If there is any content in the error string, warn the user that the project file may not have fully loaded and list the errors
            if (errorString.isNotEmpty())
            {
                AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Project File Errors", "The project file may be only partially loaded due to the following errors:" + String(NewLine::getDefault()) + String(NewLine::getDefault()) + errorString);
            }

            // If the MIDI output specified in the file isn't available, let the user know the MIDI output will remain whatever it was
            if (invalidMidiOutput.isNotEmpty())
            {
                AlertWindow::showMessageBox(AlertWindow::InfoIcon, "MIDI Output Not Found", invalidMidiOutput + " was specified as the MIDI output in the project file, but was not found in the currently available MIDI outputs. The selected MIDI Output will remain unchanged.");
            }
        }
        else
        {
            AlertWindow::showMessageBox(AlertWindow::WarningIcon, "Invalid Project File!", selectedFile.getFileName() + " does not contain valid Griddle Project data!" + String(NewLine::getDefault()) + String(NewLine::getDefault()) + "The file will not be loaded.");
        }

    }
}

void MainComponent::saveProjectAs()
{
    // Show a FileChooserDialogBox for saving a .griddle file
    WildcardFileFilter wildcardFilter("*.griddle", String(), "Griddle Project files");

    // Set the initial file for the FileBrowserComponent to the current project file by default
    File initialFile(currentProjectFile_);

    // If there isn't a current project file, set the initial file for the FileBrowserComponent to the Projects directory
    if (initialFile.getFullPathName().isEmpty())
    {
        initialFile = File::getSpecialLocation(File::currentApplicationFile).getParentDirectory().getFullPathName() + String("/Projects/");

        // If for some reason the Projects directory doesn't exist, fall back to the location of the Griddle executable
        if (!initialFile.exists())
            initialFile = File::getSpecialLocation(File::currentApplicationFile).getParentDirectory().getFullPathName();
    }

    FileBrowserComponent browser(FileBrowserComponent::canSelectFiles | FileBrowserComponent::warnAboutOverwriting | FileBrowserComponent::saveMode,
        initialFile,
        &wildcardFilter,
        nullptr);

    FileChooserDialogBox dialogBox("Save Griddle Project File",
        "Specify a Griddle Project file for saving...",
        browser,
        true,
        GriddleSuperDarkGray);

    if (dialogBox.show())
    {
        File selectedFile = browser.getSelectedFile(0);

        // Do the save processing on the selected file
        saveProject(selectedFile);
    }
}

void MainComponent::saveProject(File projectFile)
{
    // Add the .griddle extension if it was omitted
    if (projectFile.getFileName().endsWith(".griddle") == false)
    {
        projectFile = File(projectFile.getFullPathName() + ".griddle");
    }

    // Creating Identifier objects can be slow, so initialize one to reuse for each property 
    Identifier propertyId;

    // Set the properties to write to the project file as JSON
    // ********************************************************
    var masterSettings(new DynamicObject());
    propertyId = "tempo";
    masterSettings.getDynamicObject()->setProperty(propertyId, tempoSlider_.getValue());

    propertyId = "midi_output";
    masterSettings.getDynamicObject()->setProperty(propertyId, midiOutputList_.getItemText(midiOutputList_.getSelectedItemIndex()));

    var sequence(new DynamicObject());
    Array<var> tracksArray;
    for (auto trackI = 0; trackI < tracks_.size(); ++trackI)
    {
        tracksArray.add(tracks_[trackI]->getProjectTrackData());
    }

    propertyId = "tracks";
    sequence.getDynamicObject()->setProperty(propertyId, tracksArray);

    var project(new DynamicObject());
    propertyId = "master_settings";
    project.getDynamicObject()->setProperty(propertyId, masterSettings);
    propertyId = "sequence";
    project.getDynamicObject()->setProperty(propertyId, sequence);

    // Add the JSON to the project file
    projectFile.appendText(JSON::toString(project));

    // Update the project button text to show the project filename
    currentProjectFile_ = projectFile;
    projectButton_.setButtonText(currentProjectFile_.getFileName());

    // Clear the unsaved changes flag
    setUnsavedChangesFlag(false);
}

MainComponent::CancellationEnum MainComponent::promptForProjectSave()
{
    CancellationEnum cancelledValue = CancellationEnum::NOT_CANCELLED;

    // If there are unsaved changes, prompt the user to decide whether or not to save the current project
    if (unsavedProjectChanges_)
    {
        auto response = AlertWindow::showYesNoCancelBox(AlertWindow::QuestionIcon, "Unsaved Project", "The current project has unsaved changes." + String(NewLine::getDefault()) + String(NewLine::getDefault()) + "Would you like to save the current project before continuing?");
    
        if (response == 0)
        {
            // CANCEL
            cancelledValue = CancellationEnum::CANCELLED;
        }
        else if (response == 1)
        {
            // YES
            if (projectButton_.getButtonText().startsWith("(new"))
            {
                saveProjectAs();
            }
            else
            {
                saveProject(currentProjectFile_);
            }
        }
        else if (response == 2)
        {
            // NO
        }    
    }

    return cancelledValue;
}

bool MainComponent::handleWindowCloseRequest()
{
    // If the user initiates closing of the application, make sure they have the opportunity to save unsaved project changes or cancel
    return (promptForProjectSave() == CancellationEnum::NOT_CANCELLED);
}

void MainComponent::handlePlayButtonClick()
{
    // The button shouldn't be clickable if the sequence is already playing but check just in case
    if (! isPlaying_)
    {
        // Prep the playback buffer of MIDI events
        playbackBuffer_ = sourceBuffer_;
        
        // Set the time values needed to execute the sequence
        seqStartTime_ = Time::getMillisecondCounterHiRes() * 0.001;
        nextStartTime_ = seqStartTime_ + (1 / (tempoBPM_ / 60.0) * 4);

        // Reset the play line offset
        playLineX_Offset_ = 0.0;

        // Set the BPM for the current pass of the sequence
        thisPassBPM_ = tempoBPM_;

        // Call applyPendingChanges to alert each track that the sequence is now playing
        for (auto tI = 0; tI < tracks_.size(); ++tI)
        {
            tracks_[tI]->applyPendingChanges(true);
        }

        // Start the high resolution timer with an interval of 1ms to initiate play of the MIDI events
        HighResolutionTimer::startTimer(1);
        
        isPlaying_ = true;
    }

    updateMasterComponentsEnabledState();

    resetSelectedStep();
}

void MainComponent::handleStopButtonClick()
{
    // The button shouldn't be clickable if the sequence isn't playing but check just in case
    if (isPlaying_)
    {
        // Stop the high resolution timer to end sending of MIDI events
        HighResolutionTimer::stopTimer();

        // Iterate through the remaining messages in the playbackBuffer_ and send any NOTE OFFs to ensure
        // all notes are off, particularly for any synths don't honor the all notes off message
        MidiBuffer::Iterator iterator(playbackBuffer_);
        while (iterator.getNextEvent(bufferOutputMessage_, playbackSampleNumber_))
        {
            if (bufferOutputMessage_.isNoteOff())
                midiOutPtr_->sendMessageNow(bufferOutputMessage_);
        }

        // Clear necessary flags and variables
        isPlaying_ = false;
        playbackBuffer_.clear();
        previousSampleNumber_ = 0;
        playLineX_Offset_ = 0.0;
        
        // Send the all notes off MIDI message on the MIDI channel for each track to ensure the end of any NOTE ONs and call applyPendingChanges to alert
        // the tracks that the sequence is no longer playing
        for (auto tI = 0; tI < tracks_.size(); ++tI)
        {
            MidiMessage allNotesOff = MidiMessage::allNotesOff(tracks_[tI]->getMidiChannel());
            midiOutPtr_->sendMessageNow(allNotesOff);
            tracks_[tI]->applyPendingChanges(false);
        }
    }

    updateMasterComponentsEnabledState();

    resetSelectedStep();
}

void MainComponent::stepSelectionGoToPrevious()
{
    if (selectedStepPtr_ != nullptr)
    {
        // Change the selection to the next step in the track, wrapping around to the last valid step if the current selection
        // is the first step in the track
        auto targetStepIndex = selectedStepPtr_->getStepIndex() - 1;
        if (targetStepIndex < 0)
            targetStepIndex = (tracks_.at(selectedStepPtr_->getOwnerTrackIndex())->getNumSteps() - 1);
        tracks_.at(selectedStepPtr_->getOwnerTrackIndex())->setStepSelected(targetStepIndex);
    }
}

void MainComponent::stepSelectionGoToNext()
{
    if (selectedStepPtr_ != nullptr)
    {
        // Change the selection to the next step in the track, wrapping around to the first step if the current selection
        // is the last valid step in the track
        auto targetStepIndex = selectedStepPtr_->getStepIndex() + 1;
        if (targetStepIndex >= tracks_.at(selectedStepPtr_->getOwnerTrackIndex())->getNumSteps())
            targetStepIndex = 0;
        tracks_.at(selectedStepPtr_->getOwnerTrackIndex())->setStepSelected(targetStepIndex);
    }
}

void MainComponent::stepSelectionGoToPreviousTrack()
{
    if (selectedStepPtr_ != nullptr)
    {
        // Get the target track and change the selection to the first step of that track
        auto targetTrackIndex = selectedStepPtr_->getOwnerTrackIndex() - 1;
        if (targetTrackIndex < 0)
            targetTrackIndex = (static_cast<int>(tracks_.size()) - 1);

        tracks_.at(targetTrackIndex)->setStepSelected(0);
    }
}

void MainComponent::stepSelectionGoToNextTrack()
{
    if (selectedStepPtr_ != nullptr)
    {
        // Get the target track and change the selection to the first step of that track
        auto targetTrackIndex = selectedStepPtr_->getOwnerTrackIndex() + 1;
        if (targetTrackIndex >= tracks_.size())
            targetTrackIndex = 0;

        tracks_.at(targetTrackIndex)->setStepSelected(0);
    }
}

bool MainComponent::keyPressed(const KeyPress& key, Component* originatingComponent)
{
    // Allow the arrow keys on the computer keyboard to change the selected step
    auto consumed = false;
    if (key.isKeyCode(KeyPress::leftKey))
    {
        stepSelectionGoToPrevious();
        consumed = true;
    }
    else if (key.isKeyCode(KeyPress::rightKey))
    {
        stepSelectionGoToNext();
        consumed = true;
    }
    else if (key.isKeyCode(KeyPress::upKey))
    {
        stepSelectionGoToPreviousTrack();
        consumed = true;
    }
    else if (key.isKeyCode(KeyPress::downKey))
    {
        stepSelectionGoToNextTrack();
        consumed = true;
    }

    return consumed;
}

void MainComponent::handleStepSelectPreviousButtonClick()
{
    stepSelectionGoToPrevious();
}

void MainComponent::handleStepSelectNextButtonClick()
{
    stepSelectionGoToNext();
}

void MainComponent::handleStepSelectPreviousTrackButtonClick()
{
    stepSelectionGoToPreviousTrack();
}

void MainComponent::handleStepSelectNextTrackButtonClick()
{
    stepSelectionGoToNextTrack();
}

void MainComponent::handleTrackCharacteristicsChanged()
{
    // When any characteristics of a track change, udpate the source buffer...
    updateSourceMidiBuffer();

    // ...and change the step selection if the number of steps for the track changed such that the selected step is no longer valid
    if (selectedStepPtr_ != nullptr)
    {
        if (selectedStepPtr_->getStepIndex() >= tracks_[selectedStepPtr_->getOwnerTrackIndex()]->getNumSteps())
        {
            stepSelectionGoToNext();
        }
    }
    
    setUnsavedChangesFlag(true);
}

void MainComponent::updateAutoAdvanceSelectionState()
{
    autoAdvanceSelectionToggle_.setToggleState(! autoAdvanceSelectionToggle_.getToggleState(), dontSendNotification);
    
    // Change the image based on whether the toggle is checked or not
    if (autoAdvanceSelectionToggle_.getToggleState())
    {
        autoAdvanceSelectionToggle_.setImages(false, true, true, ImageCache::getFromMemory(BinaryData::RoundCheckboxChecked_png, BinaryData::RoundCheckboxChecked_pngSize), 1.0f, Colours::lightslategrey.darker(0.2f),
            ImageCache::getFromMemory(BinaryData::RoundCheckboxChecked_png, BinaryData::RoundCheckboxChecked_pngSize), 1.0f, Colours::lightslategrey,
            ImageCache::getFromMemory(BinaryData::RoundCheckboxChecked_png, BinaryData::RoundCheckboxChecked_pngSize), 1.0f, Colours::lightslategrey);
    }
    else
    {
        autoAdvanceSelectionToggle_.setImages(false, true, true, ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey.darker(0.2f),
            ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey,
            ImageCache::getFromMemory(BinaryData::RoundCheckboxUnchecked_png, BinaryData::RoundCheckboxUnchecked_pngSize), 1.0f, Colours::lightslategrey);
    }
}

void MainComponent::updateMasterComponentsEnabledState()
{
    // All of the master components except the tempo slider should be disabled when the sequence is playing
    midiOutputList_.setEnabled(! isPlaying_);
    midiOutputListLabel_.setEnabled(! isPlaying_);
    playButton_.setEnabled(! isPlaying_);
    stopButton_.setEnabled(isPlaying_);
    projectButton_.setEnabled(! isPlaying_);
    projectLabel_.setEnabled(! isPlaying_);
}

void MainComponent::updateStepEditComponentsEnabledState()
{
    // All of the step edit components should be disabled when there isn't a step selection
    // (when the sequence is playing and all of the tracks are active)
    stepEditGateSlider_.setEnabled(selectedStepPtr_ != nullptr);
    stepEditGateLabel_.setEnabled(selectedStepPtr_ != nullptr);
    stepEditVelocitySlider_.setEnabled(selectedStepPtr_ != nullptr);
    stepEditVelocityLabel_.setEnabled(selectedStepPtr_ != nullptr);
    stepEditNoteLabel_.setEnabled(selectedStepPtr_ != nullptr);
    stepEditNoteTitleLabel_.setEnabled(selectedStepPtr_ != nullptr);

    keyboardComponent_.setEnabled(selectedStepPtr_ != nullptr);
    restButton_.setEnabled(selectedStepPtr_ != nullptr);
    stepSelectNextButton_.setEnabled(selectedStepPtr_ != nullptr);
    stepSelectPreviousButton_.setEnabled(selectedStepPtr_ != nullptr);
    stepSelectNextTrackButton_.setEnabled(selectedStepPtr_ != nullptr);
    stepSelectPreviousTrackButton_.setEnabled(selectedStepPtr_ != nullptr);
    autoAdvanceSelectionToggle_.setEnabled(selectedStepPtr_ != nullptr);
    autoAdvanceSelectionLabel_.setEnabled(selectedStepPtr_ != nullptr);
    stepEditSectionLabel_.setEnabled(selectedStepPtr_ != nullptr);

    // The keyboard component doesn't dim when it's disabled, so manually dim it
    keyboardComponent_.setAlpha(keyboardComponent_.isEnabled() ? 1.0f : 0.3f);

    // The image button for the selection toggle doesn't dim when it's disabled, so manually dim it
    autoAdvanceSelectionToggle_.setAlpha(autoAdvanceSelectionToggle_.isEnabled() ? 1.0f : 0.3f);
}

void MainComponent::sliderValueChanged(Slider* slider)
{
    if (slider == &tempoSlider_)
    {
        // Rotate the tempo dial if the sequence isn't playing, otherwise just dim it to indicate there's a pending change,
        // but don't rotate it yet - it will be rotated at the next start of measure when the new tempo takes effect
        if (!isPlaying_)
            rotateTempoDialImage();
        else
            tempoDialImage_.setAlpha(0.6f);

        tempoBPM_ = slider->getValue();

        updateSourceMidiBuffer();

        setUnsavedChangesFlag(true);
    }
    else if (slider == &stepEditVelocitySlider_)
    {
        if (selectedStepPtr_ != nullptr)
        {
            // Update the velocity for the selected step and update the source buffer since a step changed
            selectedStepPtr_->setVelocity(static_cast<int>(stepEditVelocitySlider_.getValue()));
            updateSourceMidiBuffer();

            setUnsavedChangesFlag(true);
        }
    }
    else if (slider == &stepEditGateSlider_)
    {
        if (selectedStepPtr_ != nullptr)
        {
            // Update the gate percent for the selected step and update the source buffer since a step changed
            selectedStepPtr_->setGatePercent(static_cast<int>(stepEditGateSlider_.getValue()));
            updateSourceMidiBuffer();

            setUnsavedChangesFlag(true);
        }
    }
}

void MainComponent::stepSelected(GriddleStep* selectedStep)
{
    // Handle notification of a step being selected
    // *********************************************

    // Tell the currently-selected step it's no longer selected
    if ((selectedStepPtr_ != nullptr) && (selectedStepPtr_ != selectedStep))
    {
        selectedStepPtr_->setStepSelected(false);
    }

    selectedStepPtr_ = selectedStep;

    // Update the step edit controls to reflect the newly-selected step values
    if (selectedStepPtr_->getNoteNumber() >= 0)
        stepEditNoteLabel_.setText(MidiMessage::getMidiNoteName(selectedStepPtr_->getNoteNumber(), true, true, 4), dontSendNotification);
    else
        stepEditNoteLabel_.setText("", dontSendNotification);

    stepEditVelocitySlider_.setValue(selectedStepPtr_->getVelocity(), dontSendNotification);
    stepEditGateSlider_.setValue(selectedStepPtr_->getGatePercent(), dontSendNotification);
}

void MainComponent::setMidiOutput(const juce::String& identifier)
{
    // Find the passed-in MIDI output in the available MIDI outputs list and set the midiOutPtr_ accordingly
    auto midiOutputs = MidiOutput::getAvailableDevices();
    for (auto i = 0; i < midiOutputs.size(); ++i)
    {
        if (midiOutputs[i].name == identifier)
        {
            midiOutPtr_ = MidiOutput::openDevice(midiOutputs[i].identifier);

            setUnsavedChangesFlag(true);
        }
    }

}

void MainComponent::resetSelectedStep(const bool forceClearCurrentSelection)
{
    // If the passed-in flag specifies the current selection should be force-cleared, unselect the step
    if (forceClearCurrentSelection)
    {
        if (selectedStepPtr_ != nullptr)
        {
            selectedStepPtr_->setStepSelected(false);
            selectedStepPtr_ = nullptr;
        }
    }

    // If the sequence is playing, change the selected step to a valid step or clear the selection
    // based on the active state of the tracks and the number of steps in track with the current selection
    if (isPlaying_)
    {
        if (selectedStepPtr_ != nullptr)
        {
            if (tracks_.at(selectedStepPtr_->getOwnerTrackIndex())->isActive())
            {
                selectedStepPtr_->setStepSelected(false);
                selectedStepPtr_ = nullptr;

                for (auto tI = 0; tI < tracks_.size(); ++tI)
                {
                    if (false == tracks_[tI]->isActive())
                    {
                        tracks_[tI]->setStepSelected(0);
                        break;
                    }
                }
            }
        }
        else
        {
            for (auto tI = 0; tI < tracks_.size(); ++tI)
            {
                if (false == tracks_[tI]->isActive())
                {
                    tracks_[tI]->setStepSelected(0);
                    break;
                }
            }
        }
    }
    else
    {
        // If the sequence is not playing and there's no current selection, select the first step of the first track
        if (selectedStepPtr_ == nullptr)
        {
            tracks_[0]->setStepSelected(0);
        }
    }

    updateStepEditComponentsEnabledState();
}

void MainComponent::updateSourceMidiBuffer()
{
    sourceBuffer_.clear();

    // Loop through the tracks and populate the MIDI buffer with MIDI events for the steps
    for (auto i = 0; i < tracks_.size(); ++i)
    {
        // Inactive tracks are not included in the MIDI buffer
        if (tracks_[i]->isActive())
        {
            int currentSamplePos = 0;

            // The number of notes to add for one measure depends on whether the tempo is doubled for the track
            int numNotes = tracks_[i]->getNumSteps() * tracks_[i]->getTempoMultiplier();

            // Calculate the smallest possible gate length in samples equivalent to 20ms
            int minGateLengthInSamples = static_cast<int>(20.0 / ((1 / bufferSampleRate_) * 1000));

            // Calcluate the number of samples beteween NOTE ON events based on the tempo and number of notes in the track
            int sampleIncr = static_cast<int>((1 / (tempoBPM_ / 60.0) * 4.0 * bufferSampleRate_) / numNotes);

            // Loop through the steps and add the MIDI events
            for (auto stepI = 0; stepI < numNotes; ++stepI)
            {
                auto stepIndex = stepI;

                // When the tempo is doubled for the track, repeat the step indexes
                if (stepIndex >= tracks_[i]->getNumSteps())
                    stepIndex -= tracks_[i]->getNumSteps();

                // Adjust the step index if the track is set to play the steps in reverse
                if (tracks_[i]->isFlipped())
                    stepIndex = (tracks_[i]->getNumSteps() - stepIndex - 1);

                // If the step isn't a rest, add the NOTE ON and NOTE OFF messages to the buffer
                if (tracks_[i]->getStep(stepIndex).getNoteNumber() >= 0)
                {
                    auto messageNoteOn = MidiMessage::noteOn(tracks_[i]->getMidiChannel(), tracks_[i]->getStep(stepIndex).getNoteNumber(), (uint8)tracks_[i]->getStep(stepIndex).getVelocity());

                    auto messageNoteOff = MidiMessage::noteOff(tracks_[i]->getMidiChannel(), tracks_[i]->getStep(stepIndex).getNoteNumber(), (uint8)0);

                    sourceBuffer_.addEvent(messageNoteOn, currentSamplePos);

                    // Calculate the note off sample position based on the gate percent and chopped state of the track
                    int gatePercent = tracks_[i]->getStep(stepIndex).getGatePercent();
                    if (tracks_[i]->isChopped())
                        gatePercent = 10;
                    int noteOffPos = currentSamplePos + static_cast<int>(sampleIncr * (gatePercent / 100.0) - 1);

                    // Enforce the calculated minimum gate length to ensure reliable note triggering
                    if (noteOffPos < (currentSamplePos + minGateLengthInSamples))
                        noteOffPos = currentSamplePos + minGateLengthInSamples;

                    sourceBuffer_.addEvent(messageNoteOff, noteOffPos);
                }
                                
                currentSamplePos += sampleIncr;
            }
        }

    }
}
void MainComponent::setUnsavedChangesFlag(const bool unsavedChanges)
{
    String currentProjectFileDisplayText = projectButton_.getButtonText();

    if (unsavedChanges && ! unsavedProjectChanges_)
    {
        // Indicate unsaved changes with an asterisk at the end of the project button text
        currentProjectFileDisplayText.append("*", 1);
        projectButton_.setButtonText(currentProjectFileDisplayText);
    }
    else if (! unsavedChanges)
    {
        // Clear the asterisk at the end of the project button text if there are no unsaved changes
        currentProjectFileDisplayText = currentProjectFileDisplayText.trimCharactersAtEnd("*");
        projectButton_.setButtonText(currentProjectFileDisplayText);
    }

    unsavedProjectChanges_ = unsavedChanges;
}

void MainComponent::rotateTempoDialImage()
{
    // Transform the tempo dial to align with the tempo slider setting
    auto rotParams = tempoSlider_.getRotaryParameters();
    auto percentOfRange = (tempoSlider_.getValue() - tempoSlider_.getMinimum()) / (tempoSlider_.getMaximum() - (tempoSlider_.getMinimum()));
    auto angle = (percentOfRange * (static_cast<double>(rotParams.endAngleRadians) - static_cast<double>(rotParams.startAngleRadians))) + rotParams.startAngleRadians - 0.5235987756;
    tempoDialImage_.setTransform(AffineTransform::rotation(static_cast<float>(angle), 600.0f, 90.0f));

    // Ensure the tempo dial image has full opacity in case it was dimmed for a pending change during playback
    tempoDialImage_.setAlpha(1.0);
}


void MainComponent::update()
{
    // Update the play line offset to properly animate the play lines on the tracks
    if (isPlaying_)
    {
        playLineX_Offset_ += static_cast<float>(((getMillisecondsSinceLastUpdate() * 0.001) / ((1 / (thisPassBPM_ / 60.0)) * 4.0)) * static_cast<float>(STEPS_DISPLAY_PIXEL_WIDTH));
        if (playLineX_Offset_ >= static_cast<float>(STEPS_DISPLAY_PIXEL_WIDTH))
            playLineX_Offset_ = static_cast<float>(STEPS_DISPLAY_PIXEL_WIDTH);
    }
    else
    {
        playLineX_Offset_ = 0.0f;
    }

    // If the start of the measure passed during playing, apply pending changes so the displayed track conditions are updated
    // and update the step selection and tempo dial rotation as needed
    if (startOfMeasurePassed_)
    {
        // Since update() is called from its own thread, use a MessageManagerLock
        const MessageManagerLock mmLock;
        for (auto tI = 0; tI < tracks_.size(); ++tI)
        {
            tracks_[tI]->applyPendingChanges(isPlaying_);
        }
        resetSelectedStep();
        rotateTempoDialImage();

        startOfMeasurePassed_ = false;
    }
}

//==============================================================================
void MainComponent::paint(Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
 
    // Draw some small separator bars
    g.setColour(GriddleDarkGray);
    Rectangle<float> separator1(0.0f, 5.0f, static_cast<float>(getWidth()), 5.0f);
    g.fillRect(separator1);
    Rectangle<float> separator2(0.0f, 190.0f, static_cast<float>(getWidth()), 5.0f);
    g.fillRect(separator2);
    Rectangle<float> separator3(0.0f, 600.0f, static_cast<float>(getWidth()), 5.0f);
    g.fillRect(separator3);
    Rectangle<float> separator4(0.0f, 790.0f, static_cast<float>(getWidth()), 5.0f);
    g.fillRect(separator4);

    // Draw the play lines for each track
    for (auto plI = 0; plI < playLines_.size(); ++plI)
    {
        // Only animate the play line on a track if the sequence is playing and the track is active on the current pass
        if (isPlaying_ && tracks_[plI]->isActive(true))
        {
            // Ensure the play line moves twice as fast and runs over the measure twice if the track is burnt
            float mult = (tracks_[plI]->isBurnt(true) ? 2.0f : 1.0f);
            int trackPlayLineX_Offset = static_cast<int>(playLineX_Offset_ * mult);
            if (trackPlayLineX_Offset >= STEPS_DISPLAY_PIXEL_WIDTH)
                trackPlayLineX_Offset -= STEPS_DISPLAY_PIXEL_WIDTH;

            // Ensure the play line moves in reverse over the track if the track is flipped
            if (tracks_[plI]->isFlipped(true))
                trackPlayLineX_Offset = STEPS_DISPLAY_PIXEL_WIDTH - trackPlayLineX_Offset;

            Rectangle<float> rect(240.0f + trackPlayLineX_Offset, static_cast<float>(tracks_[plI]->getY()), 3.0f, static_cast<float>(tracks_[plI]->getHeight()));
            playLines_[plI].setRectangle(Parallelogram<float>(rect));

            if (tracks_[plI]->isBurnt(true))
                playLines_[plI].setFill(Colours::darkred);
            else
                playLines_[plI].setFill(Colours::lightgrey);

            playLines_[plI].setVisible(true);
        }
        else
        {
            playLines_[plI].setVisible(false);
        }
    }
}

void MainComponent::resized()
{
    // The Griddle window does not currently support resizing
}
