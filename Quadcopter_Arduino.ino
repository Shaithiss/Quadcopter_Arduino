#include <Servo/Servo.h>
#include <String.h>

#define DEBUG 1

#define LED_PIN 15
#define MOTOR1 1
#define MOTOR2 2
#define MOTOR3 3
#define MOTOR4 4
#define BAUD0 9600
#define BAUD1 57600
#define BAUD2 9600

Servo Motor1;
Servo Motor2;
Servo Motor3;
Servo Motor4;

int x_read, y_read, z_read;
int t_m1, t_m2, t_m3, t_m4;

uint16_t x_accel, y_accel, z_accel;
uint16_t x_gyro, y_gyro, z_gyro;
uint16_t x_mag, y_mag, z_mag;

String DOFRead;
String WIFIRead;

String WIFI_SSID = "ESP8266";
String WIFI_PW = "12345678";
String WIFI_CHANNEL = "1";
String WIFI_SECURITY = "3";

void setup(){

	Motor1.attach(MOTOR1);
	Motor2.attach(MOTOR2);
	Motor3.attach(MOTOR3);
	Motor4.attach(MOTOR4);
	Serial.begin(BAUD0);	//DEBUG PC
	Serial1.begin(BAUD1);	//RAZOR IMU
	Serial2.begin(BAUD2);	//WIFI SHIELD
	//pinmode(LED_PIN, Output)

	initWIFI();

	Serial.println("Init DONE");
}

String GetWIFISetupString(){
	String result = "AT+CWSAP=\"";
	result += WIFI_SSID;
	result += "\",\"";
	result += WIFI_PW;
	result += "\",";
	result += WIFI_CHANNEL;
	result += ",";
	result += WIFI_SECURITY;
	return result;
}

void initWIFI(){
	bool result = true;
	Serial.println("start wifi");
	wifiSend("AT");
	while (!Serial2.find("OK")){
		SerialFlush(2);
		delay(20);
	}
	wifiSend("AT+RST");
	while (!Serial2.find("ready")){
		SerialFlush(2);
		delay(20);
	}
	wifiSend("AT+CWMODE=3");
	delay(200);
	SerialFlush(2);
	wifiSend(GetWIFISetupString());
	while (!Serial2.find("OK")){
		SerialFlush(2);
		delay(20);
	}
	wifiSend("AT+RST");
	while (!Serial2.find("ready")){
		SerialFlush(2);
		delay(20);
	}
	Serial.println("WIFI done");
}

void get9DOFData(String data){
	if (data[0] != '$'){
		return;
	}
	char * del = ",$#";
	char * str;
	data.toCharArray(str, data.length(), 0);
	str = strtok(str, del);
	x_accel = atoi(str);
	str = strtok(NULL, del);
	y_accel = atoi(str);
	str = strtok(NULL, del);
	z_accel = atoi(str);
	str = strtok(NULL, del);
	x_gyro = atoi(str);
	str = strtok(NULL, del);
	y_gyro = atoi(str);
	str = strtok(NULL, del);
	z_gyro = atoi(str);
	str = strtok(NULL, del);
	x_mag = atoi(str);
	str = strtok(NULL, del);
	y_mag = atoi(str);
	str = strtok(NULL, del);
	z_mag = atoi(str);
}

void SetZero(){
	DOFRead = "";
	WIFIRead = "";
	x_read = 0;
	y_read = 0;
	z_read = 0;
	t_m1 = 0;
	t_m2 = 0;
	t_m3 = 0;
	t_m4 = 0;
}

void SerialFlush(int i){
	char c;
	if (i == 0){
		while (Serial.available()) {
			if (Serial.available() >0) {
				c = Serial1.read();
			}
		}
	}
	if (i == 1){
		while (Serial1.available()) {
			if (Serial1.available() >0) {
				c = Serial1.read();
			}
		}
	}
	else if (i == 2){
		Serial2.flush();
		while (Serial2.available()) {
			if (Serial2.available() >0) {
				c = Serial2.read();
			}
		}
	}
}

String readSerial(int i){
	delay(200);
	char c;
	String readString = "";
	String result = "";
	if (i == 1){
		while (Serial1.available()) {
			if (Serial1.available() >0) {
				c = Serial1.read();
				readString += c;
			}
		}
		if (readString.length() >0) {
			result = readString;
			readString = "";
		}
	}
	else if (i == 2){
		while (Serial2.available()) {
			if (Serial2.available() >0) {
				c = Serial2.read();
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
		Serial2.println(s);
	}
}

void wifiSendDATA(String s){
	if (s.length() >0) {
		s += "\r";
		String sendCMD;
		sendCMD = "AT+CIPSEND=";
		sendCMD += s.length();
		Serial2.print(sendCMD);
		s = "";
	}
}

void loop(){

	SetZero();
	//Razor IMU
	if (Serial1.available()){
		DOFRead = readSerial(1);
		get9DOFData(DOFRead);
	}
	if (Serial2.available()){
		WIFIRead = readSerial(2);
	}

#if DEBUG
	if (DOFRead.length() > 0 || WIFIRead.length() > 0){
		Serial.println("----DOF----");
		Serial.println(DOFRead);
		Serial.println("----WIFI----");
		Serial.println(WIFIRead);
		Serial.print('\n');
	}

	if (Serial.available()){
		String s = "";
		char c;
		delay(200);
		while (Serial.available()) {
			if (Serial.available() > 0) {
				c = Serial.read();
				if (c != '\n')
					s += c;
			}
		}
		Serial.println("to WIFI");
		Serial.println(s);
		Serial.println(s.length());
		Serial.println("--------");
		wifiSend(s);

	}
#endif

	Motor1.write(t_m1);
	Motor2.write(t_m2);
	Motor3.write(t_m3);
	Motor4.write(t_m4);

}
