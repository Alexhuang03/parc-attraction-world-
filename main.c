#include <string.h>
#include <allegro.h>
#include "fonctions.h"

///MAIN///---------------------------------------------------------------------------------------------

int main() {

    initialisation_allegro();

    show_mouse(screen);

    BITMAP *buffer = create_bitmap(SCREEN_W, SCREEN_H);

    BITMAP *carte = charger_image("carte.bmp");
    BITMAP *sprite = charger_image("sprites.bmp");
    BITMAP *panneau = charger_image("panneau.bmp");
    BITMAP *tableau = charger_image("tableau.bmp");
    BITMAP *stop = charger_image("stop.bmp");
    BITMAP *ticket = charger_image("ticket.bmp");
    BITMAP  *cursor = charger_image("marteau.bmp");
    BITMAP *fond = charger_image("fond.bmp");
    BITMAP *taupe = charger_image("taupe.bmp");
    BITMAP *viseur = chargerImage("viseur.bmp");

    ballon tab[NB_BALLONS];

    /// chargement de toutes les images ///
    tab[0].img = chargerImage("rouge.bmp");
    tab[1].img = chargerImage("noir.bmp");
    tab[2].img = chargerImage("rose.bmp");
    tab[3].img = chargerImage("blanc.bmp");
    tab[4].img = chargerImage("violet.bmp");
    tab[5].img = chargerImage("rouge.bmp");
    tab[6].img = chargerImage("noir.bmp");
    tab[7].img = chargerImage("rose.bmp");
    tab[8].img = chargerImage("blanc.bmp");
    tab[9].img = chargerImage("violet.bmp");
    personnage joueur[2];

    joueur[0].x = SCREEN_W / 2;
    joueur[0].y = SCREEN_H / 2 - 90;

    joueur[1].x = SCREEN_W / 2 + 90;
    joueur[1].y = SCREEN_H / 2 - 90;

    for(int j = 0 ; j < 2 ; j++){
        initialiser_joueurs(&joueur[j]);
        creer_bitmap_du_personnage(&joueur[j], sprite, j+1);
        nom_clavier(&joueur[j], j + 1, buffer);
        while(!key[KEY_ALT]) {
            textout_ex(screen, font, "Appuyer sur ALT pour continuer", 700, 650, makecol(0, 0, 0), -1);
        }
        clear_to_color(screen, makecol(0, 0, 0));
    }

    int compteur_instant = 0;
    int i = 0;
    int fin = 0;

    while (!fin && (joueur[0].nb_tickets != 0 && joueur[1].nb_tickets != 0)) {

        actualisation_personnage(&joueur[i], buffer);

        blit(carte, buffer, 0, 0, 0, 0, carte->w, carte->h);

        masked_blit(panneau, buffer, 0, 0, 330, 529 , panneau->w, panneau->h);

        afficher_personnage(&joueur[i], buffer, 0);
        rectfill(buffer, SCREEN_W - 280 , 10, SCREEN_W - 10, 40, makecol(255, 255, 255));
        textprintf_ex(buffer, font, SCREEN_W - 270, 15, makecol(0, 0, 0), -1, "%s a %d tickets", joueur[0].nom, joueur[0].nb_tickets);
        textprintf_ex(buffer, font, SCREEN_W - 270, 25, makecol(0, 0, 0), -1, "%s a %d tickets", joueur[1].nom, joueur[1].nb_tickets);

        if(i == 1)
            masked_stretch_blit(joueur[0].bas[0], buffer, 0, 0, joueur[0].bas[0]->w, joueur[0].bas[0]->h, joueur[0].x, joueur[0].y, (joueur[0].bas[0]->w)-10, (joueur[0].bas[0]->h)-10);
        else if(i == 0)
            masked_stretch_blit(joueur[1].bas[0], buffer, 0, 0, joueur[1].bas[0]->w, joueur[1].bas[0]->h, joueur[1].x, joueur[1].y, (joueur[1].bas[0]->w)-10, (joueur[1].bas[0]->h)-10);


        blit(buffer, screen, 0, 0, 0, 0, buffer->w, buffer->h);

        compteur_instant++;
        if (compteur_instant >= 10) {
            if (joueur[i].deplacement)
                joueur[i].instant = (joueur[i].instant + 1) % 3;
            else
                joueur[i].instant = 0;
            compteur_instant = 0;
        }

        fin = quitter(&joueur[i], buffer, carte);

        guitar_hero(joueur,&i,  buffer, ticket, "sauvegarde_joueur.txt", "highscore.txt");

        course_equestre(joueur, buffer, &i);

        jeu_taupe(joueur, &i, buffer, fond, cursor, taupe,ticket,"sauvegarde_joueur.txt", "highscore.txt");

        tir_ballon (joueur,&i,buffer, viseur, ticket, tab, "sauvegarde_joueur.txt", "highscore.txt");

        pong(joueur,&i,buffer, ticket, "sauvegarde_joueur.txt", "highscore.txt");

        int x = joueur[0].x;      int x1 = joueur[1].x;
        int y = joueur[0].y;      int y1 = joueur[1].y;

        traversee_riviere(joueur, &i, buffer, ticket, "sauvegarde_joueur.txt", "highscore.txt");

        joueur[0].x = x;          joueur[1].x = x1;
        joueur[0].y = y;          joueur[1].y = y1;

        peche_canard(joueur, &i, buffer, ticket,"sauvegarde_joueur.txt", "highscore.txt");

        acces_statistiques(&joueur[i], tableau, buffer, stop, "sauvegarde_joueur.txt", "highscore.txt");

        rest(10);

    }
    if(joueur[0].nb_tickets == 0 || joueur[1].nb_tickets == 0)
        ecran_victoire(joueur, buffer);

    for(int k  = 0 ; k < 3 ; k++){
        destroy_bitmap(joueur[0].bas[k]);
        destroy_bitmap(joueur[0].gauche[k]);
        destroy_bitmap(joueur[0].droite[k]);
        destroy_bitmap(joueur[0].haut[k]);
        destroy_bitmap(joueur[1].bas[k]);
        destroy_bitmap(joueur[1].gauche[k]);
        destroy_bitmap(joueur[1].droite[k]);
        destroy_bitmap(joueur[1].haut[k]);
    }
    destroy_bitmap(buffer);
    destroy_bitmap(carte);
    destroy_bitmap(sprite);
    destroy_bitmap(panneau);
    destroy_bitmap(tableau);
    destroy_bitmap(stop);
    destroy_bitmap(ticket);
    destroy_bitmap(fond);
    destroy_bitmap(taupe);
    destroy_bitmap(cursor);

    return 0;

}

END_OF_MAIN();

///---------------------------------------------------------------------------------------------
///---------------------------------------------------------------------------------------------