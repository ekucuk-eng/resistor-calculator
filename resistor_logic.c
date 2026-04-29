/*
    resistor_logic.c - 4,5 band direnç hesaplama motoru
    Kullanım: ./resistor_logic <band1> <band2> <band3> <band4> <band5?>
    Renkler: siyah, kahverengi, kirmizi, turuncu, sari, yesil, mavi, mor, gri, beyaz, altin, gumus
    Çıktı: JSON formatında sonuç veya hata mesajı
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

typedef struct 
{
    char isim[11];
    int deger;
    double carpan;
    double tolerans;
} Renk;

Renk renkler[] = 
{   // {"Renk", değer, çarpan, tolerans}
    {"siyah", 0, 1.0, 0.0},
    {"kahverengi", 1, 10.0, 1.0},
    {"kirmizi", 2, 100.0, 2.0},
    {"turuncu", 3, 1000.0, 0.05},
    {"sari", 4, 10000.0, 0.0},
    {"yesil", 5, 100000.0, 0.5},
    {"mavi", 6, 1000000.0, 0.25},
    {"mor", 7, 10000000.0, 0.1},
    {"gri", 8, 100000000.0, 0.01},
    {"beyaz", 9, 1000000000.0, 0.0},
    {"altin", -1, 0.1, 5.0},
    {"gumus", -1, 0.01, 10.0}
};

#define RENK_SAYISI (sizeof(renkler) / sizeof(Renk))

Renk* renk_bul(char* isim); //Renk struct'inin pointerini döner.
double hesapla_4_band(Renk* b1, Renk* b2, Renk* carpan, Renk* tolerans, double* r_min, double* r_max);
double hesapla_5_band(Renk* b1, Renk* b2, Renk* b3, Renk* carpan, Renk* tolerans, double* r_min, double* r_max);
int ilk_band_kontrolu(Renk* ilk_band);

int main(int band_c, char* band[]) 
{
    int band_sayisi = band_c - 1;
    double r = 0, r_min = 0, r_max = 0;
    char tolerans_str[20] = "±0%";
    char birim[3] = "&Omega;";
    if (band_c < 5 || band_c > 7) 
    {
        printf("{\"hata\":\"Kullanım: %s <band1> <band2> <band3> <band4> [band5] \\nBandlar: siyah, kahverengi, kirmizi, turuncu, sari, yesil, mavi, mor, gri, beyaz, altin, gumus\"}", band[0]);
        //JSON formatında hata mesajı: {"hata":"Kullanım: ./resistor_logic <band1> <band2>..."}
        return 1;
    }
    
    Renk* b1 = renk_bul(band[1]);
    Renk* b2 = renk_bul(band[2]);
    Renk* b3 = renk_bul(band[3]);
    Renk* b4 = renk_bul(band[4]);
    Renk* b5 = NULL;

    if (band_sayisi >=5)
    {    b5 = renk_bul(band[5]);    }
    
    //herhangi biri dogru olduğunda renk bulamamıs(NULL) oluyor
    if (!b1 || !b2 || !b3 || !b4) 
    {
        printf("{\"hata\":\"Geçersiz renk ismi!\"}");
        //JSON formatında hata mesajı: {"hata":"Geçersiz renk ismi!"}
        return 1;
    }
    
    // İlk band kontrolü (siyah/altın/gümüş olamaz)
    if (ilk_band_kontrolu(b1)) 
    {    return 1;    }
    
    if (band_sayisi == 4) 
    {
        r = hesapla_4_band(b1, b2, b3, b4, &r_min, &r_max);
        sprintf(tolerans_str, "±%.2f%%", b4->tolerans); //sprintf : formatlı string oluşturur, 
    } else if (band_sayisi == 5) 
    {
        if (!b5) 
        {
            printf("{\"hata\":\"5 band için 5. renk (tolerans) gerekli\"}");
            return 1;
        }
        r = hesapla_5_band(b1, b2, b3, b4, b5, &r_min, &r_max);
        sprintf(tolerans_str, "±%.2f%%", b5->tolerans);
        
    } 
    
    // Birim düzenleme (k, M, G)
    if (r >= 1000000000) 
    {
        r = r / 1000000000.0;   //1e9
        r_min = r_min / 1000000000.0;
        r_max = r_max / 1000000000.0;
         strcpy(birim, "G&Omega;");  // HTML entity
    } else if (r >= 1000000) 
    {
        r = r / 1000000.0;  //1e6
        r_min = r_min / 1000000.0;  
        r_max = r_max / 1000000.0;
         strcpy(birim, "M&Omega;");  //strcpy: Metni olduğu gibi degiskene atar.
    } else if (r >= 1000) 
    {
        r = r / 1000.0;     //1e3
        r_min = r_min / 1000.0;
        r_max = r_max / 1000.0;
        strcpy(birim, "k&Omega;");  
    } else 
    {   strcpy(birim, "&Omega;");    }
    
    printf("{");
    printf("\"direnc\":%.3f,", r);
    printf("\"birim\":\"%s\",", birim);
    printf("\"tolerans\":\"%s\",", tolerans_str);
    printf("\"min\":%.3f,", r_min);
    printf("\"max\":%.3f", r_max);
    printf("}");
    
    // JSON formatında çıktı: {"direnc":%.3f,"birim":"%s\","tolerans":"%s","min":%.3f,"max":%.3f}
    return 0;
}

Renk* renk_bul(char* isim) //Renk struct'inin pointerini döner.
{
    for (int i = 0; i < RENK_SAYISI; i++) 
    {
        if (strcmp(renkler[i].isim, isim) == 0) 
        {    return &renkler[i];    }
    }
    return NULL; //renk bulamazsa
}

double hesapla_4_band(Renk* b1, Renk* b2, Renk* carpan, Renk* tolerans, double* r_min, double* r_max) 
{
    // Değer hesaplama: (b1 * 10 + b2) * carpan
    int deger = (b1->deger * 10) + (*b2).deger; //b -> deger <=> (*b2).deger
    double r = deger * carpan->carpan;
    
    if (tolerans->tolerans > 0) 
    {
        *r_min = r - (r * tolerans->tolerans / 100.0);
        *r_max = r + (r * (*tolerans).tolerans / 100.0);
    } else 
    {
        *r_min = r;
        *r_max = r;
    }
    
    return r;
}

double hesapla_5_band(Renk* b1, Renk* b2, Renk* b3, Renk* carpan, Renk* tolerans, double* r_min, double* r_max) 
{
    // Değer hesaplama: (b1 * 100 + b2 * 10 + b3) * carpan
    int deger = (b1->deger * 100) + (b2->deger * 10) + b3->deger;
    double r = deger * carpan->carpan;
    
    if (tolerans->tolerans > 0) 
    {
        *r_min = r - (r * tolerans->tolerans / 100.0);
        *r_max = r + (r * (*tolerans).tolerans / 100.0);
    } else 
    {
        *r_min = r;
        *r_max = r;
    }
    
    return r;
}


int ilk_band_kontrolu(Renk* ilk_band) 
{
    // 1. Güvenlik Kontrolü: Pointer boş mu?
    if (ilk_band == NULL) 
    {
        printf("{\"hata\":\"Renk verisi alınamadı!\"}");
        return 1;
    }

    // 2.Teknik Kontrol: Siyah(0) veya Metalik (altın/gümüş -1) renkler
    if (ilk_band->deger <= 0) 
    {
        printf("{\"hata\":\"Ters tutuyorsun! İlk band siyah, altın veya gümüş olamaz.\"}");
        return 1;
    }

    return 0;
}



