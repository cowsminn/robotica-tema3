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

   -Fiecare jucător are trei butoane, fiecare asociat unui LED de o culoare diferită și un al 4-lea LED RGB.
   - La fiecare rundă, fiecare jucător este cel activ.
   - LED-ul RGB al jucătorului activ se aprinde într-o culoare corespunzătoare unuia dintre butoanele sale. Jucătorul trebuie să apese cât mai rapid butonul care corespunde culorii LED-ului RGB, pentru a obține puncte. Cu cât reacționează mai repede, cu atât primește mai multe puncte.
   - La finalul unei runde LCD-ul afișează punctajul actualizat al ambilor jucători.
   - Pe tot parcursul jocului display-ul LCD va arata punctajul fiecarui jucator

### Timpul și Finalizarea Jocului

   -Servomotorul se rotește pe parcursul jocului, indicând progresul. O rotație completă a servomotorului marchează sfârșitul jocului (voi decideti cat de repede se misca).
    -La final, LCD-ul afișează numele câștigătorului și scorul final pentru câteva secunde, apoi revine la ecranul de start cu mesajul de bun venit.


