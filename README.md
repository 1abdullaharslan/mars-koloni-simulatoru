# Mars Kolonisi Yaşam Destek Simülatörü

C diliyle yazılmış, tek karakterlik komutlarla oynanan, metin tabanlı bir hayatta kalma simülasyonu. Oyuncu, Mars'taki Ares-7 araştırma istasyonunda tek başına kalan bir mürettebat üyesini yönetir ve kurtarma ekibi gelene kadar **15 gün** hayatta kalmaya çalışır.

Proje, *Algoritma ve Programlama* dersi kapsamında **Abdullah Arslan** (öğrenci no: 1260505042) tarafından hazırlanmıştır. Bütün oyun mantığı C'nin temel yapılarıyla (aritmetik ve mantıksal operatörler, `if–else if–else`, `switch–case`, `for`, `do–while`, `break`, `continue`) kurulmuştur.

## İçindekiler

- [Özellikler](#özellikler)
- [Derleme ve çalıştırma](#derleme-ve-çalıştırma)
- [Nasıl oynanır?](#nasıl-oynanır)
- [Karakterler ve yetenekler](#karakterler-ve-yetenekler)
- [Ekonomi: Mars görevleri ve kıyafet mağazası](#ekonomi-mars-görevleri-ve-kıyafet-mağazası)
- [Zaman akışı, oyun sonu ve puan](#zaman-akışı-oyun-sonu-ve-puan)
- [Ödev gereksinimleri ve koddaki karşılıkları](#ödev-gereksinimleri-ve-koddaki-karşılıkları)
- [Test](#test)
- [Proje yapısı](#proje-yapısı)

## Özellikler

**Ödevin zorunlu özellikleri**

- Üç zorluk seviyesi (Kolay, Normal, Zor); başlangıç değerlerini ve olay eşiklerini değiştirir.
- `switch–case` ile komut sistemi; büyük/küçük harf duyarsızlığı `case 'g': case 'G':` biçiminde fall-through ile sağlanır.
- Sağlık, enerji, oksijen, su, yedek parça, modül hasarı, hava kilidi, gün ve tur takibi.
- Tüm rastgele olaylarda `srand(time(NULL))` ve `rand() % 100`; her zar ve eşik değeri ekrana yazılır (`Zar: 37 / Esik: 45 -> BASARILI`).
- `for` döngüsüyle kum fırtınası (`break` ve `continue` dahil), `do–while` döngüsüyle sınırlı denemeli hava kilidi şifresi.
- Oyun nasıl biterse bitsin sonuç raporu ve puan.

**Ek özellikler**

- **5 oynanabilir karakter:** Her birinin bir pasif ve bir aktif (`Y`) yeteneği vardır.
- **Mars Kredisi (MK) ekonomisi:** Keşiflerden, fırtına verilerinden ve `M` komutuyla üstlenilen Mars görevlerinden kredi kazanılır.
- **Kıyafet mağazası (`S`):** Kazanılan krediyle, krediniz yettiği kadar kıyafet alınır. Kıyafetler canı artırır ve kalıcı koruma sağlar.
- **Oyunun amacı ekranı:** Karakter seçiminden önce oyuncuya ne yapması gerektiği maddeler hâlinde anlatılır.
- **Günlük hikâye:** Her günün sonunda, o gün seçilen üç işleme ve sonuçlarına göre karakterin gününü anlatan kısa bir hikâye yazılır.
- **Gece olayları:** İşler çok iyi gidiyorsa geceleri volkan fırtınası, başarısız acil görev, güneş patlaması veya Mars depremi gibi olaylar çıkar; bunlar enerji harcatır ama hiçbir şey kazandırmaz.
- **Giriş kontrolü:** Sayı yerine harf girilmesi, fazladan karakterler veya girişin bitmesi (Ctrl+D / Ctrl+Z) programı kilitlemez.
- **Her konsolda düzgün görünüm:** Program çıktılarında Türkçe karakter kullanılmadığı için yazılar Windows, macOS ve Linux konsollarında bozulmaz.

## Derleme ve çalıştırma

Herhangi bir C99 derleyicisi yeterlidir; dış kütüphane gerekmez.

**macOS / Linux**

```bash
gcc -std=c99 -Wall -Wextra -o mars_koloni mars_koloni.c
./mars_koloni
```

**Windows (MinGW)**

```bash
gcc -std=c99 -Wall -Wextra -o mars_koloni.exe mars_koloni.c
mars_koloni.exe
```

**Dev-C++ / Code::Blocks / Visual Studio:** `mars_koloni.c` dosyasını yeni bir C projesine ekleyip derleyin.

Kod `-std=c99 -Wall -Wextra -pedantic` bayraklarıyla uyarısız derlenir.

## Nasıl oynanır?

Oyun başında önce zorluk seviyesi seçilir; ardından oyunun amacı anlatılır ve karakter seçilir. Sonra her turda tek harfli bir komut girilir. Her komutun öncesinde güncel durum özeti gösterilir:

```text
[Gun 3/15 | Tur 7 | Saglik 82 | Enerji 72 | Oksijen 36 | Su 16 | Parca 2 | Hasar 0 | Kredi 80 MK]
Komut >
```

### Komutlar

| Komut | İşlev | Tur harcar mı? |
|---|---|---|
| `G` | Keşif görevi: −15 enerji, −10 oksijen. Su (12–18 birim), yedek parça veya nadir mineral bulunabilir ya da eli boş dönülür. | Evet |
| `O` | Oksijen üretimi: her 2 birim su 3 birim oksijene dönüşür. | Evet |
| `T` | Tamir: yedek parçayla modül hasarını azaltır. | Evet |
| `D` | Durum raporu ve kritik uyarılar. | Hayır |
| `U` | Uyku: enerji ve sağlık artar, oksijen tüketimi sürer (−2). | Evet |
| `K` | Kum fırtınasını tur tur simüle eder. | Evet |
| `H` | Hava kilidi şifresi: 3 denemede kilidi mühürle. | Evet |
| `M` | Mars görevi: riskli görevlerle kredi kazan. | Evet (vazgeçilirse hayır) |
| `S` | Kıyafet mağazası. | Yalnızca alışveriş yapılırsa |
| `Y` | Karakterin aktif yeteneği (3 günde bir). | Evet |
| `B` | Komut listesi. | Hayır |
| `Q` | Çıkış ve sonuç raporu. | — |

Tanımsız bir komut girildiğinde uyarı verilir ve tur harcanmaz. Komutlar büyük/küçük harf duyarsızdır. Birden fazla harf yazılırsa yalnızca ilk harf okunur.

### Zorluk seviyeleri

| | Kolay | Normal | Zor |
|---|---|---|---|
| Sağlık / Enerji / Oksijen | 100 / 100 / 100 | 100 / 90 / 90 | 90 / 80 / 80 |
| Su / Yedek parça | 60 / 5 | 50 / 3 | 55 / 2 |
| Başlangıç modül hasarı | 10 | 20 | 35 |
| Başlangıç kredisi | 30 MK | 15 MK | 0 MK |
| Kum fırtınası | 3 tur, temel hasar 4 | 5 tur, temel hasar 6 | 7 tur, temel hasar 9 |
| Su bulma eşiği (yüksek / düşük) | 70 / 40 | 60 / 30 | 45 / 20 |
| Keşif yaralanma hasarı | 8 | 12 | 15 |
| Mars görevi eşiği | +10 | 0 | −10 |

## Karakterler ve yetenekler

| Karakter | Pasif yetenek | Aktif yetenek (`Y`) |
|---|---|---|
| **Abdullah** – Mühendis | *Usta Eller:* tamir +10 hasar onarır, tamir 10 yerine 5 enerji harcar. | *Acil Kaynak:* parça harcamadan modül hasarını 25 azaltır. |
| **Selçuk** – Jeolog | *Maden Gözü:* keşifte parça bulma şansı +10, görev ödülleri +%25. | *Derin Sondaj:* +2 yedek parça, +4 su ve +20 MK. |
| **Recep** – Doktor | *Saha Hekimi:* dinlenmede +10 ek sağlık, yaralanma hasarı yarıya iner. | *Acil Müdahale:* anında +30 sağlık. |
| **Emo** – Kimyager | *Verimli Reaktör:* oksijen üretimi 8 yerine 5 enerji, su bulma şansı +5. | *Elektroliz:* su harcamadan +6 oksijen. |
| **Ece** – Pilot | *Fırtına Avcısı:* fırtınada korunma eşiği +15, görev başarısı +10. | *Rover Seferi:* +6 su ve +25 MK. |

Aktif yetenekler en erken 3 günde bir kullanılabilir ve 10–15 enerji harcar.

## Ekonomi: Mars görevleri ve kıyafet mağazası

### Kredi kaynakları

| Kaynak | Kazanç |
|---|---|
| Keşif (`G`) | Su bulursa 10 MK, parça bulursa 15 MK, nadir mineral bulursa 40 MK |
| Kum fırtınası (`K`) | Zararsız atlatılan her fırtına turu için 5 MK (ölçüm verisi) |
| Mars görevleri (`M`) | Aşağıdaki tablo |
| Yetenekler | Jeolog +20 MK, Pilot +25 MK |

### Mars görevleri (`M`)

| Görev | Maliyet | Başarı eşiği (Normal) | Ödül | Başarısızlık | Radyasyon |
|---|---|---|---|---|---|
| 1. Buz Numunesi Toplama | −10 enerji, −5 oksijen | 75 | 25 MK + 6 su | −6 sağlık | 4 |
| 2. Anten Onarımı | −15 enerji, −8 oksijen | 55 | 45 MK + 1 parça | +10 modül hasarı | 7 |
| 3. Krater Madeni Tarama | −25 enerji, −12 oksijen | 35 | 90 MK | −15 sağlık, +10 hasar | 10 |

Görev başarısı `zar < eşik` ile belirlenir. Uzun yüzey görevlerinde alınan radyasyon dozu, giyilen kıyafetlerin toplam koruması kadar azalır.

### Kıyafet mağazası (`S`)

| Kıyafet | Fiyat | Can | Koruma |
|---|---|---|---|
| Termal İçlik | 30 MK | +10 | +1 |
| Isıtmalı Eldiven | 50 MK | +15 | +2 |
| Radyasyon Yeleği | 80 MK | +20 | +3 |
| Kompozit EVA Kaskı | 110 MK | +25 | +4 |
| Yeni Nesil EVA Giysisi | 160 MK | +35 | +5 |

Mağaza, krediniz yettiği sürece alışverişe devam etmenizi sağlar. Krediniz başka bir kıyafete yetmediğinde mağaza kendiliğinden kapanır ve eksik tutarı bildirir. Koruma; keşif yaralanmalarındaki, kum fırtınasındaki, görev kazalarındaki ve radyasyondaki can kaybını azaltır. Sağlık hiçbir zaman 100'ü aşmaz.

## Zaman akışı, oyun sonu ve puan

- Geçerli her komut tur sayacını 1 artırır (`D`, `B` ve tanımsız komutlar hariç).
- Her 3 turda bir (`tur % 3 == 0`) yeni gün başlar: oksijen 8 azalır, modül hasarı 70'in üzerindeyse ayrıca 5 sağlık kaybedilir.
- Gün bittiğinde o günün **günlüğü** yazılır: Günün üç işlemi `gunlukKomut[3]` ve `gunlukSonuc[3]` dizilerinde tutulur, `gunlukYazdir()` fonksiyonu bunları `for` ve `switch` ile hikâyeye çevirir.
- Ardından **gece olayı** için zar atılır. Sağlık ≥ 70, enerji ≥ 60 ve oksijen ≥ 50 ise kötü olay eşiği 35, değilse 15'tir. Kötü olaylar 10–15 enerji harcatır ve hiçbir şey kazandırmaz.
- Sağlık veya oksijen 0'a düşerse oyun kaybedilir.
- 15. gün tamamlandığında (45. tur) kurtarma ekibi gelir ve oyun kazanılır.
- Oyun her nasıl biterse bitsin, hayatta kalınan gün, toplam tur, son durum değerleri, ekonomi özeti ve puan yazdırılır.

**Puan formülü**

```text
Puan = (gün × 100) + sağlık + oksijen − modül hasarı
```

Gün katsayısı 100 seçildi; çünkü oyunun asıl hedefi hayatta kalmaktır ve bir gün fazla yaşamak, sağlığın ya da oksijenin tamamen dolu olmasıyla (100) aynı puanı getirmelidir. 15 günlük bir oyunda puanın 1500'ü günden gelir. Sağlık, oksijen ve modül hasarı 0–100 aralığında olduğu için katsayıları 1'dir; bu değerler aynı güne ulaşan oyuncuları birbirinden ayırır. Sağlık ve oksijen, kurtarma anındaki durumun ne kadar iyi olduğunu; modül hasarı ise istasyonun ne kadar korunduğunu gösterir.

## Ödev gereksinimleri ve koddaki karşılıkları

| Gereksinim | Kullanılan yapı | Koddaki yeri |
|---|---|---|
| Zorluk seçimi, geçersiz girişte tekrar | `do–while` + `switch–case` (fall-through) | Zorluk seçimi bölümü |
| Komut sistemi, tanımsız komutta uyarı | `do–while` + `switch–case`, `default` | Ana komut döngüsü |
| Keşifte su bulma ve yaralanma | `&&`, `\|\|`, `if – else if – else` | `case 'G'` |
| 2 su → 3 oksijen, tek sayıda artık su | `%`, `/`, `*` | `case 'O'` |
| Hasar kademeleri | `if – else if – else` | `case 'T'` |
| Kritik uyarılar ve "TÜM SİSTEMLER NORMAL" | Ayrı `if`'ler, tek `&&` koşulu | `case 'D'` |
| Dinlenmede oksijen kontrolü | İç içe `if` | `case 'U'` |
| Kum fırtınası | `for`, `%` (çift tur ×2), `continue`, `break` | `case 'K'` |
| Hava kilidi şifresi | `do–while`, tek mantıksal ifade, `'A' + (gun % 26)` | `case 'H'` |
| 0–100 sınırlandırması | Her komuttan sonra `if` blokları | Ana döngü sonu |
| Gün akışı ve oyun sonu | `tur % 3 == 0`, `\|\|` | Ana döngü sonu |
| Günlük hikâye (ek) | Dizi, `for`, `switch`, fonksiyon | `gunlukYazdir()` |
| Gece olayları (ek) | `&&`, `rand() % 100`, `switch` | Ana döngü sonu |

Kodun ayrıntılı açıklaması proje raporundadır (`Rapor.pdf`).

## Test

- Kod `-std=c99 -Wall -Wextra -pedantic` bayraklarıyla uyarısız derlenir.
- Program `-fsanitize=address,undefined` ile derlenip bütün komutlar ve geçersiz girişlerle çalıştırıldı; bellek hatası veya tanımsız davranış bulunmadı.
- Bir test botu her zorluk ve karakter için 60'ar oyun (toplam 900) oynadı. Her durum satırında sağlık, enerji, oksijen ve hasarın 0–100 aralığında kaldığı, su, parça ve kredinin negatif olmadığı doğrulandı.
- Rastgele komut ve geçersiz girişlerle oynanan 500 oyunda program hiç takılmadı veya çökmedi.

Botun kazanma oranları zorluk seviyelerinin birbirinden ayrıştığını gösterir:

| Bot ile kazanma oranı | Mühendis | Jeolog | Doktor | Kimyager | Pilot |
|---|---|---|---|---|---|
| Kolay | %96 | %96 | %100 | %100 | %100 |
| Normal | %68 | %85 | %65 | %98 | %91 |
| Zor | %35 | %35 | %33 | %78 | %61 |

Oksijenin tek kaynağı su olduğu için oksijen yeteneği olan Kimyager en kolay karakterdir.

## Proje yapısı

```text
.
├── mars_koloni.c   # Simülatörün tüm kaynak kodu
├── README.md       # Bu belge
└── Rapor.pdf       # Kod mantığını açıklayan proje raporu
```
