//
// Created by Elias on 04/05/2023.
//

#include <allegro.h>

#ifndef PROJET_FONCTIONS_H
#define PROJET_FONCTIONS_H

#define TAILLE_NOM 20
#define NB_BALLONS 10
#define NB_RONDINS 16
#define LARGEUR_CANARD 50
#define HAUTEUR_CANARD 50
#define LARGEUR_ECRAN 976
#define HAUTEUR_ECRAN 665
#define VITESSE_CANARD 2

typedef struct{
    BITMAP* haut[3];
    BITMAP* bas[3];
    BITMAP* gauche[3];
    BITMAP* droite[3];
    int x, y;
    int direction;
    int instant;
    int deplacement;
    char nom[20];
    int nb_tickets;
    int bet;
}personnage;

typedef struct{
    int PositionX;
    int PositionY;
    BITMAP *tab[5];
    int vitesse;
    int winner;
}t_chevaux;

typedef struct{
    int x, y;
}t_taupe;

typedef struct{
    BITMAP* img;
    int posx, posy;
    int depx, depy;
    int tx, ty;
    int boom;
}ballon;

typedef struct{
    BITMAP *img;
    int x, y;
    int depx;
    int w, h;
}rondin;

typedef struct {
    int x;
    int y;
    int vitesse;
    int vivant;
    BITMAP* sprite;
} t_canard;


void initialisation_allegro();
BITMAP *charger_image(const char *nom);
void nom_clavier(personnage *joueur, int numero_joueur, BITMAP *buffer);
void creer_bitmap_du_personnage(personnage *perso, BITMAP *A, int joueur);
void afficher_personnage(personnage *perso, BITMAP *buffer, int choix);
void actualisation_personnage(personnage* perso, BITMAP *buffer);
void initialiser_joueurs(personnage *joueur);
int quitter(personnage *joueur, BITMAP *buffer, BITMAP *carte);
void acces_statistiques(personnage *joueur, BITMAP *tableau, BITMAP *buffer, BITMAP *icon_stop,  const char *nom_fichier_perf, const char *nom_fichier_perf_ot);
void ecran_victoire(personnage joueur[], BITMAP *buffer);
void chargement_score_joueur(personnage *joueur, const char *nom_fichier_perf, BITMAP*buffer, BITMAP *tableau);


/// GUITAR HERO ///
void guitar_hero(personnage joueur[2], int *b, BITMAP *buffer, BITMAP *ticket, const char *nom_fichier_perf, const char *nom_fichier_perf_ot);
int titre_guitar_hero(BITMAP *buffer, BITMAP *ecran_titre,  BITMAP *ticket, personnage joueur_g[], int a);
void fin_guitar_hero(BITMAP *buffer, BITMAP *fond_fond, personnage joueur[], int i, double duree);
SAMPLE *charger_musique(const char *nom);
void defiler_fond(BITMAP *buffer, BITMAP *fond,int decalage);
void attendre(clock_t debut, double delai);
int verification_note(BITMAP *buffer, BITMAP *fond, int note, int decalage);
int verification_passage(BITMAP *buffer);
void remplir_pixel_en_blanc(BITMAP *fond, int x, int y);
void changer_couleur_pixel(BITMAP *fond, int x, int y);


/// TIR BALLON ///

void tir_ballon (personnage joueur_g[], int *b, BITMAP *page, BITMAP *viseur, BITMAP *ticket , ballon tab[NB_BALLONS],  const char *nom_fichier_perf, const char *nom_fichier_perf_ot);
// fonction qu'on appellera dans le ss prg 'tir_ballon'
BITMAP* chargerImage (const char* nom_image);
void initBallon (ballon* B,int posxInit, int posyInit, int depxInit, int depyInit );
void affichierBallon (BITMAP* page,ballon* B);
void deplacementBallon (ballon *B);
void supp_ballon (ballon *B, int *score);
void affichierViseur (BITMAP* page, BITMAP* image);

/// Paris Equestres ///

void refreshImageTest(t_chevaux *chevaux, BITMAP* buffer, BITMAP* decor, int c);
int jeuTest(t_chevaux  *cheval, BITMAP* buffer, BITMAP* piste, const int *tempon, const int *temponCheat);
void ParisEquestres(personnage *joueur, BITMAP *buffer, int *tempon,int *temponCheat, BITMAP *interfacePari);
void ResultatJeuxTest(t_chevaux *chevaux, BITMAP *buffer, BITMAP *piste, int *tempon, int *temponCheat, personnage joueur[2], BITMAP *winner, int *i);
void course_equestre(personnage *joueur, BITMAP *buffer, int *i);
void supprimer_chevaux(t_chevaux  *chevaux, BITMAP* piste, BITMAP *interfacePari, BITMAP *ecranWin);
//void check_position(personnage *joueur);

/// JEU DE LA TAUPE ///

void jeu_taupe(personnage joueur_g[], int *c, BITMAP *buffer, BITMAP *fond, BITMAP *cursor, BITMAP *taupe, BITMAP *ticket,const char *nom_fichier_perf, const char *nom_fichier_perf_ot);

/// TRAVERSEE RIVIERE ///

void traversee_riviere(personnage joueur_g[], int *b,  BITMAP*buffer, BITMAP *ticket, const char *nom_fichier_perf, const char *nom_fichier_perf_ot);
void initialisation_rondin(rondin *rd, BITMAP *rondin, int offset_x);
void afficher_rondin(rondin *rd, BITMAP *buffer);
void actualiser_rondin(rondin *rd);
void actualisation_personnage_traversee(personnage *perso, BITMAP *buffer);
int verification(personnage *perso, rondin *rd, BITMAP *buffer);
void victoire(BITMAP *buffer, personnage *joueur, double temps);
void defaite(BITMAP *buffer, personnage *joueur);
int titre(BITMAP *buffer, personnage *joueur, BITMAP *grass, BITMAP *ticket);


/// Peche aux canards///
int verifier_position(t_canard* canards, int nb_canards, int x, int y);
void initialiser_canard(t_canard canards[], int nb_canards, int id, BITMAP* sprite, int vitesse);
void mettre_a_jour_canard(t_canard* canard);
void dessiner_canard(t_canard* canard, BITMAP* buffer);
void pecher_canard(t_canard* canard, int* score);
void afficher_intro() ;
void mise_a_jour_jeu(t_canard* canards, int* score, BITMAP* buffer, BITMAP* fond, int* chrono, time_t* temps);
int tous_les_canards_peches(t_canard canards[]);
void reinitialiser_canards(t_canard* canards, int* vitesse_actuelle, BITMAP *sprite);
void peche_canard(personnage joueur_g[], int *c,  BITMAP* buffer, BITMAP* ticket,const char *nom_fichier_perf, const char *nom_fichier_perf_ot);

/// Pong ///
void pong (personnage joueur_g[], int *b, BITMAP *page, BITMAP *ticket,  const char *nom_fichier_perf, const char *nom_fichier_perf_ot);
#endif //PROJET_FONCTIONS_H