// Narcissa0.js - AMD/2023
// You are not allowed to change this file.


// MISCELLANEOUS FUNCTIONS

function rand(n) {		// random integer from 0 to n-1
	return Math.floor(Math.random() * n);
}

function distance(x1, y1, x2, y2) {
	let distx = Math.abs(x1 - x2);
	let disty = Math.abs(y1 - y2);
	return Math.ceil(Math.sqrt(distx*distx + disty*disty));
}

function mesg(m) {
	return alert(m);
}

function fatalError(m) {
	mesg("Fatal Error: " + m + "!");
	throw "Fatal Error!";
}

function div(a, b) {		// integer division
	return Math.floor(a/b);
}


// GAME IMAGES

const ACTOR_PIXELS_X = 16;
const ACTOR_PIXELS_Y = 16;

class GameImages {
	static loadOne(name)
	{
		GameImages.loading++;
		let im = new Image();
		GameImages[name] = im;
		im.src = GameImages.prefix + name + ".png";
		im.onload =
			function() { if( --GameImages.loading == 0 ) GameImages.next(); }	
	}	
	static loadAll(next) {
		let online = true;
		GameImages.prefix =
			online
				? "http://ctp.di.fct.unl.pt/lei/lap/projs/proj2223-3/files/resources/"
				: "resources/";
		GameImages.next = next;	// next is the action to start after loading the images
		GameImages.loading = 0;
		GameImages.loadOne("snakeHead"); 
		GameImages.loadOne("snakeBody"); 
		GameImages.loadOne("empty"); 
		GameImages.loadOne("shrub"); 
		GameImages.loadOne("berryBlue"); 
		GameImages.loadOne("berryBrown"); 
		GameImages.loadOne("berryCyan"); 
		GameImages.loadOne("berryDarkGreen"); 
		GameImages.loadOne("berryGreen"); 
		GameImages.loadOne("berryOrange"); 
		GameImages.loadOne("berryPurple"); 
		GameImages.loadOne("berryRed"); 
		GameImages.loadOne("invalid"); 
	}
}

// GAME FACTORY

class GameFactory {
	static actorFromCode(code, x, y) {
		switch( code ) {
			case '.': return null;
			case 'B': return new Berry(x, y, "berryBlue");
			case 'C': return new Berry(x, y, "berryCyan");
			case 'b': return new Berry(x, y, "berryBrown");
			case 'G': return new Berry(x, y, "berryGreen");
			case 'g': return new Berry(x, y, "berryDarkGreen");
			case 'O': return new Berry(x, y, "berryOrange");
			case 'P': return new Berry(x, y, "berryPurple");
			case 'R': return new Berry(x, y, "berryRed");
			case 'H': return new Snake(x, y);
			case 'S': return new Shrub(x, y);
			default: return new Invalid(x, y);
		}
	}
}

// GAME MAPS

const WORLD_WIDTH = 70;
const WORLD_HEIGHT = 40;

const MAPS = Object.freeze([
[
	"......................................................................",
	"......................................................................",
	"..............................................gg......................",
	"...........B.....................O....................................",
	"......................................................................",
	"..........................................................GGgg........",
	"............................................P.........................",
	"......................................................................",
	"....................R..........................................C......",
	".....B.....................S..........................S...............",
	"...................................g..................................",
	"..............S.......................................................",
	"....S.................................................................",
	".......................H........................R........OOO..........",
	"....................................g.................................",
	"......................................................................",
	"...........S.............................................S...........S",
	"....................S.................................................",
	"..................................................G...................",
	"..........R......................R....................................",
	"...........................S..........................................",
	"..............S..........................................S............",
	"......................................................................",
	"..................B...................................................",
	"..................................G...............GGB.................",
	"......................................................................",
	"......................................................................",
	"......................................................................",
	"....................................B.................................",
	"............b.........................................................",
	"......................................................................",
	"..........................C...........................................",
	".....B.............................................P..................",
	"......................................................................",
	".................C....................................................",
	".....................................................B................",
	"......................................................................",
	"..BBBBBBBBBBBBBBBB....................................................",
	"......................................................................",
	"......................................................................",
 ]
]);
