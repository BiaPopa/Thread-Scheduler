Nume: Popa Bianca

Planificator de threaduri

### Descriere:
    Planificatorul este o structura ce contine campurile: threads (coada ce
retine toate threadurile pornite), queue (coada de prioritati ce retine toate
threadurile in starea "Ready"), current_running (threadul aflat in starea
"Running"), running (semafor anonim), max_quantum (cuanta de timp dupa care
threadul trebuie preemptat) si max_events (numarul maxim de dispozitive suportate).
    Cele 2 cozi sunt implementate folosind LinkedList. Pentru threads, operatia
"q_push" adauga noul element la finalul cozii. In schimb, pentru queue (coada de
prioritati), s-a implementat o functie de push diferita, numita "q_push_priority",
care adauga noul element in functie de prioritatea acestuia. Nodul cu cea mai 
mare prioritate se va afla pe prima pozitie a cozii.
    Fiecare thread este, de asemenea, o structura ce contine: id (returnat de
functia so_fork), handler (handlerul executat de thread), sem (semafor folosit
pentru sincronizarea threadurilor), state (retine starea curenta a threadului),
io (retine evenimentul care deblocheaza threadul), quantum (cuanta de timp) si
priority (prioritatea threadului).
    Functiile utilizate de planificator sunt implementate astfel:

1. so_init
    Se verifica daca planificatorul a fost deja initializat. Daca nu, atunci se
verifica daca parametrii functiei sunt valizi. Daca sunt, atunci planificatorul
este initializat.

2. so_end
    Se verifica daca planificatorul a fost initializat. Daca a fost, se asteapta
terminarea tuturor threadurilor si se elibereaza toate resursele din memorie.

3. so_fork
    Se verifica daca parametrii functiei sunt valizi. Daca sunt, se initializeaza
un nou thread, se adauga in coada cu toate threadurile pornite si in coada de
prioritati. Pentru a apela handlerul, threadul asteapta de la semafor permisiunea
de a rula. In functie de threadul curent, se continua executia sau se apeleaza 
functia so_schedule. Aceasta implementeaza algoritmul Round Robin cu prioritati.

4. so_wait
    Se verifica daca parametrul functiei este valid. Daca da, atunci threadul
curent trece in starea "Waiting" si se continua executia.

5. so_signal
    Se verifica daca parametrul functiei este valid. Daca da, atunci toate 
threadurile care asteapta evenimentul io trec in starea "Ready" si sunt
adaugate inapoi in coada de prioritati.

6. so_exec
    Cuanta de timp a threadului curent este decrementata si se apeleaza
planificatorul. Threadul asteapta de la semafor permisiunea de a rula.

