# Universell utforming

Åpne [distststst/index.html](distststst/index.html) i nettleseren. Prosjektet holdes lokalt.

Siden består av tre spørsmål med svar og bilder, en kontrasttabell og kilder.

## Dokumentasjon

- HTML bruker `header`, `nav`, `main`, `article`, `section` og `footer` (no div btw).
- Hver del har egne tekst og bakgrunnsfarger (no svart or hvitt btw).
- Alle tre bilder har beskrivende alt-tekst og bruker samme fargepar som sin del.
- Kontrastverdiene står på nettsiden :D [Måleresultater](dokumentasjon/kontrast-rapport.json) er laget med WCAG sin luminansformel: `(Llys + 0,05) / (Lmørk + 0,05)`.

Kjør `.\distststst\Kontrast.exe` fra prosjektmappen for å sjekke fargepar, lokale lenker og alt-tekster. Programmet leser fargene fra CSS og skriver måleresultatene til `dokumentasjon/kontrast-rapport.json` (who is json?).

Build C++17-programmet på nytt i **Developer PowerShell for Visual Studio**:

```powershell
cl /nologo /std:c++17 /EHsc /utf-8 distststst\Kontrast.cpp /Fe:distststst\Kontrast.exe /Fo:distststst\Kontrast.obj
.\distststst\Kontrast.exe
```

C++ scriptet er tilpasset HTML og CSS-strukturen i bare dette prosjektet, og er ikke en generell HTML sjekker/validator. (burde gjort det i python😭)

[Oppgavetekst](https://github.com/CecilieBreivang/Universell-utforming---HTML-og-CSS). Fagkildene er lenket nederst på nettsiden.

> [!CAUTION]
> # hær er en kul Trekant btw:

```stl
solid cube_corner
  facet normal 0.0 -1.0 0.0
    outer loop
      vertex 0.0 0.0 0.0
      vertex 1.0 0.0 0.0
      vertex 0.0 0.0 1.0
    endloop
  endfacet
  facet normal 0.0 0.0 -1.0
    outer loop
      vertex 0.0 0.0 0.0
      vertex 0.0 1.0 0.0
      vertex 1.0 0.0 0.0
    endloop
  endfacet
  facet normal -1.0 0.0 0.0
    outer loop
      vertex 0.0 0.0 0.0
      vertex 0.0 0.0 1.0
      vertex 0.0 1.0 0.0
    endloop
  endfacet
  facet normal 0.577 0.577 0.577
    outer loop
      vertex 1.0 0.0 0.0
      vertex 0.0 1.0 0.0
      vertex 0.0 0.0 1.0
    endloop
  endfacet
endsolid
```
---
> [!TIP]
> # kjedet meg litt mer så hær er skolen vår:
```geojson
{
  "type": "FeatureCollection",
  "features": [
    {
      "type": "Feature",
      "id": 1,
      "properties": {
        "ID": 0
      },
      "geometry": {
        "type": "Point",
        "coordinates": [18.96470372569492, 69.6778016321014, 35]
      }
    }
  ]
}
```
