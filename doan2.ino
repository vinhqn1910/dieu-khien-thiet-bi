#include <LiquidCrystal_I2C.h>
#include <FirebaseESP32.h>
#include <DHT.h>


// Định nghĩa thông tin đăng nhập Wi-Fi và Firebase
#define WIFI_SSID "Vinh"
#define WIFI_PASSWORD "0397507254"
#define DATABASE_URL "doan2-2d60b-default-rtdb.firebaseio.com"

// Định nghĩa các chân kết nối với thiết bị
#define DEV1        23    // D23, Thiết bị 1
#define DEV2        19    // D19, Thiết bị 2
#define MOD         18    // D18, Chế độ đèn
#define MQ2_SENSOR  39    // D39, Chân kết nối với MQ2 trên ESP32
#define MQ135_SENSOR 36   // D36, Chân kết nối với MQ135 trên ESP32
#define DHTPIN      16    // RX2, Chân kết nối với DHT11 trên ESP32
#define DHTTYPE     DHT11 // Loại cảm biến DHT

DHT dht(DHTPIN, DHTTYPE);

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

LiquidCrystal_I2C lcd(0x27, 16, 2); // Địa chỉ I2C và kích thước màn hình LCD

boolean tb1, tb2, Modehd;


void setup() {
  Serial.begin(115200);
  dht.begin();
  lcd.init();//lcd.begin(16,2);
  lcd.backlight();
  pinMode(DEV1, OUTPUT);
  pinMode(DEV2, OUTPUT);
  pinMode(MOD, OUTPUT);
  digitalWrite(DEV1, LOW);
  digitalWrite(DEV2, LOW);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Đang kết nối Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.print("Đã kết nối với IP: ");
  Serial.println(WiFi.localIP());

  config.database_url = DATABASE_URL;
  config.signer.test_mode = true;
  fbdo.setBSSLBufferSize(4096, 1024);
  Firebase.begin(&config, &auth);
}

void loop() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int mq = analogRead(MQ2_SENSOR);
  int kk = analogRead(MQ135_SENSOR);

  Firebase.setFloat(fbdo, "Nhiet do", temp);
  Firebase.setFloat(fbdo, "Do am", hum);
  Firebase.setFloat(fbdo, "Khi gas", mq);
  Firebase.setFloat(fbdo, "Khong khi", kk);
  Firebase.getBool(fbdo, F("/device/Mode"), &Modehd);
  Serial.print("Mode: ");
  Serial.println(Modehd);

  if (Modehd) { // Chế độ tự động

    digitalWrite(MOD, HIGH);

    if (mq > 500) {
        tb1 = 1;
        tb2 = 1;
    }
    else if ((mq < 500 )&&( temp > 40)) {
        tb1 = 1;
        tb2 = 0;
    }
    else {
        tb1 = 0;
        tb2 = 0;
    }
  } else {
    digitalWrite(MOD, LOW);
    Firebase.getBool(fbdo, F("/device/Device1"), &tb1);
    Firebase.getBool(fbdo, F("/device/Device2"), &tb2);
  }

  digitalWrite(DEV1, tb1 ? HIGH : LOW);
  digitalWrite(DEV2, tb2 ? HIGH : LOW);

  Serial.print("Nhiet do: ");
  Serial.println(temp);
  Serial.print("Do am: ");
  Serial.println(hum);
  Serial.print("Khi gas: ");
  Serial.println(mq);
  Serial.print("Khong Khi: ");
  Serial.println(kk);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ND:");
  lcd.print(temp);
  lcd.print("C");
  lcd.setCursor(9, 0);
  lcd.print("KG:");
  lcd.print(mq);
  lcd.setCursor(0, 1);
  lcd.print("DA:");
  lcd.print(hum);
  lcd.print("%");
  lcd.setCursor(9, 1);
  lcd.print("KK:");
  lcd.print(kk);
}

