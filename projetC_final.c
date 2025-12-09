#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define ID_EDIT   101
#define ID_BUTTON 102

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hwndEdit;
    switch (msg) {
        case WM_CREATE:
            hwndEdit = CreateWindowW(L"Edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER,50, 30, 100, 25, hwnd, (HMENU)ID_EDIT, NULL, NULL);
            break;
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_BUTTON) {
                int len = GetWindowTextLengthW(hwndEdit) + 1;
                wchar_t text[len];
                GetWindowTextW(hwndEdit, text, len);

                if (wcscmp(text, L"0") == 0) {
                    PostQuitMessage(0);
                }
                else if (wcscmp(text, L"1") == 0) {
                    PostQuitMessage(1);
                }
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int RunGui() {
    WNDCLASSW wc = {0};
    HWND hwnd;
    MSG msg;

    wc.lpszClassName = L"InputWindow";
    wc.hInstance = GetModuleHandle(NULL);
    wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wc.lpfnWndProc = WndProc;
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    RegisterClassW(&wc);

    hwnd = CreateWindowW(wc.lpszClassName, L"passer au jour suivant?", WS_OVERLAPPEDWINDOW,CW_USEDEFAULT, CW_USEDEFAULT, 350, 150, 0, 0, wc.hInstance, 0);

    CreateWindowW(L"Button", L"entrer 1 ou 0", WS_CHILD | WS_VISIBLE,160, 28, 150, 30, hwnd, (HMENU)ID_BUTTON, NULL, NULL);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

typedef struct{
    int jour,mois;
}Date;

typedef struct{
    int H,M;
}Time;

typedef struct noeud2 {
    int etat;//prend l'etat du temperature captée(alerte critique(3),modérée(2),ou simple(1) ou etat bien(0))
    Time t;//et le temps de capture de la temperature associée
    struct noeud2 *next; 
}noeud2;
typedef noeud2* listch2;

typedef struct noeud1{
    int etat;//prend l'etat du temperature captée(alerte critique(3),modérée(2),ou simple(1) ou etat bien(0))
    struct noeud1 *next;
}noeud1;
typedef noeud1* listch1;

float random_between(float min, float max){
    float s= rand() / (float) RAND_MAX;  // alea dans [0, 1]
    return min + s*(max - min);         // decaler a [min, max]
}

float T_min(float t[], int n) {
    float min = t[0]; // On suppose que le premier élément est le minimum
    for (int i = 1; i<n; i++) {
        if (t[i] < min) {
            min = t[i]; // Mise à jour du minimum si un élément plus petit est trouvé
        }
    }
    return min;
}

float T_max(float t[], int n) {
    float max = t[0]; // On suppose que le premier élément est le maximum
    for (int i = 1; i < n; i++) {
        if (t[i] > max) {
            max = t[i]; // Mise à jour du maximum si un élément plus grand est trouvé
        }
    }
    return max;
}

float T_calculmoy(float t[], int n) {
    float moy=0.0; 
    for (int i = 0; i < n; i++) {
        moy=moy+t[i]/n;
    }
    return moy;
}

void add_min(Time *t) {
    (*t).M += 30;         
    if ((*t).M >= 60) {   
        (*t).M -= 60;
        (*t).H += 1;
    }
}

int alerte(float T, float val_seuil, int *nb_c, int *nb_m, int *nb_s,char msg_f[]) {
    float k = abs(val_seuil - T);
    char tmp_str[500];
    if (k < 5) {
        snprintf(tmp_str, sizeof(tmp_str), "il y a un depassement leger du temperature seuil de %.2f C",k);
        strcat(msg_f, tmp_str);
        (*nb_s)++;   
        return 1;
    }else if (k>=5 && k<= 10) {
        snprintf(tmp_str, sizeof(tmp_str), "il y a un depassement important du temperature seuil de %.2f C!",k);
        strcat(msg_f, tmp_str);
        (*nb_m)++;   
        return 2;
    }else if(k>10) { 
        snprintf(tmp_str, sizeof(tmp_str), "danger immediat:depassement du temperature seuil de %.2f C!!",k);
        strcat(msg_f, tmp_str);
        (*nb_c)++;   
        return 3;
    }
}

Time duree(listch2 L) {
    Time total = {0, 0};
    noeud2 *aux = L;
    while (aux != NULL && aux->next != NULL) {
        if (aux->etat == 1 || aux->etat == 2 || aux->etat == 3) {
            int diffH = (aux->next->t.H) - aux->t.H;
            int diffM = (aux->next->t.M) - aux->t.M;
            if (diffM < 0) {       
                diffM += 60;
                diffH -= 1;
            }
            total.H += diffH;
            total.M += diffM;
            if (total.M >= 60) {   
                total.M -= 60;
                total.H += 1;
            }
        }
        aux = aux->next;
    }
    if (aux != NULL && (aux->etat == 1 || aux->etat == 2 || aux->etat == 3)) {
        total.M += 30;
        if (total.M >= 60) {
            total.M -= 60;
            total.H += 1;
        }
    }
    return total;
}

void supprimer(listch1* head) {//head: pointeur sur L(contient l'adresse de L) 
    listch1 p = *head;// p pointe sur le premier noeud        
    *head = (*head)->next; //L=L->next
    free(p);// libération de l'ancien premier noeud
}

int fausse_alarme(listch1 *L){
    if (*L == NULL || (*L)->next == NULL || (*L)->next->next == NULL) return 0;
    if ((*L)->etat==(*L)->next->etat && (*L)->next->etat==(*L)->next->next->etat && (*L)->etat!=0 ){
        supprimer(L);
        return 1;
    }else{
        supprimer(L);
        return 0;
    }
}

int ieme(listch1 L,int i){
    listch1 aux = L;
    int k = 0;
    while(k < i){
        aux = aux->next;
        k++;
    }
    return aux->etat;
}
int LENF(FILE *fp) {
    int lines = 0;
    int c;
    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n')
            lines++;
    }
    return lines;
}

void inserer1(listch1 *L,int x) {// cette fonction permet l'insertion a la fin de la list chainee
    listch1 m = (listch1)malloc(sizeof(noeud1));
    m->etat = x;
    m->next = NULL;
    if (*L == NULL) {//liste vide
        *L = m;
        return;
    }
    listch1 p = *L;
    while (p->next != NULL) { //on avance jusqu'au dernier element
        p = p->next;
    }
    p->next = m;
}

int len(listch1 Lch){// fonction qui permet de determiner la longueur d'une liste chainee
    int count = 0;
    listch1 aux = Lch;
    while (aux != NULL) {
        count++;
        aux = aux->next;
    }
    return count;
}

void add_date(Date *d) {
    int joursParMois[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};//fevrier 28 jours
    d->jour++; // on ajoute un jour
    if (d->jour > joursParMois[d->mois - 1]) {
        d->jour = 1;       // retour au premier jour
        d->mois++;         // on passe au mois suivant
        if (d->mois > 12) {
            d->mois = 1;   // retour à janvier
        }
    }
}

int Attention(listch1 *L) {
    int i=0;
    if (len(*L)<3) return 0;
    else {
        while (i<2) {
            if (ieme(*L,i+1)-ieme(*L,i)>=2) {
                return 1;
            }
            else {
                i++;
            }
        }
        if (i==2){
            return 0;
        }
    }
}

void inserer2(listch2 *L,int et,Time temps) {// cette fonction permet l'insertion a la fin de la list chainee
    listch2 m = (listch2)malloc(sizeof(noeud2));
    m->etat = et;
    m->t=temps;
    m->next = NULL;
    if (*L == NULL) {//liste vide
        *L = m;
        return;
    }
    listch2 p = *L;
    while (p->next != NULL) { //on avance jusq'au dernier element
        p = p->next;
    }
    p->next = m;
}

void empty1(listch1 *L) {
    noeud1 *tmp;
    while (*L != NULL) {
        tmp = *L;          // garder le noeud courant
        *L = (*L)->next; // avancer
        free(tmp);             // libérer l’ancien
    }
}

void empty2(listch2 *L2) {
    noeud2 *tmp;
    while (*L2 != NULL) {
        tmp = *L2;          // garder le noeud courant
        *L2 = (*L2)->next; // avancer
        free(tmp);             // libérer l’ancien
    }
}

int main() {
    char msg_final[100000];
    FILE *fp;
    fp = fopen("config.txt", "wt");
    fprintf(fp, "%f\n", 5.0);//seuil_min==0°C
    fprintf(fp, "%f\n", 40.0);//seuil_max==40°C
    fprintf(fp, "%f\n", -10.0);//inf_intervalle==-10°C
    fprintf(fp, "%f", 55.0);//sup_intervalle==55°C
    fclose(fp);

    float inf_intervalle, sup_intervalle, seuil_min, seuil_max;
    fp = fopen("config.txt", "rt");
    fscanf(fp, "%f %f %f %f", &seuil_min, &seuil_max, &inf_intervalle, &sup_intervalle);
//fscanf traite les sauts de ligne comme des espaces,donc %f %f %f %f lira correctement 4 float séparés par des sauts de ligne
    fclose(fp);

    listch1 L=NULL;
    listch2 L2=NULL;

    int nouveau_jour =1;//utilisateur donne 1 pur passer au nouveau jour,0 sinon.
    Date date={1,1};
    srand((unsigned int)time(NULL));
    while(nouveau_jour){//while le capteur "veut" passer au jour suivant
        int nb_c=0, nb_m=0, nb_s=0, compteur_faux=0;
        Time t={0,0};//timestamp of when we captured the temperature

        int i=0;
        float Tab_jour[50];
        FILE *f = fopen("jour.txt", "wt");
        fclose(f);//pour vider le fichier des données du jour précédent
        while(t.H<=23 && t.M<=59){ //while we're in this day
            float T=random_between(inf_intervalle,sup_intervalle);
            Tab_jour[i]=T;
            i++;

            fp = fopen("jour.txt", "at");//écrire chaque mesure de température avec timestamp dans un fichier
            fprintf(fp, "%f %d:%d\n",T,t.H,t.M);
            fclose(fp);

            char temp_str[500];
            int etat;
            if (T>=seuil_min && T<=seuil_max){
                etat=0;
                snprintf(temp_str, sizeof(temp_str),"la temperature est adequate:%.2f C\n", T);
                strcat(msg_final, temp_str);
                inserer1(&L,0);
                inserer2(&L2,etat,t);
            }else if(T<seuil_min){
                etat=alerte(T,seuil_min,&nb_c,&nb_m,&nb_s,msg_final);
                inserer1(&L,etat);
                snprintf(temp_str, sizeof(temp_str)," veuiller augmenter la temperature,elle est:%.2fC\n", T);
                strcat(msg_final, temp_str);
                inserer2(&L2,etat,t);

            }else if (T>seuil_max){
                etat=alerte(T,seuil_max,&nb_c,&nb_m,&nb_s,msg_final);
                inserer1(&L,etat);
                snprintf(temp_str, sizeof(temp_str)," veuiller baisser la temperature,elle est:%.2fC\n", T);
                strcat(msg_final, temp_str);
                inserer2(&L2,etat,t); 
            }

            add_min(&t);//add 30min(un capteur réel capture à une fréquence plus élevée
                         // mais on a fait chaque 30min pour ne pas afficher beaucoup de messages)

            if (fausse_alarme(&L)){   
                strcat(msg_final,"il y avait une fausse alarme!\n");
                compteur_faux++;
            }

            if (Attention(&L)) strcat(msg_final,"attention,il y a un saut inattendu de temperature, veuiller la regler!\n");
        }
        Time durée_tot_alerte=duree(L2);

        f = fopen("jour.txt", "rt");
        int nb=LENF(f);
        fclose(f);

        float Tmoy=T_calculmoy(Tab_jour,nb),Tmin=T_min(Tab_jour,nb),Tmax=T_max(Tab_jour,nb);

        fp = fopen("rapport_journalier.txt", "at");
        if(compteur_faux>5) {
            fprintf(fp, "date:%d/%d: Tmoy:%.2f, Tmin:%.2f, Tmax:%.2f, nombre d'alertes critiques:%d, nombre d'alertes modérées:%d,nombre d'alertes simples:%d, durée totale en état d'alerte:%d:%d, veuillez faire la maintenance du capteur \n",date.jour,date.mois,Tmoy, Tmin, Tmax, nb_c, nb_m, nb_s, durée_tot_alerte.H,durée_tot_alerte.M);
        }else{
            fprintf(fp, "date:%d/%d: Tmoy:%.2f, Tmin:%.2f, Tmax:%.2f, nombre d'alertes critiques:%d, nombre d'alertes modérées:%d,nombre d'alertes simples:%d, durée totale en état d'alerte:%d:%d\n",date.jour,date.mois,Tmoy, Tmin, Tmax, nb_c, nb_m, nb_s, durée_tot_alerte.H,durée_tot_alerte.M);
        }
        fclose(fp);

        add_date(&date);
        empty1(&L);
        empty2(&L2);
        MessageBox(NULL, msg_final, "Title", MB_OK | MB_ICONINFORMATION);
        //crée une fenetre qui contient tout les messages et alertes qu'on a mit dans msg_final
        msg_final[0] = '\0';   
        nouveau_jour= RunGui();
    }
    return 0;
}   