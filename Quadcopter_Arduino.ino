#include <Servo/Servo.h>
#include <String.h>
#include <Vector3.h>

#define DEBUG_ENABLED 1
#define WIFI_SETUP_ENABLED 0

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
void wifiSendDATA(String s);

Servo Motor1;
Servo Motor2;
Servo Motor3;
Servo Motor4;

Vector3 SteeringInput(0, 0, 0);
int t_m1, t_m2, t_m3, t_m4;
int lasttime;
int ledblinkcounter, ledblinknum, ledblinkfreq;

bool LED_blinking;

Vector3 DOF_accel(0, 0, 0);
Vector3 DOF_gyro(0, 0, 0);
Vector3 DOF_mag(0, 0, 0);

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

void get9DOFData(String data){
	if (data[0] != '$'){
		return;
	}
	char * del = ",$#";
	char * str;
	data.toCharArray(str, data.length(), 0);
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
}

void SetZero(){
	DOFRead = "";
	WIFIRead = "";
	SteeringInput.x(0); 
	SteeringInput.y(0);
	SteeringInput.z(0);
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
	delay(200);
	char c;
	String readString = "";
	String result = "";
	if (i == SERIAL_WIFI){
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
	DOF_accel.Add_V3(DOF_gyro);
	SetZero();
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
		/*
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
		*/
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
		delay(200);
		while (Serial_DEBUG.available()) {
			if (Serial_DEBUG.available() > 0) {
				c = Serial_DEBUG.read();
				if (c != '\n')
					s += c;
			}
		}
		wifiSend(s);

	}
#endif

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

	}
}
