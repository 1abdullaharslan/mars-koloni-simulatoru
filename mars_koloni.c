// Mars Kolonisi Yasam Destek Simulatoru
// Algoritma ve Programlama - Odev
// Abdullah Arslan - 1260505042
//
// Bu programda Mars'taki bir istasyonda tek basina kalan bir muhendisi yonetiyoruz.
// Kurtarma ekibi 15 gun sonra geliyor, o zamana kadar hayatta kalmamiz lazim.
// Odevde istenen komutlar: G, O, T, D, U, K, H, Q
// Ben bunlara ek olarak 5 karakter (Abdullah, Selcuk, Recep, Emo, Ece; her birinin
// kendi yetenegi var), para (kredi) sistemi, Mars gorevleri (M), kiyafet magazasi (S),
// ozel yetenek (Y) ve yardim (B) komutlarini ekledim. Gorevlerden kazanilan parayla
// kiyafet alinip can artiriliyor.
// Her gunun sonunda o gun secilen islemlere gore kisa bir hikaye (gunluk) yaziliyor.
// Isler cok iyi giderse geceleri volkan firtinasi, basarisiz gorev gibi kotu olaylar
// cikiyor; bunlar enerji harcatiyor ama hicbir sey kazandirmiyor.
//
// Not: konsolda bozuk cikmasin diye yazilarda Turkce karakter kullanmadim.
//
// Derlemek icin: gcc mars_koloni.c -o mars_koloni

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// enter'a kadar kalan karakterleri siliyorum, yoksa sonraki scanf onlari okuyor
void tamponuTemizle(void) {
    int c;
    c = getchar();
    while (c != '\n' && c != EOF) {
        c = getchar();
    }
}

// komut listesini hem oyunun basinda hem de B komutunda yazdiriyorum
void komutlariYazdir(void) {
    printf("--------------------------------------------------------------\n");
    printf("  KOMUTLAR (buyuk/kucuk harf fark etmez)\n");
    printf("--------------------------------------------------------------\n");
    printf("  G - Kesif gorevi       (-15 enerji, -10 oksijen)\n");
    printf("  O - Oksijen uret       (2 su -> 3 oksijen)\n");
    printf("  T - Tamir              (yedek parca ile hasari azalt)\n");
    printf("  D - Durum raporu       (tur harcamaz)\n");
    printf("  U - Uyu / dinlen       (+enerji, +saglik)\n");
    printf("  K - Kum firtinasi\n");
    printf("  H - Hava kilidi sifresi\n");
    printf("  M - Mars gorevi        (kredi kazan)\n");
    printf("  S - Kiyafet magazasi   (kredi ile kiyafet al, can artir)\n");
    printf("  Y - Ozel yetenek       (karakterine ozel)\n");
    printf("  B - Komut listesi      (tur harcamaz)\n");
    printf("  Q - Cikis\n");
    printf("--------------------------------------------------------------\n");
}

// gunun sonunda o gun secilen 3 isleme gore kisa bir hikaye yazdiriyorum
// komutlar[i]: o gunun i. islemi, sonuclar[i]: o islemin sonucu
void gunlukYazdir(int gunNo, char ad[], char yetenek[], char komutlar[], int sonuclar[]) {
    int i;

    printf("\n  ~~~~~~~~~~~~~~~~~~~~ GUN %d GUNLUGU ~~~~~~~~~~~~~~~~~~~~\n", gunNo);
    for (i = 0; i < 3; i++) {
        if (i == 0) {
            printf("  Sabah %s ", ad);
        } else if (i == 1) {
            printf("  Ogleden sonra %s ", ad);
        } else {
            printf("  Aksam %s ", ad);
        }

        switch (komutlar[i]) {
            case 'G':
                if (sonuclar[i] == 1) {
                    printf("kesfe cikti ve bir kraterin golgesinde buz buldu.\n");
                } else if (sonuclar[i] == 2) {
                    printf("kesfe cikti, eski bir inis aracindan yedek parca sokup getirdi.\n");
                } else if (sonuclar[i] == 3) {
                    printf("kesfe cikti ve parlayan nadir bir mineral buldu.\n");
                } else {
                    printf("saatlerce kizil kumlarda yurudu ama eli bos dondu.\n");
                }
                break;
            case 'O':
                printf("reaktorun basina gecip suyu oksijene cevirdi, istasyonun havasi tazelendi.\n");
                break;
            case 'T':
                printf("alet cantasini alip modulun catlaklarini tek tek kaynakladi.\n");
                break;
            case 'U':
                printf("uyku tulumuna girip dinlendi, ruyasinda Dunya'daki evini gordu.\n");
                break;
            case 'K':
                if (sonuclar[i] == 1) {
                    printf("kum firtinasini siginakta saklanarak hic hasar almadan atlatti.\n");
                } else {
                    printf("kum firtinasina yakalandi, ruzgar istasyonu fena hirpaladi.\n");
                }
                break;
            case 'H':
                if (sonuclar[i] == 1) {
                    printf("hava kilidinin sifresini cozdu ve kapiyi muhurledi.\n");
                } else {
                    printf("hava kilidinin sifresini bulamadi, kilit mekanizmasi zorlandi.\n");
                }
                break;
            case 'M':
                if (sonuclar[i] == 1) {
                    printf("Mars gorevine cikti, gorevi basariyla bitirip kredi kazandi.\n");
                } else {
                    printf("Mars gorevine cikti ama gorev basarisiz oldu, yorgun argin dondu.\n");
                }
                break;
            case 'S':
                printf("koloni deposuna ugrayip yeni kiyafetler giydi.\n");
                break;
            case 'Y':
                printf("ozel yetenegini (%s) kullandi.\n", yetenek);
                break;
            default:
                printf("istasyonda vakit gecirdi.\n");
                break;
        }
    }
}

int main(void) {
    // odevde istenen durum degiskenleri
    int saglik = 100, enerji = 100, oksijen = 100;
    int su = 0, yedekParca = 0, modulHasari = 0;
    int kilitMuhurlu = 0;
    int gun = 1, tur = 0;

    // para ve kiyafet icin ekledigim degiskenler
    int kredi = 0;
    int koruma = 0;   // alinan kiyafetlerin toplam korumasi
    int kiyafet1 = 0, kiyafet2 = 0, kiyafet3 = 0, kiyafet4 = 0, kiyafet5 = 0;   // 1 ise alinmis
    int toplamKredi = 0, basariliGorev = 0, basarisizGorev = 0;

    // karakter degiskenleri
    int karakter = 0;
    char karakterAdi[30], meslek[30], yetenekAdi[30];
    int sonYetenekGunu = -3;   // -3 verdim ki yetenek ilk gunden kullanilabilsin

    // zorluga gore degisen degerler
    char zorluk;
    int firtinaTurSayisi = 5, firtinaHasari = 6;
    int suEsikYuksek = 60, suEsikDusuk = 30;
    int yaralanmaHasari = 12, gorevEsikBonus = 0;

    // karaktere gore degisen degerler (pasif yetenekler)
    int tamirBonus = 0, tamirEnerji = 10;
    int parcaEsikBonus = 0, gorevOdulYuzde = 100;
    int dinlenmeSaglikBonus = 0, yaralanmaBolen = 1;
    int oksijenEnerji = 8, suEsikBonus = 0;
    int firtinaKorumaBonus = 0, gorevBasariBonus = 0;

    // komutlarin icinde kullandigim degiskenler
    char komut, secim, tahmin, sifre;
    int oyunDurumu = 0;   // 0: devam ediyor, 1: kazandi, 2: kaybetti, 3: cikis yapti
    int turHarcandi;
    int zar, esik, miktar, i, kayip;
    int suEsik, parcaEsik, kazanc, yaraZari;
    int artik, kullanilan, uretilen;
    int gerekenParca, onarim, harcanan;
    int enerjiArtisi, saglikArtisi;
    int korumaEsigi, carpan, hasarliTur, guvenliTur;
    int saglikKaybi, oksijenKaybi, hasarArtisi;
    int deneme;
    int gorevNo, gorevEnerji, gorevOksijen, odul, odulParca, odulSu;
    int cezaSaglik, cezaHasar, radyasyon;
    char gorevAdi[30];
    int alinan, urun, fiyat, canArtisi, korumaArtisi, sahip, yeterli;
    int yetenekEnerji;
    int puan;

    // gunluk hikaye ve gece olaylari icin
    char gunlukKomut[3] = {' ', ' ', ' '};   // o gun secilen 3 islem
    int gunlukSonuc[3] = {0, 0, 0};          // islemin sonucu (ornek: kesifte su mu parca mi bulundu)
    int sonuc;
    int olayEsigi, olayTuru;

    srand(time(NULL));

    printf("\n==============================================================\n");
    printf("         MARS KOLONISI YASAM DESTEK SIMULATORU\n");
    printf("==============================================================\n");
    printf("  Yil 2031. Ares-7 arastirma istasyonunda buyuk bir kum firtinasi\n");
    printf("  yuzunden tahliye yapildi ve sen istasyonda tek basina kaldin.\n");
    printf("  Kurtarma ekibi ancak 15 gun sonra gelebilecek.\n\n");

    // ---------------- ZORLUK SECIMI ----------------
    // yanlis bir harf girilirse do-while ile tekrar soruyoruz
    do {
        printf("Zorluk secin [K] Kolay  [N] Normal  [Z] Zor > ");
        if (scanf(" %c", &zorluk) != 1) {
            return 0;   // giris bittiyse programi kapat
        }
        tamponuTemizle();

        switch (zorluk) {
            case 'k':
            case 'K':
                zorluk = 'K';
                saglik = 100; enerji = 100; oksijen = 100;
                su = 60; yedekParca = 5; modulHasari = 10; kredi = 30;
                firtinaTurSayisi = 3; firtinaHasari = 4;
                suEsikYuksek = 70; suEsikDusuk = 40;
                yaralanmaHasari = 8; gorevEsikBonus = 10;
                printf("  KOLAY secildi: bol kaynak, kisa firtinalar.\n");
                break;
            case 'n':
            case 'N':
                zorluk = 'N';
                saglik = 100; enerji = 90; oksijen = 90;
                su = 50; yedekParca = 3; modulHasari = 20; kredi = 15;
                firtinaTurSayisi = 5; firtinaHasari = 6;
                suEsikYuksek = 60; suEsikDusuk = 30;
                yaralanmaHasari = 12; gorevEsikBonus = 0;
                printf("  NORMAL secildi: dengeli kaynaklar, 5 turluk firtinalar.\n");
                break;
            case 'z':
            case 'Z':
                zorluk = 'Z';
                saglik = 90; enerji = 80; oksijen = 80;
                su = 55; yedekParca = 2; modulHasari = 35; kredi = 0;
                firtinaTurSayisi = 7; firtinaHasari = 9;
                suEsikYuksek = 45; suEsikDusuk = 20;
                yaralanmaHasari = 15; gorevEsikBonus = -10;
                printf("  ZOR secildi: az kaynak, yuksek hasar, 7 turluk firtinalar.\n");
                break;
            default:
                printf("  Gecersiz seviye! Sadece K, N veya Z girin.\n");
                zorluk = 0;
                break;
        }
    } while (zorluk == 0);

    // ---------------- OYUNUN AMACI ----------------
    // karakter secmeden once oyuncuya ne yapacagini anlatiyorum
    printf("\n--------------------------------------------------------------\n");
    printf("  OYUNUN AMACI: NE YAPACAKSIN?\n");
    printf("--------------------------------------------------------------\n");
    printf("  * Kurtarma ekibi gelene kadar 15 gun hayatta kal. 15. gunu bitirirsen kazanirsin.\n");
    printf("  * Her gun 3 islem yapabilirsin. Durum raporu (D) ve yardim (B) islem sayilmaz.\n");
    printf("  * Her gece yasam destegi 8 oksijen harcar. Oksijen veya saglik 0 olursa kaybedersin.\n");
    printf("  * Oksijen icin su lazim: kesfe cikip su bul (G), suyu oksijene cevir (O).\n");
    printf("  * Enerjin azalinca uyu (U). Modul hasari 70'i gecerse her gun can gider, tamir et (T).\n");
    printf("  * Hava kilidini muhurle (H), boylece kum firtinasinda (K) oksijen sizmaz.\n");
    printf("  * Mars gorevleriyle (M) kredi kazan, kredinle kiyafet al (S): canin artar ve\n");
    printf("    sonraki hasarlardan daha az etkilenirsin.\n");
    printf("  * Her karakterin bir ozel yetenegi (Y) var, 3 gunde bir kullanabilirsin.\n");
    printf("  * Her gunun sonunda o gun yaptiklarin gunluge yazilir. Ama dikkat: isler cok iyi\n");
    printf("    gidiyorsa Mars geceleri surpriz yapar (volkan firtinasi, basarisiz gorev...)\n");
    printf("    ve hicbir sey vermeden enerjini harcatir.\n");

    // ---------------- KARAKTER SECIMI ----------------
    printf("\n--------------------------------------------------------------\n");
    printf("  KARAKTER SECIMI (her karakterin bir pasif, bir aktif yetenegi var)\n");
    printf("--------------------------------------------------------------\n");
    printf("  [1] Abdullah - Muhendis\n");
    printf("      Pasif: Usta Eller      -> Tamir +10 hasar onarir, 5 enerji harcar\n");
    printf("      Aktif: Acil Kaynak     -> Parca harcamadan 25 hasar onarir\n");
    printf("  [2] Selcuk - Jeolog\n");
    printf("      Pasif: Maden Gozu      -> Parca bulma sansi +10, gorev odulu +%%25\n");
    printf("      Aktif: Derin Sondaj    -> +2 parca, +4 su, +20 kredi\n");
    printf("  [3] Recep - Doktor\n");
    printf("      Pasif: Saha Hekimi     -> Dinlenince +10 saglik, yaralanma hasari yariya iner\n");
    printf("      Aktif: Acil Mudahale   -> +30 saglik\n");
    printf("  [4] Emo - Kimyager\n");
    printf("      Pasif: Verimli Reaktor -> Oksijen uretimi 5 enerji, su bulma sansi +5\n");
    printf("      Aktif: Elektroliz      -> Su harcamadan +6 oksijen\n");
    printf("  [5] Ece - Pilot\n");
    printf("      Pasif: Firtina Avcisi  -> Firtinada korunma +15, gorev basarisi +10\n");
    printf("      Aktif: Rover Seferi    -> +6 su, +25 kredi\n");
    printf("--------------------------------------------------------------\n");

    do {
        printf("Karakter secin (1-5) > ");
        if (scanf(" %c", &secim) != 1) {
            return 0;
        }
        tamponuTemizle();

        // her karakter sadece bazi degerleri degistiriyor, komutlarin kodu ayni
        switch (secim) {
            case '1':
                karakter = 1;
                strcpy(karakterAdi, "Abdullah");
                strcpy(meslek, "Muhendis");
                strcpy(yetenekAdi, "Acil Kaynak");
                tamirBonus = 10;
                tamirEnerji = 5;
                break;
            case '2':
                karakter = 2;
                strcpy(karakterAdi, "Selcuk");
                strcpy(meslek, "Jeolog");
                strcpy(yetenekAdi, "Derin Sondaj");
                parcaEsikBonus = 10;
                gorevOdulYuzde = 125;
                break;
            case '3':
                karakter = 3;
                strcpy(karakterAdi, "Recep");
                strcpy(meslek, "Doktor");
                strcpy(yetenekAdi, "Acil Mudahale");
                dinlenmeSaglikBonus = 10;
                yaralanmaBolen = 2;
                break;
            case '4':
                karakter = 4;
                strcpy(karakterAdi, "Emo");
                strcpy(meslek, "Kimyager");
                strcpy(yetenekAdi, "Elektroliz");
                oksijenEnerji = 5;
                suEsikBonus = 5;
                break;
            case '5':
                karakter = 5;
                strcpy(karakterAdi, "Ece");
                strcpy(meslek, "Pilot");
                strcpy(yetenekAdi, "Rover Seferi");
                firtinaKorumaBonus = 15;
                gorevBasariBonus = 10;
                break;
            default:
                printf("  Gecersiz secim! 1 ile 5 arasinda bir sayi girin.\n");
                break;
        }
    } while (karakter == 0);

    printf("\n  %s (%s) istasyonun kontrolunu aldi. Bol sans!\n", karakterAdi, meslek);
    komutlariYazdir();

    // ---------------- ANA KOMUT DONGUSU ----------------
    do {
        printf("\n[Gun %d/15 | Tur %d | Saglik %d | Enerji %d | Oksijen %d | Su %d | Parca %d | Hasar %d | Kredi %d MK]\n",
               gun, tur, saglik, enerji, oksijen, su, yedekParca, modulHasari, kredi);
        printf("Komut > ");
        if (scanf(" %c", &komut) != 1) {
            komut = 'Q';   // giris biterse (Ctrl+D / Ctrl+Z) oyundan cikiyoruz
        }
        tamponuTemizle();

        turHarcandi = 0;   // gecerli bir komut calisirsa 1 yapiyorum
        sonuc = 0;         // gunluk icin komutun sonucu

        switch (komut) {

            // ---------- G: KESIF ----------
            case 'g':
            case 'G':
                if (enerji < 15) {
                    printf("  Yetersiz enerji! Kesif icin en az 15 enerji lazim (enerji: %d). Komut iptal.\n", enerji);
                    break;
                }

                // enerji ve oksijen iyiyse su bulma ihtimali yuksek oluyor
                if (enerji > 40 && oksijen > 30) {
                    suEsik = suEsikYuksek;
                    printf("  Enerjin ve oksijenin yeterli, su bulma sansin YUKSEK.\n");
                } else {
                    suEsik = suEsikDusuk;
                    printf("  Yorgunsun ya da oksijenin az, su bulma sansin DUSUK.\n");
                }
                suEsik = suEsik + suEsikBonus;   // kimyagerin bonusu
                if (suEsik > 85) {
                    suEsik = 85;
                }
                parcaEsik = suEsik + 20 + parcaEsikBonus;   // jeologun bonusu
                if (parcaEsik > 90) {
                    parcaEsik = 90;
                }

                enerji = enerji - 15;
                oksijen = oksijen - 10;
                printf("  Kesfe ciktin (-15 enerji, -10 oksijen).\n");

                // zara gore 4 farkli sonuc var: su, parca, nadir mineral, eli bos
                kazanc = 0;
                zar = rand() % 100;
                if (zar < suEsik) {
                    miktar = 12 + zar % 7;   // 12 ile 18 arasi su
                    su = su + miktar;
                    kazanc = 10;
                    sonuc = 1;
                    printf("  Su arama: Zar: %d / Esik: %d -> BASARILI\n", zar, suEsik);
                    printf("  Buz bulundu! +%d su.\n", miktar);
                } else if (zar < parcaEsik) {
                    yedekParca = yedekParca + 1;
                    kazanc = 15;
                    sonuc = 2;
                    printf("  Parca arama: Zar: %d / Esik: %d -> BASARILI\n", zar, parcaEsik);
                    printf("  Eski bir inis aracindan +1 yedek parca soktun.\n");
                } else if (zar >= 92) {
                    kazanc = 40;
                    sonuc = 3;
                    printf("  Nadir bulgu: Zar: %d / Esik: 92 -> NADIR MINERAL\n", zar);
                    printf("  Nadir bir mineral buldun, koloni merkezine satilabilir!\n");
                } else {
                    printf("  Kesif: Zar: %d / Esik: %d -> BASARISIZ\n", zar, parcaEsik);
                    printf("  Eli bos dondun, burasi daha once taranmis.\n");
                }

                if (kazanc > 0) {
                    kazanc = kazanc * gorevOdulYuzde / 100;
                    kredi = kredi + kazanc;
                    toplamKredi = toplamKredi + kazanc;
                    printf("  Kesif raporu icin +%d MK kazandin.\n", kazanc);
                }

                // yaralanma: zar 10'dan kucukse YA DA modul cok hasarliysa
                yaraZari = rand() % 100;
                if (yaraZari < 10 || modulHasari > 70) {
                    kayip = yaralanmaHasari / yaralanmaBolen - koruma;
                    if (kayip < 1) {
                        kayip = 1;
                    }
                    saglik = saglik - kayip;
                    printf("  Yaralanma: Zar: %d / Esik: 10 -> YARALANDIN\n", yaraZari);
                    if (yaraZari >= 10) {
                        printf("  Modul cok hasarli oldugu icin bir kablo seni yaraladi!\n");
                    }
                    printf("  -%d saglik (kiyafet korumasi: %d).\n", kayip, koruma);
                } else {
                    printf("  Yaralanma: Zar: %d / Esik: 10 -> GUVENDESIN\n", yaraZari);
                }
                turHarcandi = 1;
                break;

            // ---------- O: OKSIJEN URET ----------
            case 'o':
            case 'O':
                if (su < 2) {
                    printf("  Yetersiz su! En az 2 su lazim (su: %d).\n", su);
                    break;
                }
                if (enerji < oksijenEnerji) {
                    printf("  Yetersiz enerji! Reaktor icin %d enerji lazim (enerji: %d). Komut iptal.\n",
                           oksijenEnerji, enerji);
                    break;
                }
                if (oksijen >= 100) {
                    printf("  Oksijen tanki zaten dolu.\n");
                    break;
                }

                // kullanici gecerli bir sayi girene kadar soruyoruz
                do {
                    printf("  Kac birim su kullanilsin? (1-%d, iptal icin 0) > ", su);
                    if (scanf("%d", &miktar) != 1) {
                        miktar = -1;   // sayi yerine harf girildi
                    }
                    if (feof(stdin)) {
                        miktar = 0;    // giris bittiyse iptal ediyoruz
                    }
                    tamponuTemizle();

                    if (miktar < 0) {
                        printf("  Gecersiz giris, 0 veya pozitif bir sayi girin.\n");
                    } else if (miktar > su) {
                        printf("  Depoda sadece %d su var, daha fazlasi girilemez.\n", su);
                    }
                } while (miktar < 0 || miktar > su);

                if (miktar == 0) {
                    printf("  Oksijen uretimi iptal edildi.\n");
                    break;
                }

                // 2 su = 3 oksijen. tek sayi girildiyse 1 su artik kaliyor (mod ile buluyorum)
                artik = miktar % 2;
                kullanilan = miktar - artik;
                if (kullanilan == 0) {
                    printf("  1 su ile oksijen uretilemez (2 su -> 3 oksijen). Komut iptal.\n");
                    break;
                }
                uretilen = (kullanilan / 2) * 3;

                // oksijen 100'u gecmesin, bosuna su harcanmasin diye azaltiyorum
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
                printf("  %d su kullanildi -> +%d oksijen (-%d enerji).\n", kullanilan, uretilen, oksijenEnerji);
                if (artik == 1) {
                    printf("  Tek sayi girdin (%d %% 2 = 1), 1 su artik olarak depoda kaldi.\n", miktar);
                }
                if (kullanilan + artik < miktar) {
                    printf("  Tank doldu, kullanilmayan %d su depoda kaldi.\n", miktar - kullanilan - artik);
                }
                turHarcandi = 1;
                break;

            // ---------- T: TAMIR ----------
            case 't':
            case 'T':
                if (yedekParca == 0) {
                    printf("  Yedek parca yok, tamir yapilamaz! Kesif veya gorevlerle parca bul.\n");
                    break;
                }
                if (modulHasari == 0) {
                    printf("  Modul zaten saglam, tamire gerek yok.\n");
                    break;
                }
                if (enerji < tamirEnerji) {
                    printf("  Yetersiz enerji! Tamir icin %d enerji lazim (enerji: %d). Komut iptal.\n",
                           tamirEnerji, enerji);
                    break;
                }

                // hasari 3 kademeye ayiriyorum, her kademede parca ve onarim farkli
                if (modulHasari <= 30) {
                    printf("  Hasar HAFIF (%d): 1 parca ile 15 hasar onarilir.\n", modulHasari);
                    gerekenParca = 1;
                    onarim = 15;
                } else if (modulHasari <= 70) {
                    printf("  Hasar ORTA (%d): 2 parca ile 25 hasar onarilir.\n", modulHasari);
                    gerekenParca = 2;
                    onarim = 25;
                } else {
                    printf("  Hasar AGIR (%d): 3 parca ile 40 hasar onarilir.\n", modulHasari);
                    gerekenParca = 3;
                    onarim = 40;
                }

                // parca yetmiyorsa elimizdeki parca oraninda onarim yapiliyor
                if (yedekParca < gerekenParca) {
                    onarim = onarim * yedekParca / gerekenParca;
                    harcanan = yedekParca;
                    printf("  Yeterli parca yok, %d/%d parca ile yarim tamir yapiliyor.\n", yedekParca, gerekenParca);
                } else {
                    harcanan = gerekenParca;
                }
                if (tamirBonus > 0) {
                    onarim = onarim + tamirBonus;
                    printf("  Usta Eller: muhendis +%d ekstra onarim yapti.\n", tamirBonus);
                }

                modulHasari = modulHasari - onarim;
                yedekParca = yedekParca - harcanan;
                enerji = enerji - tamirEnerji;
                printf("  Tamir bitti: -%d hasar, -%d parca, -%d enerji.\n", onarim, harcanan, tamirEnerji);
                turHarcandi = 1;
                break;

            // ---------- D: DURUM RAPORU (tur harcamaz) ----------
            case 'd':
            case 'D':
                printf("  ------------------- DURUM RAPORU -------------------\n");
                printf("  Karakter     : %s (%s)\n", karakterAdi, meslek);
                printf("  Gun          : %d / 15  (toplam tur: %d)\n", gun, tur);
                printf("  Saglik       : %d / 100\n", saglik);
                printf("  Enerji       : %d / 100\n", enerji);
                printf("  Oksijen      : %d / 100\n", oksijen);
                printf("  Su           : %d\n", su);
                printf("  Yedek parca  : %d\n", yedekParca);
                printf("  Modul hasari : %d / 100\n", modulHasari);
                if (kilitMuhurlu == 1) {
                    printf("  Hava kilidi  : MUHURLU\n");
                } else {
                    printf("  Hava kilidi  : acik\n");
                }
                printf("  Kredi        : %d MK\n", kredi);
                printf("  Kiyafet      : %d tane, toplam koruma %d\n",
                       kiyafet1 + kiyafet2 + kiyafet3 + kiyafet4 + kiyafet5, koruma);
                if (gun - sonYetenekGunu >= 3) {
                    printf("  Yetenek      : %s HAZIR\n", yetenekAdi);
                } else {
                    printf("  Yetenek      : %s, %d gun sonra hazir\n", yetenekAdi, 3 - (gun - sonYetenekGunu));
                }
                printf("  ----------------------------------------------------\n");

                // kritik degerler icin ayri ayri uyari veriyorum
                if (oksijen < 25) {
                    printf("  !!! KRITIK: Oksijen cok dusuk (%d). Hemen oksijen uret!\n", oksijen);
                }
                if (saglik < 30) {
                    printf("  !!! KRITIK: Saglik tehlikede (%d). Dinlen veya kiyafet al!\n", saglik);
                }
                if (modulHasari > 70) {
                    printf("  !!! KRITIK: Modul cok hasarli (%d). Her gun saglik kaybedersin!\n", modulHasari);
                }
                if (enerji < 20) {
                    printf("  !!! KRITIK: Enerji bitmek uzere (%d). Uyu!\n", enerji);
                }
                // hepsi guvenliyse tek bir && kosulu ile yaziyorum
                if (oksijen >= 25 && saglik >= 30 && modulHasari <= 70 && enerji >= 20) {
                    printf("  >>> TUM SISTEMLER NORMAL <<<\n");
                }
                break;

            // ---------- U: UYU / DINLEN ----------
            case 'u':
            case 'U':
                enerjiArtisi = 25;
                if (zorluk == 'Z') {
                    enerjiArtisi = 20;
                }
                saglikArtisi = 10 + dinlenmeSaglikBonus;   // doktorun bonusu

                enerji = enerji + enerjiArtisi;
                oksijen = oksijen - 2;   // uyurken de nefes aliyoruz
                printf("  Uyudun: +%d enerji, -2 oksijen.\n", enerjiArtisi);

                // ic ice if: oksijen 20'nin altindaysa saglik artmiyor, azaliyor
                if (oksijen < 20) {
                    saglik = saglik - 5;
                    printf("  Oksijen 20'nin altinda! Uykuda nefes darligi cektin: -5 saglik.\n");
                } else {
                    if (modulHasari > 70) {
                        saglikArtisi = saglikArtisi / 2;
                        printf("  Modul cok hasarli, alarmlar yuzunden yarim uyudun.\n");
                    }
                    saglik = saglik + saglikArtisi;
                    printf("  Dinlendin: +%d saglik.\n", saglikArtisi);
                }
                turHarcandi = 1;
                break;

            // ---------- K: KUM FIRTINASI ----------
            case 'k':
            case 'K':
                korumaEsigi = 30 + firtinaKorumaBonus + koruma * 2;   // kiyafetler de korunmayi artiriyor
                hasarliTur = 0;
                guvenliTur = 0;
                saglikKaybi = 0;
                oksijenKaybi = 0;
                hasarArtisi = 0;

                printf("  KUM FIRTINASI GELIYOR! Sure: %d tur, korunma esigi: %d\n", firtinaTurSayisi, korumaEsigi);
                if (kilitMuhurlu == 1) {
                    printf("  Hava kilidi muhurlu, oksijen sizintisi olmayacak.\n");
                }

                for (i = 1; i <= firtinaTurSayisi; i++) {
                    zar = rand() % 100;

                    // cift turlarda ruzgar siddetleniyor ve hasar 2 katina cikiyor
                    if (i % 2 == 0) {
                        carpan = 2;
                        printf("  Tur %d/%d (ruzgar siddetli, hasar x2) Zar: %d / Esik: %d -> ",
                               i, firtinaTurSayisi, zar, korumaEsigi);
                    } else {
                        carpan = 1;
                        printf("  Tur %d/%d Zar: %d / Esik: %d -> ", i, firtinaTurSayisi, zar, korumaEsigi);
                    }

                    if (zar < korumaEsigi) {
                        printf("KORUNDUN, siginakta zarar gormedin.\n");
                        guvenliTur++;
                    } else if (zar < korumaEsigi + 25) {
                        kayip = firtinaHasari * carpan - koruma;
                        if (kayip < 1) {
                            kayip = 1;
                        }
                        saglik = saglik - kayip;
                        saglikKaybi = saglikKaybi + kayip;
                        hasarliTur++;
                        printf("Ucan bir kaya parcasi carpti! -%d saglik.\n", kayip);
                    } else if (zar < korumaEsigi + 50) {
                        // kilit muhurluyse oksijen sizintisi olmuyor, bu turu continue ile atliyorum
                        if (kilitMuhurlu == 1) {
                            printf("Sizinti tehlikesi var ama kilit muhurlu, oksijen kaybi yok.\n");
                            guvenliTur++;
                            continue;
                        }
                        kayip = firtinaHasari * carpan;
                        oksijen = oksijen - kayip;
                        oksijenKaybi = oksijenKaybi + kayip;
                        hasarliTur++;
                        printf("Hava kilidinden sizinti! -%d oksijen.\n", kayip);
                    } else {
                        kayip = firtinaHasari * carpan + 5;
                        modulHasari = modulHasari + kayip;
                        hasarArtisi = hasarArtisi + kayip;
                        hasarliTur++;
                        printf("Gunes panelleri zarar gordu! +%d modul hasari.\n", kayip);
                    }

                    // saglik veya oksijen biterse firtinayi break ile hemen bitiriyorum
                    if (saglik <= 0 || oksijen <= 0) {
                        printf("  Firtina olumcul oldu!\n");
                        break;
                    }
                }

                if (hasarliTur == 0) {
                    sonuc = 1;   // gunluk icin: firtina hasarsiz atlatildi
                }
                printf("  ----------------------------------------------------\n");
                printf("  FIRTINA OZETI: %d turda hasar alindi, %d tur guvende gecti.\n", hasarliTur, guvenliTur);
                printf("  Toplam kayip: -%d saglik, -%d oksijen, +%d modul hasari.\n",
                       saglikKaybi, oksijenKaybi, hasarArtisi);
                if (guvenliTur > 0 && saglik > 0 && oksijen > 0) {
                    kredi = kredi + guvenliTur * 5;
                    toplamKredi = toplamKredi + guvenliTur * 5;
                    printf("  Firtina olcum verileri merkeze gonderildi: +%d MK.\n", guvenliTur * 5);
                }
                printf("  ----------------------------------------------------\n");
                turHarcandi = 1;
                break;

            // ---------- H: HAVA KILIDI SIFRESI ----------
            case 'h':
            case 'H':
                if (kilitMuhurlu == 1) {
                    printf("  Hava kilidi zaten muhurlu.\n");
                    break;
                }
                if (enerji < 5) {
                    printf("  Yetersiz enerji! Kilit paneli icin en az 5 enerji lazim. Komut iptal.\n");
                    break;
                }

                sifre = 'A' + (gun % 26);   // sifre her gun degisiyor
                tahmin = '?';
                deneme = 0;
                printf("  IPUCU: Sifre, alfabede A harfinden bugunun gun sayisi (%d) kadar ilerideki harftir.\n", gun);
                printf("  3 deneme hakkin var, her yanlis deneme -5 enerji.\n");

                do {
                    printf("  Deneme %d/3 - Harf girin > ", deneme + 1);
                    if (scanf(" %c", &tahmin) != 1) {
                        break;   // giris bittiyse donguden cik
                    }
                    tamponuTemizle();

                    // kucuk harf girildiyse buyuk harfe ceviriyorum (ASCII'de aradaki fark 32)
                    if (tahmin >= 'a' && tahmin <= 'z') {
                        tahmin = tahmin - 32;
                    }
                    if (tahmin < 'A' || tahmin > 'Z') {
                        printf("  Sadece A-Z arasi bir harf girin (hakkin gitmedi).\n");
                        continue;
                    }

                    deneme++;
                    if (tahmin != sifre) {
                        enerji = enerji - 5;
                        if (tahmin < sifre) {
                            printf("  Yanlis! ILERI GIT (sifre alfabede daha sonra). -5 enerji, kalan hak: %d\n", 3 - deneme);
                        } else {
                            printf("  Yanlis! GERI GIT (sifre alfabede daha once). -5 enerji, kalan hak: %d\n", 3 - deneme);
                        }
                    }
                } while (tahmin != sifre && deneme < 3);   // hem yanlis hem hak varsa devam

                if (tahmin == sifre) {
                    kilitMuhurlu = 1;
                    sonuc = 1;
                    printf("  DOGRU! Hava kilidi muhurlendi. Firtinada oksijen sizintisi olmayacak.\n");
                } else {
                    modulHasari = modulHasari + 20;
                    printf("  3 deneme de yanlis! Kilit zorlandi: +20 modul hasari. (Sifre '%c' idi)\n", sifre);
                }
                turHarcandi = 1;
                break;

            // ---------- M: MARS GOREVI (kredi kazanma) ----------
            case 'm':
            case 'M':
                printf("  ------------------- MARS GOREVLERI -------------------\n");
                printf("  [1] Buz Numunesi Toplama  -10 enerji, -5 oksijen   Esik %d  Odul %d MK + 6 su\n",
                       75 + gorevEsikBonus + gorevBasariBonus, 25 * gorevOdulYuzde / 100);
                printf("  [2] Anten Onarimi         -15 enerji, -8 oksijen   Esik %d  Odul %d MK + 1 parca\n",
                       55 + gorevEsikBonus + gorevBasariBonus, 45 * gorevOdulYuzde / 100);
                printf("  [3] Krater Madeni Tarama  -25 enerji, -12 oksijen  Esik %d  Odul %d MK\n",
                       35 + gorevEsikBonus + gorevBasariBonus, 90 * gorevOdulYuzde / 100);
                printf("  Radyasyon: 1) 4  2) 7  3) 10 saglik (kiyafet korumasi kadar azalir, korumaniz: %d)\n", koruma);
                printf("  [0] Vazgec\n");
                printf("  Gorev secin > ");
                if (scanf(" %c", &secim) != 1) {
                    secim = '0';
                }
                tamponuTemizle();

                gorevNo = 0;
                gorevEnerji = 0; gorevOksijen = 0; esik = 0; odul = 0;
                odulParca = 0; odulSu = 0;
                cezaSaglik = 0; cezaHasar = 0; radyasyon = 0;

                switch (secim) {
                    case '1':
                        gorevNo = 1;
                        strcpy(gorevAdi, "Buz Numunesi Toplama");
                        gorevEnerji = 10; gorevOksijen = 5; esik = 75; odul = 25; odulSu = 6;
                        cezaSaglik = 6; radyasyon = 4;
                        break;
                    case '2':
                        gorevNo = 2;
                        strcpy(gorevAdi, "Anten Onarimi");
                        gorevEnerji = 15; gorevOksijen = 8; esik = 55; odul = 45; odulParca = 1;
                        cezaHasar = 10; radyasyon = 7;
                        break;
                    case '3':
                        gorevNo = 3;
                        strcpy(gorevAdi, "Krater Madeni Tarama");
                        gorevEnerji = 25; gorevOksijen = 12; esik = 35; odul = 90;
                        cezaSaglik = 15; cezaHasar = 10; radyasyon = 10;
                        break;
                    case '0':
                        printf("  Gorev panosundan cikildi.\n");
                        break;
                    default:
                        printf("  Gecersiz gorev secimi, tur harcanmadi.\n");
                        break;
                }
                if (gorevNo == 0) {
                    break;
                }

                if (enerji < gorevEnerji) {
                    printf("  Yetersiz enerji! %s icin %d enerji lazim (enerji: %d). Gorev iptal.\n",
                           gorevAdi, gorevEnerji, enerji);
                    break;
                }
                if (oksijen <= gorevOksijen) {
                    printf("  Bu gorev oksijenini bitirir (%d lazim, oksijen: %d). Gorev iptal.\n",
                           gorevOksijen, oksijen);
                    break;
                }

                esik = esik + gorevEsikBonus + gorevBasariBonus;   // zorluk ve pilot bonusu
                if (esik > 95) {
                    esik = 95;
                }
                enerji = enerji - gorevEnerji;
                oksijen = oksijen - gorevOksijen;
                printf("  %s basladi (-%d enerji, -%d oksijen).\n", gorevAdi, gorevEnerji, gorevOksijen);

                // disarida radyasyon aliyoruz, kiyafetlerin korumasi kadar azaliyor
                radyasyon = radyasyon - koruma;
                if (radyasyon < 0) {
                    radyasyon = 0;
                }
                saglik = saglik - radyasyon;
                if (radyasyon > 0) {
                    printf("  Radyasyon: -%d saglik (kiyafetler %d puan engelledi).\n", radyasyon, koruma);
                } else {
                    printf("  Kiyafetlerin radyasyonu tamamen engelledi (koruma %d).\n", koruma);
                }

                zar = rand() % 100;
                if (zar < esik) {
                    printf("  Gorev: Zar: %d / Esik: %d -> BASARILI\n", zar, esik);
                    odul = odul * gorevOdulYuzde / 100;   // jeolog %25 fazla kazaniyor
                    kredi = kredi + odul;
                    toplamKredi = toplamKredi + odul;
                    yedekParca = yedekParca + odulParca;
                    su = su + odulSu;
                    basariliGorev++;
                    sonuc = 1;
                    printf("  Gorev tamamlandi! +%d MK", odul);
                    if (odulParca > 0) {
                        printf(", +%d yedek parca", odulParca);
                    }
                    if (odulSu > 0) {
                        printf(", +%d su", odulSu);
                    }
                    printf("\n");
                } else {
                    printf("  Gorev: Zar: %d / Esik: %d -> BASARISIZ\n", zar, esik);
                    basarisizGorev++;
                    if (cezaSaglik > 0) {
                        cezaSaglik = cezaSaglik - koruma;
                        if (cezaSaglik < 1) {
                            cezaSaglik = 1;
                        }
                        saglik = saglik - cezaSaglik;
                        printf("  Gorev basarisiz! Kazada yaralandin: -%d saglik.\n", cezaSaglik);
                    }
                    if (cezaHasar > 0) {
                        modulHasari = modulHasari + cezaHasar;
                        printf("  Ekipman bozuldu: +%d modul hasari.\n", cezaHasar);
                    }
                }
                turHarcandi = 1;
                break;

            // ---------- S: KIYAFET MAGAZASI ----------
            case 's':
            case 'S':
                alinan = 0;
                do {
                    printf("  ---------------- KIYAFET MAGAZASI (Kredin: %d MK) ----------------\n", kredi);
                    printf("  [1] Termal Iclik             30 MK  +10 can, +1 koruma");
                    if (kiyafet1 == 1) {
                        printf("  [ALINDI]");
                    }
                    printf("\n  [2] Isitmali Eldiven         50 MK  +15 can, +2 koruma");
                    if (kiyafet2 == 1) {
                        printf("  [ALINDI]");
                    }
                    printf("\n  [3] Radyasyon Yelegi         80 MK  +20 can, +3 koruma");
                    if (kiyafet3 == 1) {
                        printf("  [ALINDI]");
                    }
                    printf("\n  [4] Kompozit EVA Kaski      110 MK  +25 can, +4 koruma");
                    if (kiyafet4 == 1) {
                        printf("  [ALINDI]");
                    }
                    printf("\n  [5] Yeni Nesil EVA Giysisi  160 MK  +35 can, +5 koruma");
                    if (kiyafet5 == 1) {
                        printf("  [ALINDI]");
                    }
                    printf("\n  [0] Magazadan cik\n");
                    printf("  (Koruma; yaralanma, firtina, radyasyon ve gorev kazalarindaki can kaybini azaltir.)\n");
                    printf("  Secim > ");
                    if (scanf(" %c", &secim) != 1) {
                        secim = '0';
                    }
                    tamponuTemizle();

                    urun = 0; fiyat = 0; canArtisi = 0; korumaArtisi = 0; sahip = 0;
                    switch (secim) {
                        case '1': urun = 1; fiyat = 30;  canArtisi = 10; korumaArtisi = 1; sahip = kiyafet1; break;
                        case '2': urun = 2; fiyat = 50;  canArtisi = 15; korumaArtisi = 2; sahip = kiyafet2; break;
                        case '3': urun = 3; fiyat = 80;  canArtisi = 20; korumaArtisi = 3; sahip = kiyafet3; break;
                        case '4': urun = 4; fiyat = 110; canArtisi = 25; korumaArtisi = 4; sahip = kiyafet4; break;
                        case '5': urun = 5; fiyat = 160; canArtisi = 35; korumaArtisi = 5; sahip = kiyafet5; break;
                        case '0': break;
                        default:
                            printf("  Gecersiz secim, 0-5 arasi bir sayi girin.\n");
                            break;
                    }

                    if (urun != 0) {
                        if (sahip == 1) {
                            printf("  Bu kiyafeti zaten aldin.\n");
                        } else if (kredi < fiyat) {
                            printf("  Kredin yetmiyor, %d MK eksik. Mars gorevleriyle (M) kredi kazanabilirsin.\n",
                                   fiyat - kredi);
                        } else {
                            kredi = kredi - fiyat;
                            koruma = koruma + korumaArtisi;
                            saglik = saglik + canArtisi;
                            if (saglik > 100) {
                                saglik = 100;   // can 100'u gecemez
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
                            printf("  Satin alindi! +%d can, +%d koruma. Kalan kredi: %d MK\n",
                                   canArtisi, korumaArtisi, kredi);
                        }
                    }

                    // kredi alinmamis baska bir kiyafete yetmiyorsa magazadan otomatik cikiyorum
                    yeterli = 0;
                    if (kiyafet1 == 0 && kredi >= 30) {
                        yeterli = 1;
                    }
                    if (kiyafet2 == 0 && kredi >= 50) {
                        yeterli = 1;
                    }
                    if (kiyafet3 == 0 && kredi >= 80) {
                        yeterli = 1;
                    }
                    if (kiyafet4 == 0 && kredi >= 110) {
                        yeterli = 1;
                    }
                    if (kiyafet5 == 0 && kredi >= 160) {
                        yeterli = 1;
                    }
                    if (secim != '0' && yeterli == 0) {
                        printf("  Kredin baska bir kiyafete yetmiyor, magazadan cikiliyor.\n");
                        secim = '0';
                    }
                } while (secim != '0');

                // sadece bir sey alindiysa tur harcaniyor
                if (alinan > 0) {
                    printf("  %d kiyafet aldin (1 tur gecti).\n", alinan);
                    turHarcandi = 1;
                } else {
                    printf("  Bir sey almadin, tur harcanmadi.\n");
                }
                break;

            // ---------- Y: OZEL YETENEK ----------
            case 'y':
            case 'Y':
                // yetenek 3 gunde bir kullanilabiliyor
                if (gun - sonYetenekGunu < 3) {
                    printf("  %s henuz hazir degil, %d gun sonra kullanabilirsin.\n",
                           yetenekAdi, 3 - (gun - sonYetenekGunu));
                    break;
                }
                if (karakter == 1 || karakter == 3) {
                    yetenekEnerji = 10;
                } else {
                    yetenekEnerji = 15;
                }
                if (enerji < yetenekEnerji) {
                    printf("  Yetersiz enerji! %s icin %d enerji lazim. Komut iptal.\n", yetenekAdi, yetenekEnerji);
                    break;
                }
                if (karakter == 1 && modulHasari == 0) {
                    printf("  Modulde hasar yok, Acil Kaynak'i simdi kullanmak bosa gider.\n");
                    break;
                }

                enerji = enerji - yetenekEnerji;
                switch (karakter) {
                    case 1:
                        modulHasari = modulHasari - 25;
                        printf("  ACIL KAYNAK: Abdullah modulu kaynakla yamadi. -25 hasar (parca harcanmadi).\n");
                        break;
                    case 2:
                        yedekParca = yedekParca + 2;
                        su = su + 4;
                        kredi = kredi + 20;
                        toplamKredi = toplamKredi + 20;
                        oksijen = oksijen - 5;
                        printf("  DERIN SONDAJ: Selcuk derinde buz buldu. +2 parca, +4 su, +20 MK, -5 oksijen.\n");
                        break;
                    case 3:
                        saglik = saglik + 30;
                        printf("  ACIL MUDAHALE: Recep yaralarini tedavi etti. +30 saglik.\n");
                        break;
                    case 4:
                        oksijen = oksijen + 6;
                        printf("  ELEKTROLIZ: Emo yedek hucreleri calistirdi. +6 oksijen (su harcanmadi).\n");
                        break;
                    case 5:
                        su = su + 6;
                        kredi = kredi + 25;
                        toplamKredi = toplamKredi + 25;
                        oksijen = oksijen - 5;
                        printf("  ROVER SEFERI: Ece buz deposuna gitti. +6 su, +25 MK, -5 oksijen.\n");
                        break;
                }
                printf("  (-%d enerji) Yetenek 3 gun sonra tekrar hazir olacak.\n", yetenekEnerji);
                sonYetenekGunu = gun;
                turHarcandi = 1;
                break;

            // ---------- B: YARDIM (tur harcamaz) ----------
            case 'b':
            case 'B':
                komutlariYazdir();
                break;

            // ---------- Q: CIKIS ----------
            case 'q':
            case 'Q':
                printf("  Simulasyondan cikiliyor...\n");
                oyunDurumu = 3;
                break;

            // tanimsiz komut: uyari veriyoruz, tur harcanmiyor
            default:
                printf("  Tanimsiz komut! Gecerli komutlar: G O T D U K H M S Y B Q (tur harcanmadi)\n");
                break;
        }

        // her komuttan sonra degerleri 0-100 arasinda tutuyorum
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

        // gecerli komut girildiyse tur artiyor, her 3 turda bir yeni gun basliyor
        if (turHarcandi == 1) {
            tur++;

            // gunun hikayesi icin bu turda ne yapildigini kaydediyorum (gunun 1., 2. veya 3. islemi)
            if (komut >= 'a' && komut <= 'z') {
                komut = komut - 32;
            }
            gunlukKomut[(tur - 1) % 3] = komut;
            gunlukSonuc[(tur - 1) % 3] = sonuc;

            if (tur % 3 == 0 && saglik > 0 && oksijen > 0) {
                gunlukYazdir(gun, karakterAdi, yetenekAdi, gunlukKomut, gunlukSonuc);

                if (gun == 15) {
                    oyunDurumu = 1;   // 15. gun bitti, kurtarma ekibi geldi
                } else {
                    // gece olayi: isler cok iyi gidiyorsa Mars zorlasiyor, kotu olay ihtimali artiyor
                    if (saglik >= 70 && enerji >= 60 && oksijen >= 50) {
                        olayEsigi = 35;
                    } else {
                        olayEsigi = 15;
                    }
                    zar = rand() % 100;
                    if (zar < olayEsigi) {
                        printf("  Gece olayi: Zar: %d / Esik: %d -> KOTU SURPRIZ\n", zar, olayEsigi);
                        // bu olaylar sadece enerji harcatiyor, hicbir sey kazandirmiyor
                        olayTuru = rand() % 4;
                        switch (olayTuru) {
                            case 0:
                                enerji = enerji - 15;
                                printf("  Aksam Tharsis volkanlari tarafindan bir volkan firtinasi geldi.\n");
                                printf("  Butun gece jeneratoru ayakta tutmaya calistin: -15 enerji, kazanc yok.\n");
                                break;
                            case 1:
                                enerji = enerji - 15;
                                printf("  Merkezden acil bir gorev geldi, gece yarisi disari ciktin.\n");
                                printf("  Ama sinyal kayboldu ve gorev basarisiz oldu: -15 enerji, eli bos dondun.\n");
                                break;
                            case 2:
                                enerji = enerji - 10;
                                printf("  Gece gunes patlamasi oldu, butun sistemleri tek tek yeniden baslattin.\n");
                                printf("  -10 enerji, kazanc yok.\n");
                                break;
                            default:
                                enerji = enerji - 10;
                                printf("  Gece bir Mars depremi oldu, sabaha kadar raflari topladin.\n");
                                printf("  -10 enerji, kazanc yok.\n");
                                break;
                        }
                        if (enerji < 0) { enerji = 0; }
                    } else {
                        printf("  Gece olayi: Zar: %d / Esik: %d -> SAKIN BIR GECE\n", zar, olayEsigi);
                    }

                    gun++;
                    oksijen = oksijen - 8;
                    printf("\n  *** GUN %d BASLADI *** Gece boyunca -8 oksijen harcandi.\n", gun);
                    if (modulHasari > 70) {
                        saglik = saglik - 5;
                        printf("  Modul cok hasarli (%d): soguk ve radyasyon yuzunden -5 saglik.\n", modulHasari);
                    }
                    if (oksijen < 0) { oksijen = 0; }
                    if (saglik < 0) { saglik = 0; }
                }
            }
        }

        // saglik veya oksijen biterse oyun kaybediliyor
        if (saglik <= 0 || oksijen <= 0) {
            oyunDurumu = 2;
        }

    } while (oyunDurumu == 0);

    // ---------------- SONUC RAPORU ----------------
    puan = (gun * 100) + saglik + oksijen - modulHasari;

    printf("\n==============================================================\n");
    if (oyunDurumu == 1) {
        printf("  KURTARMA EKIBI GELDI! 15 gunu tamamladin ve hayatta kaldin.\n");
    } else if (oyunDurumu == 2) {
        if (oksijen <= 0) {
            printf("  OYUN BITTI: Oksijen tukendi.\n");
        } else {
            printf("  OYUN BITTI: Saglik sifira dustu.\n");
        }
    } else {
        printf("  SIMULASYON SONLANDIRILDI (Q ile cikis yapildi).\n");
    }
    printf("==============================================================\n");
    printf("  Karakter           : %s (%s)\n", karakterAdi, meslek);
    if (zorluk == 'K') {
        printf("  Zorluk             : Kolay\n");
    } else if (zorluk == 'N') {
        printf("  Zorluk             : Normal\n");
    } else {
        printf("  Zorluk             : Zor\n");
    }
    printf("  Hayatta kalinan gun: %d / 15\n", gun);
    printf("  Toplam tur         : %d\n", tur);
    printf("  Son durum          : Saglik %d, Enerji %d, Oksijen %d, Su %d, Parca %d, Hasar %d\n",
           saglik, enerji, oksijen, su, yedekParca, modulHasari);
    printf("  Kredi              : %d MK kazanildi, %d MK kaldi\n", toplamKredi, kredi);
    printf("  Gorevler           : %d basarili, %d basarisiz\n", basariliGorev, basarisizGorev);
    printf("  Kiyafet            : %d tane (koruma %d)\n",
           kiyafet1 + kiyafet2 + kiyafet3 + kiyafet4 + kiyafet5, koruma);
    printf("--------------------------------------------------------------\n");
    printf("  PUAN = (gun x 100) + saglik + oksijen - modul hasari\n");
    printf("       = (%d x 100) + %d + %d - %d = %d\n", gun, saglik, oksijen, modulHasari, puan);

    // puana gore unvan veriyorum
    if (puan >= 1650) {
        printf("  Unvan: EFSANEVI KOLONI LIDERI\n");
    } else if (puan >= 1400) {
        printf("  Unvan: Kidemli Mars Muhendisi\n");
    } else if (puan >= 800) {
        printf("  Unvan: Dayanikli Kolonist\n");
    } else {
        printf("  Unvan: Caylak Kasif\n");
    }
    printf("==============================================================\n");

    return 0;
}
