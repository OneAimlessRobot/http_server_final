/*var textFile = null,
  makeTextFile = function (text) {
    var data = new Blob([text], {type: 'text/plain'});

    // If we are replacing a previously generated file we need to
    // manually revoke the object URL to avoid memory leaks.
    if (textFile !== null) {
      window.URL.revokeObjectURL(textFile);
    }

    textFile = window.URL.createObjectURL(data);

    // returns a URL you can use as a href
    return textFile;
  };	

window.addEventListener("DOMContentLoaded", (event) => {

var venttitle = document.getElementById('venttitle');
var ventcontent = document.getElementById('ventcontent');
const submitbutton = document.getElementById('submitbutton');
    	if (submitbutton) {
  submitbutton.addEventListener('click', function () {
 
    var link = document.createElement('a');
    link.setAttribute('download', 'info.txt');
    link.href = makeTextFile("Titulo: \n"+venttitle.value+"\n"+"Conteudo: \n"+ventcontent.value+"\n");
    link.innerText="transfere";
    document.body.appendChild(document.createElement('br'));
    document.body.appendChild(link);
    document.body.appendChild(document.createElement('br'));


  }, false);
	}
	});
*/
function start(){

	let testText=document.getElementById("test");
	testText.title= "Teste";
	testText.textContent="TESTE TESTE TESTE???!!!"
	mesg("clicaste");
	
}


function sub (form) {
    var inputValue = form.inputbox.value;
    let showedtext=document.getElementById("textshower");
    showedtext.innerText=inputValue;
}

function testResults (form) {
    var inputValue = form.inputbox.value;
    alert ("You typed: " + inputValue);
}
