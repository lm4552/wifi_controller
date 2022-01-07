
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

	let tbl = d.cE('table');
	ct.aC(tbl);
	let tbl_row = d.cE('tr');
	tbl.appendChild(tbl_row);

	let cur_cell =  d.cE('th');
	tbl_row.appendChild(cur_cell);
	cur_cell =  d.cE('th');
	tbl_row.appendChild(cur_cell);
	cur_cell.appendChild( d.cTN('Maximum Brightness'));

	for(let i = 0; i < NUM_IOS; i++){

		tbl_row = d.cE('tr');
		tbl.appendChild(tbl_row);

		cur_cell =  d.cE('td');
		tbl_row.appendChild(cur_cell);
		cur_cell.appendChild( d.cTN('Channel ' +i));

		cur_cell =  d.cE('td');
		tbl_row.appendChild(cur_cell);
		let b_in = d.cE('input');
		b_in.id= 'maxBrightness_'+i.toString();
		b_in.type = 'number';
		b_in.value = parseInt((BRIGHTNESS[i]*100)/255);
		cur_cell.appendChild(b_in);
	}
	
	let b = d.cE('input');
	b.type = 'button';
	b.value = 'Change';
	b.onclick = change_brightness_all;
	ct.aC(b);

	ct.aC(d.cE('br'));
	ct.aC(d.cE('br'));

	var canvas = document.createElement('canvas');
	if (canvas.getContext) {
		var ctx = canvas.getContext('2d');
		draw_canvas(ctx);
	}
	ct.aC(canvas);
	canvas.addEventListener('mousemove', function(e) {
		getCursorPosition(canvas, e);
	});

	tbl = d.cE('table');
	ct.aC(tbl);

	tbl_row = d.cE('tr');
	tbl.appendChild(tbl_row);
	cur_cell =  d.cE('th');
	tbl_row.appendChild(cur_cell);
	cur_cell.appendChild( d.cTN( ''));
	cur_cell =  d.cE('th');
	tbl_row.appendChild(cur_cell);
	cur_cell.appendChild( d.cTN( 'Date and Time'));

	for(let i = 0; i < 8; i++){
		tbl_row = d.cE('tr');
		tbl.appendChild(tbl_row);
		cur_cell =  d.cE('td');
		tbl_row.appendChild(cur_cell);
		cur_cell.appendChild( d.cTN( 'Index '+(i+1).toString()));
		cur_cell =  d.cE('td');
		tbl_row.appendChild(cur_cell);

		let df_s = d.cE('input');
		df_s.id = 'date_'+i;
		df_s.type = 'date';
		df_s.valueAsDate = (SCHEDULE[0])[i].valid == true  ? new Date( (SCHEDULE[0])[i].end *1000 - ((new Date()).getTimezoneOffset()*60*1000) ): new Date( new Date().valueOf() - ((new Date()).getTimezoneOffset()*60*1000));
		df_s.className = 'end_date';
		let tf_s = d.cE('input');
		tf_s.type = 'time';
		tf_s.id = 'time_'+i;
		tf_s.valueAsDate = (SCHEDULE[0])[i].valid == true  ? new Date( (SCHEDULE[0])[i].end *1000 - ((new Date()).getTimezoneOffset()*60*1000) ): new Date( new Date().valueOf() - ((new Date()).getTimezoneOffset()*60*1000));
		tf_s.className = 'end_time';

		cur_cell.appendChild( df_s);
		cur_cell.appendChild( tf_s);
	}

	b = d.cE('input');
	b.type = 'button';
	b.value = 'Change';
	b.onclick = change_intervals_all;
	ct.aC(b);


	ct.aC(d.cE('hr'));
	ct.aC(d.cTN('State from ' + (new Date(CURRENT_TIME*1000))  ));

}


function draw_label(canvas,pos1,pos2,posText,text){
	if (canvas.getContext) {
		var ctx = canvas.getContext('2d');
		ctx.clearRect(0, 0, canvas.width, canvas.height);
		draw_canvas(ctx);
		ctx.beginPath();
		ctx.moveTo(pos1, 25);
		ctx.lineTo(pos1, 90);
		ctx.stroke();
		if(pos2 != undefined){
			ctx.beginPath();
			ctx.moveTo(pos2, 25);
			ctx.lineTo(pos2, 90);
			ctx.stroke();
		}
		
		ctx.fillText(text, posText, 15);
	}
}

function clear_labels(canvas){
	if (canvas.getContext) {
		var ctx = canvas.getContext('2d');
		ctx.clearRect(0, 0, canvas.width, canvas.height);
		draw_canvas(ctx);
	}
}


function getCursorPosition(canvas, event) {
    const rect = canvas.getBoundingClientRect();
    const x = event.clientX - rect.left;
    const y = event.clientY - rect.top;

	if(x < 50){
		draw_label(canvas,25,undefined,5,'off');
	}else if(x < 100){
		draw_label(canvas,25,50,20,'dimm up');
	}else if(x < 200){
		draw_label(canvas,100,50,70,'on');
	}else if(x < 250){
		draw_label(canvas,100,125,90,'dimm down');
	}else if(x < 350){
		draw_label(canvas,125,175,145,'off');
	}else if(x < 400){
		draw_label(canvas,175,200,170,'dimm up');
	}else if(x < 500){
		draw_label(canvas,200,250,220,'on');
	}else if(x < 550){
		draw_label(canvas,275,250,240,'dimm down');
	}else if(x < 600){
		draw_label(canvas,275,299,280,'off');
	}else{
		clear_labels(canvas);
	}

}

function draw_x_label(ctx,label,x,y){
	ctx.fillText(label, x-3, y);

	ctx.beginPath();
	ctx.moveTo(x, y-19);
	ctx.lineTo(x, y-11);
	ctx.stroke();
}

function draw_canvas(ctx){
	
	ctx.beginPath();
	//ctx.moveTo(299, 1);
	//ctx.lineTo(299, 90);
	ctx.moveTo(299, 90);
	ctx.lineTo(1, 90);
	ctx.lineTo(1, 1);
	ctx.stroke();
	
	ctx.beginPath();
	ctx.moveTo(25, 84);
	ctx.lineTo(50, 25);
	ctx.lineTo(100, 25);
	ctx.lineTo(125, 84);
	ctx.stroke();

	ctx.beginPath();
	ctx.moveTo(175, 84);
	ctx.lineTo(200, 25);
	ctx.lineTo(250, 25);
	ctx.lineTo(275, 84);
	ctx.stroke();

	draw_x_label(ctx,'1',25,105);
	draw_x_label(ctx,'2',50,105);
	draw_x_label(ctx,'3',100,105);
	draw_x_label(ctx,'4',125,105);
	draw_x_label(ctx,'5',175,105);
	draw_x_label(ctx,'6',200,105);
	draw_x_label(ctx,'7',250,105);
	draw_x_label(ctx,'8',275,105);
	
}
		


function change_brightness_all(){
	change_brightness([...Array(NUM_IOS).keys()]);
}

function change_brightness(io_lst){

	let io = io_lst.pop();
	
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
			if (io_lst.length == 0){
				window.location.reload();
			}else{
				change_brightness(io_lst);
			}
		}
	};

	var data = {};
	data.io = io.toString();
	data.val = val.toString();
	
	window.scrollTo(0,0);
	r.send(JSON.stringify(data));
}


function change_intervals_all(){

	let lst_of_io_iv_pairs = [];
	for(let i = 0; i < NUM_IOS; i++){
		for(let j = 0; j < 9; j++){
			lst_of_io_iv_pairs.push({io: i, iv: j});
		}
	}

	change_interval(lst_of_io_iv_pairs);
}


function change_interval(lst_of_io_iv_pairs){

	let io_iv_pair = lst_of_io_iv_pairs.pop();
	let io = io_iv_pair.io;
	let iv = io_iv_pair.iv;

	co.log(iv);
	co.log(io);

	let elems_lists = [];
	var mapping_type = {0:'ALWAYS_OFF' ,1: 'DIMM_UP_EXP',2: 'ALWAYS_ON',3: 'DIMM_DOWN_EXP',4: 'ALWAYS_OFF',5: 'DIMM_UP_EXP',6: 'ALWAYS_ON',7: 'DIMM_DOWN_EXP',8: 'ALWAYS_OFF' };
	var mapping_type_old = {0:2 ,1: 4,2: 1,3: 6,4: 2,5: 4,6: 1,7: 6,8: 2 };

	if (iv != 8 && iv != 0){
		elems_lists[0] = d.gEBI('date_'+(iv-1)).valueAsDate.valueOf();
		elems_lists[1] = d.gEBI('time_'+(iv-1)).valueAsDate.valueOf();
		elems_lists[2] = d.gEBI('date_'+iv).valueAsDate.valueOf();
		elems_lists[3] = d.gEBI('time_'+iv).valueAsDate.valueOf();
	}else if (iv == 0){
		elems_lists[0] = d.gEBI('date_0').valueAsDate.valueOf();
		elems_lists[1] = 0; 
		elems_lists[2] = d.gEBI('date_0').valueAsDate.valueOf();
		elems_lists[3] = d.gEBI('time_0').valueAsDate.valueOf();
	}else{
		elems_lists[0] = d.gEBI('date_'+(iv-1)).valueAsDate.valueOf();
		elems_lists[1] = d.gEBI('time_'+(iv-1)).valueAsDate.valueOf();
		elems_lists[2] = d.gEBI('date_0').valueAsDate.valueOf();
		elems_lists[3] = 86400;
	}
	elems_lists[4] = mapping_type[iv];
	elems_lists[5] = true;
	elems_lists[6] = 24;
	elems_lists[7] = 0;
	elems_lists[8] = 0;


	var url = window.location.href;

	var r = new XMLHttpRequest();
	r.open("POST", url);

	r.setRequestHeader("Accept", "application/json");
	r.setRequestHeader("Content-Type", "application/json");

	r.onreadystatechange = function () {
		if (r.readyState === 4) {
			if(lst_of_io_iv_pairs.length == 0){
				window.location.reload();
			}else{
				change_interval(lst_of_io_iv_pairs);
			}
		}
	};

	var data = {};
	data.channel = io.toString();
	data.interval = iv.toString();
	data.start = (elems_lists[0]+elems_lists[1]+ ((new Date()).getTimezoneOffset()*60*1000) ).toString();
	data.end= (elems_lists[2]+elems_lists[3]+ ((new Date()).getTimezoneOffset()*60*1000) ).toString();
	data.type = elems_lists[4].toString();
	data.repeat = elems_lists[5].toString();
	data.delta_repetition = (((elems_lists[6])*60*60) + ((elems_lists[7])*60)+ (elems_lists[8])   - Math.round((elems_lists[3] - elems_lists[1])/1000)   ).toString();

	data.start = data.start.substring(0,data.start.length-3);
	data.end = data.end.substring(0,data.end.length-3);

	window.scrollTo(0,0);
	r.send(JSON.stringify(data));
}

