# Pizza Order Management System with POSIX Threads

This project implements a pizza order management system using **POSIX threads (pthreads)** and synchronization mechanisms (mutexes, condition variables) on **Linux/Lubuntu**. The code simulates the concurrent operation of customers, cooks, ovens, packers, and delivery personnel.

## Functionality Overview

### 1. Order Process
- Each customer is represented as a **separate thread**.
- Customers connect at random intervals and place orders:
  - Random number of pizzas (1-5).
  - **60% probability** for a plain pizza (`Cplain = €10`) and **40%** for a special pizza (`Cspecial = €12`).
  - **10% chance** for the order to fail.

### 2. Resource Management
- **Resource tracking** using mutexes and condition variables:
  - **Cooks**: `Ncook = 2` (prepares pizzas in `Tprep = 1 minute/pizza`).
  - **Ovens**: `Noven = 15` (bakes pizzas in `Tbake = 10 minutes`).
  - **Packers**: `Npacker = 2` (packs pizzas in `Tpack = 1 minute/pizza`).
  - **Delivery Personnel**: `Ndeliverer = 10` (delivers orders in random time `5-15 minutes`).

### 3. Statistics
At the end of execution, the system displays:
- Total income, failed/successful orders.
- Sales count per pizza type.
- Average and maximum delivery time.
- Waiting times for baking and packing.

## Code Structure
- **Main File**: `pizzeria.c`  
  Contains thread creation logic, resource synchronization, and statistics.
- **Header File**: `pizzeria.h`  
  Defines constants (e.g., number of cooks, timers).
- **Test Script**: `test-res.sh`  
  Compiles and runs the code:
  ```bash
  gcc pizzeria.c -o main -pthread
  ./main 100 1000  # 100 orders, seed=1000```
  
## Notes
- The code is written for Linux/Lubuntu.
- Parallelism is used to simulate concurrent processes (e.g., multiple customers ordering simultaneously).

## Example Execution
```bash
$ ./test-res.sh
Order #1 has been registered.
Cook #1 is handling order #1
...
Total income: €1500
Failed orders: 5
Successful orders: 95
Total special pizzas sold: 38
Total normal pizzas sold: 57```
