
window.onload = init;
var ct,NUM_IOS,NUM_INTERVALS;

let d = document;
let co = console;
d.cE = d.createElement;
d.cTN = d.createTextNode;
d.gEBI = d.getElementById;

function init(){

	ct = d.gEBI('container');
	ct.aC = ct.appendChild;

	for(let i = 0; i < NUM_IOS; i++){

		let ttl = d.cE('h2');
		ttl.appendChild( d.cTN('Channel ' +i));
		ct.aC(ttl);

		let t = d.cE('table');
		ct.aC(t);
		let r = t.insertRow();
		let c = r.insertCell();
		c.appendChild(d.cTN('Maximum Brightness'));
		c = r.insertCell();
		let b_in = d.cE('input');
		b_in.id= 'maxBrightness_'+i.toString();
		b_in.type = 'number';
		b_in.value = parseInt((BRIGHTNESS[i]*100)/255);
		b_in.min = 0;
		b_in.max = 100;
		c.appendChild(b_in);
		c = r.insertCell();
		c.appendChild(d.cTN('%'));

		r = t.insertRow();
		c = r.insertCell();
		c.appendChild(d.cTN(' '));
		c = r.insertCell();
		let b = d.cE('input');
		b.type = 'button';
		b.value = 'Apply';
		b.setAttribute('onclick','change_brightness(\\''+i+'\\')');
		c.appendChild(b);

		for(let j = 0; j< NUM_INTERVALS; j++){
			build_interval(j,i);
		}
	}

	let loader = d.cE('div');
	loader.id="loader";
	let loader_wrapper = d.cE('div');
	loader_wrapper.id="loader_wrapper";
	loader_wrapper.appendChild(loader);
	loader_wrapper.className = "hide";
	ct.aC(loader_wrapper);

	ct.aC(d.cE('hr'));
	ct.aC(d.cTN('State from ' + (new Date(CURRENT_TIME*1000))  ));

}

function build_interval(iv,io ){

	let iv_div = d.gEBI('io_'+ io + '_iv_'+iv);

	if (iv_div == undefined ){
		iv_div = d.cE('div');
		iv_div.id = 'io_'+ io + '_iv_'+iv;
		ct.append(iv_div);
	}else{
		iv_div.innerHTML = '';
	}
	iv_div.aC = iv_div.appendChild;

	if( (SCHEDULE[io])[iv].valid == false && iv != 0 ){
		iv_div.className = 'hide';
	}
		

	let ttl2 = d.cE('h3');
	if( (SCHEDULE[io])[iv].valid){
		if(CURRENT_TIME > (SCHEDULE[io])[iv].start && CURRENT_TIME <  (SCHEDULE[io])[iv].end){
			ttl2.appendChild( d.cTN('Interval ' +iv+ ' ACTIVE'));
		}else{
			ttl2.appendChild( d.cTN('Interval ' +iv));
		}
	}else{
		ttl2.appendChild( d.cTN('Interval ' +iv));
	}
	iv_div.aC(ttl2);

	let t = d.cE('table');
	iv_div.aC(t);
	t.iR = t.insertRow;

	let r = t.iR();
	let c = r.insertCell();
	c.appendChild(d.cTN('Start'));
	c = r.insertCell();
	let df_s = d.cE('input');
	df_s.type = 'date';
	df_s.valueAsDate = (SCHEDULE[io])[iv].valid == true  ? new Date( (SCHEDULE[io])[iv].start *1000 - ((new Date()).getTimezoneOffset()*60*1000) ): new Date( new Date().valueOf() - ((new Date()).getTimezoneOffset()*60*1000));
	df_s.className = 'start_date';
	let tf_s = d.cE('input');
	tf_s.type = 'time';
	tf_s.valueAsDate = (SCHEDULE[io])[iv].valid == true  ? new Date( (SCHEDULE[io])[iv].start *1000 - ((new Date()).getTimezoneOffset()*60*1000) ): new Date( new Date().valueOf() - ((new Date()).getTimezoneOffset()*60*1000));
	tf_s.className = 'start_time';
	c.appendChild(df_s);
	c.appendChild(tf_s);

	r = t.iR();
	c = r.insertCell();
	c.appendChild(d.cTN('End'));
	c = r.insertCell();
	df_s = d.cE('input');
	df_s.type = 'date';
	df_s.valueAsDate = (SCHEDULE[io])[iv].valid == true  ? new Date( (SCHEDULE[io])[iv].end *1000 - ((new Date()).getTimezoneOffset()*60*1000) ): new Date( new Date().valueOf() - ((new Date()).getTimezoneOffset()*60*1000));
	df_s.className = 'end_date';
	tf_s = d.cE('input');
	tf_s.type = 'time';
	tf_s.valueAsDate = (SCHEDULE[io])[iv].valid == true  ? new Date( (SCHEDULE[io])[iv].end *1000 - ((new Date()).getTimezoneOffset()*60*1000) ): new Date( new Date().valueOf() - ((new Date()).getTimezoneOffset()*60*1000));
	tf_s.className = 'end_time';
	c.appendChild(df_s);
	c.appendChild(tf_s);

	r = t.iR();
	c = r.insertCell();
	c.appendChild(d.cTN('Repeat'));
	c = r.insertCell();
	df_s = d.cE('input');
	df_s.type = 'checkbox';
	df_s.checked = (SCHEDULE[io])[iv].repeat;
	df_s.className = 'repeat';
	c.appendChild(df_s);
	c.appendChild(d.cTN(' after '));
	tf_s = d.cE('input');
	tf_s.type = 'number';
	tf_s.min = 0;
	tf_s.max = 840;
	tf_s.size = 2;
	tf_s.value = (SCHEDULE[io])[iv].repeat == true  ? new Date( ((SCHEDULE[io])[iv].delta_repetition *1000)  ).getUTCHours(): 0;
	tf_s.className = 'repeat_time_hours';
	c.appendChild(tf_s);
	c.appendChild(d.cTN(' h '));
	tf_s = d.cE('input');
	tf_s.type = 'number';
	tf_s.min = 0;
	tf_s.max = 60;
	tf_s.size = 2;
	tf_s.value = (SCHEDULE[io])[iv].repeat == true  ? new Date( ((SCHEDULE[io])[iv].delta_repetition *1000) ).getUTCMinutes(): 0;
	tf_s.className = 'repeat_time_minutes';
	c.appendChild(tf_s);
	c.appendChild(d.cTN(' m '));
	tf_s = d.cE('input');
	tf_s.type = 'number';
	tf_s.min = 0;
	tf_s.max = 60;
	tf_s.size = 2;
	tf_s.value = (SCHEDULE[io])[iv].repeat == true  ? new Date( ((SCHEDULE[io])[iv].delta_repetition *1000)  ).getUTCSeconds(): 0;
	tf_s.className = 'repeat_time_seconds';
	c.appendChild(tf_s);
	c.appendChild(d.cTN(' s '));
	

	r = t.iR();
	c = r.insertCell();
	c.appendChild(d.cTN('Type'));
	c = r.insertCell();
	let sel = d.cE('select');
	sel.className = 'type';
	sel.id = 'select_'+ io +'_'+ iv;
	let opt;
	let values = {'ALWAYS_ON' : 'always on', 'ALWAYS_OFF' : 'always off', 'DIMM_UP_LIN': 'dimm up linear', 'DIMM_UP_EXP':  'dimm up exponential', 'DIMM_DOWN_LIN': 'dimm down linear', 'DIMM_DOWN_EXP': 'dimm down exponential' };

	for (value in values){
		opt = d.cE('option');
		opt.value = value;
		opt.appendChild(d.cTN(values[value]));
		sel.appendChild(opt);
	}
	sel.selectedIndex =  (SCHEDULE[io])[iv].valid == true  ? (SCHEDULE[io])[iv].type -1 : 0;
	c.appendChild(sel);

	r = t.iR();
	c = r.insertCell();
	c.appendChild(d.cTN(' '));
	c = r.insertCell();
	let b = d.cE('input');
	b.type = 'button';
	b.value = 'Apply';
	b.setAttribute('onclick','change_interval(\\''+iv+'\\',\\''+io+'\\')' );
	c.appendChild(b);

	r = t.iR();
	c = r.insertCell();
	c.appendChild(d.cTN(' '));
	c = r.insertCell();

	r = t.iR();
	c = r.insertCell();
	c.appendChild(d.cTN(' '));
	c = r.insertCell();
	b = d.cE('input');
	b.type = 'button';
	b.value = '+';
	b.setAttribute('onclick','unhide_interval(\\''+(iv+1)+'\\',\\''+io+'\\')' );

	c.appendChild(b);
}

function unhide_interval(iv,io){
	try{ d.gEBI('io_'+ io + '_iv_'+iv).className = '';} catch {};
}
		

function change_brightness(io){

	let val = parseInt((d.gEBI('maxBrightness_'+ io.toString()).value *255)/100);
	val = val > 255 ? 255: val;
	val = val < 0 ? 0: val;

	var url = window.location.href + 'brightness/';

	var r = new XMLHttpRequest();
	r.open("POST", url);

	r.setRequestHeader("Accept", "application/json");
	r.setRequestHeader("Content-Type", "application/json");

	r.onreadystatechange = function () {
	if (r.readyState === 4) {
		co.log(r.status);
		co.log(r.responseText);
		
	}
	window.location.reload();
	};

	var data = {};
	data.io = io.toString();
	data.val = val.toString();
	
	d.gEBI("loader_wrapper").className ="";
	window.scrollTo(0,0);
	r.send(JSON.stringify(data));
}


function change_interval(iv,io){
	co.log(iv);
	co.log(io);

	let iv_div = d.gEBI('io_'+ io + '_iv_'+iv);

	let class_names = ['start_date','start_time','end_date','end_time','type','repeat','repeat_time_hours','repeat_time_minutes','repeat_time_seconds']; 
	let elems_lists = [];
	for(let i = 0; i < class_names.length; i++){
		elems_lists.push(d.getElementsByClassName(class_names[i]));
	}

	for(let i = 0; i < elems_lists.length; i++){
		for(let j = 0; j < elems_lists[i].length; j++){
			if (iv_div.contains((elems_lists[i])[j])){
				elems_lists[i] = (elems_lists[i])[j];
				break;
			}
		}
	}

	co.log(elems_lists);
	co.log(elems_lists[0].valueAsDate.valueOf());
	co.log(elems_lists[1].valueAsDate.valueOf());
	co.log(elems_lists[4].selectedIndex+1);
	co.log(elems_lists[5].value);
	co.log(elems_lists[6].value);

	var url = window.location.href;

	var r = new XMLHttpRequest();
	r.open("POST", url);

	r.setRequestHeader("Accept", "application/json");
	r.setRequestHeader("Content-Type", "application/json");

	r.onreadystatechange = function () {
	if (r.readyState === 4) {
		co.log(r.status);
		co.log(r.responseText);
	}
	window.location.reload();
	};

	var mapping = {0:'ALWAYS_ON' ,1: 'ALWAYS_OFF',2: 'DIMM_UP_LIN',3: 'DIMM_UP_EXP',4: 'DIMM_DOWN_LIN',5: 'DIMM_DOWN_EXP' };

	var data = {};
	data.channel = io;
	data.interval = iv;
	data.start = (elems_lists[0].valueAsDate.valueOf()+elems_lists[1].valueAsDate.valueOf() + ((new Date()).getTimezoneOffset()*60*1000) ).toString();
	data.end= (elems_lists[2].valueAsDate.valueOf()+elems_lists[3].valueAsDate.valueOf() + ((new Date()).getTimezoneOffset()*60*1000) ).toString();
	data.type = (mapping[elems_lists[4].selectedIndex]).toString();
	data.repeat = elems_lists[5].checked.toString();
	data.delta_repetition = ((parseInt(elems_lists[6].value)*60*60) + (parseInt(elems_lists[7].value)*60)+ parseInt(elems_lists[8].value)).toString();

	data.start = data.start.substring(0,data.start.length-3);
	data.end = data.end.substring(0,data.end.length-3);

	d.gEBI("loader_wrapper").className ="";
	window.scrollTo(0,0);
	r.send(JSON.stringify(data));
}

