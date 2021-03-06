//============================================================================
// Name        : Quadcopter_Arduino.ini
// Author      : Raffael Sommer
// Version     : 0.1
// Copyright   : None
// Description : Arduinosteuerung f�r den Quadcopter
//============================================================================

#include <Servo/Servo.h>
#include <String.h>
//#include <Vector3.h>

#define DEBUG_ENABLED 1
#define WIFI_SETUP_ENABLED 1
#define DOF_SETUP_ENABLED 1

#define LED_PIN 15
#define MOTOR1 1
#define MOTOR2 2
#define MOTOR3 3
#define MOTOR4 4

#define SERIAL_DEBUG 0
#define SERIAL_DOF 1
#define SERIAL_WIFI 2

#define SERIAL_DEBUG_BAUD 9600
#define SERIAL_DOF_BAUD 57600 //115200
#define SERIAL_WIFI_BAUD 9600

void get9DOFData(String data);
void initWIFI();
void LedBlink(int count, int freq);
String readSerial(int i);
void SerialFlush(int i);
void SetZero();
void wifiSend(String s);
void dofSend(String s);
void serialSend(String s);
void wifiSendDATA(String s);
void initDOF();

Servo Motor1;
Servo Motor2;
Servo Motor3;
Servo Motor4;

//Vector3 SteeringInput();
int t_m1, t_m2, t_m3, t_m4;
int lasttime;
int ledblinkcounter, ledblinknum, ledblinkfreq;

bool LED_blinking;

//Vector3 DOF_accel();
//Vector3 DOF_gyro();
//Vector3 DOF_mag();

char WIFI_Net[30];
char WIFI_IP_Adress[20];

String DOFRead;
String WIFIRead;
String DEBUGRead;

HardwareSerial Serial_DEBUG = Serial;
HardwareSerial Serial_DOF = Serial1;
HardwareSerial Serial_WIFI = Serial2;

String WIFI_SSID = "ESP8266";
String WIFI_PW = "vJ$e&5O;/%@LEXFs";
String WIFI_CHANNEL = "1";
String WIFI_SECURITY = "3";

void setup(){
	/*
	Motor1.attach(MOTOR1);
	Motor2.attach(MOTOR2);
	Motor3.attach(MOTOR3);
	Motor4.attach(MOTOR4);
	*/
#if DEBUG_ENABLED
	Serial_DEBUG.begin(SERIAL_DEBUG_BAUD);	//DEBUG PC
#endif
	Serial_DOF.begin(SERIAL_DOF_BAUD);	//RAZOR IMU
	Serial_WIFI.begin(SERIAL_WIFI_BAUD);	//WIFI SHIELD
	pinMode(LED_PIN, OUTPUT);
#if WIFI_SETUP_ENABLED
	initWIFI();
#endif
#if DOF_SETUP_ENABLED
	initDOF();
#endif
	wifiSend("AT");
	Serial_DEBUG.println("Init DONE");
}

void LedBlink(int count, int freq){
	LED_blinking = true; 
	ledblinkcounter = 0; 
	ledblinknum = count;
	ledblinkfreq = freq;
}

int WaitForWIFIResponse(){
	int i = 0;
	delay(20);
	if (!Serial_WIFI.find("OK") && !Serial_WIFI.find("ready")) {
		i++;
		Serial_DEBUG.println("Error");
	}
	SerialFlush(SERIAL_WIFI);
	SerialFlush(SERIAL_DEBUG);
	return i;
}

void initWIFI(){
	int result = 0;
	Serial_DEBUG.println("start wifi");
	wifiSend("AT");
	result += WaitForWIFIResponse();

	wifiSend("AT+CIPMUX=1");
	result += WaitForWIFIResponse();

	wifiSend("AT+CIPSERVER=1,80");
	result += WaitForWIFIResponse();

	wifiSend("AT+CIPSTO=0");
	result += WaitForWIFIResponse();
	
	wifiSend("AT+CWMODE=2");
	SerialFlush(SERIAL_WIFI);

	String WIFI_AP_SETUP = "AT+CWSAP=\"" + WIFI_SSID + "\",\"" + WIFI_PW + "\"," + WIFI_CHANNEL + "," + WIFI_SECURITY;

	wifiSend(WIFI_AP_SETUP);
	result += WaitForWIFIResponse();
	delay(1000);

	wifiSend("AT+CWSAP?");
	delay(10);

	byte len = 0;
	WIFI_Net[0] = 0;
	if (Serial_WIFI.find("AT+CWSAP?\r\r\n+CWSAP:\"")) {
		len = Serial_WIFI.readBytesUntil('\"', WIFI_Net, 20);
		WIFI_Net[len] = 0;
	}

	wifiSend("AT+CIFSR");
	delay(1000);

	len = 0;
	WIFI_IP_Adress[0] = 0;
	if (Serial_WIFI.find("AT+CIFSR\r\r\n")) {
		len = Serial_WIFI.readBytesUntil('\r', WIFI_IP_Adress, 20);
		WIFI_IP_Adress[len] = 0;
	}
	
#if DEBUG_ENABLED
	Serial_DEBUG.println("--------------------------");
	Serial_DEBUG.print("WIFI done with ");
	Serial_DEBUG.print(result);
	Serial_DEBUG.println(" Errors");
	Serial_DEBUG.print("Netzwerk: ");
	Serial_DEBUG.println(WIFI_Net);
	Serial_DEBUG.print("IP: ");
	Serial_DEBUG.println(WIFI_IP_Adress);
#endif

}

void initDOF(){
	
	Serial_DOF.println("#o0");
	Serial_DOF.println("#osct");
	Serial_DOF.println("#o1");
}
/*
	 "#o<params>" - Set OUTPUT mode and parameters. The available options are:
Streaming output
	"#o0" - DISABLE continuous streaming output. Also see #f below.
	"#o1" - ENABLE continuous streaming output.
// Angles output
	"#ob" - Output angles in BINARY format (yaw/pitch/roll as binary float, so one output frame
	is 3x4 = 12 bytes long).
	"#ot" - Output angles in TEXT format (Output frames have form like "#YPR=-142.28,-5.38,33.52",
	followed by carriage return and line feed [\r\n]).
// Sensor calibration
	"#oc" - Go to CALIBRATION output mode.
	"#on" - When in calibration mode, go on to calibrate NEXT sensor.
// Sensor data output
	"#osct" - Output CALIBRATED SENSOR data of all 9 axes in TEXT format.
	One frame consist of three lines - one for each sensor: acc, mag, gyr.
	"#osrt" - Output RAW SENSOR data of all 9 axes in TEXT format.
	One frame consist of three lines - one for each sensor: acc, mag, gyr.
	"#osbt" - Output BOTH raw and calibrated SENSOR data of all 9 axes in TEXT format.
	One frame consist of six lines - like #osrt and #osct combined (first RAW, then CALIBRATED).
	NOTE: This is a lot of number-to-text conversion work for the little 8MHz chip on the Razor boards.
	In fact it's too much and an output frame rate of 50Hz can not be maintained. #osbb.
	"#oscb" - Output CALIBRATED SENSOR data of all 9 axes in BINARY format.
	One frame consist of three 3x3 float values = 36 bytes. Order is: acc x/y/z, mag x/y/z, gyr x/y/z.
	"#osrb" - Output RAW SENSOR data of all 9 axes in BINARY format.
	One frame consist of three 3x3 float values = 36 bytes. Order is: acc x/y/z, mag x/y/z, gyr x/y/z.
	"#osbb" - Output BOTH raw and calibrated SENSOR data of all 9 axes in BINARY format.
	One frame consist of 2x36 = 72 bytes - like #osrb and #oscb combined (first RAW, then CALIBRATED).
// Error message output
	"#oe0" - Disable ERROR message output.
	"#oe1" - Enable ERROR message output.
	"#f" - Request one output frame - useful when continuous output is disabled and updates are
	required in larger intervals only. Though #f only requests one reply, replies are still
	bound to the internal 20ms (50Hz) time raster. So worst case delay that #f can add is 19.99ms.
	"#s<xy>" - Request synch token - useful to find out where the frame boundaries are in a continuous
	binary stream or to see if tracker is present and answering. The tracker will send
	"#SYNCH<xy>\r\n" in response (so it's possible to read using a readLine() function).
	x and y are two mandatory but arbitrary bytes that can be used to find out which request
	the answer belongs to.
*/
void get9DOFData(String data){
	/*
	#YPR=-155.73,-76.48,-129.51
	X axis pointing forward (towards the short edge with the connector holes)
	Y axis pointing to the right
	Z axis pointing down 
	*/
	if (data[0] != '#'){
		return;
	}
	char * del = ",=";
	char * str;
	float yawn;
	float pitch;
	float roll;
	data.toCharArray(str, data.length(), 0);
	str = strtok(str, del);
	str = strtok(str, del);
	//SteeringInput.x(atof(str));
	yawn = atof(str);
	str = strtok(str, del);
	//SteeringInput.y(atof(str));
	pitch = atof(str);
	str = strtok(str, del);
	//SteeringInput.z(atof(str));
	roll = atof(str);
	/*
	str = strtok(str, del);
	DOF_accel.x(atoi(str));
	str = strtok(NULL, del);
	DOF_accel.y( atoi(str));
	str = strtok(NULL, del);
	DOF_accel.z( atoi(str));
	str = strtok(NULL, del);
	DOF_gyro.x(atoi(str));
	str = strtok(NULL, del);
	DOF_gyro.y( atoi(str));
	str = strtok(NULL, del);
	DOF_gyro.z( atoi(str));
	str = strtok(NULL, del);
	DOF_mag.x(atoi(str));
	str = strtok(NULL, del);
	DOF_mag.y(atoi(str));
	str = strtok(NULL, del);
	DOF_mag.z( atoi(str));
	*/
}

void SetZero(){
	DOFRead = "";
	WIFIRead = "";
	//SteeringInput.x(0); 
	//SteeringInput.y(0);
	//SteeringInput.z(0);
	t_m1 = 0;
	t_m2 = 0;
	t_m3 = 0;
	t_m4 = 0;
}

void SerialFlush(int i){
	switch (i)
	{
		case 0: while (Serial_DEBUG.available()) { Serial_DEBUG.read(); } break;
		case 1: while (Serial_DOF.available()) { Serial_DOF.read(); } break;
		case 2: while (Serial_WIFI.available()) { Serial_WIFI.read(); } break;
		default:
			break;
	}
}

String readSerial(int i){
	char c;
	String readString = "";
	String result = "";
	if (i == SERIAL_WIFI){
		while (Serial_WIFI.available()) {
			c = Serial_WIFI.read();
			readString += c;
		}
		if (readString.length() >0) {
			result = readString;
			readString = "";
		}
	}
	else if (i == SERIAL_DOF){
		while (Serial_DOF.available()) {
			c = Serial_DOF.read();
			readString += c;
		}
		if (readString.length() >0) {
			result = readString;
			readString = "";
		}
	}
	else if (i == SERIAL_DEBUG) {
		while (Serial_DEBUG.available()) {
			c = Serial_DEBUG.read();
			readString += c;
		}
		if (readString.length() >0) {
			result = readString;
			readString = "";
		}
	}
	return result;
}

void serialSend(String s) {
	Serial_DEBUG.println(s);
}

void wifiSend(String s){
	if (s.length() > 0){
#if DEBUG_ENABLED
		Serial_DEBUG.println("\nwifi:" + s);
#endif
		Serial_WIFI.println(s);
	}
}

void dofSend(String s){
	if(s.length() > 0){
#if DEBUG_ENABLED
		Serial_DEBUG.println("\ndof:" + s);
#endif
		Serial_DOF.println(s);
	}
}

void wifiSendDATA(String s){
	if (s.length() >0) {
		s += "\r";
		String sendCMD;
		sendCMD = "AT+CIPSEND=";
		sendCMD += s.length();
		sendCMD += " " + s;
		wifiSend(sendCMD);
	}
}

void loop(){
	unsigned int now = millis();
	//DOF_accel.Add_V3(DOF_gyro);
	SetZero();
	//Razor IMU 9DOF
	if (Serial_DOF.available()){
		DOFRead += readSerial(SERIAL_DOF);
		//get9DOFData(DOFRead);
	}
	//ESP8266
	if (Serial_WIFI.available()){
		WIFIRead += readSerial(SERIAL_WIFI);
	}
	if (Serial_DEBUG.available()) {
		DEBUGRead += readSerial(SERIAL_DEBUG);
	}
	/*if(WIFIRead.lastIndexOf("+IPD")){
		if(WIFIRead.lastIndexOf("LED1")){
			digitalWrite(LED_PIN, HIGH);
		}
		else if(WIFIRead.lastIndexOf("LED0")){
			digitalWrite(LED_PIN, LOW);
		}
		QUAD-XXX,-YYY,-ZZZ (-255 bis 255)
		char * del = ",";
		char * str;
		data.toCharArray(str, WIFIRead.length(), WIFIRead.find("QUAD"));
		str = strtok(str, del);
		SteeringInput.x = atoi(str);
		str = strtok(str, del);
		SteeringInput.y = atoi(str);
		str = strtok(str, del);
		SteeringInput.z = atoi(str);
		
	}*/
#if DEBUG_ENABLED
	if (DOFRead.length() > 0 && !Serial_DOF.available()){
		Serial_DEBUG.println("----DOF----");
		Serial_DEBUG.println(DOFRead);
	}
	if (WIFIRead.length() > 0 && !Serial_WIFI.available()){
		Serial_DEBUG.println("----WIFI----");
		Serial_DEBUG.println(WIFIRead);
	}
	if (DEBUGRead.length() > 0 && !Serial_DEBUG.available()){
		if (DEBUGRead.endsWith("\r\n")) {
			if (DEBUGRead[0] == '#')
				dofSend(DEBUGRead);
			else
				wifiSend(DEBUGRead);
			DEBUGRead = "";
		}
	}
#endif
/*
	Motor1.write(t_m1);
	Motor2.write(t_m2);
	Motor3.write(t_m3);
	Motor4.write(t_m4);

	if (LED_blinking && ledblinkcounter < ledblinknum){
		if (now > lasttime + ledblinkfreq){
			lasttime = now;
			digitalWrite(LED_PIN, !digitalRead(LED_PIN));
			ledblinkcounter++;
		}
		else if (ledblinkcounter >= 10){
			ledblinkcounter = 0;
			LED_blinking = false;
		}

	}*/
}
