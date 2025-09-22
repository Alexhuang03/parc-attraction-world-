//
// Created by Elias on 04/05/2023.
//
#include "fonctions.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


void initialisation_allegro() {
    allegro_init();
    install_keyboard();
    install_mouse();
    install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);
    set_color_depth(desktop_color_depth());

    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 976, 665, 0, 0) != 0) {
        allegro_message("Probleme mode graphique");
        allegro_exit();
        exit(EXIT_FAILURE);
    }
}

BITMAP *charger_image(const char *nom) {
    BITMAP *img = load_bitmap(nom, NULL);
    if (!img) {
        allegro_message("Erreur de chargement de '%s'\n", nom);
        exit(1);
    }
    return img;
}

void creer_bitmap_du_personnage(personnage *perso, BITMAP *A, int joueur) {
    int offset_x;

    if (joueur == 1)
        offset_x = 8;
    else if (joueur == 2)
        offset_x = 388;

    int w = 64;   //Choix d edimensions des sprites
    int h = 64;

    for (int i = 0; i < 3; i++) {
        perso->bas[i] = create_sub_bitmap(A, offset_x + i * w, 0, w, h);      //on parcours notre fichier pour split les sprites
        perso->gauche[i] = create_sub_bitmap(A, offset_x + i * w, h, w, h);      //à certaines coordonnées
        perso->droite[i] = create_sub_bitmap(A, offset_x + i * w, 2 * h, w, h);
        perso->haut[i] = create_sub_bitmap(A, offset_x + i * w, 3 * h, w, h);
    }

    BITMAP *tmp;
    tmp = perso->bas[0];
    perso->bas[0] = perso->bas[1];
    perso->bas[1] = tmp;

    tmp = perso->gauche[0];
    perso->gauche[0] = perso->gauche[1];
    perso->gauche[1] = tmp;

    tmp = perso->droite[0];
    perso->droite[0] = perso->droite[1];
    perso->droite[1] = tmp;

    tmp = perso->haut[0];
    perso->haut[0] = perso->haut[1];
    perso->haut[1] = tmp;
}

void afficher_personnage(personnage *perso, BITMAP *buffer, int choix) {
    BITMAP *instant;
    if(perso->direction == 0)    //On vérifie dans quelle direction va le joueur
        instant = perso->bas[perso->instant];        //De cette manière, on lui assigne la frame de son déplacement correspondante
    else if(perso->direction == 1)
        instant = perso->gauche[perso->instant];
    else if(perso->direction == 2)
        instant = perso->droite[perso->instant];
    else if(perso->direction == 3)
        instant = perso->haut[perso->instant];

    if(choix == 0)         //cette variable est utile car sur la carte le personnage a des dimensions réduites mais pour le jeu traversée de rivière,
                           //ces dimensions sont celles de bases (trop grande pour la carte
    masked_stretch_blit(instant, buffer, 0, 0, instant->w, instant->h, perso->x, perso->y, instant->w - 10, instant->h - 10);
    else if(choix == 1)
        masked_blit(instant, buffer, 0, 0, perso->x, perso->y, instant->w, instant->h);
}

void actualisation_personnage(personnage *perso, BITMAP *buffer) {
    int vitesse;
    if (key[KEY_E])   //Pour sprinter
        vitesse = 4;
    else vitesse = 2;  //Sinon vitesse normale

    int couleur_route = makecol(96, 104, 112);  //on définit la couleur du sol sur lequel le joueur pour se déplacer

    perso->deplacement = 0;
    if (key[KEY_UP]) {
        perso->direction = 3;
        perso->deplacement = 1;
        if (getpixel(buffer, perso->x + 15, (perso->y + perso->haut[0]->h) - 12) == couleur_route)  //collisions
            perso->y -= vitesse;
        else
            perso->y += vitesse;
    }
    if (key[KEY_DOWN]) {
        perso->direction = 0;
        perso->deplacement = 1;
        if (getpixel(buffer, perso->x + 15, (perso->y + perso->haut[0]->h) -8) == couleur_route) //collisions
            perso->y += vitesse;
        else
            perso->y -= vitesse;
    }
    if (key[KEY_LEFT]) {
        perso->direction = 1;
        perso->deplacement = 1;
        if (getpixel(buffer, perso->x + 25, (perso->y + perso->haut[0]->h )-10) == couleur_route) //collisions
            perso->x -= vitesse;
        else
            perso->x += vitesse;
    }
    if (key[KEY_RIGHT]) {
        perso->direction = 2;
        perso->deplacement = 1;
        if (getpixel(buffer, (perso->x + perso->bas[0]->w) - 35, (perso->y + perso->haut[0]->h)-10) == couleur_route) //collisions
            perso->x += vitesse;
        else
            perso->x -= vitesse;

    }
}

void initialiser_joueurs(personnage *joueur) {   //fonction appelée uniquement au début du programme
    joueur->direction = 0;
    joueur->instant = 0;
    joueur->nb_tickets = 5;
}

int quitter(personnage *joueur, BITMAP *buffer, BITMAP *carte) {
    int vitesse = 1;
    BITMAP *instant;
    int compteur = 0;
    if ((joueur->x <= 130 && joueur->x >= 90) && joueur->y == 130 && joueur->direction == 3) { //Si le joueur atteint la sortie
        for (int i = 0; i < 80; i++) {
            joueur->direction = 3;
            joueur->deplacement = 1;
            joueur->y -= vitesse;

            blit(carte, buffer, 0, 0, 0, 0, carte->w, carte->h);

            instant = joueur->haut[joueur->instant];

            masked_stretch_blit(instant, buffer, 0, 0, instant->w, instant->h, joueur->x, joueur->y, instant->w - 10, instant->h - 10);

            blit(buffer, screen, 0, 0, 0, 0, buffer->w, buffer->h);

            compteur++;
            if (compteur >= 7) {             //Pour éviter un défilement trop rapide des images lors de l'animation
                if (joueur->deplacement)
                    joueur->instant = (joueur->instant + 1) % 3;
                else
                    joueur->instant = 0;
                compteur = 0;
            }
            rest(10);
        }
        return 1;
    } else return 0;
}

void acces_statistiques(personnage *joueur, BITMAP *tableau, BITMAP *buffer, BITMAP *icon_stop,  const char *nom_fichier_perf, const char *nom_fichier_perf_ot) {
    int x, y, vert = makecol(0, 255, 0), orange = makecol(255, 128, 0);
    int fin = 0;
    if ((joueur->x <= 340 && joueur->x >= 310) && (joueur->y >= 525 && joueur->y <= 555) && joueur->direction == 3 &&
        key[KEY_ENTER]) {     //Le joueur est devant le tableau des stats et souhaite le consulter
        while (!fin) {
            masked_blit(tableau, buffer, 0, 0, SCREEN_W / 2 - tableau->w / 2, SCREEN_H / 2 - tableau->h / 2, tableau->w,
                        tableau->h);
            masked_blit(icon_stop, buffer, 0, 0, SCREEN_W / 2 + tableau->w / 2 - 165,
                        SCREEN_H / 2 + tableau->h / 2 - 128, tableau->w, tableau->h);
            textout_ex(buffer, font, "TABLEAU DES SCORES", SCREEN_W / 2 - 75, SCREEN_H / 2 - tableau->h / 2 + 70,
                       makecol(0, 0, 0), -1);
            textout_ex(buffer, font, "Cliquez sur un des cercles de couleur ou sur STOP", SCREEN_W / 2 - 200,
                       SCREEN_H / 2 - tableau->h / 2 + 85, makecol(0, 0, 0), -1);
            textout_ex(buffer, font, "PERFORMANCES DES JOUEURS", SCREEN_W / 2 - 200,
                       SCREEN_H / 2 - tableau->h / 2 + 160, makecol(0, 0, 0), -1);
            textout_ex(buffer, font, "MEILLEURES PERFORMANCES GLOBALES", SCREEN_W / 2 - 200,
                       SCREEN_H / 2 - tableau->h / 2 + 200, makecol(0, 0, 0), -1);
            circlefill(buffer, SCREEN_W / 2 - 230, SCREEN_H / 2 - tableau->h / 2 + 160, 15, vert);
            circlefill(buffer, SCREEN_W / 2 - 230, SCREEN_H / 2 - tableau->h / 2 + 200, 15, orange);

            blit(buffer, screen, 0, 0, 0, 0, buffer->w, buffer->h);

            if (mouse_b & 1) {  //Le choix du joueur se fait à la souris
                x = mouse_x;
                y = mouse_y;
                int couleur = getpixel(screen, x, y);
                if ((x <= SCREEN_W / 2 + tableau->w / 2 + icon_stop->w && x >= SCREEN_W / 2 + tableau->w / 2 - 165) &&
                    (y <= SCREEN_H / 2 + tableau->h / 2 + icon_stop->h && y >= SCREEN_H / 2 + tableau->h / 2 - 128))
                    fin = 1;
                else if (couleur == vert || couleur == orange) {
                    int fin2 = 0;
                    while (!fin2) {
                        if (mouse_b & 1) {
                            x = mouse_x;
                            y = mouse_y;
                        }
                        masked_blit(tableau, buffer, 0, 0, SCREEN_W / 2 - tableau->w / 2, SCREEN_H / 2 - tableau->h / 2,
                                    tableau->w, tableau->h);
                        masked_blit(icon_stop, buffer, 0, 0, SCREEN_W / 2 + tableau->w / 2 - 165,
                                    SCREEN_H / 2 + tableau->h / 2 - 200, tableau->w, tableau->h);
                        if (couleur == vert) {
                            textprintf_ex(buffer, font, SCREEN_W / 2 - 100, SCREEN_H / 2 - tableau->h / 2 + 70,
                                          makecol(0, 0, 0), -1, "PERFORMANCES DE %s", joueur->nom);

                            chargement_score_joueur(joueur, nom_fichier_perf, buffer, tableau);   //On lit le fichier de sauvegarde du joueur et on écrit ses scores.
                        }
                        else{
                            textout_ex(buffer, font, "MEILLEURES PERFORMANCES GLOBALES", SCREEN_W / 2 - 140,
                                       SCREEN_H / 2 - tableau->h / 2 + 70, makecol(0, 0, 0), -1);
                        }
                        blit(buffer, screen, 0, 0, 0, 0, buffer->w, buffer->h);

                        if ((x <= SCREEN_W / 2 + tableau->w / 2 + icon_stop->w &&
                             x >= SCREEN_W / 2 + tableau->w / 2 - 165) &&
                            (y <= SCREEN_H / 2 + tableau->h / 2 + icon_stop->h &&
                             y >= SCREEN_H / 2 + tableau->h / 2 - 200))
                            fin2 = 1;
                    }

                }
            }
        }
    }
}

void chargement_score_joueur(personnage *joueur, const char *nom_fichier_perf, BITMAP*buffer, BITMAP *tableau){
    char nom[20];
    char charge[50];
    int i = 0;
    FILE  *pf1 = fopen(nom_fichier_perf, "r");
    strcpy(nom, joueur->nom);
    rewind(pf1);
    while(fgets(charge, 50, pf1) != NULL){  //parcours du fichier jusqu'à la fin
        if(charge[0] == nom[0]) {   //Si les noms sont égaux
            textprintf_ex(buffer, font, SCREEN_W / 2 - 200,
                          SCREEN_H / 2 - tableau->h / 2 + 140 + 8 * i,
                          makecol(0, 0, 0), -1, "%s", charge);
            i++;  //pour descendre d'une ligne dans le tableau des scores
        }
    }
    fclose(pf1);
}

void nom_clavier(personnage *joueur, int numero_joueur, BITMAP *buffer) {
    int saisie, saisie2, saisie3;
    int i = 0;

    char nom[TAILLE_NOM + 1]; //On ajoute 1 pour pouvoir stocker le nom complet et éviter un overflow
    char tmp_saisie[2];    // stockage de la derniere touche saisie

    tmp_saisie[1] = 0;
    nom[TAILLE_NOM] = 0;
    clear_keybuf();

    BITMAP *titre = charger_image("carte_sinnoh.bmp");
    stretch_blit(titre, buffer, 0, 0, titre->w, titre->h, 0, 0, SCREEN_W, SCREEN_H);
    rectfill(buffer, 10, 10, 400, 200, makecol(0, 0, 0));

    textout_ex(buffer, font, "BIENVENUE DANS ECE WORLD", 100, 20, makecol(255, 255, 0), -1);

    textprintf(buffer, font, 20, 50, makecol(255, 255, 255), "Quel est le nom du joueur %d", numero_joueur);

    textout_ex(buffer, font, "_", 20 + 20 * (i + 1), 90, makecol(255, 255, 255), -1);
    blit(buffer, screen, 0, 0, 0, 0, buffer->w, buffer->h);

    while (!key[KEY_ENTER]) {      //Tant que le joueur ne valide pas la saisie dse son nom

        saisie = readkey();                   //popur vérifier qu'un touche est appuyée
        saisie2 = saisie & 0xFF;             // ASCII
        saisie3 = saisie >> 8;              // scancode (je suis allé chercher dans le cours sur boostcamp)

        if ((saisie2 >= 'A' && saisie2 <= 'Z') || (saisie2 >= 'a' && saisie2 <= 'z') || saisie2 == ' ') {
            if(strlen(nom) < TAILLE_NOM -1) {
                if (i >= TAILLE_NOM)
                    i = TAILLE_NOM;
                else {
                    nom[i] = saisie2;
                    tmp_saisie[0] = saisie2;
                    nom[i + 1] = 0;
                    textprintf(buffer, font, 20 + 20 * i, 90, makecol(0, 255, 255), "%s", tmp_saisie);
                    i++;
                    textout_ex(buffer, font, "_", 20 + 20 * i, 90, makecol(0, 255, 255), -1);

                }
            }
        }
        if (saisie3 == KEY_ENTER) {         // Si l'utilisateur valide sa saisie
            textout_ex(buffer, font, " ", 20 + 20 * i, 90, makecol(0, 0, 255), -1);
            if (i == 0)
                nom[0] = 32;                // code pour l'espace
            nom[i] = 0;                     // Pour éviter qu'un potentiel caractère effacé précédemment ne soit compté dans le nom final.
        }
        if (saisie3 == KEY_BACKSPACE) {     //Si l'utilisateur veut retourner en arrière
            i--;
            if (i < 0)
                i = 0;

            textprintf(buffer, font, 20 + 20 * i, 90, makecol(0, 255, 255), "_");
            textprintf(buffer, font, 20 + 20 * (i + 1), 90, makecol(0, 255, 255), " ");
        }
        blit(buffer, screen, 0, 0, 0, 0, buffer->w, buffer->h);
    }
    textprintf_ex(buffer, font, 20, 120, makecol(255, 255, 0), -1, "Bienvenue %s !", nom);
    blit(buffer, screen, 0, 0, 0, 0, buffer->w, buffer->h);
    strcpy(joueur->nom, nom);
    clear_keybuf();
}

void ecran_victoire(personnage joueur[], BITMAP *buffer){
    BITMAP *fond_victoire = charger_image("fond2.bmp");
    BITMAP *podium = charger_image("podium.bmp");

    joueur[0].direction = 0;
    joueur[1].direction = 0;

    joueur[0].instant = 0,
            joueur[1].instant = 0;

    if(joueur[1].nb_tickets == 0) {
        joueur[0].x = SCREEN_W / 2 - 30;
        joueur[0].y = SCREEN_H / 2 + 5;
        joueur[1].x = SCREEN_W / 2 - 90;
        joueur[1].y = SCREEN_H / 2 + 18;
    }
    else{
        joueur[1].x = SCREEN_W / 2 - 30;
        joueur[1].y = SCREEN_H / 2 + 5;
        joueur[0].x = SCREEN_W / 2 - 90;
        joueur[0].y = SCREEN_H / 2 + 18;
    }

    while(!key[KEY_ESC]) {
        clear(buffer);
        stretch_blit(fond_victoire, buffer, 0, 0, fond_victoire->w, fond_victoire->h, 0, 0, SCREEN_W, SCREEN_H);
        rectfill(buffer, SCREEN_W / 2 - 100, SCREEN_H / 2 + 100, SCREEN_W / 2 + 100, SCREEN_H, makecol(0, 0, 0));
        masked_blit(podium, buffer, 0, 0, SCREEN_W/2 -100, SCREEN_H/2 - 20, podium->w, podium->h);
        textout_ex(buffer, font, "Felicitations", SCREEN_W/2 - 90, SCREEN_H/2 + 150, makecol(255, 255, 255), -1);
        if(joueur[1].nb_tickets == 0)
            textprintf_ex(buffer, font, SCREEN_W/2 - 90, SCREEN_H / 2 + 200, makecol(255, 255, 255), -1, "%s", joueur[0].nom);
        else
            textprintf_ex(buffer, font, SCREEN_W/2 - 90, SCREEN_H / 2 + 200, makecol(255, 255, 255), -1, "%s", joueur[1].nom);
        textout_ex(buffer, font, "Tu gagnes la partie", SCREEN_W/2 - 90, SCREEN_H/2 + 250, makecol(255, 255, 255), -1);
        textout_ex(buffer, font, "ESC pour quitter", SCREEN_W/2 - 90, SCREEN_H - 15, makecol(255, 255, 255), -1);

        afficher_personnage(&joueur[0], buffer, 1);
        afficher_personnage(&joueur[1], buffer, 1);

        blit(buffer, screen, 0, 0, 0, 0, buffer->w, buffer->h);
    }
    destroy_bitmap(fond_victoire);
    destroy_bitmap(podium);
}

/*----------------------------------------------------------------------------------------------------------------------------------------------------------*/
/// TIR BALLONS ///
// FONCTION POUR CHARGER LES IMAGES DE BALLONS (POUR EVITER LES REPETITIONS) //
BITMAP *chargerImage (const char *nom_image) {
    BITMAP *image = load_bitmap(nom_image, NULL);
    if (!nom_image) {
        allegro_message("image %s non trouve\n", nom_image);
        allegro_exit();
        exit(EXIT_FAILURE);
    }
    return image;
}

// FONCTION POUR INITIALISER LES PARAMETRES D'UNE IMAGE //
void initBallon(ballon* B,int posxInit, int posyInit, int depxInit, int depyInit ){
    B->tx = B->img->w;
    B->ty = B->img->h;
    B->posx = posxInit;
    B->posy = posyInit;
    B->depx = depxInit;
    B->depy = depyInit;
}

// FONCTION POUR AFFICHER UN BALLON //
void affichierBallon (BITMAP* page,ballon *B){
    draw_sprite(page, B->img, B->posx, B->posy);
}

// FONCTION PouR LE DEPLACEMENT D'UN BALLON //
void deplacementBallon (ballon *B){
    B->posx = B->depx + B->posx;
    if ((B->posx < 0 && B->depx < 0) || (B->posx + B->tx > SCREEN_W && B->depx> 0))
        B->depx = -B->depx;
    B->posy = B->depy + B->posy;
    if ((B->posy < 0 && B->depy < 0) || (B->posy + B->ty > SCREEN_H && B->depy > 0))
        B->depy = -B->depy;
    if(rand()%10 == 1)
        B->depx = -B->depx;
    if(rand()%10 == 1)
        B->depy = -B->depy;
}

// FONCTION PR AFFICHER LE VISEUR //
void affichierViseur (BITMAP* page, BITMAP* image){
    draw_sprite(page, image, mouse_x-38, mouse_y-38);
}

void supp_ballon (ballon *B, int *score){ // changer la couleur du ballon à la même couleur du fond (noir)
    if(mouse_b & 1){
        int x = mouse_x;
        int y = mouse_y;
        if((x >= B->posx && x <= B->posx + B->tx) && (y >= B->posy && y <= B->posy + B->ty)){
            B->posx = 1000000;
            B->posy = 1000000;
            (*score)++;
        }
    }
}

void tir_ballon (personnage joueur_g[], int *b, BITMAP *page, BITMAP *viseur, BITMAP *ticket , ballon tab[NB_BALLONS], const char *nom_fichier_perf, const char *nom_fichier_perf_ot ){
    int a = *b;
    int stop = 0;

    if ((joueur_g[a].x <= 782 && joueur_g[a].x >= 744) && (joueur_g[a].y >= 200 && joueur_g[a].y <= 230)
        && joueur_g[a].direction == 3 && key[KEY_ENTER]) {
        BITMAP *tir_fond = charger_image("tir_fond.bmp");

        do {
            blit (tir_fond, page, 0,0,0,0, tir_fond -> w, tir_fond -> h);
            rectfill(page, 0, 549, SCREEN_W, SCREEN_H, makecol(0, 0, 0));
            textprintf_ex(page, font, 10, 560, makecol(255, 255, 0), -1, "BIENVENUE DANS TIR BALLONS %s", joueur_g[a].nom);
            textout_ex(page, font, "Appuyez sur ENTRER pour commencer le jeu", 18,585, makecol(255, 255, 255), -1);
            textout_ex(page, font, "Appuyez sur RETOUR pour quitter", 18, 630, makecol(255, 255, 255), -1);
            textout_ex(page, font, "COUT : ", 750, 605, makecol(255, 255, 0), -1);
            masked_blit(ticket, page , 0, 0, 770, 540, ticket->w, ticket->h);
            blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
            rest (500);
        }while (!key[KEY_ENTER] && !key[KEY_BACKSPACE]);
        destroy_bitmap(tir_fond);

        if (key [KEY_BACKSPACE])
            stop = 1;

        if (!stop) {
            BITMAP *fond_charge = charger_image("fond_charge.bmp");
            BITMAP *resultat_tir = charger_image("resultat_tir.bmp");
            int fini = 0;
            double temps [2];
            personnage joueur_tab [2];
            clock_t debut, fin;
            double temps_execution;
            int score;

            if(*b == 0) {
                joueur_g[*b].nb_tickets--;
                joueur_tab[0] = joueur_g[0];
                joueur_tab[1] = joueur_g[1];
                *b = 1;
            }
            else {
                joueur_g[*b].nb_tickets--;
                joueur_tab[0] = joueur_g[*b];
                joueur_tab[1] = joueur_g[0];
                *b = 0;
            }

            for (int i = 0; i<2 ; i ++){
                clear (page);
                blit(fond_charge, page, 0, 0, 0, 0, fond_charge->w, fond_charge->h);
                textprintf_ex(page , font, SCREEN_W/2 -100, 560, makecol(255, 255, 0), -1, "C'EST LE TOUR DE %s", joueur_g[i].nom);
                blit(page , screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
                rest(1000);
                score = 0;
                fini = 0;
                /// Début clock ///
                debut = clock();

                while (!fini ){
                    clear(page);

                    /// Initialisation du paramètre images ///
                    initBallon(&tab[0], SCREEN_W / 2, SCREEN_H / 2, 19, 13);
                    initBallon(&tab[3], SCREEN_W / 2, SCREEN_H / 3, 17, 15);
                    initBallon(&tab[1], SCREEN_W / 3, SCREEN_H / 4, 16, 14);
                    initBallon(&tab[2], SCREEN_W / 4, SCREEN_H / 5, 17, 18);
                    initBallon(&tab[4], SCREEN_W / 5, SCREEN_H / 6, 19, 16);
                    initBallon(&tab[5], SCREEN_W / 6, SCREEN_H / 3, 13, 13);
                    initBallon(&tab[6], SCREEN_W / 3, SCREEN_H / 6, 17, 12);
                    initBallon(&tab[7], SCREEN_W / 9, SCREEN_H / 7, 16, 14);
                    initBallon(&tab[8], SCREEN_W / 8, SCREEN_H / 2, 19, 12);
                    initBallon(&tab[9], SCREEN_W / 5, SCREEN_H / 2, 15, 15);

                    /// Code principal pour le déplacement ///
                    while (score!= NB_BALLONS) {
                        clear_bitmap(page);

                        /// déplacement des ballons ///
                        for (int j = 0; j < NB_BALLONS; j++) {
                            deplacementBallon(&tab[j]);
                            supp_ballon(&tab[j], &score);
                            affichierBallon(page, &tab[j]);
                        }

                        affichierViseur(page, viseur);

                        blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

                        ///Delay///
                        rest(20);
                    }
                    fini = 1;
                    blit (page, screen, 0,0,0,0, SCREEN_W, SCREEN_H);

                }
                fin = clock();
                temps_execution = (double) (fin - debut) / CLOCKS_PER_SEC;
                temps [i] = temps_execution;

                /// Affiche résultat ///
                clear (page);
                while (!key[KEY_BACKSPACE]){
                    blit (resultat_tir, page, 0,0,0,0, resultat_tir->w, resultat_tir->h);
                    textout_ex(page, font, "FINI !", SCREEN_W / 2 - 150, 620, makecol(255, 0, 0), -1);
                    textprintf_ex(page, font, SCREEN_W / 2 - 100, 600, makecol(255, 255, 255), -1,
                                  "%s a éclaté tous les ballons en %f secondes", joueur_g[i].nom, temps_execution);
                    textout_ex(page, font, "Appuyez sur RETOUR pour quitter", SCREEN_W / 2 - 50, 620,
                               makecol(255, 255, 0), -1);
                    blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
                }
            }
            ///condition ticket ///
            if(temps[0] < temps[1])
                joueur_tab[0].nb_tickets++;
            else
                joueur_tab[1].nb_tickets++;
            if(a == 1){
                joueur_g[a] = joueur_tab[0];
                joueur_g[0] = joueur_tab[1];
            }
            else {
                joueur_g[0] = joueur_tab[0];
                joueur_g[1] = joueur_tab[1];
            }

            /// destroy ///
            for (int j = 0; j < NB_BALLONS; j++) {
                destroy_bitmap(tab[j].img);
            }
            destroy_bitmap(viseur);
            destroy_bitmap(resultat_tir);
            destroy_bitmap(fond_charge);
        }
    }
}


/*----------------------------------------------------------------------------------------------------------------------------------------------------------*/
/// GUITAR HERO ///

SAMPLE *charger_musique(const char *nom) {
    SAMPLE *musique = load_sample(nom);
    if (!musique) {
        allegro_message("Erreur de chargement de 'ILoveRockNRoll_out.wav'\n");
        exit(1);
    }
    return musique;
}

void defiler_fond(BITMAP *buffer, BITMAP *fond, int decalage) {    //principe du jeu, on défile le fond pour avancer
    blit(fond, buffer, 0, fond->h - decalage - SCREEN_H, (SCREEN_W - fond->w) / 2 - 10, 0, fond->w, SCREEN_H);
}

void attendre(clock_t debut, double delai) {
    while ((clock() - debut) * 1000 / CLOCKS_PER_SEC < delai) {     //pour ne pas défiler trop vite

    }
}

int verification_note(BITMAP *buffer, BITMAP *fond, int note, int decalage) {    //On vérifie que le joueur appuie bien sur la note qui passe

    int decalage_base = (SCREEN_W - fond->w) / 2;
    int colonne_w = fond->w / 5;
    int couleur = 0;

    if (note == 0) couleur = makecol(0, 255, 0);
    else if (note == 1) couleur = makecol(255, 0, 0);
    else if (note == 2) couleur = makecol(255, 255, 0);
    else if (note == 3) couleur = makecol(0, 0, 255);
    else if (note == 4) couleur = makecol(255, 128, 0);

    int couleur_actuelle;
    int i = 0;
    int oui = 0;
    do {
        couleur_actuelle = getpixel(buffer, decalage_base + note * colonne_w + colonne_w / 2, SCREEN_H - 75 - i);
        if (couleur_actuelle == couleur || couleur_actuelle == makecol(255, 255, 255))
            oui = 1;
        i++;
    } while (couleur_actuelle != makecol(0, 0, 0));

    if (oui) {
        int x = note * colonne_w + colonne_w / 2;
        int y = (fond->h - 75) - decalage;
        remplir_pixel_en_blanc(fond, x, y);
        return 1;
    } else if (couleur == makecol(0, 0, 0)) {
        return 0;
    }
}

int verification_passage(BITMAP *buffer) {   //Sert à vérifier si le joueur a laissé passer une note
    int y_bas = SCREEN_H - 1;
    for (int i = 0; i < SCREEN_W - 1; i++) {   //on parcours tout l'écran
        int pixel_actuel = getpixel(buffer, i, y_bas);
        if ((pixel_actuel == makecol(0, 255, 0) || pixel_actuel == makecol(255, 0, 0) ||
             pixel_actuel == makecol(255, 255, 0) || pixel_actuel == makecol(0, 0, 255) ||
             pixel_actuel == makecol(255, 128, 0)))   //si le pixel détecté est de la couleur d'une des notes
            return 0;
    }
    return 1;
}

void remplir_pixel_en_blanc(BITMAP *fond, int x, int y){    //Sous programme récursif qui permet de remplir les pixels d'une note sur lequel le joueur à appuyé en blanc
                                                            //J'ai en partie utilisé la fonction de Malrick sur le TP piano tiles
    int rouge = makecol(255, 0, 0), vert = makecol(0, 255, 0), jaune = makecol(255, 255, 0), bleu = makecol(0, 0, 255), orange = makecol(255, 128, 0);
    int blanc = makecol(255, 255, 255);
    int couleur_actuelle = getpixel(fond, x, y);

    if(couleur_actuelle == rouge || couleur_actuelle == bleu || couleur_actuelle == orange || couleur_actuelle == vert || couleur_actuelle == jaune){
        putpixel(fond, x, y, blanc);
        if(x > 0)
            remplir_pixel_en_blanc(fond, x - 1, y);
        if(x < fond->w - 1)
            remplir_pixel_en_blanc(fond, x + 1, y);
        if(y > 0)
            remplir_pixel_en_blanc(fond, x, y - 1);
        if(y < fond->h - 1)
            remplir_pixel_en_blanc(fond, x, y + 1);
    }
}

void changer_couleur_pixel(BITMAP *fond, int x, int y){           //Comme les deux joueurs jouent, il faut remettre les pixels blanc dans leur couleur respective avant que le prochain joueur ne joue
    for(int i = y ; i < fond->h ; i++){
        for(int j = x ; j < fond->w ; j++){
            if(getpixel(fond, j, i) == makecol(255, 255, 255)){
                if(j > 6 && j < 65)
                    putpixel(fond, j, i, makecol(0, 255, 0));
                else if(j >= 68 && j < 123)
                    putpixel(fond, j, i, makecol(255, 0, 0));
                else if(j > 127 && j < 180)
                    putpixel(fond, j, i, makecol(255, 255, 0));
            }
        }
    }
}

int titre_guitar_hero(BITMAP *buffer, BITMAP *ecran_titre, BITMAP *ticket, personnage joueur_g[], int a){
    do {
        blit(ecran_titre, buffer, 0, 0, 0, 0, ecran_titre->w, ecran_titre->h);
        rectfill(buffer, 0, 549, SCREEN_W, SCREEN_H, makecol(0, 0, 0));
        textprintf_ex(buffer, font, 10, 560, makecol(255, 255, 0), -1, "BIENVENUE DANS GUITAR HERO %s",
                      joueur_g[a].nom);
        textout_ex(buffer, font, "Appuyez sur ENTRER pour commencer", 18, 585, makecol(255, 255, 255), -1);
        textout_ex(buffer, font, "Appuyez sur RETOUR pour quitter", 18, 630, makecol(255, 255, 255), -1);
        textout_ex(buffer, font, "COUT : ", 750, 605, makecol(255, 255, 0), -1);
        masked_blit(ticket, buffer, 0, 0, 770, 540, ticket->w, ticket->h);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

    }while (!key[KEY_ENTER] && !key[KEY_BACKSPACE]);
    destroy_bitmap(ecran_titre);

    if(key[KEY_BACKSPACE])
        return 1;
    else return 0;
}

void fin_guitar_hero(BITMAP *buffer, BITMAP *fond_fond, personnage joueur[], int i, double duree){
    while (!key[KEY_BACKSPACE]) {
        clear(buffer);
        blit(fond_fond, buffer, 0, 0, 0, 0, fond_fond->w, fond_fond->h);
        textout_ex(buffer, font, "PERDU !", SCREEN_W / 2 - 150, 620, makecol(255, 0, 0), -1);
        textprintf_ex(buffer, font, SCREEN_W / 2 - 100, 600, makecol(255, 255, 255), -1,
                      "%s a tenu %f secondes", joueur[i].nom, duree);
        textout_ex(buffer, font, "Appuyez sur RETOUR pour quitter", SCREEN_W / 2 - 50, 620,
                   makecol(255, 255, 0), -1);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

    }
}

void guitar_hero(personnage joueur_g[], int *b, BITMAP *buffer, BITMAP *ticket,  const char *nom_fichier_perf, const char *nom_fichier_perf_ot) {

    int a = *b;
    if ((joueur_g[a].x <= 482 && joueur_g[a].x >= 420) && (joueur_g[a].y >= 110 && joueur_g[a].y <= 200) && joueur_g[a].direction == 3 &&
        key[KEY_ENTER]) {        //Si le joueur entre dans l'arène de jeu

        int stop = 0;

        rest(1000);

        BITMAP *ecran_titre = charger_image("guitar_hero_title_screen.bmp");

        stop = titre_guitar_hero(buffer, ecran_titre, ticket, joueur_g, a);  //On vérifie que le joueur est sûr de vouloir jouer et dépenser un ticket

        if (!stop) {

            BITMAP *fond = charger_image("vrai_fond_guitar_hero2.bmp");
            BITMAP *fond_fond = charger_image("hero.bmp");
            SAMPLE *son = charger_musique("ILoveRockNRoll_out.wav");

            int fin = 0;
            int decalage = 0;
            int decalage_base = (SCREEN_W - fond->w) / 2;
            int taille_colonne = fond->w / 5;
            personnage joueur[2];
            double temps[2];

            if(*b == 0) {   //Comme on modélise les deux joueurs par un tableau de type personnage et qu'on veut faire jouer les deux joueurs grâce à une boucle,
                           //Il faut vérifier dans quelle case du tableau est stocké le joueur qui est rentré dans le jeu car c'est lui qui doit jouer en premier
                joueur_g[*b].nb_tickets--;
                joueur[0] = joueur_g[0];
                joueur[1] = joueur_g[1];
                *b = 1;
            }
            else {
                joueur_g[*b].nb_tickets--;
                joueur[0] = joueur_g[*b];
                joueur[1] = joueur_g[0];
                *b = 0;
            }

            for(int i = 0 ; i < 2 ; i++) {
                clear(buffer);
                blit(fond_fond, buffer, 0, 0, 0, 0, fond_fond->w, fond_fond->h);
                textprintf_ex(buffer, font, SCREEN_W/2 -100, 560, makecol(255, 255, 0), -1, "C'EST LE TOUR DE %s", joueur[i].nom);
                blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
                rest(3000);
                fin = 0;
                decalage = 0;

                play_sample(son, 255, 0, 990, 1);   //On joue la musique
                clock_t debut = clock();                                  //On démarre le chronométrage

                while (!fin) {
                    clear(buffer);

                    blit(fond_fond, buffer, 0, 0, 0, 0, fond_fond->w, fond_fond->h);

                    defiler_fond(buffer, fond, decalage);   //défilement du fond au fur et à mesure du jeu

                    circle(buffer, decalage_base + taille_colonne / 2 - 6, SCREEN_H - 75,          //On affiche les cercles de couleurs typiques du guitar Hero
                           taille_colonne / 2 - 2, makecol(0, 255, 0));
                    circle(buffer, decalage_base + taille_colonne + taille_colonne / 2 - 3, SCREEN_H - 75,
                           taille_colonne / 2 - 2, makecol(255, 0, 0));
                    circle(buffer, decalage_base + 2 * taille_colonne + taille_colonne / 2 - 6, SCREEN_H - 75,
                           taille_colonne / 2 - 2, makecol(255, 255, 0));
                    circle(buffer, decalage_base + 3 * taille_colonne + taille_colonne / 2 - 9, SCREEN_H - 75,
                           taille_colonne / 2 - 2, makecol(0, 0, 255));
                    circle(buffer, decalage_base + 4 * taille_colonne + taille_colonne / 2 - 11, SCREEN_H - 75,
                           taille_colonne / 2 - 2, makecol(255, 128, 0));

                    int note = -1;
                    if (key[KEY_S]) note = 0;         //On assigne chaque input à un chiffre pour savoir plus facilement quelle note le joueur veut toucher
                    else if (key[KEY_D]) note = 1;
                    else if (key[KEY_F]) note = 2;
                    else if (key[KEY_G]) note = 3;
                    else if (key[KEY_H]) note = 4;

                    if (note != -1) {   //Si le joueur apppuie sur une touche
                        if (!verification_note(buffer, fond, note,  decalage)) {  //On vérifie qu'il ne l'a pas manqué
                            fin = 1;
                        }
                        circlefill(buffer, (SCREEN_W - fond->w) / 2 + note * taille_colonne + taille_colonne / 2,
                                   SCREEN_H - 75, 5, makecol(255, 255, 255));
                    }

                    if (!verification_passage(buffer))    //Si le joue a laissé passer la note
                        fin = 1;

                    blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

                    clock_t start = clock();
                    double delai = 9;       //6 de base
                    attendre(start, delai);          //Permet de laisser du délai avant de continuer de faire défiler le fond

                    decalage += 4;    //3 de base
                }
                stop_sample(son);   //fin, on arrête la musique

                clock_t end = clock();    //fin du chrono
                double duree = (double) (end - debut) / CLOCKS_PER_SEC;  //calcul de la durée (documentation trouvée sur internet sur la bibliothèque time.h

                temps[i] = duree;

                changer_couleur_pixel(fond, 0, 0);   //On remet toutes les notes dans leur couleur respectives

                FILE *pf1 = fopen(nom_fichier_perf, "a");
                fprintf(pf1, "%s Guitar Hero %lf secondes\n", joueur[i].nom, duree);   //Sauvegarde du score du joueur
                FILE *pf2 = fopen(nom_fichier_perf_ot, "r+");

                char nom[] = "Guitar Hero";
                char charge[50];

                rewind(pf2);
                while(fgets(charge, 50, pf2) != NULL){     //sauvegarde si jamais le joueur fait le highscore
                    if(charge[0] == nom[0]) {
                        double high_score = atoi(charge);
                        if(duree > high_score){
                            fprintf(pf2,"Guitar Hero %s %lf secondes", joueur[i].nom, duree);
                        }
                    }
                }

                fin_guitar_hero(buffer, fond_fond, joueur, i, duree);

                fclose(pf1);
                fclose(pf2);
            }
            if(temps[0] > temps[1])
                joueur[0].nb_tickets++;
            else
                joueur[1].nb_tickets++;

            if(a == 1){           //Si jamais le joueur qui est entré dans le jeu en premier n'était pas le premier
                joueur_g[a] = joueur[0];
                joueur_g[0] = joueur[1];
            }
            else {
                joueur_g[0] = joueur[0];
                joueur_g[1] = joueur[1];
            }

            destroy_bitmap(fond);
            destroy_bitmap(fond_fond);
            destroy_sample(son);
        }
    }
}

/// Paris Equestres ///

void refreshImageTest(t_chevaux *chevaux, BITMAP* buffer, BITMAP* decor, int c)
{
    stretch_blit(decor, buffer, 0, 0, decor->w, decor->h, 0, 0, 976, 665);
    masked_blit(chevaux[3].tab[c], buffer, 0, 0, chevaux[3].PositionX, chevaux[3].PositionY, 976, 665);
    masked_blit(chevaux[2].tab[c], buffer, 0, 0, chevaux[2].PositionX, chevaux[2].PositionY, 976, 665);
    masked_blit(chevaux[1].tab[c], buffer, 0, 0, chevaux[1].PositionX, chevaux[1].PositionY, 976, 665);
    masked_blit(chevaux[0].tab[c], buffer, 0, 0, chevaux[0].PositionX, chevaux[0].PositionY, 976, 665);
    blit(buffer, screen, 0, 0, 0, 0, 976, 665);
}

int jeuTest(t_chevaux  *chevaux, BITMAP* buffer, BITMAP* piste, const int *tempon, const int *temponCheat)
{
    int ROUGE= makecol(247, 45, 48);
    int BLEU= makecol(2, 145, 247);
    int JAUNE= makecol(255, 178, 9);
    int VERT= makecol(4, 245, 3);
    for (int i = 0; i < 5; i++)
    {
        if(*temponCheat==ROUGE)
        {
            chevaux[0].vitesse=rand()%15 + 10;
        }
        if(*tempon==ROUGE)
        {
            chevaux[0].vitesse=rand()%15 + 3;
        }
        else if(*temponCheat!=ROUGE && *tempon!=ROUGE)
        {
            chevaux[0].vitesse=rand()%15 + 3;
        }
        if(*temponCheat==BLEU)
        {
            chevaux[1].vitesse=rand()%15 + 10;
        }
        if(*tempon==BLEU)
        {
            chevaux[1].vitesse=rand()%15 + 3;
        }
        else if(*temponCheat!=BLEU && *tempon!=BLEU)
        {
            chevaux[1].vitesse=rand()%15 +3;
        }
        if(*temponCheat==JAUNE)
        {
            chevaux[2].vitesse=rand()%15 + 10;
        }
        if(*tempon==JAUNE)
        {
            chevaux[2].vitesse=rand()%15 + 3;
        }
        else if(*temponCheat!=JAUNE && *tempon!=JAUNE)
        {
            chevaux[2].vitesse=rand()%15 + 3;
        }
        if(*temponCheat==VERT)
        {
            chevaux[3].vitesse=rand()%15 + 10;
        }
        if(*tempon==VERT)
        {
            chevaux[3].vitesse=rand()%15 + 3;
        }
        else if (*temponCheat!=VERT && *tempon!=VERT)
        {
            chevaux[3].vitesse=rand()%15 + 3;
        }
        refreshImageTest(chevaux, buffer, piste, i);
        rest(150);
        chevaux[0].PositionX=chevaux[0].PositionX+(chevaux[0].vitesse+3);
        chevaux[1].PositionX=chevaux[1].PositionX+(chevaux[1].vitesse+3);
        chevaux[2].PositionX=chevaux[2].PositionX+(chevaux[2].vitesse+3);
        chevaux[3].PositionX=chevaux[3].PositionX+(chevaux[3].vitesse+3);
        if (i == 4)
        {
            i = 0;
        }
        if(chevaux[0].PositionX > 840)
        {
            chevaux[0].winner=1;
            return 1;
        }
        else if(chevaux[1].PositionX > 840)
        {
            chevaux[1].winner=1;
            return 2;
        }
        else if(chevaux[2].PositionX > 840)
        {
            chevaux[2].winner=1;
            return 3;
        }
        else if(chevaux[3].PositionX > 840)
        {
            chevaux[3].winner=1;
            return 4;
        }
    }
    return 0;
}

void ParisEquestres(personnage *joueur, BITMAP* buffer, int *tempon,int *temponCheat, BITMAP *interfacePari)
{
    int ON=0;
    int ROUGE = makecol(247, 45, 48);
    int BLEU = makecol(2, 145, 247);
    int JAUNE = makecol(255, 178, 9);
    int VERT = makecol(4, 245, 3);
    while(!ON) {
        stretch_blit(interfacePari, buffer, 0, 0, interfacePari->w, interfacePari->h, 0, 0, 976, 665);
        textprintf_ex(buffer, font, 10, 10, makecol(0, 0, 0), -1, "Pari du joueur: %s", joueur->nom);
        textprintf_ex(buffer, font, 410, 630, makecol(0, 0, 0), -1, "Cout en ticket: 1");
        textprintf_ex(buffer, font, 370, 510, makecol(0, 0, 0), -1, "Pari du joueur: ");
        if (key[KEY_0_PAD]) {
            BITMAP *cheat = load_bitmap("SeringueCHEAT.bmp", NULL);
            do {
                stretch_blit(interfacePari, buffer, 0, 0, interfacePari->w, interfacePari->h, 0, 0, 976, 665);
                masked_blit(cheat, buffer, 0, 0, mouse_x, mouse_y, 976, 665);
                textprintf_ex(buffer, font, 410, 630, makecol(0, 0, 0), -1, "Cout en ticket: 1");
                textprintf_ex(buffer, font, 370, 510, makecol(0, 0, 0), -1, "Pari du joueur: ");
                textprintf_ex(buffer, font, 10, 10, makecol(0, 0, 0), -1, "Pari du joueur: %s", joueur->nom);
                blit(buffer, screen, 0, 0, 0, 0, 976, 665);
            } while (!(mouse_b & 2) ||
                     (getpixel(screen, mouse_x, mouse_y) != ROUGE &&
                      getpixel(screen, mouse_x, mouse_y) != BLEU &&
                      getpixel(screen, mouse_x, mouse_y) != JAUNE &&
                      getpixel(screen, mouse_x, mouse_y) != VERT));
            *temponCheat = getpixel(screen, mouse_x, mouse_y);
            if(*temponCheat == ROUGE)
            {joueur->bet = 1;}
            if(*temponCheat == BLEU)
            {joueur->bet = 2;}
            if(*temponCheat == JAUNE)
            {joueur->bet = 3;}
            if(*temponCheat == VERT)
            {joueur->bet = 4;}
            textprintf_ex(screen, font, 500, 510, makecol(0, 0, 0), -1, "Tricheur");
            ON = 1;
            rest(1500);
        }
        if (mouse_b & 1) {
            *tempon = getpixel(screen, mouse_x, mouse_y);
            if (*tempon == ROUGE) {
                textprintf_ex(buffer, font, 500, 510, makecol(0, 0, 0), -1, "Cheval Rouge !");
                blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
                joueur->bet = 1;
                ON = 1;
            }
            if (*tempon == BLEU) {
                textprintf_ex(buffer, font, 500, 510, makecol(0, 0, 0), -1, "Cheval Bleu !");
                blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
                joueur->bet = 2;
                ON = 1;
            }
            if (*tempon == JAUNE) {
                textprintf_ex(buffer, font, 500, 510, makecol(0, 0, 0), -1, "Cheval Jaune !");
                blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
                joueur->bet = 3;
                ON = 1;
            }
            if (*tempon == VERT) {
                textprintf_ex(buffer, font, 500, 510, makecol(0, 0, 0), -1, "Cheval Vert !");
                blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
                joueur->bet = 4;
                ON = 1;
            }
            rest(1500);
        }
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
}

void ResultatJeuxTest(t_chevaux  *cheval, BITMAP* buffer, BITMAP* piste, int *tempon, int *temponCheat, personnage joueur[2], BITMAP* winner, int *i)
{
    switch(jeuTest(cheval, buffer, piste, tempon, temponCheat))
    {
        case 1:
            stretch_blit(winner, buffer, 0, 0, winner->w, winner->h, 0, 0, 976, 665);
            textprintf_ex(buffer, font, 380, 500, makecol(0, 0, 0), -1, "Cheval vainqueur: Rouge ! ");
            if(joueur[0].bet==1)
            {
                textprintf_ex(buffer, font, 380, 590, makecol(0, 0, 0), -1, "%s gagne 2 tickets !", joueur[0].nom);
                joueur[0].nb_tickets=joueur[0].nb_tickets+1;
            }
            else if(joueur[0].bet!=1 && *i==0)
            {
                textprintf_ex(buffer, font, 380, 590, makecol(0, 0, 0), -1, "%s perd 1 ticket !", joueur[0].nom);
                joueur[0].nb_tickets=joueur[0].nb_tickets-1;
            }
            if(joueur[1].bet==1)
            {
                textprintf_ex(buffer, font, 380, 610, makecol(0, 0, 0), -1, "%s gagne 2 tickets !", joueur[1].nom);
                joueur[1].nb_tickets=joueur[1].nb_tickets+1;
            }
            else if(joueur[1].bet!=1 && *i==1)
            {
                textprintf_ex(buffer, font, 380, 610, makecol(0, 0, 0), -1, "%s perd 1 ticket !", joueur[1].nom);
                joueur[1].nb_tickets=joueur[1].nb_tickets-1;
            }
            break;
        case 2:
            stretch_blit(winner, buffer, 0, 0, winner->w, winner->h, 0, 0, 976, 665);
            textprintf_ex(buffer, font, 380, 500, makecol(0, 0, 0), -1, "Cheval vainqueur: Bleu ! ");
            if(joueur[0].bet==2)
            {
                textprintf_ex(buffer, font, 380, 590, makecol(0, 0, 0), -1, "%s gagne 2 tickets !", joueur[0].nom);
                joueur[0].nb_tickets=joueur[0].nb_tickets+1;
            }
            else if(joueur[0].bet!=2 && *i==0)
            {
                textprintf_ex(buffer, font, 380, 590, makecol(0, 0, 0), -1, "%s perd 1 ticket !", joueur[0].nom);
                joueur[0].nb_tickets=joueur[0].nb_tickets-1;
            }
            if(joueur[1].bet==2)
            {
                textprintf_ex(buffer, font, 380, 610, makecol(0, 0, 0), -1, "%s gagne 2 tickets !", joueur[1].nom);
                joueur[1].nb_tickets=joueur[1].nb_tickets+1;
            }
            else if(joueur[1].bet!=2 && *i==1)
            {
                textprintf_ex(buffer, font, 380, 610, makecol(0, 0, 0), -1, "%s perd 1 ticket !", joueur[1].nom);
                joueur[1].nb_tickets=joueur[1].nb_tickets-1;
            }
            break;
        case 3:
            stretch_blit(winner, buffer, 0, 0, winner->w, winner->h, 0, 0, 976, 665);
            textprintf_ex(buffer, font, 380, 500, makecol(0, 0, 0), -1, "Cheval vainqueur: Jaune ! ");
            if(joueur[0].bet==3)
            {
                textprintf_ex(buffer, font, 380, 590, makecol(0, 0, 0), -1, "%s gagne 2 tickets !", joueur[0].nom);
                joueur[0].nb_tickets=joueur[0].nb_tickets+1;
            }
            else if(joueur[0].bet!=3 && *i==0)
            {
                textprintf_ex(buffer, font, 380, 590, makecol(0, 0, 0), -1, "%s perd 1 ticket !", joueur[0].nom);
                joueur[0].nb_tickets=joueur[0].nb_tickets-1;
            }
            if(joueur[1].bet==3)
            {
                textprintf_ex(buffer, font, 380, 610, makecol(0, 0, 0), -1, "%s gagne 2 tickets !", joueur[1].nom);
                joueur[1].nb_tickets=joueur[1].nb_tickets+1;
            }
            else if(joueur[1].bet!=3 && *i==1)
            {
                textprintf_ex(buffer, font, 380, 610, makecol(0, 0, 0), -1, "%s perd 1 ticket !", joueur[1].nom);
                joueur[1].nb_tickets=joueur[1].nb_tickets-1;
            }
            break;
        case 4:
            stretch_blit(winner, buffer, 0, 0, winner->w, winner->h, 0, 0, 976, 665);
            textprintf_ex(buffer, font, 380, 500, makecol(0, 0, 0), -1, "Cheval vainqueur: Vert ! ");
            if(joueur[0].bet==4)
            {
                textprintf_ex(buffer, font, 380, 590, makecol(0, 0, 0), -1, "%s gagne 2 tickets !", joueur[0].nom);
                joueur[0].nb_tickets=joueur[0].nb_tickets+1;
            }
            else if(joueur[0].bet!=4 && *i==0)
            {
                textprintf_ex(buffer, font, 380, 590, makecol(0, 0, 0), -1, "%s perd 1 ticket !", joueur[0].nom);
                joueur[0].nb_tickets=joueur[0].nb_tickets-1;
            }
            if(joueur[1].bet==4)
            {
                textprintf_ex(buffer, font, 380, 610, makecol(0, 0, 0), -1, "%s gagne 2 tickets !", joueur[1].nom);
                joueur[1].nb_tickets=joueur[1].nb_tickets+1;
            }
            else if(joueur[1].bet!=4 && *i==1)
            {
                textprintf_ex(buffer, font, 380, 610, makecol(0, 0, 0), -1, "%s perd 1 ticket !", joueur[1].nom);
                joueur[1].nb_tickets=joueur[1].nb_tickets-1;
            }
            break;
    }
    for(int p=0; p<4; p++){
    if(cheval[p].winner==1)
    {
        masked_blit(cheval[p].tab[0], buffer, 0, 0, 400, 335, 976, 665);
    }}
    blit(buffer, screen, 0, 0, 0, 0, 976, 665);
    rest(5000);
    clear(buffer);
}

void course_equestre(personnage joueur[2], BITMAP *buffer, int *i)
{
    if ((joueur[*i].x <= 140 && joueur[*i].x >= 120) && (joueur[*i].y >= 360 && joueur[*i].y <= 380) && joueur[*i].direction == 3 && key[KEY_ENTER] && joueur[*i].nb_tickets>0)
    {
        int tempon = 0;
        int temponCheat = 0;
        t_chevaux cheval[4];
        cheval[0].PositionY=350;
        cheval[0].PositionX=0;
        cheval[0].tab[0]=load_bitmap("CHEVALR0.bmp", NULL);
        cheval[0].tab[1]=load_bitmap("CHEVALR1.bmp", NULL);
        cheval[0].tab[2]=load_bitmap("CHEVALR2.bmp", NULL);
        cheval[0].tab[3]=load_bitmap("CHEVALR3.bmp", NULL);
        cheval[0].tab[4]=load_bitmap("CHEVALR4.bmp", NULL);
        cheval[1].PositionY=300;
        cheval[1].PositionX=0;
        cheval[1].tab[0]=load_bitmap("CHEVALB0.bmp", NULL);
        cheval[1].tab[1]=load_bitmap("CHEVALB1.bmp", NULL);
        cheval[1].tab[2]=load_bitmap("CHEVALB2.bmp", NULL);
        cheval[1].tab[3]=load_bitmap("CHEVALB3.bmp", NULL);
        cheval[1].tab[4]=load_bitmap("CHEVALB4.bmp", NULL);
        cheval[2].PositionY=250;
        cheval[2].PositionX=0;
        cheval[2].tab[0]=load_bitmap("CHEVALJ0.bmp", NULL);
        cheval[2].tab[1]=load_bitmap("CHEVALJ1.bmp", NULL);
        cheval[2].tab[2]=load_bitmap("CHEVALJ2.bmp", NULL);
        cheval[2].tab[3]=load_bitmap("CHEVALJ3.bmp", NULL);
        cheval[2].tab[4]=load_bitmap("CHEVALJ4.bmp", NULL);
        cheval[3].PositionY=200;
        cheval[3].PositionX=0;
        cheval[3].tab[0]=load_bitmap("CHEVALV0.bmp", NULL);
        cheval[3].tab[1]=load_bitmap("CHEVALV1.bmp", NULL);
        cheval[3].tab[2]=load_bitmap("CHEVALV2.bmp", NULL);
        cheval[3].tab[3]=load_bitmap("CHEVALV3.bmp", NULL);
        cheval[3].tab[4]=load_bitmap("CHEVALV4.bmp", NULL);
        for(int m=0; m<4; m++)
        {
            cheval[m].winner=0;
        }
        clear(buffer);
        BITMAP *piste = load_bitmap("piste.bmp", NULL);
        stretch_blit(piste, buffer, 0, 0, piste->w, piste->h, 0, 0, 976, 665);
        masked_blit(cheval[3].tab[0], buffer, 0, 0, cheval[3].PositionX, cheval[3].PositionY, 976, 665);
        masked_blit(cheval[2].tab[0], buffer, 0, 0, cheval[2].PositionX, cheval[2].PositionY, 976, 665);
        masked_blit(cheval[1].tab[0], buffer, 0, 0, cheval[1].PositionX, cheval[1].PositionY, 976, 665);
        masked_blit(cheval[0].tab[0], buffer, 0, 0, cheval[0].PositionX, cheval[0].PositionY, 976, 665);
        blit(buffer, screen, 0, 0, 0, 0, 976, 665);
        rest(2000);
        BITMAP *interfacePari= load_bitmap("fenetrePari.bmp", NULL);
        for(int j = 0 ; j < 2 ; j++) {
            ParisEquestres(&joueur[j], buffer, &tempon, &temponCheat, interfacePari);
        }
        BITMAP *winner= load_bitmap("winnerCourse.bmp", NULL);
        ResultatJeuxTest(cheval, buffer, piste, &tempon, &temponCheat, joueur, winner, i);
        supprimer_chevaux(cheval, piste, interfacePari, winner);
        *i = !(*i);
    }
}

void supprimer_chevaux(t_chevaux  *cheval,  BITMAP* piste, BITMAP *interfacePari, BITMAP *ecranWin)
{
    destroy_bitmap(piste);
    destroy_bitmap(interfacePari);
    destroy_bitmap(ecranWin);
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<5; j++)
            destroy_bitmap(cheval[i].tab[j]);
    }
}
/*
void check_position(personnage *joueur)
{
    if ((joueur->x <= 140 && joueur->x >= 120) && (joueur->y >= 360 && joueur->y <= 380) && joueur->direction == 3 && key[KEY_ENTER])
    {
        allegro_message("Tu es arrivé à la position");
        rest(2000);
    }
    else
    {
        printf("\nPosition X: %d", joueur->x);
        printf("\nPosition Y: %d", joueur->y);
    }
}
 */

/// JEU DE LA TAUPE ///


void jeu_taupe(personnage joueur_g[], int *c, BITMAP *buffer, BITMAP *fond, BITMAP *cursor, BITMAP *taupe, BITMAP *ticket,const char *nom_fichier_perf, const char *nom_fichier_perf_ot)
{
    int z = *c;
    if ((joueur_g[*c].x <= 300 && joueur_g[*c].x >= 280) && (joueur_g[*c].y >= 200 && joueur_g[*c].y <= 220) && joueur_g[*c].direction == 3 && key[KEY_ENTER])
    {
        rest(1000);
        int stop = 0;
        BITMAP *attrape_taupe = charger_image("attrape_taupe.bmp");
        do {
            blit (attrape_taupe, buffer, 0,0,0,0, attrape_taupe -> w, attrape_taupe -> h);
            rectfill(buffer, 0, 549, SCREEN_W, SCREEN_H, makecol(0, 0, 0));
            textprintf_ex(buffer, font, 10, 560, makecol(255, 255, 0), -1, "BIENVENUE DANS ATTRAPE TAUPE %s", joueur_g[*c].nom);
            textout_ex(buffer, font, "Appuyez sur ENTRER pour commencer le jeu", 18,585, makecol(255, 255, 255), -1);
            textout_ex(buffer, font, "Appuyez sur RETOUR pour quitter", 18, 630, makecol(255, 255, 255), -1);
            textout_ex(buffer, font, "COUT : ", 750, 605, makecol(255, 255, 0), -1);
            masked_blit(ticket, buffer , 0, 0, 770, 540, ticket->w, ticket->h);
            blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
            rest (500);
        }while (!key[KEY_ENTER] && !key[KEY_BACKSPACE]);
        destroy_bitmap(attrape_taupe);

        if(key[KEY_BACKSPACE])
            stop = 1;

        //ajoute ici ton code pour l'écran de démarrage du jeu.
        if(!stop) {
            clear(buffer);
            int score[2];
            for (int s = 0; s < 2; s++)
                score[s] = 0;
            int a = 0;
            int b = 0;

            personnage joueur[2];

            if (*c == 0) {
                joueur_g[*c].nb_tickets--;
                joueur[0] = joueur_g[0];
                joueur[1] = joueur_g[1];
                *c = 1;
            } else {
                joueur_g[*c].nb_tickets--;
                joueur[0] = joueur_g[*c];
                joueur[1] = joueur_g[0];
                *c = 0;
            }
            BITMAP *cursor1 = charger_image("marteau1.bmp");
            BITMAP *tab_marteau[2] = {cursor, cursor1};

            for (int i = 0; i < 2; i++) {


                clear_to_color(buffer, makecol(0, 0, 0));
                // blit(fond_charge, buffer, 0, 0, 0, 0, fond_charge->w, fond_charge->h);
                textprintf_ex(buffer, font, SCREEN_W / 2 - 100, SCREEN_H / 2, makecol(255, 255, 255), -1,
                              "C'EST LE TOUR DE %s", joueur[i].nom);
                blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
                rest(2000);
                time_t start_time = time(NULL);
                int chrono = 0;

                t_taupe taupe1[3] = {{385-10, 255-15},
                                     {105-10, 370-15},
                                     {70-10,  285-15}};
                t_taupe taupe2[3] = {{390-10, 435-15},
                                     {686-10, 290-15},
                                     {655-10, 385-15}};

                t_taupe taupe1_orig[3] = {{385-10, 255-15}, {105-10, 370-15}, {70-10,  285-15}};
                t_taupe taupe2_orig[3] = {{390-10, 435-15}, {686-10, 290-15}, {655-10, 385-15}};

                time_t clic = 0, clic2 = 0, randa = 0, randb = 0, shesh = 0;

                while (chrono != (25)) {

                    stretch_blit(fond, buffer, 0, 0, fond->w, fond->h, 0, 0, SCREEN_W, SCREEN_H);
                    if (difftime(time(NULL), randa) >= 2) {
                        a = rand() % 3;
                        if (difftime(time(NULL), randa) >= 2) {
                            a = rand() % 3;
                            taupe1[a].x = taupe1_orig[a].x;
                            taupe1[a].y = taupe1_orig[a].y;
                            randa = time(NULL);
                        }
                        randa = time(NULL);
                    }

                    if (difftime(time(NULL), randb) >= 2) {
                        b = rand() % 3;
                        taupe2[b].x = taupe2_orig[b].x;
                        taupe2[b].y = taupe2_orig[b].y;
                        randb = time(NULL);
                    }

                    if (difftime(time(NULL), clic) >= 2 + rand() % 2) {
                        draw_sprite(buffer, taupe, taupe1[a].x, taupe1[a].y);
                        clic = 0;
                    }

                    if (difftime(time(NULL), clic2) >= 1) {
                        draw_sprite(buffer, taupe, taupe2[b].x, taupe2[b].y);
                        clic2 = 0;
                    }

                    if (difftime(time(NULL), shesh) >= 1) {
                        chrono++;
                        shesh = time(NULL);
                    }
                    masked_blit(cursor, buffer, 0, 0, mouse_x, mouse_y - cursor->h / 2, cursor->w, cursor->h);
                    textprintf_ex(buffer, font, 580, 15, makecol(0, 0, 0), -1, "score :%d", score[i]);
                    textprintf_ex(buffer, font, 150, 15, makecol(0, 0, 0), -1, "chrono :%d secondes ", chrono);

                    if (mouse_b & 1) { // Clic droit de la souris
                        if (difftime(time(NULL), clic) >= 1 + rand() % 2 && mouse_x >= taupe1[a].x &&
                            mouse_x <= taupe1[a].x + taupe->w && mouse_y >= taupe1[a].y &&
                            mouse_y <= taupe1[a].y + taupe->h) {
                            for (int k = 0; k < 2; k++) {
                                masked_blit(tab_marteau[k], buffer, 0, 0, mouse_x, mouse_y - cursor->h / 2, SCREEN_W,
                                            SCREEN_H);
                                blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
                                rest(90);
                            }
                            taupe1[a].y = 1000; // Déplace la taupe hors de l'écran
                            clic = time(NULL);
                            score[i]++; // Augmenter le score de 1
                        }

                        if (difftime(time(NULL), clic2) >= 1 && mouse_x >= taupe2[b].x &&
                            mouse_x <= taupe2[b].x + taupe->w && mouse_y >= taupe2[b].y && mouse_y <= taupe2[b].y +
                                                                                                      taupe->h) {// Vérifier si deux secondes se sont écoulées depuis le dernier clic droit
                            for (int j = 0; j < 2; j++) {
                                masked_blit(tab_marteau[j], buffer, 0, 0, mouse_x, mouse_y, SCREEN_W, SCREEN_H);
                                blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
                                rest(90);
                            }
                            taupe2[b].y = 1000;
                            clic2 = time(NULL);
                            score[i]++;
                        }
                    }
                    blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
                }
                clear(buffer);
                while (!key[KEY_BACKSPACE]) {
                    textprintf_ex(buffer, font, SCREEN_W / 2 - 150, SCREEN_H / 2, makecol(255, 255, 0), -1,
                                  "%s a frappe %d taupes en 25 secondes", joueur[i].nom, score[i]);
                    textout_ex(buffer, font, "Appuyez sur RETOUR pour continuer", SCREEN_W - 265, SCREEN_H - 15,
                               makecol(255, 255, 0), -1);
                    blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
                }
            }
            if (score[0] > score[1])
                joueur[0].nb_tickets++;
            else if (score[0] == score[1]) {
                joueur[1].nb_tickets++;
                joueur[0].nb_tickets++;
            } else
                joueur[1].nb_tickets++;

            if (z == 1) {
                joueur_g[1] = joueur[0];
                joueur_g[0] = joueur[1];
            } else {
                joueur_g[0] = joueur[0];
                joueur_g[1] = joueur[1];
            }

            destroy_bitmap(cursor1);
        }
    }
}




/// Traversée de rivière ///

void initialisation_rondin(rondin *rd, BITMAP *rondin, int offset_x){   //Permet d'initialiser une instance de la structure
                                                                        //de rondin
    rd->img = rondin;
    rd->w = rondin->w;
    rd->h = rondin->h;
    int r = rand()%7;

    if(r == 0)         //La position de départ du rondin est aléatoire
        rd->x = 0;
    else if(r == 1)
        rd->x = SCREEN_W/2;
    else if(r == 2)
        rd->x = SCREEN_W /2 + 150;
    else if(r == 3)
        rd->x = 100;
    else if(r == 4)
        rd->x = 200;
    else if(r == 5)
        rd->x = 650;
    else if(r == 6)
        rd->x = 300;

    rd->y = SCREEN_H - 130 - offset_x * rd->h;
    rd->depx = offset_x;        //plus on avance dans la traversée plus les rondins vont vite

    if(offset_x >= 10)
        rd->depx = offset_x - 7;
    if(offset_x%2 == 0)
        rd->depx = 0 - rd->depx;
    if(offset_x == 0) {
        rd->depx = -2;
        rd->x = 500;
    }
    if(offset_x == 1)  rd->x = 10;
}

void afficher_rondin(rondin *rd, BITMAP *buffer){
    draw_sprite(buffer, rd->img, rd->x, rd->y);  //simple affichage du rondin à ses coordonnées
}

void actualiser_rondin(rondin *rd){
    rd->x = rd->x + rd->depx;
    if(rd->x > SCREEN_W + 1)    //s'il dépasse l'écran alors il revient à l'opposé
        rd->x = 0;
    if(rd->x < (0 - rd->w))       //s'il dépasse l'écran à gauche alors il revient à l'opposé
        rd->x = SCREEN_W - rd->w;
}

void actualisation_personnage_traversee(personnage *perso, BITMAP *buffer){    //quasiment pareil que la fonction de déplacement pour la carte
    int vitesse;
    if(key[KEY_E])
        vitesse = 4;
    else vitesse = 2;
    perso->deplacement = 0;
    if (key[KEY_UP]) {
        perso->direction = 3;
        perso->deplacement = 1;
        perso->y -= vitesse;
    }
    if (key[KEY_DOWN]) {
        perso->direction = 0;
        perso->deplacement = 1;
        perso->y += vitesse;
    }
    if (key[KEY_LEFT]) {
        perso->direction = 1;
        perso->deplacement = 1;
        perso->x -= vitesse;
    }
    if (key[KEY_RIGHT]) {
        perso->direction = 2;
        perso->deplacement = 1;
        perso->x += vitesse;
    }

}

int verification(personnage *perso, rondin *rd, BITMAP *buffer){      //pour vérifier que le joueur n'est pas tombé dans l'eau
    int eau = makecol(100, 155, 255);
    int vitesse;
    if(key[KEY_E])
        vitesse = 4;
    else vitesse = 2;

    if(perso->y + perso->haut[0]->h <= SCREEN_H -102) {
        if ((perso->x + 25 >= rd->x && perso->x + 25 <= rd->x + rd->w) &&
            (perso->y + perso->haut[0]->h >= rd->y && perso->y + perso->haut[0]->h <= rd->y + rd->h))    //Le personnage est sur un rondin
            perso->x = rd->x + 20;
        if (getpixel(buffer, perso->x + 25, (perso->y + perso->haut[0]->h)) == eau)    //le personnage tombe dans l'eau
            return 1;
        if (perso->x + 25 > SCREEN_W || perso->x + 25 < 0)     //le personnage est emporté hors de l'écran par un rondin
            return 1;
    }
    else {
        if (perso->x + 40 >= SCREEN_W)
            perso->x -= vitesse;
        else if(perso->x + 20 <= 0)
            perso->x += vitesse;
    }
    if(perso->y + perso->haut[0]->h >= SCREEN_H)
        perso->y -= vitesse;

    return 0;
}

void victoire(BITMAP *buffer, personnage *joueur, double temps){
    while(!key[KEY_BACKSPACE]) {
        rectfill(buffer, 0, 100, SCREEN_W, SCREEN_H - 101, makecol(100, 155, 255));
        textprintf_ex(buffer, font, SCREEN_W / 2 - 200, SCREEN_H / 2, makecol(0, 0, 0), -1, "Victoire de %s en %lf secondes ! + 1 ticket",
                      joueur->nom, temps);
        textout_ex(buffer, font, "Appuyez sur RETOUR pour continuer", SCREEN_W - 300, SCREEN_H - 15,
                   makecol(255, 0, 0), -1);
        blit(buffer, screen, 0, 0, 0, 0, buffer->w, buffer->h);
    }
}

void defaite(BITMAP *buffer, personnage *joueur){
    while(!key[KEY_BACKSPACE]) {
        rectfill(buffer, 0, 100, SCREEN_W, SCREEN_H - 101, makecol(100, 155, 255));
        textprintf_ex(buffer, font, SCREEN_W / 2 - 45, SCREEN_H / 2, makecol(0, 0, 0), -1, "Defaite de %s !",
                      joueur->nom);
        textout_ex(buffer, font, "Appuyez sur RETOUR pour continuer", SCREEN_W - 300, SCREEN_H - 15,
                   makecol(0, 0, 0), -1);
        blit(buffer, screen, 0, 0, 0, 0, buffer->w, buffer->h);
    }
}

int titre(BITMAP *buffer, personnage *joueur, BITMAP *grass, BITMAP *ticket){
    do {
        stretch_blit(grass, buffer, 0, 0, grass->w, grass->h, 0, 0, SCREEN_W, SCREEN_H);
        rectfill(buffer, 0, 100, SCREEN_W, SCREEN_H - 101, makecol(100, 155, 255));
        rectfill(buffer, 0, SCREEN_H - 101, SCREEN_W, SCREEN_H - 90, makecol(64, 64, 64));
        rectfill(buffer, 0, 90, SCREEN_W, 100, makecol(64, 64, 64));
        rectfill(buffer, 9, 435, SCREEN_W - 9 , SCREEN_H - 105, makecol(255, 255, 255));

        textprintf_ex(buffer, font, 18, 445, makecol(0, 0, 0), -1,
                      "BIENVENUE DANS LA TRAVERSEE DE LA RIVIERE %s",
                      joueur->nom);
        textout_ex(buffer, font, "Appuyez sur ENTRER pour commencer", 18, 500, makecol(0, 0, 0), -1);
        textout_ex(buffer, font, "Appuyez sur RETOUR pour quitter", 18, 545, makecol(0, 0, 0), -1);
        textout_ex(buffer, font, "COUT : ", 750, 510, makecol(0, 0, 0), -1);
        masked_blit(ticket, buffer, 0, 0, 770, 445, ticket->w, ticket->h);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }while(!key[KEY_ENTER] && !key[KEY_BACKSPACE]);

    if(key[KEY_BACKSPACE])
        return 1;
    else
        return 0;
}

void traversee_riviere(personnage joueur_g[], int *b, BITMAP*buffer, BITMAP *ticket, const char *nom_fichier_perf, const char *nom_fichier_perf_ot){

    int a = *b;
    if((joueur_g[a].x -10 >=392 && joueur_g[a].x -10 <= 450) && (joueur_g[a].y + joueur_g[a].haut[0]->h -10 >= 585 && joueur_g[a].y + joueur_g[a].haut[0]->h -10 <= 600) && key[KEY_ENTER]) {

        BITMAP *grass = charger_image("grass.bmp");
        rest(1000);
        int stop = titre(buffer, &joueur_g[a], grass, ticket);

        if(!stop) {
            personnage joueur[2];

            if(*b == 0) {          //Pareil que pour guitar Hero, on vérifie quel joueur est entré en premier pour le faire joueur en premier
                joueur_g[*b].nb_tickets--;
                joueur[0] = joueur_g[0];
                joueur[1] = joueur_g[1];
                *b = 1;
            }
            else {
                joueur_g[*b].nb_tickets--;
                joueur[0] = joueur_g[*b];
                joueur[1] = joueur_g[0];
                *b = 0;
            }


            rondin tab[NB_RONDINS];
            int compteur_instant = 0;
            int fin = 0;
            int victory = 0;

            BITMAP *grass = charger_image("grass.bmp");
            BITMAP *rondin_img = charger_image("rondin.bmp");

            for(int i = 0 ; i < 2 ; i++) {
                fin = 0;
                victory = 0;
                for (int j = 0; j < NB_RONDINS; j++)
                    initialisation_rondin(&tab[j], rondin_img, j);

                joueur[i].x = SCREEN_W / 2 - 20;
                joueur[i].y = SCREEN_H - 120;

                clear(buffer);

                clock_t debut = clock();

                while (!fin) {
                    stretch_blit(grass, buffer, 0, 0, grass->w, grass->h, 0, 0, SCREEN_W, SCREEN_H);
                    rectfill(buffer, 0, 100, SCREEN_W, SCREEN_H - 101, makecol(100, 155, 255));
                    rectfill(buffer, 0, SCREEN_H - 101, SCREEN_W, SCREEN_H - 90, makecol(64, 64, 64));
                    rectfill(buffer, 0, 90, SCREEN_W, 100, makecol(64, 64, 64));

                    actualisation_personnage_traversee(&joueur[i], buffer);

                    for (int k = 0; k < NB_RONDINS; k++) {
                        actualiser_rondin(&tab[k]);
                        afficher_rondin(&tab[k], buffer);
                        fin = verification(&joueur[i], &tab[k], buffer);
                    }
                    afficher_personnage(&joueur[i], buffer, 1);

                    compteur_instant++;
                    if (compteur_instant >= 5) {         //Pour éviter un défilement trop rapide lors de l'animation
                        if (joueur[i].deplacement)
                            joueur[i].instant = (joueur[i].instant + 1) % 3;
                        else
                            joueur[i].instant = 0;
                        compteur_instant = 0;
                    }

                    if (joueur[i].y + joueur[i].haut[0]->h <= 89) {
                        victory = 1;
                        fin = 1;
                    }

                    blit(buffer, screen, 0, 0, 0, 0, buffer->w, buffer->h);
                    rest(20);
                }
                clock_t end = clock();
                double duree = (double) (end - debut) / CLOCKS_PER_SEC;

                FILE *pf1 = fopen(nom_fichier_perf, "a");
                FILE *pf2 = fopen(nom_fichier_perf_ot, "r+");

                fprintf(pf1, "%s Riviere %lf secondes\n", joueur[i].nom, duree);  //Sauvegarde du score du joueur

                if (victory) {
                    victoire(buffer, &joueur[i], duree);
                    joueur[i].nb_tickets++;
                } else
                    defaite(buffer, &joueur[i]);

                if(a == 1){
                    joueur_g[a] = joueur[0];
                    joueur_g[0] = joueur[1];
                }
                else{
                    joueur_g[0] = joueur[0];
                    joueur_g[1] = joueur[1];
                }
            }

            destroy_bitmap(rondin_img);
        }
        destroy_bitmap(grass);
    }
}


/// Peche aux canards ///

int verifier_position(t_canard* canards, int nb_canards, int x, int y) {
    for (int i = 0; i < nb_canards; i++) {
        if (canards[i].vivant && canards[i].x == x && canards[i].y == y) {
            return 0;
        }
    }
    return 1;
}


void initialiser_canard(t_canard canards[], int nb_canards, int id, BITMAP *sprite, int vitesse) {
    int x, y;
    do {
        x = 75 + rand() % (850 - 75 + 1);
        y = 341 + rand() % (532 - 341 + 1);
    } while (!verifier_position(canards, nb_canards, x, y));
    canards[id].x = x;
    canards[id].y = y;
    canards[id].vitesse = vitesse;
    canards[id].sprite = sprite;
    canards[id].vivant = 1;
}

void mettre_a_jour_canard(t_canard* canard){
    if (canard->vivant) {
        canard->x += canard->vitesse;
        if (canard->x > 850)
            canard->x = 75;
    }
}

void dessiner_canard(t_canard* canard, BITMAP* buffer) {
    if (canard->vivant)
        draw_sprite(buffer, canard->sprite, canard->x, canard->y);
}

void pecher_canard(t_canard* canard, int* score) {
    if (canard->vivant) {
        if ((mouse_b & 1)) {
            if (mouse_x >= canard->x && mouse_x <= canard->x + LARGEUR_CANARD &&
                mouse_y >= canard->y && mouse_y <= canard->y + HAUTEUR_CANARD) {
                canard->vivant = 0;
                (*score)++;
            }
        }
    }
}

void afficher_intro() {
    BITMAP* intro = create_bitmap(LARGEUR_ECRAN, HAUTEUR_ECRAN);
    clear_to_color(intro, makecol(0,0,0));
    blit(intro, screen, 0, 0, 0, 0, LARGEUR_ECRAN, HAUTEUR_ECRAN);
    textout_centre_ex(screen, font, "L'OBJECTIF EST DE TOUCHER LE PLUS DE CANARD EN 20 SECONDES", LARGEUR_ECRAN / 2, HAUTEUR_ECRAN / 2 - 20, makecol(255,255,255), -1);
    textout_centre_ex(screen, font, "Appuyer sur la touche ENTRER pour lancer la partie", LARGEUR_ECRAN / 2, HAUTEUR_ECRAN / 2, makecol(255,255,255), -1);
    destroy_bitmap(intro);

    while (!key[KEY_ENTER]) {
        rest(100);
    }
}


void mise_a_jour_jeu(t_canard* canards, int* score, BITMAP* buffer, BITMAP* fond, int* chrono, time_t* temps) {
    clear_bitmap(buffer);
    stretch_blit(fond, buffer, 0, 0, fond->w, fond->h, 0, 0, SCREEN_W, SCREEN_H);

    for (int i = 0; i < 10; i++) {
        mettre_a_jour_canard(&canards[i]);
        pecher_canard(&canards[i],score);
    }

    for (int i = 0; i < 10; i++) {
        dessiner_canard(&canards[i], buffer);
    }

    if(difftime(time(NULL), *temps)>=1) {
        (*chrono)++;
        *temps=time(NULL);
    }

    textprintf_ex(buffer, font, 300, 200, makecol(255, 255, 255), -1, "Score: %d", *score);
    textprintf_ex(buffer, font, 300, 210, makecol(255, 255, 255), -1, "Temps restant: %d secondes ", 20 - *chrono);

    blit(buffer, screen, 0, 0, 0, 0, LARGEUR_ECRAN, HAUTEUR_ECRAN);
}


int tous_les_canards_peches(t_canard canards[]) {
    for (int i = 0; i < 10; i++) {
        if (canards[i].vivant) {
            return 0;
        }
    }
    return 1;
}

void reinitialiser_canards(t_canard* canards, int* vitesse_actuelle, BITMAP *sprite) {
    (*vitesse_actuelle)++;
    for (int i = 0; i < 10; i++) {
        initialiser_canard(canards, 10, i, sprite, *vitesse_actuelle);
    }
}

void peche_canard(personnage joueur_g[], int *c,  BITMAP* buffer, BITMAP* ticket,const char *nom_fichier_perf, const char *nom_fichier_perf_ot) {

    int z = *c;
    if ((joueur_g[*c].x <= 220 && joueur_g[*c].x >= 180) && (joueur_g[*c].y >= 200 && joueur_g[*c].y <= 220) &&
        joueur_g[*c].direction == 3 && key[KEY_ENTER]) {
        rest(1000);
        int stop = 0;
        BITMAP *pokemon = charger_image("canard_pokemon.bmp");
        do {
            blit (pokemon, buffer, 0,0,0,0, pokemon -> w, pokemon -> h);
            rectfill(buffer, 0, 549, SCREEN_W, SCREEN_H, makecol(0, 0, 0));
            textprintf_ex(buffer, font, 10, 560, makecol(255, 255, 0), -1, "BIENVENUE DANS LA PECHE AUX CANARDS %s", joueur_g[*c].nom);
            textout_ex(buffer, font, "Appuyez sur ENTRER pour commencer le jeu", 18,585, makecol(255, 255, 255), -1);
            textout_ex(buffer, font, "Appuyez sur RETOUR pour quitter", 18, 630, makecol(255, 255, 255), -1);
            textout_ex(buffer, font, "COUT : ", 750, 605, makecol(255, 255, 0), -1);
            masked_blit(ticket, buffer , 0, 0, 770, 540, ticket->w, ticket->h);
            blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
            rest (500);
        }while (!key[KEY_ENTER] && !key[KEY_BACKSPACE]);
        destroy_bitmap(pokemon);

        if (key[KEY_BACKSPACE])
            stop = 1;

        if (!stop) {
            int score[2];
            for (int s = 0; s < 2; s++)
                score[s] = 0;

            int a = 0;
            int b = 0;

            personnage joueur[2];
            joueur_g[*c].nb_tickets--;
            if (*c == 0) {
                joueur[0] = joueur_g[0];
                joueur[1] = joueur_g[1];
                *c = 1;
            } else {

                joueur[0] = joueur_g[*c];
                joueur[1] = joueur_g[0];
                *c = 0;
            }

            BITMAP *fond = charger_image("fond_canard.bmp");
            BITMAP *canard = charger_image("canard.bmp");




            for(int i=0;i<2;i++) {

                clear_to_color(buffer, makecol(0, 0, 0));
                // blit(fond_charge, buffer, 0, 0, 0, 0, fond_charge->w, fond_charge->h);
                textprintf_ex(buffer, font, SCREEN_W / 2 - 100, SCREEN_H / 2, makecol(255, 255, 255), -1,
                              "C'EST LE TOUR DE %s", joueur[i].nom);
                blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
                rest(2000);
                t_canard canards[10];

                int vitesse_actuelle = VITESSE_CANARD;

                for (int j = 0; j < 10; j++) {
                    initialiser_canard(canards, 10, j, canard, vitesse_actuelle);
                }

                //int score = 0;

                afficher_intro();

                time_t temps = 0;
                int chrono = 0;

                while (chrono != 20) {

                    mise_a_jour_jeu(canards, &score[i], buffer, fond, &chrono, &temps);


                    if (tous_les_canards_peches(canards)) {
                        reinitialiser_canards(canards, &vitesse_actuelle, canard);
                    }

                    rest(1);
                }
                while (!key[KEY_BACKSPACE]) {
                    clear_to_color(buffer, makecol(0, 0, 0));
                    textprintf_ex(buffer, font, SCREEN_W / 2 - 150, SCREEN_H / 2, makecol(255, 255, 0), -1,
                                  "%s a capturé %d  canards en 20 secondes", joueur[i].nom, score[i]);
                    textout_ex(buffer, font, "Appuyez sur RETOUR pour continuer", SCREEN_W - 265, SCREEN_H - 15,
                               makecol(255, 255, 0), -1);
                    blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
                }

            }
            if (score[0] > score[1])
                joueur[0].nb_tickets++;
            else if (score[0] == score[1]) {
                    joueur[1].nb_tickets++;
                    joueur[0].nb_tickets++;
            }
            else if(score[0] < score[1])
                joueur[1].nb_tickets++;

            if (z == 1) {
                joueur_g[1] = joueur[0];
                joueur_g[0] = joueur[1];
            } else {
                joueur_g[0] = joueur[0];
                joueur_g[1] = joueur[1];
            }
            destroy_bitmap(canard);
            destroy_bitmap(fond);
        }

    }
}

/// Pong ///
void pong (personnage joueur_g[], int *b, BITMAP *page, BITMAP *ticket,  const char *nom_fichier_perf, const char *nom_fichier_perf_ot){
    int a = *b;
    int stop = 0;

    if ((joueur_g[a].x <= 610 && joueur_g[a].x >= 520) && (joueur_g[a].y >= 200 && joueur_g[a].y <= 230)&& joueur_g[a].direction == 3 && key[KEY_ENTER]) {
        BITMAP *pong_accueil = charger_image("pong_accueil.bmp");

        do {
            blit (pong_accueil, page, 0,0,0,0, pong_accueil -> w, pong_accueil -> h);
            rectfill(page, 0, 549, SCREEN_W, SCREEN_H, makecol(0, 0, 0));
            textprintf_ex(page, font, 10, 560, makecol(255, 255, 0), -1, "BIENVENUE DANS PONG %s", joueur_g[a].nom);
            textout_ex(page, font, "Appuyez sur ENTRER pour commencer le jeu", 18,585, makecol(255, 255, 255), -1);
            textout_ex(page, font, "Appuyez sur RETOUR pour quitter", 18, 630, makecol(255, 255, 255), -1);
            textout_ex(page, font, "COUT : ", 750, 605, makecol(255, 255, 0), -1);
            masked_blit(ticket, page , 0, 0, 770, 540, ticket->w, ticket->h);
            blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
            rest (500);
        }while (!key[KEY_ENTER] && !key[KEY_BACKSPACE]);
        destroy_bitmap(pong_accueil);

        if (key [KEY_BACKSPACE])
            stop = 1;

        if (!stop) {
            BITMAP * pg_chgm= charger_image("pong_chargement.bmp");
            BITMAP *pong_result = charger_image("pong_resultat.bmp");
            int fini = 0;
            double temps [2];
            personnage joueur_tab [2];
            clock_t debut, fin;
            double temps_execution;

            if(*b == 0) {
                joueur_g[*b].nb_tickets--;
                joueur_tab[0] = joueur_g[0];
                joueur_tab[1] = joueur_g[1];
                *b = 1;
            }
            else {
                joueur_g[*b].nb_tickets--;
                joueur_tab[0] = joueur_g[*b];
                joueur_tab[1] = joueur_g[0];
                *b = 0;
            }

            // paramètres de l'élément à animer
            int posx, posy;    // coordonnées
            int tx, ty;        // taille (largeur et hauteur)
            int deplacement;  // amplitude absolu des déplacements

            // paramètres de l'élément 1 à animer (disque)
            int posx1, posy1;    // coordonnées
            int rayon1;          // taille (rayon du disque)
            int depx1, depy1;    // vecteur déplacement effectif en x et y

            int cpt = 1;

            for (int i = 0; i<2 ; i ++){
                clear (page);
                blit(pg_chgm, page, 0, 0, 0, 0, pg_chgm->w, pg_chgm->h);
                textprintf_ex(page , font, SCREEN_W/2 -100, 560, makecol(255, 255, 0), -1, "C'EST LE TOUR DE %s", joueur_g[i].nom);
                blit(page , screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
                rest(1000);
                fini = 0;

                rayon1 = 30;
                posx1 = rayon1;
                posy1 = rayon1;
                depx1 = 5;
                depy1 = 9;

                // tailles et position initiale au centre
                tx = 90;
                ty = 30;
                posx = SCREEN_W / 2 - tx / 2;
                posy = SCREEN_H - ty;

                // mouvements :
                // se déplacera de 5 pixels à chaque étape de déplacement
                deplacement = 15;

                /// Début clock ///
                debut = clock();

                while (!fini){
                        clear_bitmap(page);
                        // 1) EFFACER POSITION ACTUELLE (redessiner à la couleur du fond)
                        rectfill(page, posx, posy, posx + tx, posy + ty, makecol(0, 0, 0));

                        // 2) DETERMINER NOUVELLE POSITION
                        // contrôle des bords pour la barre rouge
                        if (posx + tx < 0) posx = SCREEN_W + posx + tx;
                        if (posx >= SCREEN_W) posx = posx - SCREEN_W - tx;

                        // déplacement de la boule sur l'écran
                        if ((posx1 - rayon1 < 0 && depx1 < 0) || (posx1 + rayon1 > SCREEN_W && depx1 > 0))
                            depx1 = -depx1;
                        if ((posy1 - rayon1 < 0 && depy1 < 0) || (posy1 + rayon1 > SCREEN_H && depy1 < 0))
                            depy1 = -depy1;

                        // Vérifier si la boule touche la barre
                        if (posy1 + rayon1 >= posy && posy1 - rayon1 <= posy + ty) {
                            if (posx1 + rayon1 >= posx && posx1 - rayon1 <= posx + tx) {
                                depy1 = -depy1;  // Inverser la direction verticale (rebond sur la barre)
                                cpt++;
                            }
                        }

                        // nouvelle position
                        posx1 = posx1 + (cpt)*depx1;
                        posy1 = posy1 + (cpt)*depy1;

                        // prise en compte du clavier : déplacement direct de la position
                        if (key[KEY_LEFT])
                            posx = posx - deplacement * cpt; // mouvement négatif en abscisses
                        if (key[KEY_RIGHT])
                            posx = posx + deplacement * cpt; // mouvement positif en abscisses

                        // 3) AFFICHAGE NOUVELLE POSITION
                        rectfill(page, posx, posy, posx + tx, posy + ty, makecol(255, 128, 128));
                        circlefill(page, posx1, posy1, rayon1, makecol(200, 255, 200));

                        blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
                        // 4) ON FAIT UNE PETITE PAUSE à chaque fois sinon ça va trop vite...
                        rest(20);

                        // condition de défaite
                        if (posy1 + rayon1 > SCREEN_H && depy1 > 0){
                            fini = 1;
                        }
                        blit (page, screen, 0,0,0,0, SCREEN_W, SCREEN_H);
                }
                cpt = 1;
                fin = clock();
                temps_execution = (double) (fin - debut) / CLOCKS_PER_SEC;
                temps [i] = temps_execution;

                /// Affiche résultat ///
                clear (page);
                while (!key[KEY_BACKSPACE]){
                    blit (pong_result, page, 0,0,0,0, pong_result->w, pong_result->h);
                    textout_ex(page, font, "FINI !", SCREEN_W / 2 - 150, 620, makecol(255, 0, 0), -1);
                    textprintf_ex(page, font, SCREEN_W / 2 - 100, 600, makecol(0, 0, 0), -1,
                                  "%s a tenu %f secondes", joueur_g[i].nom, temps_execution);
                    textout_ex(page, font, "Appuyez sur RETOUR pour quitter", SCREEN_W / 2 - 50, 620,
                               makecol(255, 255, 0), -1);
                    blit(page, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
                }
            }

            ///condition ticket ///
            if(temps[0] > temps[1])
                joueur_tab[0].nb_tickets++;
            else
                joueur_tab[1].nb_tickets++;
            if(a == 1){
                joueur_g[a] = joueur_tab[0];
                joueur_g[0] = joueur_tab[1];
            }
            else {
                joueur_g[0] = joueur_tab[0];
                joueur_g[1] = joueur_tab[1];
            }

            /// destroy ///
            destroy_bitmap(pg_chgm);
            destroy_bitmap(pong_result);
        }
    }
}