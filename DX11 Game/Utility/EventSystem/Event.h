#pragma once

//Holds the list of possible events
enum class EVENTID {
	
	//UI
	//HUD
	ToolModeEvent,
	EnergyUpdateEvent,

	//input
	UIMouseInput,
	UIKeyInput,

	//UICam
	WorldOrthMatrixEvent,

	GamePauseEvent,
	GAmeSettingsEvent,
	//utitlity
	WindowSizeChangeEvent,
	QuitGameEvent,
	UpdateSettingsEvent
};

class Event {
public:

	//Constructor that sends event with data (cant be altered from the other side i believe)
	Event(EVENTID eventID, void* data) {
		this->data = data;
		this->eventID = eventID;
	}

	//Alternate constructor to just call an event proc without data
	Event(EVENTID eventID) {
		this->eventID = eventID;
		this->data = nullptr;
	}


	//Destructor
	~Event() { data = nullptr; }

	//Getters
	EVENTID GetEventID() const { return eventID; }
	void* GetData() { return data; }



private:

	EVENTID eventID;
	void* data;
};