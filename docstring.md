# Feribot Simülasyonu Dokümantasyonu

## Proje Açıklaması

Bu proje, iki liman arasında çalışan bir feribot sisteminin çok iş parçacıklı (multithreaded) simülasyonunu gerçekleştirir. Sistem, farklı tipteki araçların feribot ile karşıya geçişini simüle eder.

## Sistem Bileşenleri

### 1. Feribot
- Kapasite: 20 araç birimi
- Özellikler:
  - Mevcut liman bilgisi
  - Kapasite yönetimi
  - Yükleme/boşaltma senkronizasyonu
  - Mutex ve semafor kullanımı

### 2. Araçlar
- Toplam 30 araç:
  - 12 Otomobil (1 birim kapasite)
  - 10 Minibüs (2 birim kapasite)
  - 8 Kamyon (3 birim kapasite)
- Her araç için:
  - Benzersiz ID
  - Araç tipi
  - Başlangıç limanı
  - Mevcut liman
  - Gişe numarası

### 3. Gişeler
- Toplam 4 gişe:
  - Her limanda 2 adet
  - Mutex ile senkronizasyon
  - Semafor ile erişim kontrolü

### 4. Bekleme Alanları
- Her limanda 1 adet
- Araç kuyruğu yönetimi
- Mutex ile senkronizasyon

## Senkronizasyon Mekanizmaları

### Mutex Kullanımı
1. `printMutex`: Çıktı senkronizasyonu
2. `ferry.mutex`: Feribot işlemleri senkronizasyonu
3. `tolls[].mutex`: Gişe erişimi senkronizasyonu
4. `waitingAreas[].mutex`: Bekleme alanı senkronizasyonu

### Semafor Kullanımı
1. `ferry.loadingSem`: Yükleme işlemi senkronizasyonu
2. `ferry.unloadingSem`: Boşaltma işlemi senkronizasyonu
3. `tolls[].semaphore`: Gişe erişim kontrolü

## Thread Yönetimi

### Araç Thread'leri
- Her araç için bir thread
- İşlem sırası:
  1. Gişeden geçiş
  2. Bekleme alanına giriş
  3. Feribota yüklenme
  4. Karşıya geçiş
  5. İniş

### Feribot Thread'i
- Sürekli çalışan tek thread
- İşlem döngüsü:
  1. Mevcut araçları boşalt
  2. Yeni araçları yükle
  3. Diğer limana hareket
  4. Döngüyü tekrarla

## Veri Yapıları

### Vehicle (Araç)
```c
typedef struct {
    int id;              // Araç ID'si
    VehicleType type;    // Araç tipi (CAR, MINIBUS, TRUCK)
    int startPort;       // Başlangıç limanı (0 veya 1)
    int currentPort;     // Mevcut liman
    int tollNumber;      // Gişe numarası
} Vehicle;
```

### Ferry (Feribot)
```c
typedef struct {
    int currentPort;     // Mevcut liman
    int capacity;        // Toplam kapasite
    int currentLoad;     // Mevcut yük
    pthread_mutex_t mutex;
    sem_t loadingSem;
    sem_t unloadingSem;
} Ferry;
```

### Toll (Gişe)
```c
typedef struct {
    int portNumber;      // Liman numarası
    int tollNumber;      // Gişe numarası
    pthread_mutex_t mutex;
    sem_t semaphore;
} Toll;
```

### WaitingArea (Bekleme Alanı)
```c
typedef struct {
    int portNumber;      // Liman numarası
    Vehicle* vehicles[FERRY_CAPACITY];  // Araç kuyruğu
    int count;           // Mevcut araç sayısı
    pthread_mutex_t mutex;
} WaitingArea;
```

## Hata Yönetimi

1. Thread oluşturma hataları
2. Mutex ve semafor başlatma hataları
3. Bellek yönetimi
4. Senkronizasyon hataları

## Performans Özellikleri

1. Eşzamanlı araç işleme
2. Verimli kaynak kullanımı
3. Ölçeklenebilir yapı
4. Senkronizasyon optimizasyonu

## Güvenlik Önlemleri

1. Mutex kilitleri
2. Semafor kontrolleri
3. Thread güvenliği
4. Kaynak sızıntısı önleme

