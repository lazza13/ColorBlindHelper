# Color Blind Helper

Color Blind Helper è un dispositivo elettronico creato per aiutare le persone daltoniche nel riconoscimento dei colori. Il colore identificato viene visualizzato su un display OLED I2C.

## Descrizione

Il progetto, realizzato con una board Arduino (NANO) e un sensore di colore (ad esempio TCS3200), rileva e mostra il nome del colore riconosciuto tramite display OLED I2C. Non sono previsti feedback audio, né altri tipi di output oltre all'OLED (ma potrebbero essere inseriti in futuro).

## Requisiti

### Hardware
- Arduino (Nano ma anche altri arduino a scelta)
- Sensore di colore (es. TCS3200)
- Display OLED I2C (es. SSD1306 da 0,96'')
- Cavi jumper e breadboard secondo necessità

### Software
- [Visual Studio Code](https://code.visualstudio.com/)
- [PlatformIO IDE](https://platformio.org/install/ide?install=vscode)
- Drivers per la board utilizzata
- Librerie per sensore di colore e display OLED (vedi `platformio.ini`)

## Installazione e uso

1. **Clona** il repository localmente.
2. **Apri** la cartella del progetto con VSCode.
3. **Assicurati** che PlatformIO sia installato come estensione.
4. **Installa** le librerie specificate in `platformio.ini` (verranno installate automaticamente).
5. **Assembla** il dispositivo seguendo lo schema (sensore di colore e display OLED I2C collegati all’Arduino).
6. **Compila** e **carica** il firmware tramite PlatformIO (`Ctrl+Alt+U`).
7. **Accendi e utilizza**: il colore rilevato verrà visualizzato sul display OLED.

## Ringraziamenti
Un caro grazie a tutta la comunità di Arduino e agli sviluppatori delle librerie utilizzate nel progetto

**PS:** il README l'ho fatto con l'AI perchè non avevo volglia di scrivere