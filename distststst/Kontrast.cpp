#include <algorithm>
#include <cmath>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <map>
#include <set>
#include <regex>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;
using std::string;

void sjekk(bool riktig, const string& melding) {
    if (!riktig) throw std::runtime_error(melding);
}
string lesFil(const fs::path& fil) {
    std::ifstream inn(fil, std::ios::binary);
    sjekk(inn.is_open(), "Kan ikke lese: " + fil.string());
    return {std::istreambuf_iterator<char>(inn), std::istreambuf_iterator<char>()};
}
string storeBokstaver(string tekst) {
    for (char& tegn : tekst) tegn = static_cast<char>(std::toupper(static_cast<unsigned char>(tegn)));
    return tekst;
}
string finn(const string& tekst, const string& uttrykk) {
    std::smatch treff;
    sjekk(std::regex_search(tekst, treff, std::regex(uttrykk)), "Fant ikke: " + uttrykk);
    return treff[1].str();
}
string cssRegel(const string& css, const string& selektor) {
    // basically selektoren sendes inn som et "regular/normalt" utrykk. for eksempel: \\.kildeheader.
    return finn(css, "(?:^|[}\\n])\\s*" + selektor + "\\s*\\{([^}]*)\\}");
}
string cssFarge(const string& regel, const string& egenskap) {
    return storeBokstaver(finn(regel, "(?:^|;)\\s*" + egenskap + "\\s*:\\s*(#[0-9a-fA-F]{6})\\s*;"));
}
double luminans(const string& farge) {
    sjekk(std::regex_match(farge, std::regex("#[0-9a-fA-F]{6}")), "Ugyldig farge: " + farge);
    double rgb[3];
    for (int i= 0; i < 3; ++i) {
        double verdi = std::stoi(farge.substr(1 + i * 2, 2), nullptr, 16) / 255.0;
        rgb[i] = verdi <= 0.04045 ? verdi / 12.92 : std::pow((verdi + 0.055) / 1.055, 2.4);
    }
    return 0.2126 * rgb[0] + 0.7152 * rgb[1] + 0.0722 * rgb[2];
}
double kontrast(const string& tekst, const string& bakgrunn) {
    double a = luminans(tekst), b = luminans(bakgrunn);
    return (std::max(a, b) + 0.05) / (std::min(a, b) + 0.05);
}
struct Fargepar { string navn, tekst, bakgrunn; };
struct Dokument {
    std::map<string, int> tagger;
    std::set<string> ider;
    std::vector<string> lenker;
    std::vector<std::map<string, string>> bilder;
    string språk;
};
Dokument lesHtml(const string& html) {
    Dokument dokument;
    string innhold = std::regex_replace(html, std::regex("<!--[\\s\\S]*?-->"), "");
    const std::regex tagg(R"tag(<([A-Za-z][A-Za-z0-9:-]*)\b((?:[^"'<>]|"[^"]*"|'[^']*')*)>)tag");
    const std::regex attributt(R"attr(([A-Za-z_:][A-Za-z0-9_:.-]*)\s*=\s*(?:"([^"]*)"|'([^']*)'|([^\s"'=<>`]+)))attr");
    for (auto it = std::sregex_iterator(innhold.begin(), innhold.end(), tagg); it != std::sregex_iterator(); ++it) {
        string navn = (*it)[1].str(), attributter = (*it)[2].str();
        std::map<string, string> verdier;
        for (auto a = std::sregex_iterator(attributter.begin(), attributter.end(), attributt); a != std::sregex_iterator(); ++a) {
            verdier[(*a)[1].str()] = (*a)[2].matched ? (*a)[2].str() : ((*a)[3].matched ? (*a) [3].str() : (*a)[4].str());
        }
        ++dokument.tagger[navn];
        if(navn == "html") dokument.språk = verdier["lang"];
        if (verdier.count("id")) sjekk(dokument.ider.insert(verdier["id"]).second, "Duplikat id: " + verdier["id"]);
        if (navn == "img") dokument.bilder.push_back(verdier);
        for (const auto& attr : {"href", "src"}) if (verdier.count(attr)) dokument.lenker.push_back(verdier[attr]);
    }
    return dokument;
}
string urlDekod(const string& tekst) {
    string resultat;
    for (size_t i = 0; i < tekst.size(); ++i) {
        if (tekst[i] == '%' && i + 2 < tekst.size() && std::isxdigit(static_cast<unsigned char>(tekst[i + 1])) && std::isxdigit(static_cast<unsigned char> (tekst[i + 2]))) {
            resultat += static_cast<char>(std::stoi(tekst.substr(i + 1, 2), nullptr, 16));
            i += 2;
        } else resultat += tekst[i];
    }
    return resultat
}
string jsonTekst(const string& tekst) {
    std::ostringstream ut;
    ut << '"';
    for (unsigned char tegn : tekst) {
        if (tegn == '"' || tegn == '\\') ut << '\\' << tegn;
        else if (tegn < 32) ut << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(tegn);
        else ut << tegn;
    }
    return ut.str() + '"';
}
int main(int argc, char* argv[]) {
    try {
        const fs::path rot = fs::absolute(argc > 1 ? fs::path(argv[1]) : fs::current_path());
        const fs::path nettside = rot / "dist";
        const string css = lesFil(nettside / "styles.css");
        const string rotRegel cssRegel(css, ":root");
        std::vector<Fargepar> farger;
        for (int i = 1; i <= 3; ++i) {
            string nummer = std::to_string(i);
            Fargepar par{"Del " + nummer, cssFarge(rotRegel, "--blekknmr" + nummer), cssFarge(rotRegel, "--bgnmr" + nummer)};
            sjekk(par.tekst != "#000000" && par.tekst != "#FFFFFF" && par.bakgrunn != "#000000" && par.bakgrunn != "#FFFFFF", "Svart/hvitt i " + par.navn);
            farger.push_back(par);
        }
        sjekk(farger[0].tekst != farger[1].tekst && farger[0].tekst != farger[2].tekst && farger[1].tekst != farger[2].tekst, "Delene skal ha ulike tekstfarger");
        sjekk(farger[0].bakgrunn != farger[1].bakgrunn && farger[0].bakgrunn != farger[2].bakgrunn && farger[1].bakgrunn != farger[2].bakgrunn, "Delene skal ha ulike bakgrunnsfarger");
        const string tittel = cssRegel(css, "h1"), dialog = cssRegel(css, "dialog");
        farger.push_back({"Meny og bunntekst", cssFarge(rotRegel, "color"), cssFarge(cssRegel(css, "body"), "background")});
        farger.push_back({"Tittellinjer", cssFarge(tittel, "color"), cssFarge(tittel, "background")});
        farger.push_back({"Kildeboks", cssFarge(dialog, "color"), cssFarge(dialog, "background")});
        for (const auto& selektor : {"\\.skip-link", "\\.kildeheader"}) {
            string regel = cssRegel(css, selektor);
            sjekk(cssFarge(regel, "color") == farger[4].tekst && cssFarge(regel, "background") == farger[4].bakgrunn, "Tittellinjer har ulike fargepar");
        }
        for (const auto& par : farger) sjekk(kontrast(par.tekst, par.bakgrunn) >= 4.5, "Kontrast under 4.5 : 1: " + par.navn);
        std::map<fs::path, Dokument> dokumenter;
        for (const auto& fil : fs::directory_iterator(nettside)) {
            if (fil.path().extension() == ".html") dokumenter[fil.path().lexically_normal()] = lesHtml(lesFil(fil.path()));
        }
        for (auto& [fil, dokument] : dokumenter) {
            sjekk(dokument.språk == "nb", "Mangler norsk språk: " + fil.string());
            sjekk(dokument.tagger["h1"] == 1 && dokument.tagger["main"] == 1, "Feil hovedstruktur: " + fil.string());
            for (auto& bilde : dokument.bilder) sjekk(bilde["alt"].find_first_not_of(" \t\r\n") != string::npos && bilde["alt"].size() > 15, "Mangler beskrivende alt-tekst: " + bilde["src"]);
            for (const auto& lenke : dokument.lenker) {
                if (std::regex_search(lenke, std::regex("^[A-Za-z][A-Za-z0-9+.-]*:")) || lenke.rfind("//", 0) == 0) continue;
                auto hash = lenke.find('#');
                string sti = lenke.substr(0, hash);
                sti = urlDekod(sti.substr(0, sti.find('?')));
                fs::path mål = sti.empty() ? fil : (fil.parent_path() / fs::u8path(sti)).lexically_normal();
                sjekk(fs::is_regular_file(mål), "Mangler lokal fil: " + lenke);
                if (hash != string::npos && hash + 1 < lenke.size () && mål.extension() == ".html") {
                    sjekk(dokumenter.count(mål) && dokumenter.at(mål).ider.count(urlDekod(lenke.substr(hash + 1))), "Mangler anker: " + lenke);
                }
            }
        }
        sjekk(dokumenter.count(nettside / "index.html"), "Mangler index.html");
        auto& indeks = dokumenter.at(nettside / "index.html");
        sjekk(indeks.tagger["article"] == 3 && indeks.bilder.size() == 3, "Krever tre deler og tre bilder");
        for (size_t i = 0; i < indeks.bilder.size(); ++i) {
            string svg = lesFil(nettside / indeks.bilder[i]["src"]);
            std::set<string> bildeFarger;
            const std::regex hex("#[0-9a-fA-F]{6}");
            for (auto it = std::sregex_iterator(svg.begin(), svg.end(), hex); it != std::sregex_iterator(); ++it) bildeFarger.insert(storeBokstaver(it->str()));
            sjekk(bildeFarger == std::set<string>{farger[i].tekst, farger[i].bakgrunn}, "Bildet bruker ikke delens farger: " + indeks.bilder[i]["src"]);
        }
        std::ostringstream rapport;
        rapport << "{\n \"tool\": \"scripts/kontroller.cpp\",\n \"formula\": \"WCAG 2.x relative luminance\",\n \"html_files\":" << dokumenter.size()
            << ",\n \"articles\": 3,\n \"images_with_alt\": 3,\n \"div_elements\": " << indeks.tagger["div"]
            << ",\n \"local_links\": \"passed\",\n \"color_pairs\": [\n";
        for (size_t i = 0; i < farger.size(); ++i) {
        }
    }
}