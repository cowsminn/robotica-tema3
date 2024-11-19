# Tema 3 - Quick time

## Descriere:
Aceasta tema consta in dezvoltarea unui joc competitiv de reflex pentru doi jucatori, in care participantii isi testeaza viteza reflexelor in incercarea de a obtine un scor cat mai mare.

Fiecare jucător dispune de un set de 3 butoane, 3 LED-uri simple si un LED RGB, iar jocul se desfasoara pe parcursul mai multor runde. Scopul fiecarui jucator este sa apese cat mai rapid butonul care corespunde culorii afisate de LED-ul sau RGB. Punctajul fiecarui jucător va fi afisat si actualizat in timp real pe monitorul serial, iar la finalul jocului se afiseaza scorul final al ambilor jucatori pe LCD si jucatorul cu cel mai mare punctaj va fi declarat castigator.

  ## Componentele folosite:
  - 6x LED-uri (2 grupuri de câte 3 leduri, în cadrul unui grup trebuie să avem culori diferite)
   - 2x LED RGB (1 pentru fiecare jucător)
   - 7x butoane (3 pentru fiecare jucător + un buton de start)
  - 1x LCD
- 1x Servomotor
 - 2x Breadboard
 - Fire de legatura
 - 2x Arduino Uno
 - 1x Potentiometru
 - 1x Buzzer
 - ?x Rezistoare (?x 330ohm pentru leduri, ?x 1kohm pentru butoane)

## Flow:

### Inițializare

Jocul pornește cu afișarea unui mesaj de bun venit pe LCD. Apăsarea unui buton declanșează startul jocului.

Pentru începerea jocului, butonul de start poate fi implementat într-un mod flexibil, rămânând la latitudinea studenților să aleagă una dintre următoarele variante:
    -în această variantă, jocul pornește la apăsarea oricărui buton.
    -Un buton specific începe jocul* - se poate desemna un buton anume, clar marcat pe breadboard, pentru a porni jocul.
    -Un al 7-lea buton dedicat* – se poate adăuga un buton suplimentar destinat exclusiv pornirii jocului.

### Desfășurarea Rundelor

   - Fiecare jucător are trei butoane, fiecare asociat unui LED de o culoare diferită și un al 4-lea LED RGB.
   - La fiecare rundă, fiecare jucător este cel activ.
   - LED-ul RGB al jucătorului activ se aprinde într-o culoare corespunzătoare unuia dintre butoanele sale. Jucătorul trebuie să apese cât mai rapid butonul care corespunde culorii LED-ului RGB, pentru a obține puncte. Cu cât reacționează mai repede, cu atât primește mai multe puncte.
   - La finalul unei runde LCD-ul afișează punctajul actualizat al ambilor jucători.
   - Pe tot parcursul jocului display-ul LCD va arata punctajul fiecarui jucator

### Timpul și Finalizarea Jocului

  - Servomotorul se rotește pe parcursul jocului, indicând progresul. O rotație completă a servomotorului marchează sfârșitul jocului (voi decideti cat de repede se misca).
  - La final, LCD-ul afișează numele câștigătorului și scorul final pentru câteva secunde, apoi revine la ecranul de start cu mesajul de bun venit.

## Detalii Tehnice:

 SPI: Tema aceasta implică foarte multe legături. Atât de multe încât un singur arduino uno nu ne oferă suficienți pini GPIO. De aceea pentru această temă este nevoie de 2 arduino uno care vor comunica folosind SPI. Arduinoul master va fi cel responsabil pentru controlul LCD-ului, servomotorului și va fi cel care ține în memorie starea jocului (ex. punctajul fiecărui jucator, ledul care trebuie să fie aprins acum, etc.). Arduino-ul slave va controla butoanele și ledurile, primind mesaje de la arduinoul master pentru a ști ce led să aprindă și trimițând înapoi mesaje despre butonul apăsat. 
 
 ### Butoane:

   - Pentru începerea jocului butonul de start poate fi implementat în diverse moduri:
   - Orice buton începe jocul
   - Un anume buton începe jocul (ar trebui să fie clar pe breadboard care este acel buton care pornește jocul)• Un al 7-lea buton dedicat poate fi pus pentru pornirea jocului
   - Cât timp jocul este în desfășurare butoanele trebuie să poată fi folosite doar pentru controlul jocului și să nu reseteze progresul
   - Doar butoanele jucătorului din acea rundă trebuie să poată controla jocul
   - Chiar și cu 2 plăci arduino nu avem suficienți pini pentru toate componentele. De aceea, putem multiplexa butoanele folosind rezistențe.

 ###  LED-uri:

   - Fiecare buton are asociat un LED de o culoare diferită. Pe parcursul jocului acestea trebuie să fie aprinse pentru a vedea cărei culoare îi corespunde fiecare buton
   - LED-ul rgb trebuie să se aprindă în una din cele 3 culori ale butoanelor
   - LED-ul rgb trebuie să fie stins dacă nu este runda jucătorului corespunzător acelui LED

 ### LCD:

   - Pentru controlul acestuia ne putem folosi de biblioteca LiquidCrystal
   - Trebuie să aibă setată o luminozitate și un contrast suficient de bune cât să fie vizibil textul pe ecran.
   - Vor fi folosiți doar pini D4-7 pentru liniile de date ale ecranului
   - Pe parcursul jocului trebuie să afișeze punctajele celor 2 jucători

 ### Servomotor:

   - Servomotorul va începe de la poziția de 0 grade și se va deplasa în sens antiorar pentru a indica scurgerea timpului.
   - Anteție la valoarea trimisă către servo-motor. Prin biblioteca Servo.h noi trimitem la servomotor rotație absolută, nu relativă la cea curentă.

Aveți libertatea de a decide următoarele elemente:

   - Punctajul primit pentru un răspuns corect în funcție de viteza de reacție
   - Timpul dintre runde (puteți avea un mic timp de pregătire între runde sau pot fi imediat una după cealaltă pentru o dificultatea crescută)
   - Timpul întregului joc


