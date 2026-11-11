/*
 * ============================================================================
 *  MARS KOLONİSİ YAŞAM DESTEK SİMÜLATÖRÜ
 *  Karakter tabanlı, metin tabanlı hayatta kalma oyunu (C99)
 * ============================================================================
 *
 *  Oyuncu, Mars'taki bir araştırma istasyonunda tek başına kalmış bir mürettebat
 *  üyesini yönetir ve kurtarma ekibi gelene kadar 15 gün hayatta kalmaya çalışır.
 *
 *  Zorunlu komutlar : G, O, T, D, U, K, H, Q
 *  Ek komutlar      : M (Mars görevi, kredi kazanma)
 *                     S (kıyafet mağazası, kredi ile can artırma)
 *                     Y (karakterin aktif yeteneği)
 *                     B (komut listesi / yardım)
 *
 *  Derleme : gcc -std=c99 -Wall -Wextra -o mars_koloni mars_koloni.c
 *  Çalıştırma: ./mars_koloni        (Windows: mars_koloni.exe)
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define HEDEF_GUN 15        /* Kurtarma ekibinin geleceği gün            */
#define GUN_BASINA_TUR 3    /* Her 3 turda bir yeni gün başlar           */
#define YETENEK_BEKLEME 3   /* Aktif yetenek en erken 3 günde bir kullanılır */

/* ----------------------------------------------------------------------------
 *  Yardımcı fonksiyonlar (oyun durumunu değiştirmezler)
 * ------------------------------------------------------------------------- */

/* Satır sonuna kadar kalan karakterleri atar (taşan girişleri temizlemek için). */
void tamponuTemizle(void)
{
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

/*
 * Kullanıcıdan bir satır okur ve satırdaki TEK karakteri döndürür.
 *  - Satırda tam olarak bir karakter varsa o karakter döner.
 *  - Boş satır veya birden fazla karakter girildiyse '?' döner (geçersiz komut).
 *  - Giriş sona erdiyse (EOF) 0 döner.
 */
char karakterOku(void)
{
    char satir[64];
    char ilk = 0;
    int adet = 0;
    int i;

    fflush(stdout);   /* Soru metni girişten önce mutlaka ekranda görünsün */
    if (fgets(satir, sizeof satir, stdin) == NULL) {
        return 0;
    }
    if (strchr(satir, '\n') == NULL) {
        tamponuTemizle();
    }
    for (i = 0; satir[i] != '\0'; i++) {
        if (satir[i] != ' ' && satir[i] != '\t' && satir[i] != '\n' && satir[i] != '\r') {
            if (adet == 0) {
                ilk = satir[i];
            }
            adet++;
        }
    }
    if (adet != 1) {
        return '?';
    }
    return ilk;
}

/*
 * Kullanıcıdan negatif olmayan bir tam sayı okur.
 *  -1: geçersiz giriş (harf, negatif sayı, "12abc" gibi)
 *  -2: giriş sona erdi (EOF)
 */
int sayiOku(void)
{
    char satir[64];
    int sayi;
    char fazla;

    fflush(stdout);
    if (fgets(satir, sizeof satir, stdin) == NULL) {
        return -2;
    }
    if (strchr(satir, '\n') == NULL) {
        tamponuTemizle();
    }
    if (sscanf(satir, "%d %c", &sayi, &fazla) != 1) {
        return -1;
    }
    if (sayi < 0) {
        return -1;
    }
    return sayi;
}

/* Ödevin istediği biçimde zar sonucunu yazdırır: "Zar: 37 / Eşik: 45 -> BAŞARILI" */
void zarYazdir(const char *baslik, int zar, int esik, const char *basarili, const char *basarisiz)
{
    printf("  %s Zar: %d / Eşik: %d -> %s\n", baslik, zar, esik, (zar < esik) ? basarili : basarisiz);
}

void cizgi(void)
{
    printf("------------------------------------------------------------------\n");
}

void komutListesi(void)
{
    cizgi();
    printf("  KOMUTLAR (büyük/küçük harf fark etmez)\n");
    cizgi();
    printf("  G  Keşif görevi        (-15 enerji, -10 oksijen; su/parça/kredi)\n");
    printf("  O  Oksijen üret        (2 birim su -> 3 birim oksijen)\n");
    printf("  T  Tamir               (yedek parça ile modül hasarını azalt)\n");
    printf("  D  Durum raporu        (tur harcamaz)\n");
    printf("  U  Uyu / dinlen        (+enerji, +sağlık, oksijen tüketimi sürer)\n");
    printf("  K  Kum fırtınası       (fırtınayı tur tur simüle et)\n");
    printf("  H  Hava kilidi şifresi (3 denemede kilidi mühürle)\n");
    printf("  M  Mars görevi         (riskli görevlerle Mars Kredisi kazan)\n");
    printf("  S  Kıyafet mağazası    (kredinle kıyafet al, canını artır)\n");
    printf("  Y  Özel yetenek        (karakterine özgü aktif yetenek)\n");
    printf("  B  Bu listeyi göster   (tur harcamaz)\n");
    printf("  Q  Çıkış\n");
    cizgi();
}

/* ----------------------------------------------------------------------------
 *  ANA PROGRAM
 * ------------------------------------------------------------------------- */
int main(void)
{
    /* --- Durum değişkenleri (ödevde istenen) --- */
    int saglik = 100, enerji = 100, oksijen = 100;
    int su = 0, yedekParca = 0, modulHasari = 0;
    int kilitMuhurlu = 0;
    int gun = 1, tur = 0;

    /* --- Ek sistemler: para, kıyafet, karakter --- */
    int kredi = 0;          /* Mars Kredisi (MK)                            */
    int koruma = 0;         /* Giyilen kıyafetlerin toplam hasar koruması    */
    int kiyafet1 = 0, kiyafet2 = 0, kiyafet3 = 0, kiyafet4 = 0, kiyafet5 = 0;
    int karakter = 0;
    int sonYetenekGunu = -YETENEK_BEKLEME; /* İlk günden itibaren kullanılabilsin */
    int toplamKazanilanKredi = 0, gorevBasari = 0, gorevBasarisiz = 0;

    /* --- Zorluk seviyesine bağlı ayarlar --- */
    char zorluk;
    int firtinaTurSayisi = 5, firtinaHasari = 6;
    int suEsikYuksek = 60, suEsikDusuk = 30;
    int yaralanmaHasari = 12, gorevEsikBonus = 0;

    /* --- Karaktere bağlı yetenek katsayıları --- */
    int tamirBonus = 0, tamirEnerji = 10;
    int parcaEsikBonus = 0, gorevOdulYuzde = 100;
    int dinlenmeSaglikBonus = 0, yaralanmaBolen = 1;
    int oksijenEnerji = 8, suEsikBonus = 0;
    int firtinaKorumaBonus = 0, gorevBasariBonus = 0;
    const char *karakterAdi = "";
    const char *karakterMeslek = "";
    const char *yetenekAdi = "";

    /* --- Döngü ve işlem değişkenleri --- */
    char komut, secim, tahmin, sifre;
    int oyunDurumu = 0;   /* 0: sürüyor, 1: kazandı, 2: kaybetti, 3: çıkış */
    int turHarcandi;
    int zar, esik, miktar, i;
    int puan;

#ifdef _WIN32
    SetConsoleOutputCP(65001); /* Türkçe karakterler Windows konsolunda doğru görünsün */
    SetConsoleCP(65001);
#endif

    srand((unsigned int) time(NULL));

    printf("\n==================================================================\n");
    printf("        MARS KOLONİSİ YAŞAM DESTEK SİMÜLATÖRÜ\n");
    printf("==================================================================\n");
    printf("  Ares-7 araştırma istasyonundasın. Mürettebatın geri kalanı tahliye\n");
    printf("  edildi; kurtarma ekibi %d gün sonra gelecek. Oksijenini, enerjini\n", HEDEF_GUN);
    printf("  ve modülünü koru, görevlerden kredi kazan, hayatta kal.\n\n");

    /* ===================== ZORLUK SEÇİMİ (SWITCH-CASE) ===================== */
    do {
        printf("Zorluk seviyesi seçin  [K] Kolay  [N] Normal  [Z] Zor > ");
        zorluk = karakterOku();
        if (zorluk == 0) {
            printf("\nGiriş sona erdi. Program kapatılıyor.\n");
            return 0;
        }
        switch (zorluk) {
        case 'k':
        case 'K':
            zorluk = 'K';
            saglik = 100; enerji = 100; oksijen = 100;
            su = 60; yedekParca = 5; modulHasari = 10; kredi = 30;
            firtinaTurSayisi = 3; firtinaHasari = 4;
            suEsikYuksek = 70; suEsikDusuk = 40;
            yaralanmaHasari = 8; gorevEsikBonus = 10;
            printf("  KOLAY seçildi: bol kaynak, kısa ve hafif fırtınalar.\n");
            break;
        case 'n':
        case 'N':
            zorluk = 'N';
            saglik = 100; enerji = 90; oksijen = 90;
            su = 50; yedekParca = 3; modulHasari = 20; kredi = 15;
            firtinaTurSayisi = 5; firtinaHasari = 6;
            suEsikYuksek = 60; suEsikDusuk = 30;
            yaralanmaHasari = 12; gorevEsikBonus = 0;
            printf("  NORMAL seçildi: dengeli kaynaklar ve 5 turluk fırtınalar.\n");
            break;
        case 'z':
        case 'Z':
            zorluk = 'Z';
            saglik = 90; enerji = 80; oksijen = 80;
            su = 55; yedekParca = 2; modulHasari = 35; kredi = 0;
            firtinaTurSayisi = 7; firtinaHasari = 9;
            suEsikYuksek = 45; suEsikDusuk = 20;
            yaralanmaHasari = 15; gorevEsikBonus = -10;
            printf("  ZOR seçildi: az su, yüksek hasar, 7 turluk fırtınalar.\n");
            break;
        default:
            printf("  Geçersiz seviye! Lütfen yalnızca K, N veya Z girin.\n");
            zorluk = 0;
            break;
        }
    } while (zorluk == 0);

    /* ===================== KARAKTER SEÇİMİ (SWITCH-CASE) ===================== */
    printf("\n");
    cizgi();
    printf("  MÜRETTEBAT SEÇİMİ - Her karakterin bir pasif, bir aktif yeteneği var\n");
    cizgi();
    printf("  [1] Kerem Aydın  - Mühendis\n");
    printf("      Pasif  Usta Eller     : Tamir +10 hasar onarır, enerji maliyeti 5.\n");
    printf("      Aktif  Acil Kaynak    : Parça harcamadan modül hasarını 25 azaltır.\n");
    printf("  [2] Elif Yıldız  - Jeolog\n");
    printf("      Pasif  Maden Gözü     : Keşifte parça bulma şansı +10, görev ödülü +%%25.\n");
    printf("      Aktif  Derin Sondaj   : +2 yedek parça, +4 su ve +20 kredi bulur.\n");
    printf("  [3] Selin Kaya   - Doktor\n");
    printf("      Pasif  Saha Hekimi    : Dinlenmede +10 ek sağlık, yaralanma hasarı yarıya.\n");
    printf("      Aktif  Acil Müdahale  : Anında +30 sağlık.\n");
    printf("  [4] Mert Demir   - Kimyager\n");
    printf("      Pasif  Verimli Reaktör: Oksijen üretimi 5 enerji, su bulma şansı +5.\n");
    printf("      Aktif  Elektroliz     : Su harcamadan +6 oksijen üretir.\n");
    printf("  [5] Ayşe Çelik   - Pilot\n");
    printf("      Pasif  Fırtına Avcısı : Fırtınada korunma +15, görev başarısı +10.\n");
    printf("      Aktif  Rover Seferi   : Rover ile +6 su ve +25 kredi getirir.\n");
    cizgi();

    do {
        printf("Karakterinizi seçin (1-5) > ");
        secim = karakterOku();
        if (secim == 0) {
            printf("\nGiriş sona erdi. Program kapatılıyor.\n");
            return 0;
        }
        switch (secim) {
        case '1':
            karakter = 1; karakterAdi = "Kerem Aydın"; karakterMeslek = "Mühendis";
            yetenekAdi = "Acil Kaynak";
            tamirBonus = 10; tamirEnerji = 5;
            break;
        case '2':
            karakter = 2; karakterAdi = "Elif Yıldız"; karakterMeslek = "Jeolog";
            yetenekAdi = "Derin Sondaj";
            parcaEsikBonus = 10; gorevOdulYuzde = 125;
            break;
        case '3':
            karakter = 3; karakterAdi = "Selin Kaya"; karakterMeslek = "Doktor";
            yetenekAdi = "Acil Müdahale";
            dinlenmeSaglikBonus = 10; yaralanmaBolen = 2;
            break;
        case '4':
            karakter = 4; karakterAdi = "Mert Demir"; karakterMeslek = "Kimyager";
            yetenekAdi = "Elektroliz";
            oksijenEnerji = 5; suEsikBonus = 5;
            break;
        case '5':
            karakter = 5; karakterAdi = "Ayşe Çelik"; karakterMeslek = "Pilot";
            yetenekAdi = "Rover Seferi";
            firtinaKorumaBonus = 15; gorevBasariBonus = 10;
            break;
        default:
            printf("  Geçersiz seçim! 1 ile 5 arasında bir rakam girin.\n");
            karakter = 0;
            break;
        }
    } while (karakter == 0);

    printf("\n  %s (%s) istasyonun kontrolünü aldı. Bol şans!\n", karakterAdi, karakterMeslek);
    komutListesi();

    /* ======================= ANA KOMUT DÖNGÜSÜ (DO-WHILE) ======================= */
    do {
        printf("\n[Gün %d/%d | Tur %d | Sağlık %d | Enerji %d | Oksijen %d | Su %d | Parça %d | Hasar %d | Kredi %d MK]\n",
               gun, HEDEF_GUN, tur, saglik, enerji, oksijen, su, yedekParca, modulHasari, kredi);
        printf("Komut > ");
        komut = karakterOku();
        if (komut == 0) {
            printf("\nGiriş sona erdi, simülasyon kapatılıyor.\n");
            komut = 'Q';
        }
        turHarcandi = 0;

        switch (komut) {

        /* ------------------------------ G: KEŞİF ------------------------------ */
        case 'g':
        case 'G': {
            int suEsik, parcaEsik, kazanc = 0, yaraZari, kayip;

            if (enerji < 15) {
                printf("  Yetersiz enerji! Keşif için en az 15 enerji gerekir (mevcut: %d). Komut iptal.\n", enerji);
                break;
            }
            /* Su bulma eşiği, yola çıkmadan önceki duruma göre belirlenir (&& ile). */
            if (enerji > 40 && oksijen > 30) {
                suEsik = suEsikYuksek;
                printf("  Dinç ve oksijenin yeterli: su bulma şansın YÜKSEK.\n");
            } else {
                suEsik = suEsikDusuk;
                printf("  Yorgunsun veya oksijenin az: su bulma şansın DÜŞÜK.\n");
            }
            suEsik = suEsik + suEsikBonus;
            if (suEsik > 85) {
                suEsik = 85;
            }
            parcaEsik = suEsik + 20 + parcaEsikBonus;
            if (parcaEsik > 90) {
                parcaEsik = 90;
            }

            enerji = enerji - 15;
            oksijen = oksijen - 10;
            printf("  Keşif başladı (-15 enerji, -10 oksijen).\n");

            zar = rand() % 100;
            if (zar < suEsik) {
                miktar = 12 + zar % 7;          /* 12-18 birim arası su */
                su = su + miktar;
                kazanc = 10;
                zarYazdir("Su arama:", zar, suEsik, "BAŞARILI", "BAŞARISIZ");
                printf("  SONUÇ: Buz cebi bulundu! +%d birim su.\n", miktar);
            } else if (zar < parcaEsik) {
                yedekParca = yedekParca + 1;
                kazanc = 15;
                zarYazdir("Parça arama:", zar, parcaEsik, "BAŞARILI", "BAŞARISIZ");
                printf("  SONUÇ: Eski bir iniş aracının enkazından +1 yedek parça söküldü.\n");
            } else if (zar >= 92) {
                kazanc = 40;
                zarYazdir("Nadir bulgu:", zar, 92, "YOK", "VAR");
                printf("  SONUÇ: Nadir bir mineral örneği buldun! Koloni merkezine satılabilir.\n");
            } else {
                zarYazdir("Keşif:", zar, parcaEsik, "BAŞARILI", "BAŞARISIZ");
                printf("  SONUÇ: Eli boş döndün. Bölge daha önce taranmış.\n");
            }
            if (kazanc > 0) {
                kazanc = kazanc * gorevOdulYuzde / 100;
                kredi = kredi + kazanc;
                toplamKazanilanKredi = toplamKazanilanKredi + kazanc;
                printf("  Keşif raporu için +%d MK kazandın.\n", kazanc);
            }

            /* Yaralanma kontrolü: zar < 10 VEYA modül ağır hasarlıysa (|| ile). */
            yaraZari = rand() % 100;
            zarYazdir("Yaralanma:", yaraZari, 10, "YARALANDIN", "GÜVENDESİN");
            if (yaraZari < 10 || modulHasari > 70) {
                kayip = yaralanmaHasari / yaralanmaBolen - koruma;
                if (kayip < 1) {
                    kayip = 1;
                }
                saglik = saglik - kayip;
                if (yaraZari >= 10) {
                    printf("  Ağır hasarlı modülden çıkarken kayıp bir kablo seni yaraladı!\n");
                }
                printf("  Yaralandın: -%d sağlık (kıyafet koruması: %d).\n", kayip, koruma);
            }
            turHarcandi = 1;
            break;
        }

        /* --------------------------- O: OKSİJEN ÜRET --------------------------- */
        case 'o':
        case 'O': {
            int artik, kullanilan, uretilen;

            if (su < 2) {
                printf("  Yetersiz su! Oksijen üretmek için en az 2 birim su gerekir (mevcut: %d).\n", su);
                break;
            }
            if (enerji < oksijenEnerji) {
                printf("  Yetersiz enerji! Reaktör için %d enerji gerekir (mevcut: %d). Komut iptal.\n",
                       oksijenEnerji, enerji);
                break;
            }
            if (oksijen >= 100) {
                printf("  Oksijen tankı zaten dolu. Su harcanmadı.\n");
                break;
            }
            do {
                printf("  Kaç birim su kullanılsın? (1-%d, iptal: 0) > ", su);
                miktar = sayiOku();
                if (miktar == -2) {
                    miktar = 0;
                } else if (miktar == -1) {
                    printf("  Geçersiz giriş, lütfen bir sayı girin.\n");
                } else if (miktar > su) {
                    printf("  Depoda yalnızca %d birim su var, daha fazlası girilemez.\n", su);
                }
            } while (miktar == -1 || miktar > su);

            if (miktar == 0) {
                printf("  Oksijen üretimi iptal edildi.\n");
                break;
            }
            artik = miktar % 2;              /* Tek sayıda 1 birim artık kalır */
            kullanilan = miktar - artik;
            if (kullanilan == 0) {
                printf("  1 birim su oksijene dönüştürülemez (2 su -> 3 oksijen). Komut iptal.\n");
                break;
            }
            uretilen = (kullanilan / 2) * 3;
            /* Tank 100'ü aşmasın: gereğinden fazla su harcanmasın diye miktarı azalt. */
            while (kullanilan > 2 && oksijen + uretilen - 3 >= 100) {
                kullanilan = kullanilan - 2;
                uretilen = uretilen - 3;
            }
            if (oksijen + uretilen > 100) {
                uretilen = 100 - oksijen;
            }
            su = su - kullanilan;
            oksijen = oksijen + uretilen;
            enerji = enerji - oksijenEnerji;
            printf("  %d birim su elektrolize edildi -> +%d oksijen (-%d enerji).\n",
                   kullanilan, uretilen, oksijenEnerji);
            if (artik == 1) {
                printf("  Tek sayı girildi: 1 birim su artık olarak depoda kaldı (%d %% 2 = 1).\n", miktar);
            }
            if (kullanilan + artik < miktar) {
                printf("  Tank doldu; kullanılmayan %d birim su depoda bekliyor.\n", miktar - kullanilan - artik);
            }
            turHarcandi = 1;
            break;
        }

        /* ------------------------------ T: TAMİR ------------------------------ */
        case 't':
        case 'T': {
            int gerekenParca, onarim, harcanan;
            const char *kademe;

            if (yedekParca == 0) {
                printf("  Yedek parça yok, tamir yapılamaz! Keşif veya görevlerle parça bul.\n");
                break;
            }
            if (modulHasari == 0) {
                printf("  Modül zaten sağlam, tamire gerek yok.\n");
                break;
            }
            if (enerji < tamirEnerji) {
                printf("  Yetersiz enerji! Tamir için %d enerji gerekir (mevcut: %d). Komut iptal.\n",
                       tamirEnerji, enerji);
                break;
            }
            /* Hasar seviyesi IF - ELSE IF - ELSE zinciriyle üç kademeye ayrılır. */
            if (modulHasari <= 30) {
                kademe = "HAFİF";
                gerekenParca = 1;
                onarim = 15;
            } else if (modulHasari <= 70) {
                kademe = "ORTA";
                gerekenParca = 2;
                onarim = 25;
            } else {
                kademe = "AĞIR";
                gerekenParca = 3;
                onarim = 40;
            }
            printf("  Hasar kademesi: %s (%d) - %d parça ile %d hasar onarılabilir.\n",
                   kademe, modulHasari, gerekenParca, onarim);
            if (yedekParca < gerekenParca) {
                onarim = onarim * yedekParca / gerekenParca;   /* Eksik parçayla kısmi onarım */
                harcanan = yedekParca;
                printf("  Yeterli parça yok: %d/%d parçayla kısmi onarım yapılıyor.\n", yedekParca, gerekenParca);
            } else {
                harcanan = gerekenParca;
            }
            if (tamirBonus > 0) {
                onarim = onarim + tamirBonus;
                printf("  Usta Eller: mühendislik deneyimiyle +%d ek onarım.\n", tamirBonus);
            }
            modulHasari = modulHasari - onarim;
            yedekParca = yedekParca - harcanan;
            enerji = enerji - tamirEnerji;
            printf("  Tamir tamamlandı: -%d hasar, -%d parça, -%d enerji.\n", onarim, harcanan, tamirEnerji);
            turHarcandi = 1;
            break;
        }

        /* --------------------------- D: DURUM RAPORU --------------------------- */
        case 'd':
        case 'D':
            cizgi();
            printf("  DURUM RAPORU - %s (%s)\n", karakterAdi, karakterMeslek);
            cizgi();
            printf("  Gün          : %d / %d   (toplam tur: %d)\n", gun, HEDEF_GUN, tur);
            printf("  Sağlık       : %d / 100\n", saglik);
            printf("  Enerji       : %d / 100\n", enerji);
            printf("  Oksijen      : %d / 100\n", oksijen);
            printf("  Su           : %d birim\n", su);
            printf("  Yedek parça  : %d\n", yedekParca);
            printf("  Modül hasarı : %d / 100\n", modulHasari);
            printf("  Hava kilidi  : %s\n", kilitMuhurlu == 1 ? "MÜHÜRLÜ" : "açık");
            printf("  Kredi        : %d MK\n", kredi);
            printf("  Kıyafet      : %d parça, toplam koruma %d\n",
                   kiyafet1 + kiyafet2 + kiyafet3 + kiyafet4 + kiyafet5, koruma);
            if (gun - sonYetenekGunu >= YETENEK_BEKLEME) {
                printf("  Yetenek      : %s HAZIR\n", yetenekAdi);
            } else {
                printf("  Yetenek      : %s %d gün sonra hazır\n", yetenekAdi,
                       YETENEK_BEKLEME - (gun - sonYetenekGunu));
            }
            cizgi();
            /* Her kritik değer için ayrı uyarı */
            if (oksijen < 25) {
                printf("  !!! KRİTİK: Oksijen çok düşük (%d). Hemen oksijen üretin!\n", oksijen);
            }
            if (saglik < 30) {
                printf("  !!! KRİTİK: Sağlık tehlikede (%d). Dinlenin veya kıyafet alın!\n", saglik);
            }
            if (modulHasari > 70) {
                printf("  !!! KRİTİK: Modül ağır hasarlı (%d). Her gün sağlık kaybedersiniz!\n", modulHasari);
            }
            if (enerji < 20) {
                printf("  !!! KRİTİK: Enerji tükenmek üzere (%d). Uyuyun!\n", enerji);
            }
            /* Tüm değerler güvenliyse tek bir && koşuluyla */
            if (oksijen >= 25 && saglik >= 30 && modulHasari <= 70 && enerji >= 20) {
                printf("  >>> TÜM SİSTEMLER NORMAL <<<\n");
            }
            cizgi();
            break;

        /* ---------------------------- U: UYU / DİNLEN ---------------------------- */
        case 'u':
        case 'U': {
            int enerjiArtisi = 25, saglikArtisi = 10 + dinlenmeSaglikBonus;

            if (zorluk == 'Z') {
                enerjiArtisi = 20;
            }
            enerji = enerji + enerjiArtisi;
            oksijen = oksijen - 2;               /* Uyurken de nefes alınır */
            printf("  Uyudun: +%d enerji, -2 oksijen.\n", enerjiArtisi);
            if (oksijen < 20) {
                /* İç içe IF: oksijen azsa sağlık artmaz, azalır */
                saglik = saglik - 5;
                if (saglik < 0) {
                    saglik = 0;
                }
                printf("  Oksijen 20'nin altında! Uykuda nefes darlığı çektin: -5 sağlık.\n");
            } else {
                if (modulHasari > 70) {
                    saglikArtisi = saglikArtisi / 2;
                    printf("  Modül ağır hasarlı; alarmlar yüzünden yarım uyku çektin.\n");
                }
                saglik = saglik + saglikArtisi;
                printf("  Dinlendin: +%d sağlık.\n", saglikArtisi);
            }
            turHarcandi = 1;
            break;
        }

        /* --------------------------- K: KUM FIRTINASI --------------------------- */
        case 'k':
        case 'K': {
            int korumaEsigi, carpan, kayip;
            int hasarliTur = 0, guvenliTur = 0;
            int toplamSaglikKaybi = 0, toplamOksijenKaybi = 0, toplamModulHasari = 0;

            korumaEsigi = 30 + firtinaKorumaBonus + koruma * 2;
            printf("  KUM FIRTINASI YAKLAŞIYOR! Süre: %d tur. Korunma eşiği: %d\n",
                   firtinaTurSayisi, korumaEsigi);
            if (kilitMuhurlu == 1) {
                printf("  Hava kilidi mühürlü: oksijen sızıntıları engellenecek.\n");
            }

            for (i = 1; i <= firtinaTurSayisi; i++) {
                zar = rand() % 100;
                if (i % 2 == 0) {
                    carpan = 2;            /* Çift turlarda rüzgar şiddetlenir */
                } else {
                    carpan = 1;
                }
                printf("  Tur %d/%d%s ", i, firtinaTurSayisi, carpan == 2 ? " (rüzgar şiddetli, hasar x2)" : "");
                printf("Zar: %d / Eşik: %d -> ", zar, korumaEsigi);

                if (zar < korumaEsigi) {
                    printf("KORUNDUN. Sığınakta zararsız atlattın.\n");
                    guvenliTur++;
                } else if (zar < korumaEsigi + 25) {
                    kayip = firtinaHasari * carpan - koruma;
                    if (kayip < 1) {
                        kayip = 1;
                    }
                    saglik = saglik - kayip;
                    toplamSaglikKaybi = toplamSaglikKaybi + kayip;
                    hasarliTur++;
                    printf("Uçuşan kaya parçası! -%d sağlık.\n", kayip);
                } else if (zar < korumaEsigi + 50) {
                    if (kilitMuhurlu == 1) {
                        printf("Sızıntı tehlikesi; ama kilit mühürlü, oksijen kaybı yok.\n");
                        guvenliTur++;
                        continue;          /* Bu turdaki oksijen sızıntısı olayı atlanır */
                    }
                    kayip = firtinaHasari * carpan;
                    oksijen = oksijen - kayip;
                    toplamOksijenKaybi = toplamOksijenKaybi + kayip;
                    hasarliTur++;
                    printf("Hava kilidinden sızıntı! -%d oksijen.\n", kayip);
                } else {
                    kayip = firtinaHasari * carpan + 5;
                    modulHasari = modulHasari + kayip;
                    toplamModulHasari = toplamModulHasari + kayip;
                    hasarliTur++;
                    printf("Güneş panelleri zarar gördü! +%d modül hasarı.\n", kayip);
                }

                if (saglik < 0) {
                    saglik = 0;
                }
                if (oksijen < 0) {
                    oksijen = 0;
                }
                if (modulHasari > 100) {
                    modulHasari = 100;
                }
                if (saglik <= 0 || oksijen <= 0) {
                    printf("  Fırtına ölümcül oldu! Simülasyon fırtına ortasında sona eriyor.\n");
                    break;                 /* Sağlık veya oksijen bitti: döngüyü hemen bitir */
                }
            }

            cizgi();
            printf("  FIRTINA ÖZETİ: %d turda hasar alındı, %d tur güvende geçti.\n", hasarliTur, guvenliTur);
            printf("  Toplam kayıp: -%d sağlık, -%d oksijen, +%d modül hasarı.\n",
                   toplamSaglikKaybi, toplamOksijenKaybi, toplamModulHasari);
            if (guvenliTur > 0 && saglik > 0 && oksijen > 0) {
                kredi = kredi + guvenliTur * 5;
                toplamKazanilanKredi = toplamKazanilanKredi + guvenliTur * 5;
                printf("  Fırtına ölçüm verisi koloni merkezine iletildi: +%d MK.\n", guvenliTur * 5);
            }
            cizgi();
            turHarcandi = 1;
            break;
        }

        /* ------------------------- H: HAVA KİLİDİ ŞİFRESİ ------------------------- */
        case 'h':
        case 'H': {
            int deneme = 0;

            if (kilitMuhurlu == 1) {
                printf("  Hava kilidi zaten mühürlü.\n");
                break;
            }
            if (enerji < 5) {
                printf("  Yetersiz enerji! Kilit paneli için en az 5 enerji gerekir. Komut iptal.\n");
                break;
            }
            sifre = (char) ('A' + (gun % 26));   /* Şifre her gün değişir */
            tahmin = '?';
            printf("  İPUCU: Şifre, alfabede A harfinden bugünün gün sayısı (%d) kadar ilerideki harftir.\n", gun);
            printf("  3 deneme hakkın var; her yanlış deneme 5 enerji harcar.\n");
            do {
                printf("  Deneme %d/3 - Harf girin > ", deneme + 1);
                tahmin = karakterOku();
                if (tahmin == 0) {         /* Giriş bitti: kalan hakları yak */
                    deneme = 3;
                    tahmin = '?';
                }
                if (tahmin >= 'a' && tahmin <= 'z') {
                    tahmin = (char) (tahmin - 'a' + 'A');   /* Küçük harfi büyüğe çevir */
                }
                if (tahmin < 'A' || tahmin > 'Z') {
                    if (deneme < 3) {
                        printf("  Yalnızca A-Z arası tek bir harf girin (hak düşmedi).\n");
                    }
                    continue;
                }
                deneme++;
                if (tahmin != sifre) {
                    enerji = enerji - 5;
                    if (enerji < 0) {
                        enerji = 0;
                    }
                    if (tahmin < sifre) {
                        printf("  Yanlış! İLERİ GİT (şifre alfabede daha sonra). -5 enerji, kalan hak: %d\n", 3 - deneme);
                    } else {
                        printf("  Yanlış! GERİ GİT (şifre alfabede daha önce). -5 enerji, kalan hak: %d\n", 3 - deneme);
                    }
                }
            } while (tahmin != sifre && deneme < 3);

            if (tahmin == sifre) {
                kilitMuhurlu = 1;
                printf("  DOĞRU! Hava kilidi mühürlendi. Fırtınalarda oksijen sızıntısı olmayacak.\n");
            } else {
                modulHasari = modulHasari + 20;
                printf("  3 deneme de başarısız. Kilit mekanizması zorlandı: +20 modül hasarı.\n");
                printf("  (Doğru şifre '%c' idi.)\n", sifre);
            }
            turHarcandi = 1;
            break;
        }

        /* ---------------------- M: MARS GÖREVİ (KREDİ KAZANMA) ---------------------- */
        case 'm':
        case 'M': {
            int gorevEnerji, gorevOksijen, odul, odulParca = 0, odulSu = 0;
            int cezaSaglik = 0, cezaHasar = 0, radyasyon = 0;
            const char *gorevAdi;

            cizgi();
            printf("  MARS GÖREV PANOSU (zorluk ve karakter bonusları eşiğe eklenmiştir)\n");
            cizgi();
            printf("  [1] Buz Numunesi Toplama -10 enerji, -5 oksijen   Eşik %2d  Ödül %d MK + 6 su\n",
                   75 + gorevEsikBonus + gorevBasariBonus, 25 * gorevOdulYuzde / 100);
            printf("  [2] Anten Onarımı        -15 enerji, -8 oksijen   Eşik %2d  Ödül %d MK + 1 parça\n",
                   55 + gorevEsikBonus + gorevBasariBonus, 45 * gorevOdulYuzde / 100);
            printf("  [3] Krater Madeni Tarama -25 enerji, -12 oksijen  Eşik %2d  Ödül %d MK\n",
                   35 + gorevEsikBonus + gorevBasariBonus, 90 * gorevOdulYuzde / 100);
            printf("  Radyasyon: görev 1: 4, görev 2: 7, görev 3: 10 sağlık (kıyafet koruması kadar azalır).\n");
            printf("  Radyasyon dozu: 1) 4  2) 7  3) 10 sağlık (kıyafet koruması kadar azalır, koruman: %d)\n", koruma);
            printf("  [0] Vazgeç\n");
            printf("  Görev seçin > ");
            secim = karakterOku();

            switch (secim) {
            case '1':
                gorevAdi = "Buz Numunesi Toplama";
                gorevEnerji = 10; gorevOksijen = 5; esik = 75; odul = 25; odulSu = 6;
                cezaSaglik = 6; radyasyon = 4;
                break;
            case '2':
                gorevAdi = "Anten Onarımı";
                gorevEnerji = 15; gorevOksijen = 8; esik = 55; odul = 45; odulParca = 1;
                cezaHasar = 10; radyasyon = 7;
                break;
            case '3':
                gorevAdi = "Krater Madeni Tarama";
                gorevEnerji = 25; gorevOksijen = 12; esik = 35; odul = 90;
                cezaSaglik = 15; cezaHasar = 10; radyasyon = 10;
                break;
            default:
                gorevAdi = NULL;
                gorevEnerji = 0; gorevOksijen = 0; esik = 0; odul = 0;
                break;
            }

            if (gorevAdi == NULL) {
                if (secim == '0') {
                    printf("  Görev panosundan çıkıldı.\n");
                } else {
                    printf("  Geçersiz görev seçimi. Tur harcanmadı.\n");
                }
                break;
            }
            if (enerji < gorevEnerji) {
                printf("  Yetersiz enerji! %s için %d enerji gerekir (mevcut: %d). Görev iptal.\n",
                       gorevAdi, gorevEnerji, enerji);
                break;
            }
            if (oksijen <= gorevOksijen) {
                printf("  Bu görev oksijenini tüketir (%d gerekli, mevcut %d). Görev iptal.\n",
                       gorevOksijen, oksijen);
                break;
            }

            esik = esik + gorevEsikBonus + gorevBasariBonus;
            if (esik > 95) {
                esik = 95;
            }
            enerji = enerji - gorevEnerji;
            oksijen = oksijen - gorevOksijen;
            printf("  %s başladı (-%d enerji, -%d oksijen).\n", gorevAdi, gorevEnerji, gorevOksijen);

            /* Uzun yüzey görevlerinde kozmik radyasyon: kıyafet koruması dozu azaltır. */
            radyasyon = radyasyon - koruma;
            if (radyasyon < 0) {
                radyasyon = 0;
            }
            saglik = saglik - radyasyon;
            if (radyasyon > 0) {
                printf("  Radyasyon dozu: -%d sağlık (kıyafet koruması %d puan engelledi).\n", radyasyon, koruma);
            } else {
                printf("  Kıyafetlerin radyasyonu tamamen engelledi (koruma %d).\n", koruma);
            }

            zar = rand() % 100;
            zarYazdir("Görev:", zar, esik, "BAŞARILI", "BAŞARISIZ");
            if (zar < esik) {
                odul = odul * gorevOdulYuzde / 100;
                kredi = kredi + odul;
                toplamKazanilanKredi = toplamKazanilanKredi + odul;
                yedekParca = yedekParca + odulParca;
                su = su + odulSu;
                gorevBasari++;
                printf("  Görev tamamlandı! +%d MK", odul);
                if (odulParca > 0) {
                    printf(" ve +%d yedek parça", odulParca);
                }
                if (odulSu > 0) {
                    printf(" ve +%d birim su", odulSu);
                }
                printf(".\n");
            } else {
                gorevBasarisiz++;
                if (cezaSaglik > 0) {
                    cezaSaglik = cezaSaglik - koruma;
                    if (cezaSaglik < 1) {
                        cezaSaglik = 1;
                    }
                    saglik = saglik - cezaSaglik;
                    printf("  Görev başarısız! Kazada yaralandın: -%d sağlık.\n", cezaSaglik);
                }
                if (cezaHasar > 0) {
                    modulHasari = modulHasari + cezaHasar;
                    printf("  Ekipman bozuldu: +%d modül hasarı.\n", cezaHasar);
                }
            }
            turHarcandi = 1;
            break;
        }

        /* ------------------------ S: KIYAFET MAĞAZASI ------------------------ */
        case 's':
        case 'S': {
            int alinan = 0, urun, fiyat, artis, korumaArtisi, sahip, enUcuz;

            do {
                cizgi();
                printf("  KOLONİ DEPOSU - KIYAFETLER   (Kredin: %d MK)\n", kredi);
                cizgi();
                printf("  [1] Termal İçlik           30 MK  +10 can, +1 koruma %s\n", kiyafet1 ? "[GİYİLİYOR]" : "");
                printf("  [2] Isıtmalı Eldiven       50 MK  +15 can, +2 koruma %s\n", kiyafet2 ? "[GİYİLİYOR]" : "");
                printf("  [3] Radyasyon Yeleği       80 MK  +20 can, +3 koruma %s\n", kiyafet3 ? "[GİYİLİYOR]" : "");
                printf("  [4] Kompozit EVA Kaskı    110 MK  +25 can, +4 koruma %s\n", kiyafet4 ? "[GİYİLİYOR]" : "");
                printf("  [5] Yeni Nesil EVA Giysisi 160 MK +35 can, +5 koruma %s\n", kiyafet5 ? "[GİYİLİYOR]" : "");
                printf("  [0] Mağazadan çık\n");
                printf("  (Koruma; yaralanma, fırtına ve görev kazalarındaki can kaybını azaltır.)\n");
                printf("  Seçiminiz > ");
                secim = karakterOku();
                if (secim == 0) {
                    secim = '0';
                }

                urun = 0; fiyat = 0; artis = 0; korumaArtisi = 0; sahip = 0;
                switch (secim) {
                case '1': urun = 1; fiyat = 30;  artis = 10; korumaArtisi = 1; sahip = kiyafet1; break;
                case '2': urun = 2; fiyat = 50;  artis = 15; korumaArtisi = 2; sahip = kiyafet2; break;
                case '3': urun = 3; fiyat = 80;  artis = 20; korumaArtisi = 3; sahip = kiyafet3; break;
                case '4': urun = 4; fiyat = 110; artis = 25; korumaArtisi = 4; sahip = kiyafet4; break;
                case '5': urun = 5; fiyat = 160; artis = 35; korumaArtisi = 5; sahip = kiyafet5; break;
                case '0': break;
                default:
                    printf("  Geçersiz seçim, 0-5 arası bir rakam girin.\n");
                    break;
                }

                if (urun != 0) {
                    if (sahip == 1) {
                        printf("  Bu kıyafet zaten üzerinde.\n");
                    } else if (kredi < fiyat) {
                        printf("  Kredin yetmiyor: %d MK eksik. Mars görevleriyle (M) kredi kazanabilirsin.\n",
                               fiyat - kredi);
                    } else {
                        kredi = kredi - fiyat;
                        koruma = koruma + korumaArtisi;
                        if (saglik + artis > 100) {
                            printf("  Can 100'e ulaştı (%d puanlık artışın fazlası kullanılamadı).\n",
                                   saglik + artis - 100);
                            saglik = 100;
                        } else {
                            saglik = saglik + artis;
                            printf("  Yeni kıyafetle kendini daha iyi hissediyorsun: +%d can.\n", artis);
                        }
                        if (urun == 1) {
                            kiyafet1 = 1;
                        } else if (urun == 2) {
                            kiyafet2 = 1;
                        } else if (urun == 3) {
                            kiyafet3 = 1;
                        } else if (urun == 4) {
                            kiyafet4 = 1;
                        } else {
                            kiyafet5 = 1;
                        }
                        alinan++;
                        printf("  Satın alındı! Kalan kredi: %d MK, toplam koruma: %d.\n", kredi, koruma);
                    }
                }

                /* Kredinin yettiği, henüz alınmamış en ucuz kıyafeti bul */
                enUcuz = 0;
                if (!kiyafet1 && kredi >= 30) {
                    enUcuz = 30;
                } else if (!kiyafet2 && kredi >= 50) {
                    enUcuz = 50;
                } else if (!kiyafet3 && kredi >= 80) {
                    enUcuz = 80;
                } else if (!kiyafet4 && kredi >= 110) {
                    enUcuz = 110;
                } else if (!kiyafet5 && kredi >= 160) {
                    enUcuz = 160;
                }
                if (secim != '0' && enUcuz == 0) {
                    printf("  Kredin başka bir kıyafete yetmiyor; mağazadan çıkılıyor.\n");
                    secim = '0';
                }
            } while (secim != '0');

            if (alinan > 0) {
                printf("  Alışveriş %d kıyafetle tamamlandı (1 tur sürdü).\n", alinan);
                turHarcandi = 1;
            } else {
                printf("  Hiçbir şey alınmadı, tur harcanmadı.\n");
            }
            break;
        }

        /* --------------------------- Y: ÖZEL YETENEK --------------------------- */
        case 'y':
        case 'Y': {
            int yetenekEnerji;

            if (gun - sonYetenekGunu < YETENEK_BEKLEME) {
                printf("  %s henüz hazır değil: %d gün sonra kullanılabilir.\n", yetenekAdi,
                       YETENEK_BEKLEME - (gun - sonYetenekGunu));
                break;
            }
            if (karakter == 2 || karakter == 4 || karakter == 5) {
                yetenekEnerji = 15;
            } else {
                yetenekEnerji = 10;
            }
            if (enerji < yetenekEnerji) {
                printf("  Yetersiz enerji! %s için %d enerji gerekir. Komut iptal.\n", yetenekAdi, yetenekEnerji);
                break;
            }
            if (karakter == 1 && modulHasari == 0) {
                printf("  Modül hasarsız; Acil Kaynak'ı şimdi kullanmak boşa gider.\n");
                break;
            }

            enerji = enerji - yetenekEnerji;
            switch (karakter) {
            case 1:
                modulHasari = modulHasari - 25;
                printf("  ACİL KAYNAK: Kerem gövdeyi kaynakla yamadı. -25 modül hasarı (parça harcanmadı).\n");
                break;
            case 2:
                yedekParca = yedekParca + 2;
                su = su + 4;
                kredi = kredi + 20;
                toplamKazanilanKredi = toplamKazanilanKredi + 20;
                oksijen = oksijen - 5;
                printf("  DERİN SONDAJ: Elif derinde bir buz damarına ulaştı. +2 parça, +4 su, +20 MK, -5 oksijen.\n");
                break;
            case 3:
                saglik = saglik + 30;
                printf("  ACİL MÜDAHALE: Selin kendi yaralarını tedavi etti. +30 sağlık.\n");
                break;
            case 4:
                oksijen = oksijen + 6;
                printf("  ELEKTROLİZ: Mert yedek hücreleri devreye aldı. +6 oksijen (su harcanmadı).\n");
                break;
            default:
                su = su + 6;
                kredi = kredi + 25;
                toplamKazanilanKredi = toplamKazanilanKredi + 25;
                oksijen = oksijen - 5;
                printf("  ROVER SEFERİ: Ayşe kutuptaki buz deposuna gitti. +6 su, +25 MK, -5 oksijen.\n");
                break;
            }
            printf("  (-%d enerji) Yetenek %d gün sonra yeniden hazır olacak.\n", yetenekEnerji, YETENEK_BEKLEME);
            sonYetenekGunu = gun;
            turHarcandi = 1;
            break;
        }

        /* ------------------------------ B: YARDIM ------------------------------ */
        case 'b':
        case 'B':
            komutListesi();
            break;

        /* ------------------------------ Q: ÇIKIŞ ------------------------------ */
        case 'q':
        case 'Q':
            printf("  Simülasyondan çıkılıyor...\n");
            oyunDurumu = 3;
            break;

        default:
            printf("  Tanımsız komut! Geçerli komutlar: G O T D U K H M S Y B Q (tur harcanmadı)\n");
            break;
        }

        /* ---------- Sınırlandırma: değerler her komuttan sonra IF ile 0-100'de tutulur ---------- */
        if (saglik < 0) { saglik = 0; }
        if (saglik > 100) { saglik = 100; }
        if (enerji < 0) { enerji = 0; }
        if (enerji > 100) { enerji = 100; }
        if (oksijen < 0) { oksijen = 0; }
        if (oksijen > 100) { oksijen = 100; }
        if (modulHasari < 0) { modulHasari = 0; }
        if (modulHasari > 100) { modulHasari = 100; }
        if (su < 0) { su = 0; }
        if (yedekParca < 0) { yedekParca = 0; }
        if (kredi < 0) { kredi = 0; }

        /* ---------------------- Zaman akışı: tur ve gün sayacı ---------------------- */
        if (turHarcandi == 1) {
            tur++;
            if (tur % GUN_BASINA_TUR == 0 && saglik > 0 && oksijen > 0) {
                if (gun == HEDEF_GUN) {
                    oyunDurumu = 1;        /* 15. gün tamamlandı: kurtarma ekibi geldi */
                } else {
                    gun++;
                    oksijen = oksijen - 8;
                    printf("\n  *** GÜN %d BAŞLADI *** Gece boyunca yaşam destek oksijen tüketti: -8 oksijen.\n", gun);
                    if (modulHasari > 70) {
                        saglik = saglik - 5;
                        printf("  Modül ağır hasarlı (%d): soğuk ve radyasyon -5 sağlık.\n", modulHasari);
                    }
                    if (oksijen < 0) { oksijen = 0; }
                    if (saglik < 0) { saglik = 0; }
                }
            }
        }

        /* ---------------------- Oyun sonu kontrolü (her turun sonunda) ---------------------- */
        if (saglik <= 0 || oksijen <= 0) {
            oyunDurumu = 2;
        }

    } while (oyunDurumu == 0);

    /* ============================== SONUÇ RAPORU ============================== */
    puan = (gun * 100) + saglik + oksijen - modulHasari;

    printf("\n==================================================================\n");
    switch (oyunDurumu) {
    case 1:
        printf("  KURTARMA EKİBİ GELDİ! %d günü tamamladın ve hayatta kaldın.\n", HEDEF_GUN);
        break;
    case 2:
        if (oksijen <= 0) {
            printf("  OYUN BİTTİ: Oksijen tükendi.\n");
        } else {
            printf("  OYUN BİTTİ: Sağlık sıfıra düştü.\n");
        }
        break;
    default:
        printf("  SİMÜLASYON SONLANDIRILDI (çıkış komutu).\n");
        break;
    }
    printf("==================================================================\n");
    printf("  Karakter          : %s (%s)\n", karakterAdi, karakterMeslek);
    printf("  Zorluk            : %s\n", zorluk == 'K' ? "Kolay" : (zorluk == 'N' ? "Normal" : "Zor"));
    printf("  Hayatta kalınan gün: %d / %d\n", gun, HEDEF_GUN);
    printf("  Toplam tur        : %d\n", tur);
    printf("  Son durum         : Sağlık %d, Enerji %d, Oksijen %d, Su %d, Parça %d, Hasar %d\n",
           saglik, enerji, oksijen, su, yedekParca, modulHasari);
    printf("  Ekonomi           : %d MK kazanıldı, %d MK kaldı; görevler %d başarılı / %d başarısız\n",
           toplamKazanilanKredi, kredi, gorevBasari, gorevBasarisiz);
    printf("  Kıyafet           : %d parça (koruma %d)\n",
           kiyafet1 + kiyafet2 + kiyafet3 + kiyafet4 + kiyafet5, koruma);
    printf("------------------------------------------------------------------\n");
    printf("  PUAN = (gün x 100) + sağlık + oksijen - modül hasarı\n");
    printf("       = (%d x 100) + %d + %d - %d = %d\n", gun, saglik, oksijen, modulHasari, puan);
    if (puan >= 1650) {
        printf("  Unvan: EFSANEVİ KOLONİ LİDERİ\n");
    } else if (puan >= 1400) {
        printf("  Unvan: Kıdemli Mars Mühendisi\n");
    } else if (puan >= 800) {
        printf("  Unvan: Dayanıklı Kolonist\n");
    } else {
        printf("  Unvan: Çaylak Kâşif\n");
    }
    printf("==================================================================\n");
    return 0;
}
