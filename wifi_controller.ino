
#include <HTTPSServer.hpp>
#include <SSLCert.hpp> 
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>

#include <WiFi.h>
#include "time.h"
#include <EEPROM.h>

// Include certificate data (see note above)
#include "certs/cert.h"
#include "certs/key.h"

// Include minified HTML,CSS and Javascript
#include "src/web/style.h"
#include "src/web/script.h"
#include "src/web/html.h"
#include "src/web/html_config.h"


#define DEBUG

#ifdef DEBUG
	#define DebugPrintln(a) (Serial.println(a))
	#define DebugPrint(a) (Serial.print(a))
	#define DebugBegin(a) (Serial.begin(a))
#else
	#define DebugPrintln(a)
	#define DebugPrint(a) 
	#define DebugBegin(a) 
#endif

#define RESET false

const uint8_t PIN_LAYOUT_IOS[] = {14};
#define NUM_IOS sizeof(PIN_LAYOUT_IOS) // maximum of 8 

#define NUM_INTERVALS 5

#define PWM_FREQ 5000
#define PWM_RES 8

#define BRIGHTNESS_TYPE uint8_t
#define BRIGHTNESS_TYPE_TWICE uint16_t
//#define BRIGHTNESS_TYPE_FOUR_TIMES uint32_t
#define MAX_BRIGHTNESS 255

#define REFRESH_TIME_IN_SECS (24*60*60) //one day

#define NTP_SERVER "de.pool.ntp.org"
#define GTM_OFFSET_SEC 7200
#define DAYLIGHT_OFFSET_SEC 0

#define DEVICE_NAME "WiFi Controller"

// pwm states 
enum{
	UNDEFINED_STATE,
	ALWAYS_ON,
	ALWAYS_OFF,
	DIMM_UP_LIN,
	DIMM_UP_EXP,
	DIMM_DOWN_LIN,
	DIMM_DOWN_EXP
};

// io type
enum{
	UNDEFINED_IO,
	PWM_IO
};

// operation modes
enum{
	UNDEFINED_MODE,
	MANUAL_MODE,
	AUTOMATIC_MODE
};

// device configuration modes
enum{
	NOT_YET_CONFIGURED,
	CONFIGURED
};

struct interval{
	time_t start = 0;
	time_t end = 0;
	uint8_t state_type = UNDEFINED_STATE;
	time_t delta_repetition = 0;
	bool repeat = false;
};

struct schedule {
	interval intv[NUM_INTERVALS];
	uint8_t io_type = UNDEFINED_IO;
	BRIGHTNESS_TYPE maximum_brightness = MAX_BRIGHTNESS;
	uint8_t operation_mode = UNDEFINED_MODE;
};


//TODO delete default values
struct wifi_config{
  char ssid[32] = "";
  char password[32] = "";
  bool is_set = false;
};

httpsserver::SSLCert * cert;
httpsserver::HTTPSServer * secureServer;

schedule s[NUM_IOS];
uint8_t deviceStatus = NOT_YET_CONFIGURED;
wifi_config wifi_cfg;
time_t lastNTPconnectTime; 
BRIGHTNESS_TYPE values_IOs[NUM_IOS] = {127};

void setup() {

	
	//TODO read wifi config here 
	//deviceStatus = CONFIGURED;

	DebugBegin(115200);
	DebugPrintln(String(MAX_BRIGHTNESS));

	init_SSLcert();
	delay(2000);

	if (RESET){
		save_configuration();
	}else{
		restore_configuration();
	}


	switch(deviceStatus){

		case NOT_YET_CONFIGURED:

			setup_wifi_ap();

			init_httpsServer_configuration();

			break;

		case CONFIGURED:

			connect_wifi();

			init_time();

			//offline_config();

			init_httpsServer();

			//read flash and config

			initialize_IOs();
			// setup IOs
			break;
	}
	DebugPrintln("done with setup");
}

 void loop() {
	switch (deviceStatus){

		case NOT_YET_CONFIGURED:
			// loop config server
			if (wifi_cfg.is_set){
				secureServer->loop();
				delay(1000);
				secureServer->loop();
				delay(100);
				ESP.restart();
			}else{
				secureServer->loop();
			}
			break;

		case CONFIGURED:
			// loop webinterface
			secureServer->loop();
			// loop io updates
			update_IOs();
			// check if intervals should be repeated
			update_interval_repetitions();

			refresh_time();
			break;
	}

	//check for reset
	// update time if neceassary
	//print_schedule();
 	//delay(200);
 	//DebugPrintln("main_loop!");

 }


 void restore_configuration(){

	DebugPrintln("restore config");

	int size_bytes = sizeof(wifi_config);
	size_bytes += sizeof(schedule) * NUM_IOS; 

	EEPROM.begin(size_bytes);
	int adr = 0;

	EEPROM.get(adr,wifi_cfg);
	adr += sizeof(wifi_config);
	EEPROM.get(adr,s);

	EEPROM.end();

	if (wifi_cfg.is_set){
		deviceStatus = CONFIGURED;
	}
 }

 void save_configuration(){

	DebugPrintln("save config");

	int size_bytes = sizeof(wifi_config);
	size_bytes += sizeof(schedule ) * NUM_IOS; 

	EEPROM.begin(size_bytes);
	int adr = 0;

	EEPROM.put(adr,wifi_cfg);
	adr += sizeof(wifi_config);
	EEPROM.put(adr,s);

	EEPROM.commit();
	EEPROM.end();
 }

 void initialize_IOs(){

	for(uint8_t i = 0; i < NUM_IOS; i++){

	    switch( s[i].io_type ){

	    	case PWM_IO: 
	    		initialize_PWM(i);
	    		break;

	    	case UNDEFINED_IO:
				//DebugPrintln("WARNING: undefined input type!");
				break;	

	    }

	}

}


void setup_wifi_ap(){

    DebugPrintln("Setting AP (Access Point)…");
    DebugPrintln(WiFi.softAP(DEVICE_NAME));

	delay(1000);

    IPAddress local_IP(192,168,1,1);
    IPAddress gateway(192,168,1,1);
    IPAddress subnet(255,255,255,0);
    DebugPrintln(WiFi.softAPConfig(local_IP, gateway, subnet));

    delay(1000);
  
    IPAddress IP = WiFi.softAPIP();
    DebugPrintln("AP IP address: ");
    DebugPrintln(IP);
    
}


void initialize_PWM(uint8_t ind){
    ledcAttachPin(PIN_LAYOUT_IOS[ind], ind);
    ledcSetup(ind, PWM_FREQ, PWM_RES);
	values_IOs[ind] = 127;
}

void update_IOs(){
	time_t current_time = time(nullptr);

	for(uint8_t i = 0; i < NUM_IOS; i++){

	    switch( s[i].io_type ){

	    	case PWM_IO: 

	    		//DebugPrintln("PWM IO");
	    		update_PWM(i,current_time);
	    		break;

	    	case UNDEFINED_IO:
				//DebugPrintln("WARNING: undefined input type!");
				break;	

	    }
	}
}


void update_PWM(uint8_t ind,time_t current_time){

	switch (s[ind].operation_mode){

		case MANUAL_MODE:
			ledcWrite(ind, s[ind].maximum_brightness);
			//DebugPrintln("MANUAL_MODE");
			break;

		case UNDEFINED_MODE:
			empty_func();
			break;

		case AUTOMATIC_MODE:
			//DebugPrintln("AUTOMATIC_MODE");

			int ind_intv = find_matching_interval(ind, current_time);

			time_t diff_interval,diff_current;
			BRIGHTNESS_TYPE_TWICE numerator,denominator; 
			BRIGHTNESS_TYPE current_brightness;

			if (ind_intv != -1){

				switch (s[ind].intv[ind_intv].state_type){

					case ALWAYS_ON:
						if (values_IOs[ind] != s[ind].maximum_brightness){
							ledcWrite(ind, s[ind].maximum_brightness);
							values_IOs[ind] = s[ind].maximum_brightness;
						}
						break;
					
					case ALWAYS_OFF:
						if (values_IOs[ind] != 0){
							ledcWrite(ind, 0);
							values_IOs[ind] = 0;
						}
						break;
					
					case DIMM_UP_LIN:
						
						diff_interval = s[ind].intv[ind_intv].end - s[ind].intv[ind_intv].start; 
						diff_current = current_time - s[ind].intv[ind_intv].start; 
						current_brightness = (BRIGHTNESS_TYPE) (((BRIGHTNESS_TYPE_TWICE) diff_current * (BRIGHTNESS_TYPE_TWICE) s[ind].maximum_brightness)/ (BRIGHTNESS_TYPE_TWICE) diff_interval); 
						ledcWrite(ind, current_brightness);
						break;
						
					case DIMM_UP_EXP:

						diff_interval = s[ind].intv[ind_intv].end - s[ind].intv[ind_intv].start; 
						diff_current = current_time - s[ind].intv[ind_intv].start; 
						
						//easy solution but has worse resolution (skips some values from 0 to 255 )
						current_brightness = (BRIGHTNESS_TYPE) (((BRIGHTNESS_TYPE_TWICE) diff_current * (BRIGHTNESS_TYPE_TWICE) s[ind].maximum_brightness)/ (BRIGHTNESS_TYPE_TWICE) diff_interval);
						current_brightness = (BRIGHTNESS_TYPE) ((BRIGHTNESS_TYPE_TWICE) (current_brightness * current_brightness) / (BRIGHTNESS_TYPE_TWICE) (MAX_BRIGHTNESS ));
						ledcWrite(ind, current_brightness);
						break;

					case DIMM_DOWN_LIN:
						
						diff_interval = s[ind].intv[ind_intv].end - s[ind].intv[ind_intv].start; 
						diff_current = current_time - s[ind].intv[ind_intv].start;
						diff_current = diff_interval - diff_current;
						current_brightness = (BRIGHTNESS_TYPE) (((BRIGHTNESS_TYPE_TWICE) diff_current * (BRIGHTNESS_TYPE_TWICE) s[ind].maximum_brightness)/ (BRIGHTNESS_TYPE_TWICE) diff_interval); 
						ledcWrite(ind, current_brightness);
						break;
						
					case DIMM_DOWN_EXP:

						diff_interval = s[ind].intv[ind_intv].end - s[ind].intv[ind_intv].start; 
						diff_current = current_time - s[ind].intv[ind_intv].start;
						diff_current = diff_interval - diff_current;
						current_brightness = (BRIGHTNESS_TYPE) (((BRIGHTNESS_TYPE_TWICE) diff_current * (BRIGHTNESS_TYPE_TWICE) s[ind].maximum_brightness)/ (BRIGHTNESS_TYPE_TWICE) diff_interval);
						current_brightness = (BRIGHTNESS_TYPE) ((BRIGHTNESS_TYPE_TWICE) (current_brightness * current_brightness) / (BRIGHTNESS_TYPE_TWICE) (MAX_BRIGHTNESS ));
						ledcWrite(ind, current_brightness);
						break;
					
					case UNDEFINED_STATE:
						//DebugPrintln("UNDEFINED_STATE");
						empty_func();
						break;
				}
			}
			break;
	}
}

int find_matching_interval(uint8_t schedule_ind,time_t current_time){

	//DebugPrintln("trying to find matching interval...");

	for(int i = 0; i < NUM_INTERVALS; i++){
		if( (s[schedule_ind].intv[i].state_type != UNDEFINED_STATE) 
			&& (s[schedule_ind].intv[i].start <= current_time)
			&& (s[schedule_ind].intv[i].end >= current_time)){
			return i;
		}
		//DebugPrintln("interval not matching: "+ String(i)+ " (start,end)=("+String(s[schedule_ind].intv[i].start)+ ","+ s[schedule_ind].intv[i].end +")");
	}
	return -1;
}

void offline_config(){
	time_t current_time = time(nullptr);
	s[0].io_type = PWM_IO;
	s[0].maximum_brightness = MAX_BRIGHTNESS;
	s[0].operation_mode = AUTOMATIC_MODE;
	s[0].intv[0].start =  current_time +5  ;
	s[0].intv[0].end =  current_time +15 ;
	s[0].intv[0].state_type = ALWAYS_ON;
	s[0].intv[1].start =  current_time  ;
	s[0].intv[1].end = current_time +10000 ;
	s[0].intv[1].state_type = ALWAYS_OFF;
}


void connect_wifi(){
	DebugPrintln("Connecting to ");
    DebugPrintln(wifi_cfg.ssid);
    WiFi.begin(wifi_cfg.ssid, wifi_cfg.password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      DebugPrint(".");
    }
    DebugPrintln("");
    DebugPrintln("WiFi connected.");
    IPAddress ip = WiFi.localIP();
  	DebugPrintln(ip);
}


void init_SSLcert(){
    
	cert= new httpsserver::SSLCert(
		cert_der,
		cert_der_len,
		key_der,
		key_der_len
		);
   
    DebugPrintln("Certificate read with success"); 
}

void print_schedule(){

	DebugPrintln("######");
	DebugPrintln("Schedule with " + String(NUM_IOS)+ "IOs" );
	DebugPrintln("--------------------------------------" );

	for(int i = 0; i< NUM_IOS;i++){
		DebugPrintln("------ \n io #"+ String(i));
		DebugPrintln("Input type: " + String(s[i].io_type) );
		DebugPrintln("Max Brighntess: " + String(s[i].maximum_brightness) );
		DebugPrintln("operation_mode: " + String(s[i].operation_mode) );
		DebugPrintln("number of intervals: "+ String(NUM_INTERVALS));

		for(int j = 0; j < NUM_INTERVALS; j++){
			DebugPrintln("...... \n intv #"+ String(j));
			DebugPrintln("(start,end): (" +String(s[i].intv[j].start)+","+String(s[i].intv[j].start)+ ")");
			DebugPrintln("intv type: "+ String(s[i].intv[j].state_type)); 
			DebugPrintln("repeat: " + String(s[i].intv[j].repeat) +" ,delta: "+  s[i].intv[j].delta_repetition);
		}
	} 
	DebugPrintln("######");
} 

void update_interval_repetitions(){

	time_t current_time = time(nullptr);

	for(int i = 0; i< NUM_IOS;i++){

		for(int j = 0; j < NUM_INTERVALS; j++){

			while( (s[i].intv[j].start < current_time) && (s[i].intv[j].end < current_time) && s[i].intv[j].repeat){
				time_t old_start = s[i].intv[j].start;
				s[i].intv[j].start = s[i].intv[j].end + s[i].intv[j].delta_repetition;
				s[i].intv[j].end = (2*s[i].intv[j].end) + s[i].intv[j].delta_repetition - old_start;
				print_schedule();
			}
		}
	} 
} 

void empty_func(){
	DebugPrintln("!!!empty func!!!");
}


void init_time(){
    // Init and get the time
    configTime((long) GTM_OFFSET_SEC, (int) DAYLIGHT_OFFSET_SEC, (char*) NTP_SERVER);
    struct tm timeinfo;
    while(!getLocalTime(&timeinfo)){
      delay(200);
      DebugPrintln("WARNING: Failed to obtain time! Could not connect to NTP Server. Retrying ...");
    }
    lastNTPconnectTime = time(nullptr);
}

void refresh_time(){
	//refresh time once a day
  	time_t current_time = time(nullptr);
  	if (((current_time - lastNTPconnectTime) >REFRESH_TIME_IN_SECS) || (current_time < lastNTPconnectTime)){
    	configTime((long) GTM_OFFSET_SEC, (int) DAYLIGHT_OFFSET_SEC, (char*) NTP_SERVER);
    	lastNTPconnectTime = current_time;
  	}
}


void parse_interval_config_json(String str_json,uint8_t* val_io,uint8_t* val_iv,time_t* val_start,time_t* val_end,uint8_t* val_type, bool* val_repeat,time_t* val_delta){

	DebugPrintln( str_json);
	int current_parsing_pos = str_json.indexOf("{", current_parsing_pos) +1;

	for (int i = 0; i< 7; i++){

		current_parsing_pos = str_json.indexOf("\"", current_parsing_pos) +1;

		String current_key = str_json.substring(current_parsing_pos,str_json.indexOf("\"", current_parsing_pos));
		current_parsing_pos = current_parsing_pos + current_key.length() +1;

		current_parsing_pos = str_json.indexOf(":", current_parsing_pos) +1;
		current_parsing_pos = str_json.indexOf("\"", current_parsing_pos) +1;

		String current_value = str_json.substring(current_parsing_pos,str_json.indexOf("\"", current_parsing_pos));
		current_parsing_pos = current_parsing_pos + current_value.length() +1;

		current_parsing_pos = str_json.indexOf(",", current_parsing_pos) +1;

		DebugPrintln("Key, Value:");
		DebugPrintln(current_key);
		DebugPrintln(current_value);

		if (current_key.equals("channel")){
			*val_io = current_value.toInt();
		}else if (current_key.equals("interval")){
			*val_iv = current_value.toInt();
		}else if (current_key.equals("start")){
			*val_start = current_value.toInt();
		}else if (current_key.equals("end")){
			*val_end = current_value.toInt();
		}else if (current_key.equals("type")){
			if (current_value.equals("ALWAYS_ON")){
				*val_type = ALWAYS_ON;
			}else if (current_value.equals("ALWAYS_OFF")){
				*val_type = ALWAYS_OFF;
			}else if (current_value.equals("DIMM_UP_LIN")){
				*val_type = DIMM_UP_LIN;
			}else if (current_value.equals("DIMM_UP_EXP")){
				*val_type = DIMM_UP_EXP;
			}else if (current_value.equals("DIMM_DOWN_LIN")){
				*val_type = DIMM_DOWN_LIN;
			}else if (current_value.equals("DIMM_DOWN_EXP")){
				*val_type = DIMM_DOWN_EXP;
			}else{
				*val_type = UNDEFINED_STATE;
			}
		}else if (current_key.equals("repeat")){
			*val_repeat = current_value.equals("true") ? true :false;
		}else if (current_key.equals("delta_repetition")){
			*val_delta = current_value.toInt();
		}
	}
}


String get_js_variables(){

	time_t current_time = time(nullptr);

	DebugPrintln("current_time:!!!");
	DebugPrintln(current_time);
	DebugPrintln(s[0].intv[0].delta_repetition);

	String variables = 	"<script> \
						NUM_IOS="+String(NUM_IOS)+"; \
						NUM_INTERVALS="+String(NUM_INTERVALS)+"; \
						CURRENT_TIME="+String(current_time)+"; \
						SCHEDULE=[";

	for(int i = 0; i< NUM_IOS; i++){
		variables += "["; 
		for(int j = 0; j < NUM_INTERVALS; j++){
			if (s[i].intv[j].state_type != UNDEFINED_STATE){
				variables += "{valid: true, start: "+String(s[i].intv[j].start)
							+", end: "+String(s[i].intv[j].end)
							+" ,type:  "+String(s[i].intv[j].state_type)
							+" ,repeat:  "+String(s[i].intv[j].repeat)
							+" ,delta_repetition:  "+String(s[i].intv[j].delta_repetition)
							+" }"; 
			}else{
				variables += "{valid: false }"; 
			}
			if (j < NUM_INTERVALS-1){
				variables += ",";
			}
		}
		variables += "]";
		if (i < NUM_IOS-1){
			variables += ",";
		}
	}
	variables += "];";

	variables += "BRIGHTNESS=[";
	for(int i = 0; i< NUM_IOS-1; i++){
		variables += String(s[i].maximum_brightness)+",";
	}
	variables += String(s[NUM_IOS-1].maximum_brightness)+ "];";
	variables += "\n </script> ";
	return variables;
}


void init_httpsServer_configuration(){

	secureServer = new httpsserver::HTTPSServer(cert);

   	httpsserver::ResourceNode * nodeRoot = new httpsserver::ResourceNode("/", "GET", &handleRootConfiguration);
    httpsserver::ResourceNode * nodePost  = new httpsserver::ResourceNode("/", "POST", &handlePostConfiguration);
	
    secureServer->registerNode(nodeRoot);
    secureServer->registerNode(nodePost);
   
    secureServer->start();
     
    if (secureServer->isRunning()) {
      DebugPrintln("Server ready.");
    }

}


void handleRootConfiguration(httpsserver::HTTPRequest * req, httpsserver::HTTPResponse * res){

	String response = 	html_config;
	response.replace("<link rel=\"stylesheet\" href=\"style.css\">", style );
	response.replace("REPLACE_DEVICE_NAME_REPLACE", String(DEVICE_NAME));
	res->println(response);
}

void handlePostConfiguration(httpsserver::HTTPRequest * req, httpsserver::HTTPResponse * res){
	DebugPrintln("handlePostConfiguration");
	DebugPrintln(req->getContentLength());

	const size_t capacity = req->getContentLength();
	byte buffer[capacity];
	req->readChars((char*)buffer,capacity);
	String str_json = String((char*)buffer);

	DebugPrintln(str_json);

	int pos = str_json.indexOf("{\"ssid\":\"")+9;
	int pos2 = str_json.indexOf("\",\"pw\":\"",pos);
	int pos3 = str_json.indexOf("\"}",pos2+9);
	str_json.substring(pos,pos2).toCharArray(wifi_cfg.ssid,32);
	str_json.substring(pos2+8,pos3).toCharArray(wifi_cfg.password,32);
	wifi_cfg.is_set = true;

	DebugPrintln(wifi_cfg.ssid);
	DebugPrintln(wifi_cfg.password);

	save_configuration();

	res->setHeader("Content-Type","text/plain");
	res->println("Success!");
	return;
}



void init_httpsServer(){

	secureServer = new httpsserver::HTTPSServer(cert);

    httpsserver::ResourceNode * nodeRoot = new httpsserver::ResourceNode("/", "GET", &handleRoot);
    httpsserver::ResourceNode * nodePost404  = new httpsserver::ResourceNode("", "POST", &handlePost);
	httpsserver::ResourceNode * nodePost  = new httpsserver::ResourceNode("/", "POST", &handlePost);
	httpsserver::ResourceNode * nodePostBrightness  = new httpsserver::ResourceNode("/brightness/", "POST", &handlePostBrightness);
	httpsserver::ResourceNode * node404  = new httpsserver::ResourceNode("", "GET", &handleRoot);
   
    secureServer->registerNode(node404);
    secureServer->registerNode(nodeRoot);
	secureServer->registerNode(nodePostBrightness);
	secureServer->registerNode(nodePost);
	secureServer->registerNode(nodePost404);
   
    secureServer->start();
     
    if (secureServer->isRunning()) {
      DebugPrintln("Server ready.");
    }

}


void handleRoot(httpsserver::HTTPRequest * req, httpsserver::HTTPResponse * res){
	DebugPrintln("handleRoot");

	String variables = get_js_variables();
	
	String temp = html;
	temp.replace("<link rel=\"stylesheet\" href=\"style.css\">", style );
	temp.replace("<script src=\"script.js\"></script>",script );
	temp.replace("<script src=\"variables.js\"></script>",variables );
	temp.replace("REPLACE_DEVICE_NAME_REPLACE",String(DEVICE_NAME));
	res->println(temp);
	return;
}


void handlePostBrightness(httpsserver::HTTPRequest * req, httpsserver::HTTPResponse * res){
	DebugPrintln("handlePostBrightness");
	DebugPrintln(req->getContentLength());

	const size_t capacity = req->getContentLength();
	byte buffer[capacity];
	req->readChars((char*)buffer,capacity);
	String str_req = String((char*)buffer);

	DebugPrintln(str_req);

	int current_parsing_pos = str_req.indexOf("{\"io\":\"", current_parsing_pos) +7;
	DebugPrintln(str_req.substring(current_parsing_pos,str_req.indexOf("\"", current_parsing_pos)));
	uint8_t io = (str_req.substring(current_parsing_pos,str_req.indexOf("\"", current_parsing_pos))).toInt() ;
	current_parsing_pos = str_req.indexOf("\",\"val\":\"", current_parsing_pos) +9;
	DebugPrintln(str_req.substring(current_parsing_pos,str_req.indexOf("\"}", current_parsing_pos)));
	BRIGHTNESS_TYPE maxBrightness = (str_req.substring(current_parsing_pos,str_req.indexOf("\"}", current_parsing_pos))).toInt();

	DebugPrintln(io);
	DebugPrintln(maxBrightness);
	s[io].maximum_brightness = maxBrightness;

	save_configuration();

	res->setHeader("Content-Type","text/plain");
	res->println("Success!");
	return;
}

void handlePost(httpsserver::HTTPRequest * req, httpsserver::HTTPResponse * res){
	DebugPrintln("handlePost");
	DebugPrintln(req->getContentLength());

	const size_t capacity = req->getContentLength();
	byte buffer[capacity];
	req->readChars((char*)buffer,capacity);
	String str_json = String((char*)buffer);

	uint8_t val_io, val_iv, val_type;
	bool val_repeat;
	time_t  val_start, val_end, val_delta; 
	parse_interval_config_json(str_json, &val_io, &val_iv, &val_start, &val_end, &val_type, &val_repeat, &val_delta);

	if (val_io < NUM_IOS && val_iv < NUM_INTERVALS){
		s[val_io].intv[val_iv].start = val_start;
		s[val_io].intv[val_iv].end = val_end;
		s[val_io].intv[val_iv].state_type = val_type;
		s[val_io].intv[val_iv].repeat = val_repeat;
		s[val_io].intv[val_iv].delta_repetition = val_delta;
		s[val_io].operation_mode = AUTOMATIC_MODE;
		s[val_io].io_type = PWM_IO;
		initialize_IOs();
	}else{
		DebugPrintln("Channel or Interval out of bounds!!!");
	}

	save_configuration();

	res->setHeader("Content-Type","text/plain");
	res->println("Success!");
	return;
}