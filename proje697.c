/** Mars Kolonisi Yasam Destek Simulatoru**/
 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* ---------- DURUM DEGISKENLERI ---------- */
int saglik, enerji, oksijen;      /* 0-100 */
int su, parca;                    /* birim / adet */
int modulHasari;                  /* 0-100 */
int kilitMuhurlu = 0;             /* 0 / 1 */
int gun = 1, tur = 0;

/* Zorluk seviyesine bagli parametreler */
int firtinaTur;                   /* kum firtinasi tur sayisi */
int firtinaHasar;                 /* firtinada taban hasar */
int bulmaBonus;                   /* su/parca bulma esigine eklenir */

/* ---------- YARDIMCI FONKSIYONLAR ---------- */

/* Her komuttan sonra degerleri 0-100 araliginda tutar */
void sinirla(void)
{
    if (saglik < 0)       saglik = 0;
    if (saglik > 100)     saglik = 100;
    if (enerji < 0)       enerji = 0;
    if (enerji > 100)     enerji = 100;
    if (oksijen < 0)      oksijen = 0;
    if (oksijen > 100)    oksijen = 100;
    if (modulHasari < 0)  modulHasari = 0;
    if (modulHasari > 100) modulHasari = 100;
    if (su < 0)           su = 0;
    if (parca < 0)        parca = 0;
}

/* Girdi tamponunu satir sonuna kadar temizler */
void tamponTemizle(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

/* Zar ve esik degerini ekrana yazdirir */
void zarYaz(int zar, int esik)
{
    if (zar < esik)
        printf("  Zar: %d / Esik: %d -> BASARILI\n", zar, esik);
    else
        printf("  Zar: %d / Esik: %d -> BASARISIZ\n", zar, esik);
}

void durumSatiri(void)
{
    printf("\n[Gun %d | Tur %d] Saglik:%d Enerji:%d Oksijen:%d Su:%d Parca:%d Hasar:%d Kilit:%d\n",
           gun, tur, saglik, enerji, oksijen, su, parca, modulHasari, kilitMuhurlu);
}

/* ---------- KOMUTLAR ---------- */

/* G: Kesif. Basarili (tur harcayan) ise 1, iptal ise 0 dondurur */
int kesif(void)
{
    int zar, esik, zarYara, esikYara, bulunan;

    if (enerji < 15) {
        printf("  Kesif icin enerji yetersiz (gerekli: 15). Komut iptal edildi.\n");
        return 0;
    }
    enerji -= 15;
    oksijen -= 10;
    printf("  Kesfe ciktin: Enerji -15, Oksijen -10\n");

    /* Su bulma esigi: enerji ve oksijen durumuna gore */
    if (enerji > 40 && oksijen > 30)
        esik = 55 + bulmaBonus;
    else
        esik = 25 + bulmaBonus;

    zar = rand() % 100;
    zarYaz(zar, esik);

    /* En az 3 sonuc: su bulma, yedek parca bulma, eli bos donme */
    if (zar < esik) {
        bulunan = 2 + rand() % 3;        /* 2-4 birim su */
        su += bulunan;
        printf("  SONUC: %d birim su buldun!\n", bulunan);
    } else if (zar < esik + 20) {
        parca += 1;
        printf("  SONUC: Bir yedek parca buldun!\n");
    } else {
        printf("  SONUC: Eli bos dondun.\n");
    }

    /* Yaralanma kontrolu */
    zarYara = rand() % 100;
    esikYara = 10;
    if (zarYara < esikYara || modulHasari > 70) {
        saglik -= 10;
        printf("  Zar: %d / Esik: %d -> YARALANDIN! Saglik -10\n", zarYara, esikYara);
    } else {
        printf("  Zar: %d / Esik: %d -> Yaralanmadin.\n", zarYara, esikYara);
    }
    return 1;
}

/* O: Oksijen uretimi (2 su -> 3 oksijen) */
int oksijenUret(void)
{
    int miktar, artik, kullanilabilir, cift, maksCift;

    if (enerji < 10) {
        printf("  Oksijen uretimi icin enerji yetersiz (gerekli: 10). Komut iptal edildi.\n");
        return 0;
    }
    if (su < 2) {
        printf("  En az 2 birim su gerekli. Komut iptal edildi.\n");
        return 0;
    }
    printf("  Mevcut su: %d. Kac birim su harcanacak? ", su);
    if (scanf("%d", &miktar) != 1) {
        tamponTemizle();
        printf("  Gecersiz giris. Komut iptal edildi.\n");
        return 0;
    }
    tamponTemizle();

    if (miktar < 2 || miktar > su) {
        printf("  Gecersiz miktar (2 - %d arasi olmali). Komut iptal edildi.\n", su);
        return 0;
    }

    artik = miktar % 2;                  /* tek ise 1 birim artik kalir */
    kullanilabilir = miktar - artik;
    cift = kullanilabilir / 2;           /* kac adet 2'li su cifti */

    /* Oksijen 100'u asmasin */
    maksCift = (100 - oksijen) / 3;
    if (cift > maksCift) {
        cift = maksCift;
        printf("  Oksijen deposu dolacagi icin uretim sinirlandi.\n");
    }
    if (cift == 0) {
        printf("  Oksijen deposu dolu, uretim yapilamadi. Komut iptal edildi.\n");
        return 0;
    }

    su -= cift * 2;
    oksijen += cift * 3;
    enerji -= 10;

    if (artik == 1)
        printf("  Girdiginiz miktar tek: 1 birim su artik olarak depoda kaldi.\n");
    else
        printf("  Girdiginiz miktar cift: tamami donusturuldu.\n");
    printf("  %d su harcandi, %d oksijen uretildi, Enerji -10\n", cift * 2, cift * 3);
    return 1;
}

/* T: Tamir (hasar kademesine gore) */
int tamir(void)
{
    int gerekliParca, onarim;

    if (parca < 1) {
        printf("  Yedek parca yok, tamir yapilamaz. Komut iptal edildi.\n");
        return 0;
    }
    if (enerji < 10) {
        printf("  Tamir icin enerji yetersiz (gerekli: 10). Komut iptal edildi.\n");
        return 0;
    }
    if (modulHasari == 0) {
        printf("  Modul hasari yok, tamire gerek yok. Komut iptal edildi.\n");
        return 0;
    }

    if (modulHasari <= 30) {             /* hafif */
        gerekliParca = 1;
        onarim = 15;
        printf("  Hasar seviyesi: HAFIF\n");
    } else if (modulHasari <= 70) {      /* orta */
        gerekliParca = 2;
        onarim = 30;
        printf("  Hasar seviyesi: ORTA\n");
    } else {                             /* agir */
        gerekliParca = 3;
        onarim = 50;
        printf("  Hasar seviyesi: AGIR\n");
    }

    if (parca < gerekliParca) {
        printf("  Bu kademe icin %d parca gerekli, sende %d var. Komut iptal edildi.\n",
               gerekliParca, parca);
        return 0;
    }

    parca -= gerekliParca;
    modulHasari -= onarim;
    enerji -= 10;
    printf("  %d parca harcandi, hasar -%d, Enerji -10\n", gerekliParca, onarim);
    return 1;
}

/* D: Durum raporu (tur harcamaz) */
void durumRaporu(void)
{
    printf("\n===== DURUM RAPORU =====\n");
    printf("  Gun: %d / 15   Tur: %d\n", gun, tur);
    printf("  Saglik: %d   Enerji: %d   Oksijen: %d\n", saglik, enerji, oksijen);
    printf("  Su: %d   Yedek parca: %d   Modul hasari: %d\n", su, parca, modulHasari);
    printf("  Hava kilidi: %s\n", kilitMuhurlu == 1 ? "MUHURLU" : "muhursuz");

    if (oksijen < 25)     printf("  [UYARI] Oksijen kritik seviyede!\n");
    if (saglik < 30)      printf("  [UYARI] Saglik kritik seviyede!\n");
    if (modulHasari > 70) printf("  [UYARI] Modul hasari kritik seviyede!\n");
    if (enerji < 20)      printf("  [UYARI] Enerji kritik seviyede!\n");

    if (oksijen >= 25 && saglik >= 30 && modulHasari <= 70 && enerji >= 20)
        printf("  TUM SISTEMLER NORMAL\n");
    printf("========================\n");
}

/* U: Uyu / dinlen */
int dinlen(void)
{
    int enerjiArtis, saglikArtis;

    enerjiArtis = 25 - modulHasari / 10;   /* hasar arttikca dinlenme verimi duser */
    enerji += enerjiArtis;

    if (oksijen < 20) {
        saglik -= 5;
        printf("  Oksijen cok dusuk! Dinlenirken saglik -5, Enerji +%d\n", enerjiArtis);
    } else {
        if (enerji > 50)
            saglikArtis = 8;
        else
            saglikArtis = 12;
        saglik += saglikArtis;
        printf("  Dinlendin: Enerji +%d, Saglik +%d\n", enerjiArtis, saglikArtis);
    }
    return 1;
}

/* K: Kum firtinasi (FOR dongusu) */
int kumFirtinasi(void)
{
    int i, zar, carpan, hasarMiktari;
    int esikCan, esikOks, esikModul;
    int hasarliTur = 0, toplamSaglik = 0, toplamOksijen = 0;
    int onceSaglik = saglik, onceOksijen = oksijen;

    printf("  Kum firtinasi basladi! Sure: %d tur\n", firtinaTur);

    for (i = 1; i <= firtinaTur; i++) {
        zar = rand() % 100;
        carpan = (i % 2 == 0) ? 2 : 1;           /* cift turlarda hasar 2 kat */
        hasarMiktari = firtinaHasar * carpan;

        /* Oyuncunun durumuna bagli esikler: hasar arttikca daha kotu */
        esikCan   = 20 + modulHasari / 10;
        esikOks   = esikCan + 25;
        esikModul = esikOks + 25;

        printf("  Tur %d%s -> Zar: %d (Esikler: %d/%d/%d) ", i,
               carpan == 2 ? " [RUZGAR SIDDETLENDI]" : "", zar, esikCan, esikOks, esikModul);

        if (zar < esikCan) {
            saglik -= hasarMiktari;
            hasarliTur++;
            printf("-> Yaralandin, Saglik -%d\n", hasarMiktari);
        } else if (zar < esikOks) {
            if (kilitMuhurlu == 1) {
                printf("-> Muhurlu kilit sayesinde oksijen sizintisi engellendi.\n");
                continue;                        /* sizinti olayi bu tur atlanir */
            }
            oksijen -= hasarMiktari;
            hasarliTur++;
            printf("-> Oksijen sizintisi, Oksijen -%d\n", hasarMiktari);
        } else if (zar < esikModul) {
            modulHasari += hasarMiktari;
            hasarliTur++;
            printf("-> Modul hasar aldi, Hasar +%d\n", hasarMiktari);
        } else {
            printf("-> Firtinadan zarar gormeden korundun.\n");
        }

        sinirla();
        if (saglik <= 0 || oksijen <= 0) {
            printf("  Durum kritik! Firtina sirasinda hayati deger sifirlandi.\n");
            break;
        }
    }

    toplamSaglik = onceSaglik - saglik;
    toplamOksijen = onceOksijen - oksijen;
    printf("  --- Firtina Ozeti ---\n");
    printf("  Hasar alinan tur: %d, Kaybedilen saglik: %d, Kaybedilen oksijen: %d\n",
           hasarliTur, toplamSaglik, toplamOksijen);
    return 1;
}

/* H: Hava kilidi sifresi (DO-WHILE) */
int havaKilidi(void)
{
    char sifre, tahmin;
    int deneme = 0;

    if (kilitMuhurlu == 1) {
        printf("  Hava kilidi zaten muhurlu. Komut iptal edildi.\n");
        return 0;
    }
    if (enerji < 5) {
        printf("  Enerji yetersiz (gerekli: 5). Komut iptal edildi.\n");
        return 0;
    }

    sifre = 'A' + (gun % 26);
    tahmin = 0;
    printf("  IPUCU: Sifre, alfabede A harfinden bugunun gun sayisi (%d) kadar ilerideki harftir.\n", gun);
    printf("  3 deneme hakkin var.\n");

    do {
        printf("  Deneme %d/3 - Harf gir: ", deneme + 1);
        if (scanf(" %c", &tahmin) != 1) {
            tahmin = 0;
        }
        tamponTemizle();

        if (tahmin >= 'a' && tahmin <= 'z')
            tahmin = tahmin - 'a' + 'A';     /* kucuk harfi buyuge cevir */

        deneme++;
        if (tahmin != sifre) {
            enerji -= 5;
            if (tahmin < 'A' || tahmin > 'Z')
                printf("  Gecersiz karakter! (hak sayilir) Enerji -5\n");
            else if (tahmin < sifre)
                printf("  Yanlis! Ileri git. Enerji -5\n");
            else
                printf("  Yanlis! Geri git. Enerji -5\n");
        }
    } while (tahmin != sifre && deneme < 3);

    if (tahmin == sifre) {
        kilitMuhurlu = 1;
        printf("  DOGRU! Hava kilidi muhurlendi.\n");
    } else {
        modulHasari += 20;
        printf("  3 deneme basarisiz! Modul hasari +20\n");
    }
    return 1;
}

/* ---------- ZORLUK SECIMI (SWITCH-CASE) ---------- */
void zorlukSec(void)
{
    char secim;
    int gecerli;

    do {
        gecerli = 1;
        printf("Zorluk seviyesi secin (K: Kolay, N: Normal, Z: Zor): ");
        if (scanf(" %c", &secim) != 1) {
            secim = '?';
        }
        tamponTemizle();

        switch (secim) {
        case 'k': case 'K':
            su = 10; parca = 3; modulHasari = 0;
            firtinaTur = 3; firtinaHasar = 5; bulmaBonus = 10;
            printf("Kolay seviye secildi.\n");
            break;
        case 'n': case 'N':
            su = 6; parca = 2; modulHasari = 10;
            firtinaTur = 5; firtinaHasar = 8; bulmaBonus = 0;
            printf("Normal seviye secildi.\n");
            break;
        case 'z': case 'Z':
            su = 3; parca = 1; modulHasari = 20;
            firtinaTur = 7; firtinaHasar = 12; bulmaBonus = -10;
            printf("Zor seviye secildi.\n");
            break;
        default:
            printf("Gecersiz seviye! Lutfen K, N veya Z girin.\n");
            gecerli = 0;
        }
    } while (gecerli == 0);
}

/* ---------- ANA PROGRAM ---------- */
int main(void)
{
    char komut;
    int turHarcandi;
    int cikis = 0, kazandi = 0, kaybetti = 0;
    int hayattaGun, puan;

    srand((unsigned int)time(NULL));

    saglik = 100;
    enerji = 100;
    oksijen = 100;

    printf("=====================================\n");
    printf("  MARS KOLONISI YASAM DESTEK SIMULATORU\n");
    printf("=====================================\n");
    printf("Hedef: Kurtarma ekibi gelene kadar 15 gun hayatta kal.\n");
    printf("Komutlar: G:Kesif O:Oksijen T:Tamir D:Durum U:Uyu K:Kum firtinasi H:Hava kilidi Q:Cikis\n\n");

    zorlukSec();

    do {
        durumSatiri();
        printf("Komut: ");
        if (scanf(" %c", &komut) != 1) {
            komut = 'Q';                     /* girdi bittiyse cikis */
        }
        tamponTemizle();

        turHarcandi = 0;

        switch (komut) {
        case 'g': case 'G':
            turHarcandi = kesif();
            break;
        case 'o': case 'O':
            turHarcandi = oksijenUret();
            break;
        case 't': case 'T':
            turHarcandi = tamir();
            break;
        case 'd': case 'D':
            durumRaporu();                   /* tur harcamaz */
            break;
        case 'u': case 'U':
            turHarcandi = dinlen();
            break;
        case 'k': case 'K':
            turHarcandi = kumFirtinasi();
            break;
        case 'h': case 'H':
            turHarcandi = havaKilidi();
            break;
        case 'q': case 'Q':
            cikis = 1;
            break;
        default:
            printf("  Tanimsiz komut! (tur harcanmadi)\n");
        }

        sinirla();

        /* Zaman akisi */
        if (turHarcandi == 1) {
            tur++;
            if (tur % 3 == 0) {
                gun++;
                oksijen -= 8;
                printf("  >>> YENI GUN BASLADI: Gun %d. Oksijen -8", gun);
                if (modulHasari > 70) {
                    saglik -= 5;
                    printf(", modul hasari yuksek oldugu icin Saglik -5");
                }
                printf("\n");
            }
            sinirla();
        }

        /* Oyun sonu kontrolu (turun sonunda) */
        if (saglik <= 0 || oksijen <= 0) {
            kaybetti = 1;
        } else if (gun > 15) {
            kazandi = 1;
        }

    } while (cikis == 0 && kaybetti == 0 && kazandi == 0);

    /* ---------- SONUC RAPORU ---------- */
    printf("\n=====================================\n");
    if (kazandi == 1)
        printf("  TEBRIKLER! Kurtarma ekibi geldi, hayatta kaldin!\n");
    else if (kaybetti == 1)
        printf("  OYUN BITTI! Mars'ta hayatta kalamadin.\n");
    else
        printf("  Simulasyondan cikildi.\n");

    hayattaGun = (kazandi == 1) ? 15 : gun;
    puan = (hayattaGun * 100) + saglik + oksijen - modulHasari;

    printf("  Hayatta kalinan gun: %d\n", hayattaGun);
    printf("  Toplam tur: %d\n", tur);
    printf("  Saglik: %d  Enerji: %d  Oksijen: %d\n", saglik, enerji, oksijen);
    printf("  Su: %d  Yedek parca: %d  Modul hasari: %d\n", su, parca, modulHasari);
    printf("  PUAN: %d\n", puan);
    printf("=====================================\n");

    return 0;
}
