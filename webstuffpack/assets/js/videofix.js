var interval = setInterval(function(){
          var countForVideo = document.getElementById('vid').readyState;
          if(countForVideo == 4){
            document.getElementById('vid').play();
            clearInterval(interval);
          }
        },2000);
