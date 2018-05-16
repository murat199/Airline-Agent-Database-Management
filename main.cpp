#include <iostream>
#include <string>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include <locale.h>

MYSQL* dbConnect;
MYSQL_ROW row;
MYSQL_RES *res;
MYSQL_ROW row2;
MYSQL_RES *res2;
MYSQL_ROW row3;
MYSQL_RES *res3;
MYSQL_ROW row4;
MYSQL_RES *res4;
MYSQL_ROW row5;
MYSQL_RES *res5;
MYSQL_ROW row6;
MYSQL_RES *res6;
MYSQL_ROW row7;
MYSQL_RES *res7;


bool MysqlInit();
void MysqlConnect();
void MainApp();
void DbInsertRecord();
void DbGetUserRecord();
void ClearConsole();

char *SanitilizerStr(char[]);
char *SanitilizerInsert(char[]);

void YolcuVarMi(char ukn[],char tckn[],char baslangic[],char bitis[],char tarih[],int i);
void YolcuEkle();
void YolcuEkle2();
bool IkiSehirArasiSefer(char bn[],char bin[], char tarih[]);
int KacYolcuVar(char ukn[],int bilet_sayisi,char tarih[],char bn[],char bin[]);
int YerVarmi(char ukn1[],char ukn2[]);

void DbInsertSefer();
void YolcuAktar(int id);
bool DirekUcus();
bool TekAktarma(char bn[],char bin[],char tarih[],int bilet_sayisi);
void CiftAktarma(char bn[],char bin[],char tarih[],int bilet_sayisi);
void DbBiletAl();
void DbBiletAl2();
void DbYolcuSorgula();
void DbUcusIptal();
void DbBiletIptal();
void DbSeferYolcuListele();
void DbDolulukEnFazla();

int sno_index=0;
int ucus_sayac=0;
char ucus_harf[]="0";
char ucus_liste_dizi[50][150];
int ucus_dizi[50];
int global_sayac=0;
int yolcu_sayac=0;
int yolcu_id[50][3];
char dizi_ukn[50][50],dizi_bn[50][50],dizi_bin[50][50],dizi_tarih[50][10],dizi_saat[50][6],dizi_aktarma[50][15];

void UcusListeEkle(char index[],char ukn[],char bn[],char bin[],char tarih[],char saat[],char aktarma[]){
    strcpy(dizi_ukn[global_sayac-1],ukn);
    strcpy(dizi_bn[global_sayac-1],bn);
    strcpy(dizi_bin[global_sayac-1],bin);
    strcpy(dizi_tarih[global_sayac-1],tarih);
    strcpy(dizi_saat[global_sayac-1],saat);
    strcpy(dizi_aktarma[global_sayac-1],aktarma);
}

void TabloYolcuGuncelle(char bn[]){
    int queryState;
    char query[100];
    strcpy(query,"update yolcu set bn=\"");
    strcat(query,bn);
    strcat(query,"\" where bn is null;");
    queryState=mysql_query(dbConnect,query);
    if(queryState){
        printf("Hata: %s\n",mysql_error(dbConnect));
    }
}

void SeferNoIndex(){
    int queryState;
    char query[100];
    strcpy(query,"select seferno from yolcu order by seferno desc");
    queryState=mysql_query(dbConnect,query);
    if(!queryState){
        res7=mysql_store_result(dbConnect);
        if((row7=mysql_fetch_row(res7))){
            sno_index=atoi(row7[0]);
            sno_index++;
        }
    }
    else{
        printf("Hata: %s\n",mysql_error(dbConnect));
    }
}

void UcusAktarmaSil(char ukn[], char bn[], char bin[], char tarih[]){
    int queryState;
    char query[250];
    char seferno[3];
    strcpy(query,"select * from yolcu where seferno in( select seferno from yolcu where ukn=\"");
    strcat(query,ukn);
    strcat(query,"\" and bn=\"");
    strcat(query,bn);
    strcat(query,"\" and bin=\"");
    strcat(query,bin);
    strcat(query,"\" and tarih=\"");
    strcat(query,tarih);
    strcat(query,"\" and seferno in (select seferno from yolcu));");
    queryState=mysql_query(dbConnect,query);
    if(!queryState){
        res2=mysql_store_result(dbConnect);
        while((row2=mysql_fetch_row(res2))){
            strcpy(query,"delete from yolcu where seferno=\"");
            strcat(query,row2[9]);
            strcat(query,"\";");
            queryState=mysql_query(dbConnect,query);
        }
    }
    else{
        printf("Hata: %s\n",mysql_error(dbConnect));
    }
}

int main(){
    setlocale(LC_ALL,"Turkish");
    if(MysqlInit()){
        MysqlConnect();
        if(dbConnect)
        {
            SeferNoIndex();
            MainApp();
        }
        else
        {
            printf("Mysql Error : %s\n",mysql_error(dbConnect));
        }
        mysql_close(dbConnect);
    }
    return 0;
}

bool MysqlInit(){
    dbConnect = mysql_init(0);
    if(dbConnect)
    {
        printf("Veritabanina baglanildi. Baglanti = %d\n",dbConnect);
        return true;
    }
    else
    {
        printf("Varitabani baglanti problemi: %s\n",mysql_error(dbConnect));
        return false;
    }
}

void MysqlConnect(){
    dbConnect = mysql_real_connect(dbConnect,"localhost","root","1234","testdb7",0,NULL,0);
}

void MainApp(){
    char *select_menu=(char *)malloc(1);
    Menu:
    printf("\n\n******************************************************\n");
    printf("| Yeni sefer tanimlamak icin                          :1 |\n");
    printf("--------------------------------\n");
    printf("| Bilet satin almak icin                              :2 |\n");
    printf("--------------------------------\n");
    printf("| Yolcu sorgulamak icin                               :3 |\n");
    printf("--------------------------------\n");
    printf("| Ucus iptal etmek icin                               :4 |\n");
    printf("--------------------------------\n");
    printf("| Bilet iptal etmek icin                              :5 |\n");
    printf("--------------------------------\n");
    printf("| Sefer yolcu listelemek icin                         :6 |\n");
    printf("--------------------------------\n");
    printf("| Doluluk orani fazla olan ucuslari listelemek icin   :7 |\n");
    printf("--------------------------------\n");
    printf("| Cikis yapmak icin                                   :8 |\n");
    printf("**********************************************************\n\n");
    printf("Seciminiz : ");
    scanf("%s",select_menu);
    if(strlen(select_menu)>1){
        select_menu[0]='0';
    }
    if(select_menu[0]=='1'){
        ClearConsole();
        DbInsertSefer();
        goto Menu;
    }
    else if(select_menu[0]=='2'){
        ClearConsole();
        DbBiletAl2();
        //goto Menu;
    }
    else if(select_menu[0]=='3'){
        ClearConsole();
        DbYolcuSorgula();
        goto Menu;
    }
    else if(select_menu[0]=='4'){
        ClearConsole();
        DbUcusIptal();
        goto Menu;
    }
    else if(select_menu[0]=='5'){
        ClearConsole();
        DbBiletIptal();
        goto Menu;
    }
    else if(select_menu[0]=='6'){
        ClearConsole();
        DbSeferYolcuListele();
        goto Menu;
    }
    else if(select_menu[0]=='7'){
        ClearConsole();
        DbDolulukEnFazla();
        goto Menu;
    }
    else if(select_menu[0]=='8'){
        printf("\n\nCIKIS YAPILDI.\n\n");
        exit(0);
    }
    else{
        ClearConsole();
        printf("Yanlis secim!\n");
        goto Menu;
    }
}

void DbInsertRecord(){
    int queryState;
    char *query = "insert into users(name) values(\"codeblocks1\")";
    queryState = mysql_query(dbConnect,query);
    if(!queryState)
    {
        printf("Record insert success...\n");
    }
    else
    {
        printf("query problem: %s\n",mysql_error(dbConnect));
    }
}

void DbGetUserRecord(){
    int queryState;
    queryState = mysql_query(dbConnect,"select * from users");
    if(!queryState)
    {
        res = mysql_store_result(dbConnect);
        while(row=mysql_fetch_row(res))
        {
            printf("id: %s name: %s\n",row[0],row[1]);
        }
    }
    else
    {
        printf("Mysql error : %s\n",mysql_error(dbConnect));
    }
}

void ClearConsole(){
    system("CLS");
}

char *SanitilizerStr(char *sanStr){
    for(int i=0; i<strlen(sanStr);i++){
        if(!(sanStr[i]=='S' && sanStr[i+1]=='e' && sanStr[i+2]=='l' && sanStr[i+3]=='e' && sanStr[i+4]=='c' && sanStr[i+5]=='t')){
            sanStr[i]=' ';
        }
        else{
            break;
        }
    }
    return sanStr;
}

char *SanitilizerInsert(char *sanStr){
    for(int i=0; i<strlen(sanStr);i++){
        if(!(sanStr[i]=='I' && sanStr[i+1]=='n' && sanStr[i+2]=='s' && sanStr[i+3]=='e' && sanStr[i+4]=='r' && sanStr[i+5]=='t')){
            sanStr[i]=' ';
        }
        else{
            break;
        }
    }
    return sanStr;
}

void DbInsertSefer(){
    int queryState;
    char ukn[10];
    char bn[50];
    char bin[50];
    char tarih[9];
    char saat[5];
    char query[150];
    bool ukn_sayi=false;
    bool ukn_harf=false;

    InsertSefer:
    ukn_sayi=false;
    ukn_harf=false;
    printf("------------------------\n");
    printf("| YENI SEFER TANIMLAMA |\n");
    printf("------------------------\n");
    printf("Ucak kuyruk no (harf - rakam): ");
    scanf("%s",ukn);
    ukn[9]='\0';

    for(int i=0;i<strlen(ukn);i++){
        if(isalpha(ukn[i])){
            ukn_harf=true;
        }
        if(isdigit(ukn[i])){
            ukn_sayi=true;
        }
    }
    if(!(ukn_sayi && ukn_harf)){
        printf("---Harf ve rakamlardan olusmali!\n");
        printf("---Maksimum 9 eleman olmali!\n");
        goto InsertSefer;
    }
    strcat(query,"Select * from sefer where ukn=\"");
    strcat(query,ukn);
    strcat(query,"\";");
    strcpy(query,SanitilizerStr(query));
    queryState=mysql_query(dbConnect,query);
    if(!queryState){
        res=mysql_store_result(dbConnect);
        if(row=mysql_fetch_row(res)){
            printf("Baslangic noktasi: ");
            scanf("%s",bn);
            bn[49]='\0';
            printf("Bitis noktasi: ");
            scanf("%s",bin);
            bin[49]='\0';
            printf("Tarih: ");
            scanf("%s",tarih);
            tarih[8]='\0';
            printf("Saat: ");
            scanf("%s",saat);
            saat[4]='\0';
            strcpy(query,"Select * from sefer where ukn=\"");
            strcat(query,ukn);
            strcat(query,"\" and bn=\"");
            strcat(query,bn);
            strcat(query,"\" and bin=\"");
            strcat(query,bin);
            strcat(query,"\" and tarih=\"");
            strcat(query,tarih);
            strcat(query,"\" and saat=\"");
            strcat(query,saat);
            strcat(query,"\"");
            strcpy(query,SanitilizerStr(query));

            queryState = mysql_query(dbConnect,query);
            if(!queryState){
                res=mysql_store_result(dbConnect);
                if(row=mysql_fetch_row(res)){
                    printf("\n\n---Girilen degerlerde uyusan birebir sefer bulunmaktadir!\n");
                    printf("---Lutfen farkli degerler girin!\n");
                }
                else{
                    strcpy(query,"Insert into sefer(ukn,bn,bin,tarih,saat) values('");
                    strcat(query,ukn);
                    strcat(query,"','");
                    strcat(query,bn);
                    strcat(query,"','");
                    strcat(query,bin);
                    strcat(query,"','");
                    strcat(query,tarih);
                    strcat(query,"','");
                    strcat(query,saat);
                    strcat(query,"')");
                    strcpy(query,SanitilizerInsert(query));
                    queryState=mysql_query(dbConnect,query);
                    if(!queryState){;
                        printf("\nSefer Bilgileri\n");
                        printf("*******************************\n");
                        printf(" ucak no             :%s\n",ukn);
                        printf(" baslangic noktasi   :%s\n",bn);
                        printf(" bitis noktasi       :%s\n",bin);
                        printf(" tarih               :%s\n",tarih);
                        printf(" saat                :%s\n",saat);
                        printf("*******************************\n");
                        printf("Sefer basariyla eklendi...\n",query);
                    }
                    else{
                        printf("Hata : %s\n",mysql_error(dbConnect));
                    }
                }
            }
            else{
                printf("Hata: %s\n",mysql_error(dbConnect));
            }
        }
        else{
            printf("%s ucak kuyruk numarali ucak bulunamadi!\n\n",ukn);
        }
    }
    else{
        printf("Hata :%s\n",mysql_error(dbConnect));
    }
    MainApp();
}

void YolcuVarMi(char ukn[],char tckn[],char baslangic[],char bitis[],char tarih[],int i){
    int queryState;
    char query[350];
    char isim[50];
    char adres[50];
    char sehir[50];
    char eposta[50];
    bool isSuccess=false;
    strcpy(query,"Select * from yolcu where tckn=\"");
    strcat(query,tckn);
    strcat(query,"\";");
    strcpy(query,SanitilizerStr(query));

    queryState=mysql_query(dbConnect,query);
    if(!queryState){
        res3=mysql_store_result(dbConnect);
        if(row3=mysql_fetch_row(res3)){
            strcpy(isim,row3[1]);
            strcpy(adres,row3[2]);
            strcpy(sehir,row3[3]);
            strcpy(eposta,row3[4]);
            isSuccess=true;

        }
        else{
            printf("%d. yolcu isim : ",(i+1));
            scanf("%s",isim);
            isim[49]='\0';
            printf("%d. yolcu adres : ",(i+1));
            scanf("%s",adres);
            adres[49]='\0';
            printf("%d. yolcu sehir : ",(i+1));
            scanf("%s",sehir);
            sehir[49]='\0';
            printf("%d. yolcu eposta : ",(i+1));
            scanf("%s",eposta);
            eposta[49]='\0';
            isSuccess=true;
        }
    }
    else{
        printf("Hata: %s\n",mysql_error(dbConnect));
    }
    if(isSuccess){
        char sayisal[5];
        itoa(sno_index,sayisal,10);

        strcpy(query,"Insert into yolcu(tckn,isim,adres,sehir,eposta,ukn,bn,bin,tarih,seferno) values(\"");
        strcat(query,tckn);
        strcat(query,"\",\"");
        strcat(query,isim);
        strcat(query,"\",\"");
        strcat(query,adres);
        strcat(query,"\",\"");
        strcat(query,sehir);
        strcat(query,"\",\"");
        strcat(query,eposta);
        strcat(query,"\",\"");
        strcat(query,ukn);
        strcat(query,"\",\"");
        strcat(query,baslangic);
        strcat(query,"\",\"");
        strcat(query,bitis);
        strcat(query,"\",\"");
        strcat(query,tarih);
        strcat(query,"\",\"");
        strcat(query,sayisal);
        strcat(query,"\");");
        strcpy(query,SanitilizerInsert(query));
        queryState=mysql_query(dbConnect,query);
        if(!queryState){
            printf("Kisi eklendi...\n");
        }
        else{
            printf("Hata: %s\n",mysql_error(dbConnect));
        }
    }
}

void YolcuEkle(char ukn1[],char ukn2[],char baslangic[],char aktarma[],char bitis[],char tarih1[],char tarih2[],int yolcu_sayisi){
    char tckn[12];
    for(int i=0; i<yolcu_sayisi; i++){
        printf("%d. yolcu tckn : ",(i+1));
        scanf("%s",tckn);
        tckn[11]='\0';
        YolcuVarMi(ukn1,tckn,baslangic,aktarma,tarih1,i);
        YolcuVarMi(ukn2,tckn,aktarma,bitis,tarih2,i);
    }
}

void YolcuEkle2(char ukn1[],char ukn2[],char ukn3[],char baslangic[],char aktarma1[],char aktarma2[],char bitis[],char tarih1[],char tarih2[],char tarih3[],int yolcu_sayisi){
    char tckn[12];
    for(int i=0; i<yolcu_sayisi; i++){
        printf("%d. yolcu tckn : ",(i+1));
        scanf("%s",tckn);
        tckn[11]='\0';
        YolcuVarMi(ukn1,tckn,baslangic,aktarma1,tarih1,i);
        YolcuVarMi(ukn2,tckn,aktarma1,aktarma2,tarih2,i);
        YolcuVarMi(ukn3,tckn,aktarma2,bitis,tarih3,i);
    }
}

bool IkiSehirArasiSefer(char bn[],char bin[], char tarih[]){
    int queryState;
    char query[200];

    strcpy(query,"Select * from sefer where bn=\"");
    strcat(query,bn);
    strcat(query,"\" and bin=\"");
    strcat(query,bin);
    strcat(query,"\" and tarih>=\"");
    strcat(query,tarih);
    strcat(query,"\";");
    strcpy(query,SanitilizerStr(query));
    queryState=mysql_query(dbConnect,query);
    if(!queryState){
        res3=mysql_store_result(dbConnect);
        if(row3=mysql_fetch_row(res3)){
                return true;
        }
        else{
            return false;
        }
    }
    else{
        printf("Hata: %s\n",mysql_error(dbConnect));
    }
}

bool YerVarmi(char ukn[],int bilet_sayisi){
    int kapasite;
    int kayit_sayisi;
    int queryState;
    char query[150];
    strcpy(query,"Select kapasite from ucak where ukn=\"");
    strcat(query,ukn);
    strcat(query,"\";");
    strcpy(query,SanitilizerStr(query));
    queryState=mysql_query(dbConnect,query);
    if(!queryState){
        res2=mysql_store_result(dbConnect);
        if(row2=mysql_fetch_row(res2)){
            kapasite=atoi(row2[0]);
            strcpy(query,"Select count(*) from yolcu where ukn=\"");
            strcat(query,ukn);
            strcat(query,"\";");
            queryState=mysql_query(dbConnect,query);
            if(!queryState){
                res3=mysql_store_result(dbConnect);
                if(row3=mysql_fetch_row(res3)){
                    kayit_sayisi=atoi(row3[0]);
                    if(kapasite-kayit_sayisi-bilet_sayisi>=0){
                        return true;
                    }
                    else{
                        return false;
                    }
                }
            }
        }
    }
    else{
        printf("Hata : ",mysql_error(dbConnect));
        kapasite=-1;
        return false;
    }
    return false;
}

void DirekUcusListele(char baslangic[],char bitis[],char tarih[],int bilet_sayisi){
    char query[150];
    int queryState;
    bool hasRecord=false;
    strcpy(query,"Select * from sefer where bn=\"");
    strcat(query,baslangic);
    strcat(query,"\" and bin=\"");
    strcat(query,bitis);
    strcat(query,"\" and tarih>=\"");
    strcat(query,tarih);
    strcat(query,"\";");
    queryState=mysql_query(dbConnect,query);
    if(!queryState){
        res=mysql_store_result(dbConnect);
        printf("\n\nDIREK UCUS LISTESI\n");
        while(row=mysql_fetch_row(res)){
            if(YerVarmi(row[0],bilet_sayisi)){
                hasRecord=true;
                ucus_sayac++;
                ucus_harf[0]++;
                ucus_dizi[global_sayac]=ucus_sayac;
                global_sayac++;
                printf("--------------------------------\n");
                printf("        SECIM ICIN NO : %d\n",ucus_sayac);
                printf("| ukn       : %15s |\n",row[0]);
                printf("| Baslangic : %15s |\n",row[1]);
                printf("| Bitis     : %15s |\n",row[2]);
                printf("| Tarih     : %15s |\n",row[3]);
                printf("| Saat      : %15s |\n",row[4]);
                printf("--------------------------------\n\n");
                UcusListeEkle(ucus_harf,row[0],row[1],row[2],row[3],row[4],"direk");
            }
        }
    }
    else{
        printf("Hata : %s\n",mysql_error(dbConnect));
    }
    if(!hasRecord){
        printf("--------------------------------\n");
        printf("Kriterlere uygun ucus bulunamadi!\n");
    }
}

void TekAktarmaListele(char baslangic[],char bitis[],char tarih[],int bilet_sayisi){
    char query[250];
    int queryState;
    bool hasRecord=false;
    strcpy(query,"select * from sefer s1, sefer s2 where s1.tarih>=");
    strcat(query,tarih);
    strcat(query," and s1.bn=\"");
    strcat(query,baslangic);
    strcat(query,"\" and s1.bin=s2.bn and s2.bin=\"");
    strcat(query,bitis);
    strcat(query,"\" and s1.bn!=s2.bin and s1.tarih<=s2.tarih;");
    queryState=mysql_query(dbConnect,query);
    if(!queryState){
        res=mysql_store_result(dbConnect);
        printf("\n\nTEK AKTARMA UCUS LISTESI\n");
        while(row=mysql_fetch_row(res)){
            if(YerVarmi(row[0],bilet_sayisi) && YerVarmi(row[5],bilet_sayisi)){
                hasRecord=true;
                ucus_sayac++;
                ucus_harf[0]++;
                printf("--------------------------------\n");
                printf("        SECIM ICIN NO : %d\n",ucus_sayac);
                printf("| ukn       : %15s |\n",row[0]);
                printf("| Baslangic : %15s |\n",row[1]);
                printf("| Aktarma   : %15s |\n",row[2]);
                printf("| Tarih     : %15s |\n",row[3]);
                printf("| Saat      : %15s |\n",row[4]);
                printf("| ukn       : %15s |\n",row[5]);
                printf("| Aktarma   : %15s |\n",row[6]);
                printf("| Bitis     : %15s |\n",row[7]);
                printf("| Tarih     : %15s |\n",row[8]);
                printf("| Saat      : %15s |\n",row[9]);
                printf("--------------------------------\n\n");
                ucus_dizi[global_sayac]=ucus_sayac;
                global_sayac++;
                UcusListeEkle(ucus_harf,row[0],row[1],row[2],row[3],row[4],"tekaktarma");
                ucus_dizi[global_sayac]=ucus_sayac;
                global_sayac++;
                UcusListeEkle(ucus_harf,row[5],row[6],row[7],row[8],row[9],"tekaktarma");
            }

        }
    }
    else{
        printf("Hata : %s\n",mysql_error(dbConnect));
    }
    if(!hasRecord){
        printf("--------------------------------\n");
        printf("Kriterlere uygun ucus bulunamadi!\n");
    }
}

void CiftAktarmaListele(char baslangic[],char bitis[],char tarih[],int bilet_sayisi){
    char query[250];
    int queryState;
    bool hasRecord=false;
    strcpy(query,"select * from sefer s1, sefer s2, sefer s3 where s1.tarih>=");
    strcat(query,tarih);
    strcat(query," and s1.bn=\"");
    strcat(query,baslangic);
    strcat(query,"\"  and s1.bin=s2.bn and s2.bin=s3.bn and s3.bin=\"");
    strcat(query,bitis);
    strcat(query,"\" and s1.bn!=s2.bin and s2.bn!=s3.bin and s1.tarih<=s2.tarih and s2.tarih<=s3.tarih;");
    queryState=mysql_query(dbConnect,query);

    if(!queryState){
        res=mysql_store_result(dbConnect);
        printf("\n\nCIFT AKTARMA UCUS LISTESI\n");
        while(row=mysql_fetch_row(res)){
            if(YerVarmi(row[0],bilet_sayisi) && YerVarmi(row[5],bilet_sayisi) && YerVarmi(row[10],bilet_sayisi)){
                hasRecord=true;
                ucus_sayac++;
                ucus_harf[0]++;
                ucus_dizi[global_sayac]=ucus_sayac;
                global_sayac++;
                printf("--------------------------------\n");
                printf("        SECIM ICIN NO : %d\n",ucus_sayac);
                printf("| ukn       : %15s |\n",row[0]);
                printf("| Baslangic : %15s |\n",row[1]);
                printf("| Aktarma   : %15s |\n",row[2]);
                printf("| Tarih     : %15s |\n",row[3]);
                printf("| Saat      : %15s |\n",row[4]);
                printf("| ukn       : %15s |\n",row[5]);
                printf("| 1.Aktarma : %15s |\n",row[6]);
                printf("| Bitis     : %15s |\n",row[7]);
                printf("| Tarih     : %15s |\n",row[8]);
                printf("| Saat      : %15s |\n",row[9]);
                printf("| ukn       : %15s |\n",row[10]);
                printf("| 2.Aktarma : %15s |\n",row[11]);
                printf("| Bitis     : %15s |\n",row[12]);
                printf("| Tarih     : %15s |\n",row[13]);
                printf("| Saat      : %15s |\n",row[14]);
                printf("--------------------------------\n\n");
                UcusListeEkle(ucus_harf,row[0],row[1],row[2],row[3],row[4],"ciftaktarma");
                ucus_dizi[global_sayac]=ucus_sayac;
                global_sayac++;
                UcusListeEkle(ucus_harf,row[5],row[6],row[7],row[8],row[9],"ciftaktarma");
                ucus_dizi[global_sayac]=ucus_sayac;
                global_sayac++;
                UcusListeEkle(ucus_harf,row[10],row[11],row[12],row[13],row[14],"ciftaktarma");
            }
        }
    }
    else{
        printf("Hata : %s\n",mysql_error(dbConnect));
    }
    if(!hasRecord){
        printf("--------------------------------\n");
        printf("Kriterlere uygun ucus bulunamadi!\n");
    }
}

void YolcuAktar(int id,int bilet_sayisi){
    char tckn[12];
    for(int i=0; i<global_sayac;i++){
        if(ucus_dizi[i]==id){
            if(strcmp(dizi_aktarma[i],"direk")==0){
                for(int j=0;j<bilet_sayisi;j++){
                    printf("%d. yolcu tckn : ",(j+1));
                    scanf("%s",tckn);
                    tckn[11]='\0';
                    YolcuVarMi(dizi_ukn[i],tckn,dizi_bn[i],dizi_bin[i],dizi_tarih[i],j);
                    sno_index++;
                }
                break;
            }
            else if(strcmp(dizi_aktarma[i],"tekaktarma")==0){
                YolcuEkle(dizi_ukn[i],dizi_ukn[i+1],dizi_bn[i],dizi_bin[i],dizi_bin[i+1],dizi_tarih[i],dizi_tarih[i+1],bilet_sayisi);
                sno_index++;
                break;
            }
            else if(strcmp(dizi_aktarma[i],"ciftaktarma")==0){
                YolcuEkle2(dizi_ukn[i],dizi_ukn[i+1],dizi_ukn[i+2],dizi_bn[i],dizi_bin[i],dizi_bin[i+1],dizi_bin[i+2],dizi_tarih[i],dizi_tarih[i+1],dizi_tarih[i+2],bilet_sayisi);
                sno_index++;
                break;
            }
            yolcu_sayac++;
        }
    }
}

void DbBiletAl2(){
    int queryState;
    char query[150];
    char baslangic[50];
    char bitis[50];
    char tarih[9];
    int bilet_sayisi=0;
    printf("------------------\n");
    printf("| BILET SATIN AL |\n");
    printf("------------------\n");
    printf("Baslangic noktasi : ");
    scanf("%s",baslangic);
    baslangic[49]='\0';
    printf("Bitis noktasi : ");
    scanf("%s",bitis);
    baslangic[49]='\0';
    printf("Tarih : ");
    scanf("%s",tarih);
    baslangic[8]='\0';
    printf("Bilet sayisi : ");
    scanf("%d",&bilet_sayisi);
    if(bilet_sayisi==0){
        printf("Bilet sayisi alani icin lutfen sayi giriniz!");
    }
    global_sayac=0;
    ucus_sayac=0;
    DirekUcusListele(baslangic,bitis,tarih,bilet_sayisi);
    TekAktarmaListele(baslangic,bitis,tarih,bilet_sayisi);
    CiftAktarmaListele(baslangic,bitis,tarih,bilet_sayisi);
    if(ucus_sayac==0){
        MainApp();
    }
    else{
        if(ucus_sayac>0){
            Secim:
            int index_secim=0;
            printf("\n\nUcus seciminiz : ");
            scanf("%d",&index_secim);
            if(index_secim<0 || index_secim>ucus_sayac){
                goto Secim;
            }
            else{
                YolcuAktar(index_secim,bilet_sayisi);
            }
        }
        MainApp();
    }
}

void DbYolcuSorgula(){
    int queryState;
    char tckn[12];
    char query[100];
    bool hasRecord=false;
    int userCount=0;
    printf("------------------------\n");
    printf("| YOLCU SORGULA |\n");
    printf("------------------------\n");
    printf("Tc kimlik no: ");
    scanf("%s",tckn);
    tckn[11]='\0';
    strcat(query,"Select * FROM yolcu where tckn=\"");
    strcat(query,tckn);
    strcat(query,"\"");
    strcpy(query,SanitilizerStr(query));

    queryState=mysql_query(dbConnect,query);
    if(!queryState){
        res=mysql_store_result(dbConnect);
        while(row=mysql_fetch_row(res)){
            if(hasRecord==false){
                printf("\n            UCUS LISTESI\n");
            }
            hasRecord=true;
            printf(" -----------------------------------\n");
            printf(" | Tc No     : %20s |\n",row[0]);
            printf(" | Adi       : %20s |\n",row[1]);
            printf(" | Tarih     : %20s |\n",row[8]);
            printf(" | Baslangic : %20s |\n",row[6]);
            printf(" | Hedef     : %20s |\n",row[7]);
            printf(" | Ukn       : %20s |\n",row[5]);
            printf(" -----------------------------------\n\n");
            userCount++;
        }
        printf("Kisinin toplamda yaptigi ucus sayisi : %d\n\n",userCount);
    }
    else{
        printf("Hata: %s\n",mysql_error(dbConnect));
    }
    if(!hasRecord){
        printf("%s tc kimlik numarasina ait kayit bulunamadi!\n",tckn);
    }
    MainApp();
}

void DbUcusIptal(){
    int queryState;
    char ukn[50];
    char bn[50];
    char bin[50];
    char tarih[9];
    char query[100];
    bool hasRecord=false;
    printf("-------------------\n");
    printf("| UCUS IPTAL ETME |\n");
    printf("-------------------\n");
    printf("Ucak kuyruk numarasi: ");
    scanf("%s",ukn);
    printf("Baslangic noktasi: ");
    scanf("%s",bn);
    printf("Bitis noktasi: ");
    scanf("%s",bin);
    printf("Tarihi: ");
    scanf("%s",tarih);

    ukn[49]='\0';
    bn[49]='\0';
    bin[49]='\0';
    tarih[8]='\0';

    strcat(query,"Select * from sefer where ukn=\"");
    strcat(query,ukn);
    strcat(query,"\" and bn=\"");
    strcat(query,bn);
    strcat(query,"\" and bin=\"");
    strcat(query,bin);
    strcat(query,"\" and tarih=\"");
    strcat(query,tarih);
    strcat(query,"\";");
    strcpy(query,SanitilizerStr(query));
    queryState=mysql_query(dbConnect,query);
    if(!queryState){
        res=mysql_store_result(dbConnect);
        while(row=mysql_fetch_row(res)){
            hasRecord=true;
            UcusAktarmaSil(ukn,bn,bin,tarih);
            break;
        }
    }
    else{
        printf("Hata: %s\n",mysql_error(dbConnect));
    }
    if(hasRecord){
        strcpy(query,"delete from sefer where ukn=\"");
        strcat(query,ukn);
        strcat(query,"\" and bn=\"");
        strcat(query,bn);
        strcat(query,"\" and bin=\"");
        strcat(query,bin);
        strcat(query,"\" and tarih=\"");
        strcat(query,tarih);
        strcat(query,"\";");
        queryState=mysql_query(dbConnect,query);
        if(!queryState){
            TabloYolcuGuncelle(bn);
            printf("%s tarihli %s-%s arasi %s ucak kuyruk numarali sefer iptal edildi.",tarih,bn,bin,ukn);
        }
        else{
            printf("Hata: %s",mysql_error(dbConnect));
        }
    }
    else{
        printf("Sefer bulunamadi!");
    }
}

void DbBiletIptal(){
    int queryState;
    char tckn[12];
    char ukn[50];
    char tarih[9];
    char bn[5];
    char bin[5];
    char query[150];
    char recordsno[5];
    bool hasRecord=false;
    printf("--------------------\n");
    printf("| BILET IPTAL ETME |\n");
    printf("--------------------\n");
    printf("Tc kimlik numarasi: ");
    scanf("%s",tckn);
    printf("Ucak kuyruk no: ");
    scanf("%s",ukn);
    printf("Baslangic: ");
    scanf("%s",bn);
    printf("Bitis: ");
    scanf("%s",bin);
    printf("Tarihi: ");
    scanf("%s",tarih);

    tckn[11]='\0';
    ukn[49]='\0';
    tarih[8]='\0';
    bn[4]='\0';
    bin[4]='\0';

    strcat(query,"Select * from yolcu where tckn=\"");
    strcat(query,tckn);
    strcat(query,"\" and ukn=\"");
    strcat(query,ukn);
    strcat(query,"\" and tarih=\"");
    strcat(query,tarih);
    strcat(query,"\" and bn=\"");
    strcat(query,bn);
    strcat(query,"\" and bin=\"");
    strcat(query,bin);
    strcat(query,"\";");

    strcpy(query,SanitilizerStr(query));
    queryState=mysql_query(dbConnect,query);
    if(!queryState){
        res=mysql_store_result(dbConnect);
        while(row=mysql_fetch_row(res)){
            hasRecord=true;
            strcpy(recordsno,row[9]);
            break;
        }
    }
    else{
        printf("Hata: %s\n",mysql_error(dbConnect));
    }
    if(hasRecord){
        strcpy(query,"delete from yolcu where tckn=\"");
        strcat(query,tckn);
        strcat(query,"\" and ukn=\"");
        strcat(query,ukn);
        strcat(query,"\" and tarih=\"");
        strcat(query,tarih);
        strcat(query,"\";");
        queryState=mysql_query(dbConnect,query);
        strcpy(query,"delete from yolcu where seferno=\"");
        strcat(query,recordsno);
        strcat(query,"\";");


        mysql_query(dbConnect,query);
        if(!queryState){
            printf("-%s tarihinde\n-%s tc kimlik numarali\n-%s ucak kuyruk numarali ucakta bileti iptal edilmistir.",tarih,tckn,ukn);
        }
        else{
            printf("Hata: %s",mysql_error(dbConnect));
        }
    }
    else{
        printf("Bilet bulunamadi!");
    }
}

void DbSeferYolcuListele(){
    int queryState;
    char ukn[50];
    char tarih[9];
    char query[100];
    bool hasRecord=false;
    printf("------------------------\n");
    printf("| UCUS YOLCU LISTELEME |\n");
    printf("------------------------\n");
    printf("Ucak kuyruk no: ");
    scanf("%s",ukn);
    printf("Tarih: ");
    scanf("%s",tarih);
    ukn[49]='\0';
    tarih[8]='\0';
    strcat(query,"Select * FROM yolcu where ukn=\"");
    strcat(query,ukn);
    strcat(query,"\" and tarih=\"");
    strcat(query,tarih);
    strcat(query,"\"");
    strcpy(query,SanitilizerStr(query));

    queryState=mysql_query(dbConnect,query);
    if(!queryState){
        res=mysql_store_result(dbConnect);
        while(row=mysql_fetch_row(res)){
            if(hasRecord==false){
                printf("-------------\n");
                printf("YOLCU LISTESI\n");
                printf("-------------\n");
            }
            hasRecord=true;
            printf("| Tc nosu: %11s | Adi : %15s |\n",row[0],row[1]);
        }
    }
    else{
        printf("Hata: %s\n",mysql_error(dbConnect));
    }
    if(!hasRecord){
        printf("Herhangi bir yolcu bulunamadi!\n");
    }
    MainApp();
}

void DbDolulukEnFazla(){
    int queryState;
    char query[300];

    strcat(query,"Select *,Count(*) as Doluluk from yolcu group by ukn, tarih order by Doluluk desc;");
    strcpy(query,SanitilizerStr(query));
    queryState=mysql_query(dbConnect,query);
    if(!queryState){
        res=mysql_store_result(dbConnect);
        printf("DOLULUK ORANI EN FAZLA OLAN UCUSLAR\n");
        while(row=mysql_fetch_row(res)){
            printf("--------------------------------\n");
            printf("| ukn       : %15s |\n",row[5]);
            printf("| Baslangic : %15s |\n",row[6]);
            printf("| Bitis     : %15s |\n",row[7]);
            printf("| Tarih     : %15s |\n",row[8]);
            printf("| Doluluk   : %15s |\n",row[10]);
            printf("--------------------------------\n\n");
        }
    }
    else{
        printf("Hata: %s\n",mysql_error(dbConnect));
    }
    MainApp();
}
