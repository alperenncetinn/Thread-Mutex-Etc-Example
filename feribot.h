/**
 * @file feribot.h
 * @brief Feribot simülasyonu için header dosyası
 * @author [Öğrenci Adı]
 * @date 2024
 */

#ifndef FERIBOT_H
#define FERIBOT_H

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

/**
 * @brief Araç tipleri için enum
 */
typedef enum
{
    CAR,     ///< Otomobil
    MINIBUS, ///< Minibüs
    TRUCK    ///< Kamyon
} VehicleType;

/**
 * @brief Araç yapısı
 */
typedef struct
{
    int id;           ///< Araç ID'si
    VehicleType type; ///< Araç tipi
    int startPort;    ///< Başlangıç limanı (0 veya 1)
    int currentPort;  ///< Mevcut liman
    int tollNumber;   ///< Gişe numarası
} Vehicle;

/**
 * @brief Feribot yapısı
 */
typedef struct
{
    int currentPort;       ///< Mevcut liman
    int capacity;          ///< Toplam kapasite
    int currentLoad;       ///< Mevcut yük
    pthread_mutex_t mutex; ///< Feribot mutex'i
    sem_t loadingSem;      ///< Yükleme semaforu
    sem_t unloadingSem;    ///< Boşaltma semaforu
} Ferry;

/**
 * @brief Gişe yapısı
 */
typedef struct
{
    int portNumber;        ///< Liman numarası
    int tollNumber;        ///< Gişe numarası
    pthread_mutex_t mutex; ///< Gişe mutex'i
    sem_t semaphore;       ///< Gişe semaforu
} Toll;

/**
 * @brief Bekleme alanı yapısı
 */
typedef struct
{
    int portNumber;                    ///< Liman numarası
    Vehicle *vehicles[FERRY_CAPACITY]; ///< Araç kuyruğu
    int count;                         ///< Mevcut araç sayısı
    pthread_mutex_t mutex;             ///< Bekleme alanı mutex'i
} WaitingArea;

// Global değişkenler
extern Ferry ferry;                 ///< Feribot yapısı
extern Toll tolls[TOLL_COUNT];      ///< Gişe dizisi
extern WaitingArea waitingAreas[2]; ///< Bekleme alanları dizisi
extern pthread_mutex_t printMutex;  ///< Yazdırma mutex'i

/**
 * @brief Rastgele bir liman numarası döndürür
 * @return int Rastgele seçilen liman numarası
 */
int getRandomPort();

/**
 * @brief Belirli bir liman için rastgele bir gişe numarası döndürür
 * @param port Liman numarası
 * @return int Rastgele seçilen gişe numarası
 */
int getRandomToll(int port);

/**
 * @brief Araç tipine göre kapasite birimini döndürür
 * @param type Araç tipi
 * @return int Araç tipine göre kapasite birimi
 */
int getVehicleSize(VehicleType type);

/**
 * @brief Thread-safe mesaj yazdırma fonksiyonu
 * @param message Yazdırılacak mesaj
 */
void printMessage(const char *message);

/**
 * @brief Araç thread fonksiyonu
 * @param arg Araç yapısı pointer'ı
 * @return void* Thread dönüş değeri
 */
void *vehicleThread(void *arg);

/**
 * @brief Feribot thread fonksiyonu
 * @param arg Thread parametresi
 * @return void* Thread dönüş değeri
 */
void *ferryThread(void *arg);

#endif // FERIBOT_H