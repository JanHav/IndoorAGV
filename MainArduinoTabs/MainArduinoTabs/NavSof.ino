/*****
Doel: Bevat het navigatiealogritme
Parameters: /
Return: /
*****/

void NavSof()
{
  
  if (UWB_lock == 0)                          //UWB update locatie enkel als dit gewenst is 
  {                                           //d.w.z. als de flag op 0 staat
    yhuidig_wp = Y_Huidig;
    xhuidig_wp = X_Huidig;
    UWB_lock = 1;
  }
  
  if (xyStart_lock == 0x00)                  //Ervoor zorgen dat de eerste trackline berekend kan worden door het startwaypoint te voorzien van de huidige locatie
  {
    xstart_wp = xhuidig_wp;                  //Het allereerste punt kunnen we niet opslaan in een array omdat deze bepaald wordt door de plaats waar de AGV wordt neergezet
    ystart_wp = yhuidig_wp;                 //daarom bevat xstart en ystart initieel de waarde van xhuidig en yhuidig
    xyStart_lock = 0x01;                    //Nadat de xstart_wp en ystart_wp zijn upgedatet met de huidige locatie, 
                                            //mag dit vervolgens niet meer zo gebeuren,daarom dat we ons lock resetten
                                            //nu is het de bedoeling dat xstart_wp en ystart_wp steeds verwijzen naar het laatste eindwaypoint, 
                                            //zodat de trackline correct kan berekend worden
  }

///InitialisatieRoutine
if (OpStart_lock == 0)
{
  UWB_lock = 0;
  if (Encoder_lock == 0)
  {
    beginEncoder = EncoderData;
    Encoder_lock = 1;
  }
  else if (30 >= abs(EncoderData - beginEncoder))
  {
    VooruitRijden();
  }
  else 
  {
    Stoppen();
    Encoder_lock = 0;
    OpStart_lock = 1;
  }
}

//Ervoor zorgen dat de AGV steeds weet of deze links of rechts van de trackline is
HoekTrack = atan2(ystart_wp - wps[current_wp].Yas, xstart_wp - wps[current_wp].Xas)*57.29577951;
HoekAGV = atan2(yhuidig_wp - wps[current_wp].Yas, xhuidig_wp - wps[current_wp].Xas)*57.29577951;

//Pas als de initialisatie van de AGV afgelopen is mag onderstaande code uitgevoerd worden
if (OpStart_lock == 1)
{
  if (HoekTrack < HoekAGV)
  {
    if (BNO_lock ==0)
    {
      beginBNO = RotatieData;
      BNO_lock =1;
    }
    dot = (cos(beginBNO)*cos(RotatieData))+(sin(beginBNO)*sin(RotatieData));
    det = (cos(beginBNO)*sin(RotatieData))-(sin(beginBNO)*cos(RotatieData));
    angle = (atan2(det,dot))*57.29577951;

    if (abs(HoekTrack - HoekAGV)>= abs(angle))
    {
      LinksStuur();
    }
  }

  else if (HoekTrack > HoekAGV)
  {
    if (BNO_lock ==0)
    {
      beginBNO = RotatieData;
      BNO_lock =1;
    }
    dot = (cos(beginBNO)*cos(RotatieData))+(sin(beginBNO)*sin(RotatieData));
    det = (cos(beginBNO)*sin(RotatieData))-(sin(beginBNO)*cos(RotatieData));
    angle = (atan2(det,dot))*57.29577951;

    if (abs(HoekTrack - HoekAGV)>= abs(angle))
    {
      RechtsStuur();
    }    
  }

  else 
  {
    NeutraalStuur();
  }

///Uitrekenen wat de afstand tot het volgende waypoint is
  AfstandTotWaypoint = AfstandPuntPunt(xstart_wp, ystart_wp, wps[current_wp].Xas,wps[current_wp].Yas);

//Tot hiertoe kennen we de richting en de afstand tot het volgende waypoint
//Nu kan dus gemakkelijk op encoder en BNO055 tot het volgende waypoint gereden worden
  if (Encoder_lock == 0)
  {
    beginEncoder = EncoderData;
    Encoder_lock = 1;
  }

  if (AfstandTotWaypoint > abs(EncoderData - beginEncoder))
  {
    VooruitRijden();
  }
  else
  {
    Stoppen;
    previous_wp = current_wp;
    current_wp++;
    xstart_wp = wps[previous_wp].Xas;
    ystart_wp = wps [previous_wp].Yas;
    BNO_lock = 0;
    UWB_lock = 0;
    if (current_wp >= waypoints)                  //Als de index die in current_wp zit gelijk is aan het totaal aantal waypoints in de array 
    {                                             //dan moet de index terug naar nul gaan zodat we terug een coördinaat uit onze array kunnen halen
    current_wp = 0;  
    }
  }
}
  
}





