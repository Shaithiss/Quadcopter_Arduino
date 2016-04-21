#include <Servo/Servo.h>
#include <String.h>
#include <Vector3.h>

#define DEBUG_ENABLED 1
#define WIFI_SETUP_ENABLED 0
#define DOF_SETUP_ENABLED 0

#define LED_PIN 15
#define MOTOR1 1
#define MOTOR2 2
#define MOTOR3 3
#define MOTOR4 4

#define SERIAL_DEBUG 0
#define SERIAL_DOF 1
#define SERIAL_WIFI 2

#define SERIAL_DEBUG_BAUD 9600
#define SERIAL_DOF_BAUD 115200 //57600
#define SERIAL_WIFI_BAUD 9600

void get9DOFData(String data);
String GetWIFISetupString();
void initWIFI();
void LedBlink(int count, int freq);
String readSerial(int i);
void SerialFlush(int i);
void SetZero();
void wifiSend(String s);
void dofSend(String s);
void wifiSendDATA(String s);

Servo Motor1;
Servo Motor2;
Servo Motor3;
Servo Motor4;

int t_m1, t_m2, t_m3, t_m4;
int lasttime;
int ledblinkcounter, ledblinknum, ledblinkfreq;

float[3] YPR = {};
float[3] DOF_accel = {};
float[3] DOF_gyro = {}; 
float[3] DOF_mag = {};
int[3] Input = {};

bool LED_blinking;
bool YPR_enabled = false;

char WIFI_Net[30];
char WIFI_IP_Adress[20];

String DOFRead;
String WIFIRead;

HardwareSerial Serial_DEBUG = Serial;
HardwareSerial Serial_DOF = Serial1;
HardwareSerial Serial_WIFI = Serial2;

String WIFI_SSID = "ESP8266"; 
String WIFI_PW = "12345678";
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
#if DEBUG_ENABLED
	Serial_DEBUG.println("Init DONE");
#endif
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
	SerialDOF.println("#o0");
	SerialDOF.println("#osct");
	SerialDOF.println("#o1");
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
	
	"#A-C=000.00,000.00,000.00\r\n"
	"#M-C=000.00,000.00,000.00\r\n"
	"#G-C=000.00,000.00,000.00\r\n"
	*/
	
	char * str;
	char * del = ",=";
	data.toCharArray(str, data.length(), 0);
	if (str[0] != '#'){
		return;
	}
	if(str[1] == 'Y'){
		str = strtok(str, del);
		YPR[0] = atof(str);
		str = strtok(str, del);
		YPR[1] = atof(str);
		str = strtok(str, del);
		YPR[2] = atof(str);
	} 
	else {
		switch(str[1]){
			case 'A'{
				str = strtok(str, del);
				str = strtok(NULL, del);
				DOF_accel[0] = atof(str);
				str = strtok(NULL, del);
				DOF_accel[1] = atof(str);
				str = strtok(NULL, del);
				DOF_accel[2] = atof(str);
				break;
			}
			case 'M'{
				str = strtok(str, del);
				str = strtok(NULL, del);
				DOF_mag[0] = atof(str);
				str = strtok(NULL, del);
				DOF_mag[1] = atof(str);
				str = strtok(NULL, del);
				DOF_mag[2] = atof(str);
				break;
			}
			case 'G'{
				str = strtok(str, del);
				str = strtok(NULL, del);
				DOF_gyro[0] = atof(str);
				str = strtok(NULL, del);
				DOF_gyro[1] = atof(str);
				str = strtok(NULL, del);
				DOF_gyro[2] = atof(str);
				break;
			}
		}
	}
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
		delay(100);
		while (Serial_WIFI.available()) {
			if (Serial_WIFI.available() >0) {
				c = Serial_WIFI.read();
				readString += c;
			}
		}
		if (readString.length() >0) {
			result = readString;
			readString = "";
		}
	}
	else if (i == SERIAL_DOF){
		while (Serial_DOF.available()) {
			if (Serial_DOF.available() >0) {
				c = Serial_DOF.read();
				readString += c;
			}
		}
		if (readString.length() >0) {
			result = readString;
			readString = "";
		}
	}
	return result;
}

void wifiSend(String s){
	if (s.length() > 0){
#if DEBUG_ENABLED
		Serial_DEBUG.println(s);
#endif
		Serial_WIFI.println(s);
	}
}

void dofSend(String s){
	if(s.length() > 0){
#if DEBUG_ENABLED
		Serial_DEBUG.println(s);
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
	//Razor IMU 9DOF
	if (Serial_DOF.available()){
		DOFRead = readSerial(SERIAL_DOF);
		get9DOFData(DOFRead);
	}
	//ESP8266
	if (Serial_WIFI.available()){
		WIFIRead = readSerial(SERIAL_WIFI);
	}
	if(WIFIRead.lastIndexOf("+IPD")){
		if(WIFIRead.lastIndexOf("LED1")){
			digitalWrite(LED_PIN, HIGH);
		}
		else if(WIFIRead.lastIndexOf("LED0")){
			digitalWrite(LED_PIN, LOW);
		}
		//Q=-XXX,-YYY,-ZZZ (-255 bis 255)
	} 
	else if(WIFIRead.lastIndexOf("Q=")) {
		char * del = ":,";
		char * str;
		WIFIRead.toCharArray(str, WIFIRead.length(), 0);
		str = strtok(str, del);
		Input[0] = atoi(str);
		str = strtok(NULL, del);
		Input[1] = atoi(str);
		str = strtok(NULL, del);
		Input[2] = atoi(str);
	}

#if DEBUG_ENABLED
	if (DOFRead.length() > 0){
		Serial_DEBUG.println("----DOF----");
		Serial_DEBUG.println(DOFRead);
	}
	if (WIFIRead.length() > 0){
		Serial_DEBUG.println("----WIFI----");
		Serial_DEBUG.println(WIFIRead);
		Serial_DEBUG.print('\n');
	}

	if (Serial_DEBUG.available()){
		String s = "";
		char c;
		while (Serial_DEBUG.available()) {
			c = Serial_DEBUG.read();
			if (c != '\n')
				s += c;
			}
		}
		if(s[0] == '#')
			dofSend(s);
		else
			wifiSend(s);

	}
#endif
/*
	Motor1.write(t_m1);
	Motor2.write(t_m2);
	Motor3.write(t_m3);
	Motor4.write(t_m4);
*/
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

	}
}
