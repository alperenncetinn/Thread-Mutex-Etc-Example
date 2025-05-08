# Feribot Simülasyonu

Bu proje, iki liman arasında çalışan bir feribot sisteminin simülasyonunu gerçekleştirir.

## Özellikler

- 20 araç kapasiteli feribot
- 4 gişe (her limanda 2 adet)
- 12 otomobil, 10 minibüs ve 8 kamyon
- Her araç tipi için farklı kapasite kullanımı:
  - Otomobil: 1 birim
  - Minibüs: 2 birim
  - Kamyon: 3 birim

## Derleme

Projeyi derlemek için:

```bash
make
```

## Çalıştırma

Programı çalıştırmak için:

```bash
./feribot
```

## Temizleme

Derleme dosyalarını temizlemek için:

```bash
make clean
```

## Sistem Gereksinimleri

- GCC derleyici
- POSIX thread kütüphanesi
- Unix benzeri işletim sistemi (Linux, macOS)

## Proje Yapısı

- `feribot.c`: Ana program dosyası
- `ferry_operations.c`: Feribot işlemlerini yöneten fonksiyonlar
- `feribot.h`: Header dosyası
- `Makefile`: Derleme yapılandırması 