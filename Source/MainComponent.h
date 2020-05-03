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

#pragma once

#include <JuceHeader.h>

#include "GriddleTrack.h"

//==============================================================================
/*
    This component lives inside our window and holds all of the child components
    for the Griddle application
*/
class MainComponent : public AnimatedAppComponent, 
                      public HighResolutionTimer, 
                      public MidiKeyboardStateListener, 
                      public KeyListener, 
                      public Slider::Listener, 
                      public GriddleStep::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();
    //==============================================================================
    void paint(Graphics&) override;
    void resized() override;

    /** Updates the animated play lines and other GUI elements during playback

        This is an override of the AnimatedAppComponent method.
    */
    void update() override;
    
    /** Timer callback that manages real-time transmission of MIDI events for the sequence

        This is an override of the HighResolutionTimer method.
    */
    void hiResTimerCallback() override;

    /** Handles NOTE ON events from the step edit MIDI keyboard

        This is an override of the MidiKeyboardStateListener method.
    */
    void handleNoteOn(MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;

    /** Handles NOTE OFF events from the step edit MIDI keyboard

        This is an override of the MidiKeyboardStateListener method.
    */
    void handleNoteOff(MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;

    /** Handles values changes for the Slider components on GUI (Tempo, Velocity & Gate Percent)

        This is an override of the Slider::Listener method.
    */
    void sliderValueChanged(Slider* slider) override;

    /** Callback made when a GriddleStep gets selected 

        This is an override of the GriddleStep::Listener method.

        A pointer to the selected step is passed-in.
    */
    void stepSelected(GriddleStep* selectedStep) override;

    /** Handles key press events from the computer keyboard for changing the selected step

        This is an override of the KeyListener method.
    */
    bool keyPressed(const KeyPress& key, Component* originatingComponent) override;

    /** Ensures the user has the opportunity to save any unsaved changes before the application closes
    
        This method is for the MainWindow to call when the user is attempts to close the application
    */
    bool handleWindowCloseRequest();

    /** Callback registered with a GriddleTrack to be notified that track characteristics have changed
    *
    *   When track characteristics change, the MainComponent updates the source buffer accordingly and handles changes to the project
    */
    void handleTrackCharacteristicsChanged();

private:
    //==============================================================================
    // MIDI Output Variables
    std::unique_ptr<MidiOutput> midiOutPtr_;
    MidiBuffer playbackBuffer_;
    MidiBuffer sourceBuffer_;
    MidiMessage bufferOutputMessage_;
    //==============================================================================

    //==============================================================================
    // Playback Variables
    int previousSampleNumber_;
    double seqStartTime_;
    double nextStartTime_;
    double bufferSampleRate_;
    double tempoBPM_;
    double thisPassBPM_;
    int seqStartFrameCount_;
    bool isPlaying_;
    bool startOfMeasurePassed_;
    int playbackSampleNumber_;
    //==============================================================================

    //==============================================================================
    // Project File Variables
    File currentProjectFile_;
    bool unsavedProjectChanges_;
    var trackDefaultData_;
    var loadedProject_;
    //==============================================================================

    //==============================================================================
    // Animated Play Line Variables
    float playLineX_Offset_;
    std::array<DrawableRectangle, 4> playLines_;
    //==============================================================================
    
    // Pointer to the currently selected GriddleStep object
    GriddleStep* selectedStepPtr_;

    // Array of GriddleTracks for the sequence
    std::array<std::shared_ptr<GriddleTrack>, 4> tracks_;

    //==============================================================================
    // GUI Components
    ImageComponent titleImage_;
    ImageComponent subtitleImage_;

    TextButton projectButton_;
    PopupMenu projectMenu_;
    Label projectLabel_;

    TextButton aboutButton_;
    TextButton tutorialVideoButton_;

    ComboBox midiOutputList_;
    Label midiOutputListLabel_;

    ImageButton playButton_;
    ImageButton stopButton_;
    Slider tempoSlider_;
    ImageComponent tempoDialImage_;
    
    Label stepEditSectionLabel_;
    Label stepEditVelocityLabel_;
    Slider stepEditVelocitySlider_;
    Label stepEditGateLabel_;
    Slider stepEditGateSlider_;
    Label stepEditNoteLabel_;
    Label stepEditNoteTitleLabel_;

    MidiKeyboardState keyboardState_;
    MidiKeyboardComponent keyboardComponent_;

    ImageButton restButton_;
    
    ImageButton stepSelectPreviousButton_;
    ImageButton stepSelectNextButton_;
    ImageButton stepSelectPreviousTrackButton_;
    ImageButton stepSelectNextTrackButton_;
    ImageButton autoAdvanceSelectionToggle_;
    Label autoAdvanceSelectionLabel_;
    //==============================================================================

    //==============================================================================
    // Numeric Constants
    const int REST_NOTE_VALUE;
    const int STEPS_DISPLAY_PIXEL_WIDTH;
    //==============================================================================

    //==============================================================================
    // Colour Constants
    const Colour GriddleLightGray;
    const Colour GriddleDarkGray;
    const Colour GriddleSuperDarkGray;
    //==============================================================================
    
    //==============================================================================
    // Private Member Methods

    /** Reverts all of the step, track and master settings to defaults to start a new project */
    void startNewProject();

    /** This enumeration provides a return value type for the promptForProjectSave() method to indicate whether the user cancelled or not */
    enum class CancellationEnum
    {
        CANCELLED = 0,
        NOT_CANCELLED = 1
    };

    /** Checks for unsaved project changes and gives the opportunity for the user to save the current project, if needed

        @returns    a CancellationEnum that indicates whether the user cancelled the prompted Save operation or not
    */
    CancellationEnum promptForProjectSave();

    /** Saves the project contents to the passed-in File

        @param projectFile    The file object to which the project should be saved
    */
    void saveProject(File projectFile);

    /** Brings up a FileBrowserDialog for the user to save the current project to a file they specify */
    void saveProjectAs();

    /** Loads a Griddle Project file specified by the user with a FileBrowserDialog */
    void loadProject();

    /** Sets a flag indicating the presence of unsaved changes to the current project based on passed-in boolean value

        @param unsavedChanges    Pass true to indicate the project has unsaved changes, otherwise pass false
    */
    void setUnsavedChangesFlag(const bool unsavedChanges);

    /** Pops up the application's About window */
    void showAboutDialog();

    /** Opens a link to the tutorial video in the default browser */
    void openTutorialVideoLink();

    /**  Opens the MIDI output for the MIDI output device specified by the passed-in identifier

        @param identifier    The string identifier for the MIDI output to be used 
    */
    void setMidiOutput(const juce::String& identifier);
    
    /**  Handles the processing to be done when the Play button of the master section is clicked */
    void handlePlayButtonClick();

    /**  Handles the processing to be done when the Stop button of the master section is clicked */
    void handleStopButtonClick();

    /**  Handles the processing to be done when the Project button of the master section is clicked */
    void handleProjectButtonClick();

    /**  Handles clicks of the Rest button of the Step Edit section*/
    void handleRestButtonClick();

    /**  Handles clicks of the Select Previous Step button of the Step Edit section */
    void handleStepSelectPreviousButtonClick();

    /**  Handles clicks of the Select Next Step button of the Step Edit section */
    void handleStepSelectNextButtonClick();

    /**  Handles clicks of the Select Step in Previous Track button of the Step Edit section */
    void handleStepSelectPreviousTrackButtonClick();

    /**  Handles clicks of the Select Step in Next Track button of the Step Edit section */
    void handleStepSelectNextTrackButtonClick();

    /**  Changes the selected step to the previous step of the current track */
    void stepSelectionGoToPrevious();

    /**  Changes the selected step to the next step of the current track */
    void stepSelectionGoToNext();

    /**  Changes the selected step to the first step of the previous track from the current selection */
    void stepSelectionGoToPreviousTrack();

    /**  Changes the selected step to the first step of the previous track from the current selection */
    void stepSelectionGoToNextTrack();

    /**  Toggles the auto-advance selection state for the Step Edit section */
    void updateAutoAdvanceSelectionState();

    /**  Populates the source MidiBufffer based on the current step, track, and master settings */
    void updateSourceMidiBuffer();

    /** Enables or disables components in the master section based on whether the sequence is playing or not */
    void updateMasterComponentsEnabledState();

    /**  Enables or disables components in the Step Edit section based on whether there is a selected step  */
    void updateStepEditComponentsEnabledState();

    /**  Changes the step selection, as needed, based on various conditions of the sequence

        @param forceClearCurrentSelection    Pass true if the current selection should be force-cleared
                                             before resetting to a valid step selection. If false is passed,
                                             the method may leave the current selection unchanged if it is
                                             still valid.
    */
    void resetSelectedStep(const bool forceClearCurrentSelection = false);

    /**  Performs a tranform on the tempo dial image to rotate it to the current tempo slider value */
    void rotateTempoDialImage();
         
    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
