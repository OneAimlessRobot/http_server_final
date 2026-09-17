var navigation_sound = "/webcrookedstuff/.sfx/.ui.wav"

var selection_sound = "/webcrookedstuff/.sfx/.ui2.wav"

var the_audio = new Audio(navigation_sound);

var the_audio_2 = new Audio(selection_sound);



const list_of_matches = document.querySelectorAll(".ref_for_dir");

const list_of_matches_2 = document.querySelectorAll(".ref_for_file");


for (let i = 0; i < list_of_matches.length; i++) {
     list_of_matches[i].addEventListener("click", function(event) {
		event.preventDefault();

		const destination = this.href;

		the_audio.currentTime = 0;

		the_audio.onended = function() {
			window.location.href = destination;
		};

		the_audio.play();
     });
 }

for (let i = 0; i < list_of_matches_2.length; i++) {
     list_of_matches_2[i].addEventListener("click", function(event) {
		event.preventDefault();

		const destination = this.href;

		the_audio_2.currentTime = 0;

		the_audio_2.onended = function() {
			window.location.href = destination;
		};

		the_audio_2.play();
     });
 }

the_audio.load();
the_audio_2.load();
