# Easy Notepad

## Spis treści
- [Opis](#opis)
- [Zrzuty ekranu](#zrzuty-ekranu)
- [Instalacja](#instalacja)
- [Technologie](#technologie)
- [Licencja](#licencja)

## Opis
**Easy Notepad** to prosty edytor tekstu. Edytor ten obsługuje szerokie znaki (tj. ą, ć, ó, ź itp.), a także znaki tabulacji. Edytor pozwala zaznaczać tekst, a obsługuje skróty klawiaturowe takie jak **Ctrl+C**, **Ctrl+V** i **Ctrl+X**. Niestety w chwili obecnej nie można zapisywać tekstu jako plik. Ta funkcjonalność zostanie dodane w najbliższym czasie. 

## Zrzuty ekranu
![Zrzut ekranu](screenshots/01.png)
![Zrzut ekranu](screenshots/02.png)
![Zrzut ekranu](screenshots/03.png)

## Instalacja
1. Pobierz i zainstaluje Visual Studio 2022
2. Pobierz i zainstaluje CMake
2. Pobierz bibliotekę SFML i umieść ją w katalogu `C:\SFML-2.6.2`.
3. Otwórz **Wiersz poleceń (cmd)**.
4. Przejdź do katalogu projektu:
`
cd ..\..\Easy Notepad
`
5. Utwórz folder **build**:
`
mkdir build
`
6. Wejdź do folderu **build**:
`
cd build
`
7. Uruchom konfigurację projektu za pomocą **CMake**:
`
cmake ..
`
8. Zbuduj projekt:
`
cmake --build .
`

9. Plik wykonywalny znajdziesz w **build/Debug** lub **build/Release**

## Technologie
Program stworzono w języku C++ z wykorzystaniem biblioteki SFML 2.6.2.  
  
## Licencja
Licencja Otwarta – Uznanie autorstwa  
  
Ten program może być:  
-Pobierany  
-Kopiowany  
-Modyfikowany  
-Wykorzystywany w projektach prywatnych i komercyjnych  
  
Pod warunkiem, że:  
-Zachowana zostanie informacja o autorze oryginalnego programu  
-Podane zostanie źródło (link do repozytorium)  
-W przypadku modyfikacji, należy wyraźnie zaznaczyć, że program został zmodyfikowany oraz przez kogo.  