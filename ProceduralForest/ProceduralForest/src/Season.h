#pragma once
#include <string>
enum class Season { Winter, Spring, Summer, Fall };

class SeasonHandler {
private:
	Season currentSeason = Season::Winter;
	bool newSeasonJustBegan = true;
	float seasonTime = 0.0f;

public:
	void UpdateSeason() {
		newSeasonJustBegan = true;
		seasonTime = 0.0f;

		switch (currentSeason) {
		case Season::Winter:
			currentSeason = Season::Spring;
			break;
		case Season::Spring:
			currentSeason = Season::Summer;
			break;
		case Season::Summer:
			currentSeason = Season::Fall;
			break;
		case Season::Fall:
			currentSeason = Season::Winter;
			break;
		}
	}

	std::string CurrentSeasonToString() {
		std::string season;
		switch (currentSeason) {
		case Season::Winter:
			season = "Winter";
			break;
		case Season::Spring:
			season = "Spring";
			break;
		case Season::Summer:
			season = "Summer";
			break;
		case Season::Fall:
			season = "Fall";
			break;
		}
		return season;
	}

	Season GetSeason() const { return currentSeason; }
	bool GetSeasonState() const { return newSeasonJustBegan; }
	void SetSeasonState(bool newState) { newSeasonJustBegan = newState; }
	void SetSeasonTime(float newTime) { seasonTime = newTime; }
	float GetSeasonTime() { return seasonTime; }
};

