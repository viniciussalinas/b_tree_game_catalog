# B-Tree-based Game Catalog System
This project was developed by me as part of the "Estrutura de Dados 2" (Data Structures 2) course at the Federal University of São Carlos, in November 2018.

## Overview
This project is a game catalog system based on B-trees. It allows you to manage a collection of game records with various attributes. The system provides the following features:

### Cadastrar (Register): 
Add a new game to the catalog, including details such as title, developer, release date, classification, price, discount, and category.

### Alterar (Modify): 
Change the discount of a game.

### Buscar (Search): 
Search for a game in the catalog.

### Listar (List): 
List all the games in the catalog.

### Imprimir o Arquivo de Dados (Print Data File): 
View the entire data file.

### Imprimir o Arquivo de Índice Primário (Print Primary Index File): 
View the primary index file.

### Imprimir o Arquivo de Índice Secundário (Print Secondary Index File): 
View the secondary index file.

### Sair (Exit): 
Terminate the program and release all allocated memory.

## Data Structure
The core of this system relies on B-trees, both for the primary index (IP) and secondary index (IS). B-trees provide efficient search and insertion operations, making this system suitable for managing a large number of game records.

## How to Run
To run this program, you'll need to compile and execute the provided source code. It includes the functionality to load an existing data file or create a new one if needed. Follow the on-screen menu instructions to navigate through the system's features.

## Note
This is a text-based console application and doesn't have a graphical user interface. It was developed as part of an academic project, and its primary purpose is to demonstrate the use of B-trees in data structure management.
