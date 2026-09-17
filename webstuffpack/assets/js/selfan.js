


const load = () => {
  const request = new XMLHttpRequest();
  request.open("GET", "/resources/audios/button.mp3");
  request.responseType = "arraybuffer";
  request.onload = function() {
    let undecodedAudio = request.response;
    audio_ctx.decodeAudioData(undecodedAudio, (data) => buffer = data);
  };
  request.send();
}
const play = () => {
  const source = audio_ctx.createBufferSource();
  source.buffer = buffer;
  source.connect(audio_ctx.destination);
  source.start();
};
let object1=document.getElementById("animate");
let object2=document.getElementById("an2");
class vectorz{
	constructor(vectorzx, vectorzy){
	
	this.vectorzx=vectorzx;
	this.vectorzy=vectorzy;


	}
	reflectTopWall(){
		
		this.vectorzy=-this.vectorzy;

	}
	reflectLeftWall(){

		
		this.vectorzx=-this.vectorzx;
	}
	reflectRightWall(){
		
		this.vectorzx=-this.vectorzx;

	}
	reflectBottomWall(){

	
		this.vectorzy=-this.vectorzy;
	}
	add(vetor){
	this.vectorzy+=vetor.vectorzy;
	this.vectorzx+=vetor.vectorzx;

	}
	mult(factor){
	this.vectorzy*=factor;
	this.vectorzx*=factor;

	}
	getLength(){
	return Math.sqrt(((this.vectorzy)*(this.vectorzy))+((this.vectorzx)*(this.vectorzx)));

	}
	getNormalised(){
	let result= new vectorz(this.vectorzx,this.vectorzy);
	result.mult(1/result.getLength());
	return result;
	}
	static vectorzFromAtoB(vA,vB){
		let newVx= vB.vectorzx-vA.vectorzx;
		let newVy= vB.vectorzy-vA.vectorzy;
		return new vectorz(newVx,newVy);

	}
}

class Objecto{
	constructor(vetorpos,vetorspeed,htmlelement,e){
	
	this.pos=vetorpos;
	this.element=htmlelement;
	this.speed=vetorspeed;
	this.e=e;
	this.element.style.top =this.pos.vectorzx +"px";
	this.element.style.left = this.pos.vectorzy+"px";
 	
	}
	playAudioAsync(){
	play();
	}
	playAudio(){
	
//Then, attach the audio file to an AudioNode, and that AudioNode to the dac.

//Finally, play the sound.

audio.play();
	console.log("audio played");
	
	}
	dampenVector(){
	
		this.speed.mult(this.e);

		//this.speed.mult(1);

	}
	update(){
	this.pos.add(this.speed);
	this.element.style.top =this.pos.vectorzx +"px";
	this.element.style.left = this.pos.vectorzy+"px";
 	
	}


}
class Collider{

	constructor(width,height,x,y,planet,element,density){
		this.objectos = [];
		this.planet=planet;
		this.x=x;
		this.y=y;
		this.width=width;
		this.height=height;
		this.element= element;
		this.fluid=new FluidLayer(width,height,x,y,density);
	}
	addobject(objecto){

		console.log(objecto);
		this.objectos.push(objecto);
	}
	update(){
		let phys=new PhysicsEngine();
		this.planet.objecto.update();
		
		for(let m of this.objectos){
			let i=m.objecto;
			i.update();
			for(let l of this.objectos){
				if(!(m===l)){
					phys.applyElectroVectorzOneSided(m,l);
					phys.applyGravVectorzOneSided(m,l);
				}

			}
			phys.applyGravVectorzOneSided(m,this.planet);
			phys.applyDragVectorz(m,this.fluid)
			if(i.pos.vectorzx >= this.x+this.width-m.height){
				i.pos=new vectorz(this.x+this.width-m.height-1,i.pos.vectorzy)
				i.speed.reflectRightWall();
				i.dampenVector();
			}
			else if(i.pos.vectorzy >= this.y+this.height-m.width){
			
				i.pos=new vectorz(i.pos.vectorzx,this.y+this.height-m.width-1)
				i.speed.reflectBottomWall();
				i.dampenVector();
			}
			else if(i.pos.vectorzx <= this.x){


				i.pos=new vectorz(this.x+1,i.pos.vectorzy)
				i.speed.reflectLeftWall();
				i.dampenVector();
			}
			else if(i.pos.vectorzy <= this.y){
	
				i.pos=new vectorz(i.pos.vectorzx,this.y+1)
				i.speed.reflectTopWall();
				i.dampenVector();
		
			}
		}

	}
	getTotalEnergy(){
		let result=0;
		for(let i of this.objectos){
		let vel=i.objecto.speed.getLength();
		let mass= i.mass;
			result+=0.5*mass*vel*vel;
		

		}
		return result;
	}

}
class PhysicsEngine{
	
	constructor(){}
	getDistance(pVectorz1,pVectorz2){
		let diffx=(pVectorz2.vectorzx-pVectorz1.vectorzx);
		let diffy=(pVectorz2.vectorzy-pVectorz1.vectorzy);
		let prod1=(diffx*diffx);
		let prod2=(diffy*diffy);
		let product=(prod1+prod2);
		return Math.sqrt(product);

	}
	getGravIntensity(part1,part2){
		let distance=this.getDistance(part1.objecto.pos,part2.objecto.pos);
		
		let m1=part1.mass;
		let m2=part2.mass;
		let intensity= ((m1*m2*gravConst)/(distance*distance));
		if(distance < 50){

			intensity=0;
		}
		return intensity;
	}
	applyGravVectorzOneSided(part1,part2){
		let center1=part1.getCenter();
		let center2=part2.getCenter();
		let intensity=this.getGravIntensity(part1,part2);
		let nv1=vectorz.vectorzFromAtoB(center1,center2).getNormalised();
		nv1.mult(intensity);
		part1.accel(nv1);

	}
	applyRepelVectorzOnPos(vectorz1,part1){
		let center1=part1.getCenter();
		let intensity=50;
		let nv1=vectorz.vectorzFromAtoB(vectorz1,center1).getNormalised();
		nv1.mult(intensity);
		part1.accel(nv1);

	}
	
	getElectroIntensity(part1,part2){
		let distance=this.getDistance(part1.objecto.pos,part2.objecto.pos);
		
		let q1=part1.charge;
		let q2=part2.charge;
		let intensity= Math.abs(((q1*q2*electroConst)/(distance*distance)));
		if(distance < 50){

			intensity=0;
		}
		return intensity;
	}
	applyElectroVectorzOneSided(part1,part2){
		let charge1=part1.charge;
		let charge2=part2.charge;
		let center1=part1.getCenter();
		let center2=part2.getCenter();
		let intensity=this.getElectroIntensity(part1,part2);
		var nv1=vectorz.vectorzFromAtoB(center1,center2).getNormalised();
		if((charge1*charge2)<0){
		nv1.mult(intensity);
		}
		else{
		nv1.mult(-intensity);
		}
		part1.accel(nv1);
	}
	getAirDragIntensity(part1,fluid){
		let vel= part1.objecto.speed.getLength();
		let intensity= -1*(vel*vel*0.5*fluid.density*part1.cD*part1.frontal_area);
		
		return intensity;
	}
	applyDragVectorz(part1,fluid){
	let intensity=this.getAirDragIntensity(part1,fluid);
	let movementVec=part1.objecto.speed.getNormalised();
	movementVec.mult(intensity);
	part1.accel(movementVec);
	
	}

}
class FluidLayer{

	constructor(width,height,x,y,density){

		this.width=width;
		this.height=height;
		this.x=x;
		this.y=y;
		this.density=density;
	}

}
class Particle{

	constructor(objecto,mass,charge,cD){
		this.objecto = objecto;
		this.mass=mass;
		this.charge=charge;
		this.posInfo=objecto.element.getBoundingClientRect();
		this.width=this.posInfo.width;
		this.height=this.posInfo.height;
		this.frontal_area=this.width*this.height;
		this.cD=cD;
	}
	getCenter(){
	let x=this.objecto.pos.vectorzx;
	let y=this.objecto.pos.vectorzy;
	let centerX=x+(this.width*0.5);
	let centerY=y+(this.height*0.5);
	let result=new vectorz(centerX,centerY);
	return result;
	}
	accel(vctorz){
	vctorz.mult(1/this.mass);
	this.objecto.speed.add(vctorz);
	}
}

function getRandomNumber(min,max) {
if(min<max){
tmp=min;
min=max;
max=tmp;
}
  return Math.random() * (max - min) + min;

}
function getRandomInteger(min,max) {

  return Math.floor(getRandomNumber(min,max));

}
function getRandomVectorz(min,max) {

  return new vectorz(getRandomNumber(min,max),getRandomNumber(min,max));

}
function getRandomColorString(){

var color= "rgb("+getRandomNumber(0,255)+","+getRandomNumber(0,255)+","+getRandomNumber(0,255)+","+1+")";
console.log(color);
return color;
}
function getDiv(width,height,visibility){

var object= document.createElement("div");
object.setAttribute("style","position: absolute;background-color: "+getRandomColorString()+"; width: "+width+"px; height: "+height+"px; color: "+getRandomColorString()+";");
object.innerHTML="RELEASE ME";
object.style.visibility=visibility;
document.body.appendChild(object);
return object;
}
function getRandomDiv(maxsize,visibility){

var object= document.createElement("div");
object.setAttribute("style","position: absolute;background-color: "+getRandomColorString()+"; width: "+getRandomNumber(50,maxsize).toString()+"px; height: "+getRandomNumber(50,maxsize).toString()+"px; color: "+getRandomColorString()+";");
object.innerHTML="RELEASE ME";
object.style.visibility=visibility;
document.body.appendChild(object);
return object;
}
function getRandomRect(minx,miny,maxx,maxy,minvx,minvy,maxvx,maxvy,element,e) {
  return new Objecto(getRandomVectorz(getRandomNumber(minx,maxx),getRandomNumber(miny,maxy)),getRandomVectorz(getRandomNumber(minvx,maxvx),getRandomNumber(minvy,maxvy)),element,e);
}

function getExactPosRect(x,y,minvx,minvy,maxvx,maxvy,element,e) {
  return new Objecto(getRandomVectorz(x,y),getRandomVectorz(getRandomNumber(minvx,maxvx),getRandomNumber(minvy,maxvy)),element,e);
}

function generateRandRect(minx,miny,maxx,maxy,minvx,minvy,maxvx,maxvy,maxsize,e){

return getRandomRect(minx,miny,maxx,maxy,minvx,minvy,maxvx,maxvy,getRandomDiv(maxsize,"visible"),e);

}
function generateExactRect(x,y,minvx,minvy,maxvx,maxvy,maxsize,e){

return getExactPosRect(x,y,minvx,minvy,maxvx,maxvy,getRandomDiv(maxsize,"visible"),e);

}
function generateRandMass(objecto,mass,charge){

return new Particle(objecto,mass,charge,1);

}
function addLotsOfObjectsToCollider(collider,ammount,size){
	for(let i=0;i<ammount;i++){
		console.log("it added");
		collider.addobject(generateRandMass(generateRandRect(collider.x,collider.y, collider.height+collider.x,collider.width+collider.y,-speeds,-speeds,speeds,speeds,size,getRandomNumber(0,1)),mass,getRandomNumber(-charge,charge)));
		
	}


}
const  myMove = () => {
  load();
  let id = null;
  clearInterval(id);
  id = setInterval(frame, 5);
function frame(){
	col.update();
}	
};
const getMaus = (e) => {
		
		col.addobject(generateRandMass(generateExactRect(e.clientX,e.clientY,-speeds,-speeds,speeds,speeds,110,getRandomNumber(0,1)),mass,getRandomNumber(-charge,charge)));
};
const getKey = (e) => {
			if(e.keyCode == 68){
			for(let e of col.objectos){
				e.objecto.element.remove();
				e=null;
			}
			col.objectos=[];
			alert("OBJETOS APAGADOS!!!!");
			}
			if(e.keyCode == 83){
			for(let e of col.objectos){
				e.objecto.speed.vectorzx= 0;
				e.objecto.speed.vectorzy= 0;
			}
			alert("OBJETOS PARADOS!!!!");
			}
			if(e.keyCode == 69){
			alert("ENERGIA TOTAL: "+col.getTotalEnergy()+"J!!!!!!!!");
			}
};
let colwidth= 400;
let colheight=400;
let speeds=100;
let mass=1000;
let charge=2000;
let audio_ctx = new AudioContext();
let buffer = null;
let mousePos={
		x:0,
		y:0
		};
let gravConst=0.01
let planetMass=200000
let electroConst=0.01

var colobject= document.createElement("div");
colobject.setAttribute("id","colliderbox");
colobject.setAttribute("style","opacity: 0.6;position: absolute;background-color: "+getRandomColorString()+"; width: "+colwidth+"px; height: "+colheight+"px;");
document.body.appendChild(colobject);
var posInfo=colobject.getBoundingClientRect();
let colx= posInfo.x;
let coly= posInfo.y;
let planet= new Particle(new Objecto(new vectorz(1000,colwidth*0.5),new vectorz(0.00001,0.00001),getDiv(50,50,"hidden"),0),planetMass,4,1);
let col=new Collider(colheight,colwidth,colx,coly,planet,colobject,0.001);

addLotsOfObjectsToCollider(col,1,110);
document.onmousedown= getMaus;
document.onkeydown= getKey;
document.body.addEventListener("load",myMove());
