# Resistor Color Code Calculator (C-Powered Web App)

Bu proje, direnç renk kodlarını kullanarak direnç değerlerini hesaplayan hibrit bir web uygulamasıdır. Hesaplama motoru yüksek hassasiyet için **C** diliyle yazılmış, kullanıcı arayüzü ise **Python/Flask** ve modern web teknolojileri ile geliştirilmiştir.

## 🌟 Özellikler

* **4 ve 5 Band Desteği:** Standart ve yüksek hassasiyetli tüm direnç tiplerini hesaplar.
* **Gerçek Zamanlı Görselleştirme:** Seçilen renklere göre dinamik olarak güncellenen CSS tabanlı direnç figürü.
* **Hata Yönetimi:** Geçersiz renk dizilimleri (örn: ilk bandın siyah olması) için kullanıcıya anlamlı geri bildirimler verir.
* **Birim Dönüştürme:** Sonuçları otomatik olarak Ω, kΩ, MΩ veya GΩ cinsinden formatlar.
* **Tolerans Analizi:** Minimum ve maksimum direnç değerlerini (hata payı dahil) hesaplar.

## 🚀 Teknoloji Yığını (Tech Stack)

- **Hesaplama Motoru:** C (GCC 11+)
- **Web Backend:** Python 3.x, Flask
- **Frontend:** HTML5, CSS3 (Modern Flexbox), JavaScript (Vanilla)
- **Veri Formatı:** JSON (C ve Python arasındaki iletişim için)

## ⚙️ Çalışma Mantığı

1.  **Kullanıcı Girişi:** Kullanıcı web arayüzünden band sayısını ve renkleri seçer.
2.  **API İsteği:** Seçilen renkler JSON formatında Flask sunucusuna iletilir.
3.  **C Subprocess:** Flask, önceden derlenmiş olan `resistor_logic` C binary dosyasını `subprocess` kütüphanesi ile çağırır.
4.  **Hızlı Hesaplama:** C programı, gelen argümanları işler, matematiksel formülleri uygular ve sonucu standart çıktıya JSON olarak döner.
5.  **Render:** Flask'tan gelen veri, JavaScript tarafından parse edilerek sonuç kartına yansıtılır.

## 🛠 Kurulum ve Çalıştırma

### Gereksinimler
- GCC (C derleyicisi)
- Python 3.8+
- Flask (`pip install flask`)

### Adımlar

1. **Depoyu klonlayın:**
   ```bash
   git clone [https://github.com/kullaniciadi/resistor-calculator.git](https://github.com/kullaniciadi/resistor-calculator.git)
   cd resistor-calculator
