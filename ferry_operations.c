/**
 * @file ferry_operations.c
 * @brief Feribot işlemlerini yöneten fonksiyonlar
 * @author [Öğrenci Adı]
 * @date 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include "feribot.h"

/**
 * @brief Feribot thread fonksiyonu
 *
 * Bu fonksiyon feribotun sürekli çalışan ana döngüsünü yönetir:
 * 1. Mevcut limandaki araçları boşaltır
 * 2. Yeni araçları yükler
 * 3. Diğer limana hareket eder
 * 4. İşlemi tekrarlar
 *
 * @param arg Thread parametresi (kullanılmıyor)
 * @return void* Thread dönüş değeri
 */
void *ferryThread(void *arg)
{
    char message[256];

    while (1)
    {
        // Mevcut limandaki araçları boşalt
        sem_wait(&ferry.unloadingSem);
        pthread_mutex_lock(&ferry.mutex);

        if (ferry.currentLoad > 0)
        {
            sprintf(message, "Feribot %d. limanda araçları boşaltıyor", ferry.currentPort);
            printMessage(message);
            sleep(2); // Boşaltma işlemi simülasyonu
            ferry.currentLoad = 0;
        }

        pthread_mutex_unlock(&ferry.mutex);
        sem_post(&ferry.unloadingSem);

        // Yeni araçları yükle
        sem_wait(&ferry.loadingSem);
        pthread_mutex_lock(&ferry.mutex);
        pthread_mutex_lock(&waitingAreas[ferry.currentPort].mutex);

        int remainingCapacity = ferry.capacity;
        int loadedVehicles = 0;

        // Bekleme alanındaki araçları feribota yükle
        while (remainingCapacity > 0 && waitingAreas[ferry.currentPort].count > 0)
        {
            Vehicle *vehicle = waitingAreas[ferry.currentPort].vehicles[0];
            int vehicleSize = getVehicleSize(vehicle->type);

            if (vehicleSize <= remainingCapacity)
            {
                // Aracı feribota yükle
                sprintf(message, "Araç %d feribota yükleniyor", vehicle->id);
                printMessage(message);

                // Bekleme alanından aracı çıkar
                for (int i = 0; i < waitingAreas[ferry.currentPort].count - 1; i++)
                {
                    waitingAreas[ferry.currentPort].vehicles[i] = waitingAreas[ferry.currentPort].vehicles[i + 1];
                }
                waitingAreas[ferry.currentPort].count--;

                remainingCapacity -= vehicleSize;
                ferry.currentLoad += vehicleSize;
                loadedVehicles++;

                sleep(1); // Yükleme işlemi simülasyonu
            }
            else
            {
                break;
            }
        }

        pthread_mutex_unlock(&waitingAreas[ferry.currentPort].mutex);
        pthread_mutex_unlock(&ferry.mutex);
        sem_post(&ferry.loadingSem);

        if (loadedVehicles > 0)
        {
            // Diğer limana hareket et
            sprintf(message, "Feribot %d. limandan %d. limana hareket ediyor",
                    ferry.currentPort, 1 - ferry.currentPort);
            printMessage(message);

            sleep(3); // Yolculuk simülasyonu

            ferry.currentPort = 1 - ferry.currentPort;

            sprintf(message, "Feribot %d. limana vardı", ferry.currentPort);
            printMessage(message);
        }
        else
        {
            // Eğer yüklenecek araç yoksa, diğer limana geç
            if (waitingAreas[1 - ferry.currentPort].count > 0)
            {
                sprintf(message, "Feribot boş olarak %d. limana hareket ediyor", 1 - ferry.currentPort);
                printMessage(message);

                sleep(3); // Yolculuk simülasyonu

                ferry.currentPort = 1 - ferry.currentPort;

                sprintf(message, "Feribot %d. limana vardı", ferry.currentPort);
                printMessage(message);
            }
            else
            {
                // Her iki limanda da araç yoksa bekle
                sleep(1);
            }
        }
    }

    return NULL;
}