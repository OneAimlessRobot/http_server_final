var Sound = (function() {

    var sound = {};

    if (Audio != undefined) {
        // sounds
        
        var audio = new Audio();
        var ext = "ogg";
        //if(!audio.canPlayType("audio/ogg")) ext = "mp3";

        sound.paddle0 = new Audio("sound/paddle0."+ext);
        sound.paddle1 = new Audio("sound/paddle1."+ext);
        sound.paddle2 = new Audio("sound/paddle2."+ext);
        sound.paddle3 = new Audio("sound/paddle3."+ext);
        sound.paddle4 = new Audio("sound/paddle4."+ext);
        sound.paddle5 = new Audio("sound/paddle4."+ext);
        sound.table0 = new Audio("sound/table0."+ext);
        sound.table1 = new Audio("sound/table1."+ext);
        sound.table2 = new Audio("sound/table2."+ext);
        sound.table3 = new Audio("sound/table3."+ext);
        sound.table4 = new Audio("sound/table4."+ext);
        sound.table5 = new Audio("sound/table5."+ext);

        sound.floor0 = new Audio("sound/floor0."+ext);
        sound.floor1 = new Audio("sound/floor1."+ext);

    }
    
    // setup
    var numOfChannels = 8;
    var audiochannels = [];
    for (var a=0;a<numOfChannels ;a++) {
        audiochannels[a] = new Array();
        if (Audio != undefined) {
            audiochannels[a]["channel"] = new Audio();
        }
        audiochannels[a]["finished"] = -1;
    }


    sound.playStaticSound = function ( id, vol ) {
        var volume = 1;
        if (vol != undefined) {
            volume = vol;
        }
        if (volume > 1) volume = 1;
        if (volume < 0) volume = 0;

        //volume = 0;
    
        for (var a=0;a<numOfChannels;a++) {
            var thistime = new Date();
            if (audiochannels[a]["finished"] < thistime.getTime()) {   // is this channel finished?
                audiochannels[a]["finished"] = thistime.getTime() + id.duration*1000;
                audiochannels[a]["channel"].src = id.src;
                audiochannels[a]["channel"].load();
                audiochannels[a]["channel"].volume = volume;
                audiochannels[a]["channel"].play();
                break;
            }
        }
    }

    return sound;

})();