# Σύστημα Διαχείρισης Παραγγελιών Πίτσας με POSIX Threads [![Static Badge](https://img.shields.io/badge/English-orange)](README.en.md)

Αυτό το έργο υλοποιεί ένα σύστημα διαχείρισης παραγγελιών πίτσας χρησιμοποιώντας **POSIX threads (pthreads)** και μηχανισμούς συγχρονισμού (mutexes, condition variables) σε **Linux/Lubuntu**. Ο κώδικας προσομοιώνει την ταυτόχρονη λειτουργία πελατών, μαγείρων, φούρνων, πακεταριστών και διανομέων.

## Περιγραφή Λειτουργίας

### 1. Διαδικασία Παραγγελίας
- Κάθε πελάτης αναπαρίσταται ως **ξεχωριστό thread**.
- Οι πελάτες συνδέονται τυχαία και παραγγέλνουν:
  - Τυχαίο αριθμό πίτσες (1-5).
  - **60% πιθανότητα** για απλή πίτσα (`Cplain = 10€`) και **40%** για special (`Cspecial = 12€`).
  - Υπάρχει **10% πιθανότητα** η παραγγελία να αποτύχει.

### 2. Διαχείριση Πόρων
- **Παρακολούθηση πόρων** με mutexes και condition variables:
  - **Μάγειρες**: `Ncook = 2` (προετοιμασία πίτσας σε `Tprep = 1 λεπτό/πίτσα`).
  - **Φούρνοι**: `Noven = 15` (ψήσιμο σε `Tbake = 10 λεπτά`).
  - **Πακεταριστές**: `Npacker = 2` (πακετάρισμα σε `Tpack = 1 λεπτό/πίτσα`).
  - **Διανομείς**: `Ndeliverer = 10` (παράδοση σε τυχαίο χρόνο `5-15 λεπτά`).

### 3. Στατιστικά
Στο τέλος της εκτέλεσης εμφανίζονται:
- Συνολικά έσοδα, αποτυχημένες/επιτυχημένες παραγγελίες.
- Αριθμός πωλήσεων ανά τύπο πίτσας.
- Μέσος και μέγιστος χρόνος παράδοσης.
- Χρόνοι αναμονής για ψήσιμο και πακετάρισμα.

## Δομή Κώδικα
- **Κύριο Αρχείο**: `pizzeria.c`  
  Περιέχει τη λογική δημιουργίας threads, συγχρονισμού πόρων και στατιστικών.
- **Header Αρχείο**: `pizzeria.h`  
  Ορίζει σταθερές (π.χ., αριθμό μαγείρων, χρόνους).
- **Δοκιμαστικό Script**: `test-res.sh`  
  Μεταγλωττίζει και εκτελεί τον κώδικα:
  ```bash
  gcc pizzeria.c -o main -pthread
  ./main 100 1000  # 100 παραγγελίες, seed=1000```
  
 ## Σημειώσεις
- Ο κώδικας είναι γραμμένος για Linux/Lubuntu.
- Συγχρονισμός: Χρήση mutexes `pthread_mutex_t` και condition variables `pthread_cond_t` για αποφυγή race conditions.
- Παράλληλη Επεξεργασία: Κάθε φάση (πληρωμή, προετοιμασία, ψήσιμο, κλπ.) εκτελείται παράλληλα για διαφορετικές παραγγελίες.

## Παράδειγμα Εκτέλεσης
```bash
$ ./test-res.sh
Η παραγγελία με αριθμό 1 καταχωρήθηκε.
Ο μάγειρας 1 ανέλαβε την παραγγελία 1
...
Total income: 1500€
Failed orders: 5
Successful orders: 95
Total special pizzas sold: 38
Total normal pizzas sold: 57
```
