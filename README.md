# projetC du group CodePulse class ii1f 

Le programme enregistre les valeurs seuil_min,seuil_max,et les bornes de l'intervalle de températures permises dans le fichier config.txt.

On effectue une boucle while pour chaque jour,controlée par la variable int nouveau_jour qui indique le passage au jour suivant(en réalité, le capteur passe automatiquement au jour suivant puisq'il capte les valeurs d'une facon continue. on a fait cette boucle pour simuler ce passage): on capte cette variable à travers une fenetre où l'utilisateur ecrit 1 ou 0 et tappe le bouton pour entrer sa réponse.

Dans chaque jour,on capte la température chaque 30 minutes(en réalité le capteur capte les températures d'une facon quasi continue,mais on a réduit la fréaquence de capture pour la clarté des résultats du programme)(T prend une valeur aléatoire entre les bornes de l'intervalle de températures permises pour simuler les fluctuations de température dans la réalité).
On met chaque température captée avec son timestamp dans le fichier jour.txt .

On compare chaque avec les deux seuils de température et on l'associe avec un état (état d'alerte s'il y a dépassement des seuils: alerte critique(symbolisé par 3),alerte modérée(2) et alerte simple(1), et état normale(0) sinon) à travers la fonction alerte.

on remplie une liste chainée qui contient les 3 températures récentes et, à travers la fonction fausse_alerte, on détecte la présence des fausses alertes(qui consistent à la répétition du meme alerte 3 fois consécutifs,ce qui indique qu'il y a une problème avec le capteur car une régulation aurait dû être effectuée, et l’alerte ne devrait pas se répéter).

La fonction attention indique s'il y a un saut imprévu dans la température, et selon la valeur recue(1 ou 0) on affiche un message d'alerte. on affiche l'etat de chaque température et les alertes dans une fenetre.

En fin, on génére un rapport dans le fichier rapport_journalier.txt contenant: la date, la température moyenne du jour, les températures minimale et maximale de cet jour,le nombre d'alertes critiques,modérées et simples, la durée totale en état d'alerte et dans le cas où le nombre de fausses alertes dépasse 5, on demande de l'utilisateur de faire la maintenance du capteur.

**LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)** Cette fonction gère tous les messages envoyés à la fenêtre principale (comme la création, les clics sur les boutons, la fermeture, etc.)

**int RunGui()** Configure et exécute toute la fenêtre.Lorsque le code est exécuté, Une fenêtre apparaît intitulée « passer au jour suivant ? » avec un bouton nommé « Entrez 0 ou 1 ».L'utilisateur tape 0 ou 1 dans la zone d'édition puis Clique sur le bouton. Le programme se termine avec le code de retour 0 ou 1. Si on donne 0, la fenêtre est fermée (et la boucle se termine)

**float random_between(float min, float max)** génère un nombre aléatoire entre min et max 

**int T_max(int t[], int n)** retourne le maximum d'un tableau.

**int T_min(int t[], int n)** calcule le minimum d’un tableau .

**int T_calculmoy(int t[], int n)** calcule la moyenne d’un tableau.

**void add_min(Time *t)*** elle avance de 30 minutes le temps t. elle représente une horloge qui avance de 30 minutes à chaque nouvelle température, elle indique le temps en heures et minutes.

**int alerte(int T, int val_seuil, int *nb_c, int *nb_m, int *nb_s)***** À partir d’une température et d’une valeur seuil, cette fonction affiche un message correspondant au type de dépassement, incrémente de 1 le compteur associé à ce type d’alerte, et retourne 1 pour un avertissement léger, 2 pour une alerte moyenne et 3 pour une alerte critique.

**Time duree(listch2 L)** à partir d’une liste chainée de type listch2, elle détermine le total des durées des alertes .

**void supprimer(listch1 * head)** supprime le premier élement d’une liste chainée de type listch1.

**int fausse_alarme(listch1 *L)*** on donne les trois derniers états de température(soit alerte critique(3),ou modérée(2) ou simple(1) ou pas en état d'alerte(0)) si on a trois valeurs consécutifs qui sont égaux et différents de 0(càd on trois alertes consécutifs de méme type) donc on a une fausse alerte car le regulateur aurait du corriger la température.

**int iéme(listch1 L,int i)** retourne l'ième élément d'une liste chainée de type listch1.

**int LENF(FILE *fp)*** retourne la longueur d'un fichier.

**void inserer1(listch1 *L,int x)*** permet l'insertion d'un element x à la fin d'une liste chainée de type listch1.

**int len(listch1 Lch)** retourne la longueur d'une liste chainée de type listch1.

**void add_date(Date *d)*** permet d'avancer d'un jour d'une date donnée.si l'on depasse le nombre maximum de jours du mois, on passe au mois suivant.

**int Attention(listch1 *L)*** retourne 1 si il ya des dépassements imprévus de la température sinon 0.

**void inserer2(listch2 *L,int et,Time temps)*** permet l'insertion d'un nouvel element a la fin d'une liste chainée de type listch2.

**void empty1(listch1 *L)*** vide une liste chainée de type listch1

**void empty2(listch2 *L2)*** vide une liste chainée de type listch2
