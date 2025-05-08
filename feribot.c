/**
 * @file feribot.c
 * @brief Feribot simülasyonunun ana program dosyası
 * @author [Öğrenci Adı]
 * @date 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

// Sabit tanımlamalar
#define CAR_COUNT 12      // Toplam otomobil sayısı
#define MINIBUS_COUNT 10  // Toplam minibüs sayısı
#define TRUCK_COUNT 8     // Toplam kamyon sayısı
#define FERRY_CAPACITY 20 // Feribot kapasitesi
#define TOLL_COUNT 4      // Toplam gişe sayısı

// Araç tipleri
typedef enum
{
    CAR,
    MINIBUS,
    TRUCK
} VehicleType;

// Araç yapısı
typedef struct
{
    int id;
    VehicleType type;
    int startPort; // 0 veya 1
    int currentPort;
    int tollNumber;
} Vehicle;

// Feribot yapısı
typedef struct
{
    int currentPort;
    int capacity;
    int currentLoad;
    pthread_mutex_t mutex;
    sem_t loadingSem;
    sem_t unloadingSem;
} Ferry;

// Gişe yapısı
typedef struct
{
    int portNumber;
    int tollNumber;
    pthread_mutex_t mutex;
    sem_t semaphore;
} Toll;

// Bekleme alanı yapısı
typedef struct
{
    int portNumber;
    Vehicle *vehicles[FERRY_CAPACITY];
    int count;
    pthread_mutex_t mutex;
} WaitingArea;

// Global değişkenler
Ferry ferry;
Toll tolls[TOLL_COUNT];
WaitingArea waitingAreas[2];
pthread_mutex_t printMutex;

// Yardımcı fonksiyonlar
/**
 * @brief Rastgele bir liman numarası döndürür (0 veya 1)
 * @return int Rastgele seçilen liman numarası
 */
int getRandomPort()
{
    return rand() % 2;
}

/**
 * @brief Belirli bir liman için rastgele bir gişe numarası döndürür
 * @param port Liman numarası (0 veya 1)
 * @return int Rastgele seçilen gişe numarası
 */
int getRandomToll(int port)
{
    return port * 2 + (rand() % 2);
}

/**
 * @brief Araç tipine göre kapasite birimini döndürür
 * @param type Araç tipi (CAR, MINIBUS, TRUCK)
 * @return int Araç tipine göre kapasite birimi
 */
int getVehicleSize(VehicleType type)
{
    switch (type)
    {
    case CAR:
        return 1;
    case MINIBUS:
        return 2;
    case TRUCK:
        return 3;
    default:
        return 0;
    }
}

/**
 * @brief Thread-safe mesaj yazdırma fonksiyonu
 * @param message Yazdırılacak mesaj
 */
void printMessage(const char *message)
{
    pthread_mutex_lock(&printMutex);
    printf("%s\n", message);
    pthread_mutex_unlock(&printMutex);
}

// Araç thread fonksiyonu
/**
 * @brief Araç thread fonksiyonu
 * @param arg Araç yapısı pointer'ı
 * @return void* Thread dönüş değeri
 */
void *vehicleThread(void *arg)
{
    Vehicle *vehicle = (Vehicle *)arg;
    char message[256];

    sprintf(message, "Araç %d (%s) %d. limanda başladı",
            vehicle->id,
            vehicle->type == CAR ? "Otomobil" : vehicle->type == MINIBUS ? "Minibüs"
                                                                         : "Kamyon",
            vehicle->startPort);
    printMessage(message);

    // Gişeden geçiş
    pthread_mutex_lock(&tolls[vehicle->tollNumber].mutex);
    sprintf(message, "Araç %d %d. gişeden geçiyor", vehicle->id, vehicle->tollNumber);
    printMessage(message);
    sleep(1); // Gişe işlemi simülasyonu
    pthread_mutex_unlock(&tolls[vehicle->tollNumber].mutex);

    // Bekleme alanına giriş
    pthread_mutex_lock(&waitingAreas[vehicle->currentPort].mutex);
    waitingAreas[vehicle->currentPort].vehicles[waitingAreas[vehicle->currentPort].count++] = vehicle;
    pthread_mutex_unlock(&waitingAreas[vehicle->currentPort].mutex);

    return NULL;
}

/**
 * @brief Ana program fonksiyonu
 * @return int Program çıkış kodu
 */
int main()
{
    srand(time(NULL));

    // Mutex ve semaforların başlatılması
    pthread_mutex_init(&printMutex, NULL);
    pthread_mutex_init(&ferry.mutex, NULL);
    sem_init(&ferry.loadingSem, 0, 1);
    sem_init(&ferry.unloadingSem, 0, 1);

    for (int i = 0; i < TOLL_COUNT; i++)
    {
        pthread_mutex_init(&tolls[i].mutex, NULL);
        sem_init(&tolls[i].semaphore, 0, 1);
        tolls[i].portNumber = i / 2;
        tolls[i].tollNumber = i;
    }

    for (int i = 0; i < 2; i++)
    {
        pthread_mutex_init(&waitingAreas[i].mutex, NULL);
        waitingAreas[i].portNumber = i;
        waitingAreas[i].count = 0;
    }

    // Feribot başlangıç durumu
    ferry.currentPort = getRandomPort();
    ferry.capacity = FERRY_CAPACITY;
    ferry.currentLoad = 0;

    printf("Feribot %d. limanda başladı\n", ferry.currentPort);

    // Araçların oluşturulması
    pthread_t threads[CAR_COUNT + MINIBUS_COUNT + TRUCK_COUNT];
    Vehicle vehicles[CAR_COUNT + MINIBUS_COUNT + TRUCK_COUNT];
    int vehicleIndex = 0;

    // Otomobillerin oluşturulması
    for (int i = 0; i < CAR_COUNT; i++)
    {
        vehicles[vehicleIndex].id = vehicleIndex;
        vehicles[vehicleIndex].type = CAR;
        vehicles[vehicleIndex].startPort = getRandomPort();
        vehicles[vehicleIndex].currentPort = vehicles[vehicleIndex].startPort;
        vehicles[vehicleIndex].tollNumber = getRandomToll(vehicles[vehicleIndex].startPort);

        pthread_create(&threads[vehicleIndex], NULL, vehicleThread, &vehicles[vehicleIndex]);
        vehicleIndex++;
    }

    // Minibüslerin oluşturulması
    for (int i = 0; i < MINIBUS_COUNT; i++)
    {
        vehicles[vehicleIndex].id = vehicleIndex;
        vehicles[vehicleIndex].type = MINIBUS;
        vehicles[vehicleIndex].startPort = getRandomPort();
        vehicles[vehicleIndex].currentPort = vehicles[vehicleIndex].startPort;
        vehicles[vehicleIndex].tollNumber = getRandomToll(vehicles[vehicleIndex].startPort);

        pthread_create(&threads[vehicleIndex], NULL, vehicleThread, &vehicles[vehicleIndex]);
        vehicleIndex++;
    }

    // Kamyonların oluşturulması
    for (int i = 0; i < TRUCK_COUNT; i++)
    {
        vehicles[vehicleIndex].id = vehicleIndex;
        vehicles[vehicleIndex].type = TRUCK;
        vehicles[vehicleIndex].startPort = getRandomPort();
        vehicles[vehicleIndex].currentPort = vehicles[vehicleIndex].startPort;
        vehicles[vehicleIndex].tollNumber = getRandomToll(vehicles[vehicleIndex].startPort);

        pthread_create(&threads[vehicleIndex], NULL, vehicleThread, &vehicles[vehicleIndex]);
        vehicleIndex++;
    }

    // Thread'lerin tamamlanmasını bekle
    for (int i = 0; i < vehicleIndex; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Kaynakların temizlenmesi
    pthread_mutex_destroy(&printMutex);
    pthread_mutex_destroy(&ferry.mutex);
    sem_destroy(&ferry.loadingSem);
    sem_destroy(&ferry.unloadingSem);

    for (int i = 0; i < TOLL_COUNT; i++)
    {
        pthread_mutex_destroy(&tolls[i].mutex);
        sem_destroy(&tolls[i].semaphore);
    }

    for (int i = 0; i < 2; i++)
    {
        pthread_mutex_destroy(&waitingAreas[i].mutex);
    }

    return 0;
}