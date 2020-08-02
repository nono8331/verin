#define VITESSE_DU_VERIN 11 //port 11, port qui fait de la pwm
#define SENS_DU_VERIN 12 //port 12
#define POTENTIOMETRE_VERIN 0 //PORT A0 de l'arduino uno
#define TEMPS_DE_FONCTIONNEMENT_MAX 60000 //correspond à 1min en mili secondes
#define CYCLE_DE_FOCNTIONNEMNT 240000 //correspond à 4min en mili secondes

int valeur_du_can_arduino=0; //permet de stocker la valeur numérique obtenue par le CAN du port A0 de l'arduino uno

unsigned long temps_de_fonctionnement_actuelle=0; //permet de stocker le temps de fonctionnement du vérin
unsigned long temps_cycle_de_fonctionnement_actuelle=0; //permet de stocker le temps actuelle du cycle (1 cycle c'est 4min)

int flag_indique_possition_verin=0; //cette variable prendra 0 si le verin n'est pas sorti est prendra 1 si le verin est sorti
int flag_premier_tour=0; //cette variable permettra de traiter le cas ou notre système viens de démarrer, cette variable prendra 0 si le programme vient de démarrer et prendra la valeur 1 si le programme à déjà tourné

void setup()
{
  Serial.begin(9600);
  pinMode(VITESSE_DU_VERIN, OUTPUT); //déclare le port 11 comme une sortie
  pinMode(SENS_DU_VERIN, OUTPUT); //déclare le port 12 comme une sortie 
  analogWrite(VITESSE_DU_VERIN,0); //a l'initialisation la vitesse est de 0
}

void loop()
{
  if( ((millis() - temps_cycle_de_fonctionnement_actuelle) >= CYCLE_DE_FOCNTIONNEMNT) || (flag_premier_tour ==0) ) //si le cycle est supérieur a 4min ou que le programme viens de démarrer alors on rentre dans le if
  {
    while( (millis() + temps_de_fonctionnement_actuelle) < TEMPS_DE_FONCTIONNEMENT_MAX ) //tant que le vérin n'a pas fonctionné pendant 1min on le fait fonctionner
    {
      switch(flag_indique_possition_verin) 
      {
        case 0: //si le vérin n'est pas sorti alors on le fait sortir 
          lire_valeur();//appel de la fonction lire_valeur
          while( (valeur_du_can_arduino != 1023) && ((millis() + temps_de_fonctionnement_actuelle) < TEMPS_DE_FONCTIONNEMENT_MAX) ) //1023 correspond à la valeur max du CAN ce qui signifie que tant que le verin n'est pas en fin de course alors on sort le verin et on vérifie que le temps de fonctionnement est inférieur à 1min
          {
            sortir(); //appel de la fonction sortir
            lire_valeur(); //appel de la fonction lire_valeur
          }
          analogWrite(VITESSE_DU_VERIN,0); //permet d'arrêter le verin 
          flag_indique_possition_verin = 1; //le verin est enfin sorti au maximum
        break;
        case 1: //si le verin est sorti alors on le fait rentrer 
          lire_valeur();//appel de la fonction lire_valeur
          while( (valeur_du_can_arduino != 0) && ((millis() + temps_de_fonctionnement_actuelle) < TEMPS_DE_FONCTIONNEMENT_MAX) )//0 correspond a la valeur min du CAN ce qui signifie que tant que le verin n'est pas en rentré entièrement alors on rentre le vérin et on vérifie que le temps de fonctionnement est inferieur à 1min
          {
            rentrer(); //appel de la fonction rentrer
            lire_valeur(); //appel de la fonction lire_valeur
          }
          analogWrite(VITESSE_DU_VERIN,0); //permet d'arrêter le vérin
          flag_indique_possition_verin = 0; //le vérin est enfin rentré au maximum
        break;
      }
      temps_de_fonctionnement_actuelle=millis(); //récupère le temps actuelle du fonctionnement du verin
    }
    flag_premier_tour=1; //mettre cette variable à 1 comme ça le if testera uniquement si le le cyle des 4 min à était dépassé ou non
    temps_cycle_de_fonctionnement_actuelle=millis(); //récupère le temps actuelle du cycle de 4min
  }
}

/* **************** FAIRE SORTIR LE VERIN **************** */
void sortir()
{
  digitalWrite(SENS_DU_VERIN, HIGH);//active le sens horaire du moteur
  analogWrite(VITESSE_DU_VERIN,255); //indique qu'au port 11 la vitesse sera au maximum. 
}

/* **************** FAIRE RENTRER LE VERIN **************** */
void rentrer()
{
  digitalWrite(SENS_DU_VERIN, LOW);//active le sens antihoraire du moteur
  analogWrite(VITESSE_DU_VERIN,255); //indique qu'au port 11 la vitesse sera au maximum. 
}

/* **************** LIT LA VALEUR NUMERIQUE OBTENUE PAR LE CAN **************** */
void lire_valeur()
{
  valeur_du_can_arduino=analogRead(POTENTIOMETRE_VERIN); //stock la valeur du CAN du port A0 dans la variable global valeur_du_can_arduino
}
