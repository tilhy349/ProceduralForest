#pragma once
enum class Season { Winter, Spring, Summer, Fall };

struct SeasonHandler {
	
	Season currentSeason = Season::Winter;

	void UpdateSeason() {
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

	
};

