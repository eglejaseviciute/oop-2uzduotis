#include "funkcijos.h"
#include "myLib.h"
#include "studentas.h"
#include "strategijos.h"


map<string, VeikimoGreicioMatavimai> veikimoGreicioRezultatai;


// Funkcija skaiciuojanti vidurki
double skaiciuotiVidurki(const list<int>& namuDarbai) {
    if (namuDarbai.empty()) return 0;
    return accumulate(namuDarbai.begin(), namuDarbai.end(), 0.0) / namuDarbai.size();
}


// Funkcija skaiciuojanti mediana
double skaiciuotiMediana(const list<int>& namuDarbai) {
    if (namuDarbai.empty()) {
        return 0.0;
    }
    
    list<int> sortedList = namuDarbai;
    sortedList.sort();
    
    size_t size = sortedList.size();
    
    if (size % 2 == 0) {
        // Jei lyginis skaicius
        auto it = sortedList.begin();
        advance(it, size/2 - 1);
        int first = *it;
        advance(it, 1);
        int second = *it;
        return (first + second) / 2.0;
    } else {
        // Jei nelyginis skaicius
        auto it = sortedList.begin();
        advance(it, size/2);
        return static_cast<double>(*it);
    }
}


// Funkcija gaunanti tinkama simboli is vartotojo
char gautiTinkamaSymboli(const string &pranesimas, const string &tinkamisymboliai) {
    string ivestis;
    while (true) {
        cout << pranesimas;
        cin >> ivestis;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (ivestis.length() == 1 && tinkamisymboliai.find(ivestis[0]) != string::npos) {
            return ivestis[0];
        }
        cout << "Klaida! Iveskite viena is siu simboliu: " << tinkamisymboliai << endl;
    }
}


// Funkcija generuojanti atsitiktinius rezultatus studentui
void generuotiRezultatus(Studentas &studentas, int namuDarbaiKiekis) {
    Rezultatai naujiRezultatai;
    naujiRezultatai.clearNamuDarbai();
    
    for (int i = 0; i < namuDarbaiKiekis; ++i) {
        naujiRezultatai.addNamuDarbas(rand() % 11);
    }
    naujiRezultatai.setEgzaminas(rand() % 11);
    
    studentas.setRezultatai(naujiRezultatai);
}


// Funkcija nuskaitanti duomenis is failo
double nuskaitytiDuomenisIsFailo(const string& failoPavadinimas, list<Studentas>& studentai) {
    auto start = std::chrono::high_resolution_clock::now();
    while (true) {
        ifstream failas(failoPavadinimas);
        if (!failas) {
            throw runtime_error("Klaida! Nepavyko atidaryti failo: " + failoPavadinimas);
        }

        string line;
        getline(failas, line);

        int lineNumber = 1;
        while (getline(failas, line)) {
            lineNumber++;
            Studentas studentas;
            stringstream ss(line);

            string vardas, pavarde;
            if (!(ss >> vardas >> pavarde)) {
                cout << "Klaida eiluteje " << lineNumber << "! Nepavyko nuskaityti vardo ir pavardes." << endl;
                continue;
            }

            studentas.setVardas(vardas);
            studentas.setPavarde(pavarde);


            Rezultatai rezultatai;
            list<int> namuDarbai;
            string input;
            int nd;
            bool egzaminasNuskaitytas = false;

            while (ss >> input) {
                try {
                    nd = stoi(input);
                    if (nd < 0 || nd > 10) {
                        throw out_of_range("Rezultatas ne intervale [0, 10]");
                    }
                    if (!egzaminasNuskaitytas) {
                        namuDarbai.push_back(nd);
                    } else {
                        cout << "Klaida eiluteje " << lineNumber << "! Po egzamino rezultato aptikti papildomi duomenys." << endl;
                        break;
                    }
                } catch (const invalid_argument&) {
                    cout << "Klaida eiluteje " << lineNumber << "! Netinkamas simbolis '" << input << "' studentui(-ei): " 
                         << studentas.vardas() << " " << studentas.pavarde() << "." << endl;
                    cout << "Iveskite tinkama rezultata (skaicius nuo 0 iki 10): ";
                    while (!(cin >> nd) || nd < 0 || nd > 10) {
                        cout << "Klaida! Iveskite skaiciu tarp 0 ir 10: ";
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }
                    namuDarbai.push_back(nd);
                } catch (const out_of_range&) {
                    cout << "Klaida eiluteje " << lineNumber << "! Rezultatas '" << input << "' ne intervale [0, 10] studentui(-ei): " 
                         << studentas.vardas() << " " << studentas.pavarde() << "." << endl;
                    cout << "Iveskite tinkama rezultata (skaicius nuo 0 iki 10): ";
                    while (!(cin >> nd) || nd < 0 || nd > 10) {
                        cout << "Klaida! Iveskite skaiciu tarp 0 ir 10: ";
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }
                    namuDarbai.push_back(nd);
                }
            }

            if (namuDarbai.empty()) {
                cout << "Klaida eiluteje " << lineNumber << "! Nerasta namu darbu rezultatu studentui(-ei): " 
                     << studentas.vardas() << " " << studentas.pavarde() << "." << endl;
                continue;
            }

            rezultatai.setEgzaminas(namuDarbai.back());
            namuDarbai.pop_back();
            rezultatai.setNamuDarbai(namuDarbai);

            if (namuDarbai.empty()) {
                cout << "Klaida eiluteje " << lineNumber << "! Nerasta namu darbu rezultatu studentui(-ei): " 
                     << studentas.vardas() << " " << studentas.pavarde() << " (tik egzamino rezultatas)." << endl;
                continue;
            }

            studentai.push_back(studentas);
        }

        failas.close();
        break; 
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    double readingTime = diff.count();
    
    veikimoGreicioRezultatai[failoPavadinimas].setSkaitymoLaikas(readingTime);
    
    return readingTime;
}


// Funkcija generuojanti failus
double generuotiFailus(int studentuKiekis, int namuDarbaiKiekis, const string &filePrefix) {
    auto start = std::chrono::high_resolution_clock::now();
    string fileName = filePrefix + to_string(studentuKiekis) + ".txt";
    ofstream failas(fileName);
    if (!failas) {
        throw runtime_error("Klaida! Nepavyko sukurti failo: " + fileName);
    }

    failas << left << setw(20) << "Vardas" << setw(20) << "Pavarde";
    for (int i = 1; i <= namuDarbaiKiekis; ++i) {
        failas << "ND" << setw(8) << i;
    }
    failas << "Egz." << endl;

    for (int i = 1; i <= studentuKiekis; ++i) {
        failas << left << setw(20) << "Vardas" + to_string(i) 
               << setw(20) << "Pavarde" + to_string(i);
        
        for (int j = 0; j < namuDarbaiKiekis; ++j) {
            failas << setw(10) << (rand() % 11);
        }
        failas << (rand() % 11) << endl;
    }

    failas.close();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    double creationTime = diff.count();
    
    cout << "\nFailas " << fileName << " buvo sukurtas." << endl;
    cout << "Failo kurimo laikas: " << creationTime << " sekundziu" << endl;
    
    veikimoGreicioRezultatai[fileName].setKurimoLaikas(creationTime);
    
    return creationTime;
}


// Funkcija rezultatu rodymui ekrane
void rodytiRezultatus(const list<Studentas>& studentai, bool naudotiVidurki) {
    cout << "\n" << endl;
    cout << left << setw(18) << "Vardas"
         << setw(22) << "Pavarde"
         << setw(25) << "Adresas atmintyje"
         << (naudotiVidurki ? "Galutinis (Vid.)" : "Galutinis (Med.)") << endl;
    cout << string(83, '-') << endl;

    for (const auto& studentas : studentai) {
        double galutinis = studentas.skaiciuotiGalutiniBala(naudotiVidurki);
        cout << left << setw(18) << studentas.vardas()
             << setw(22) << studentas.pavarde()
             << setw(25) << &studentas //adresas atmintyje
             << fixed << setprecision(2) << galutinis << endl;
    }
}


// Funkcija greicio rezultatu rodymui
void rodytiGreicioAnalizesRezultatus(const string& failoPavadinimas) {
    const auto& rezultatai = veikimoGreicioRezultatai[failoPavadinimas];
    cout << "\nGreicio analizes rezultatai failui: " << failoPavadinimas << endl;
    cout << "- Failo skaitymo laikas: " << rezultatai.getSkaitymoLaikas() << " sekundziu" << endl;
    cout << "- Rusiavimo i dvi grupes laikas: " << rezultatai.getRusiavimoLaikas() << " sekundziu" << endl;
    cout << "- Rasymo laikas (vargsiukai): " << rezultatai.getRasymoLaikasVargsiukai() << " sekundziu" << endl;
    cout << "- Rasymo laikas (galvociai): " << rezultatai.getRasymoLaikasGalvociai() << " sekundziu" << endl;
    
    double bendrasLaikas = rezultatai.getSkaitymoLaikas() + rezultatai.getRusiavimoLaikas() + 
                          rezultatai.getRasymoLaikasVargsiukai() + rezultatai.getRasymoLaikasGalvociai();
    cout << "Bendras laikas: " << bendrasLaikas << " sekundziu" << endl;
}


// Funkcija failu apdorojimo ciklui
void failuApdorojimoCiklas() {
    string failoPavadinimas;
    char testiApdorojima = 't';
    list<string> apdorotiFailai;

    while (tolower(testiApdorojima) == 't') {
        list<Studentas> studentai;

        cout << "\nIveskite failo pavadinima (pvz., studentai.txt): ";
        getline(cin, failoPavadinimas);
        
        // Tikrinama ar failas jau buvo apdorotas
        if (find(apdorotiFailai.begin(), apdorotiFailai.end(), failoPavadinimas) != apdorotiFailai.end()) {
            cout << "\nSis failas jau buvo apdorotas! Pasirinkite kita faila." << endl;
            continue;
        }

        try {
            nuskaitytiDuomenisIsFailo(failoPavadinimas, studentai);
            apdorotiFailai.push_back(failoPavadinimas); // Pridedame i apdorotu failu sarasa
        } catch (const runtime_error& e) {
            cout << e.what() << " Bandykite dar karta." << endl;
            continue;
        }
        
        if (studentai.empty()) {
            cout << "Nera studentu duomenu faile!" << endl;
            continue;
        }

        char metodoPasirinkimas;
        do {
            metodoPasirinkimas = gautiTinkamaSymboli(
            "\nAr norite skaiciuoti studento galutini bala pagal:\n"
            "* vidurki (v),\n"
            "* mediana (m)?\n"
            "(Iveskite 'v' arba 'm'!): ",
            "'v' arba 'm'!\n");
        } while (tolower(metodoPasirinkimas) != 'v' && tolower(metodoPasirinkimas) != 'm');

        bool naudotiVidurki = (tolower(metodoPasirinkimas) == 'v');

        try {
            studentai.sort(lygintiPagalVardaIrPavarde);
            rodytiRezultatus(studentai, naudotiVidurki);
        } catch (const exception& e) {
            cout << "Ivyko klaida apdorojant duomenis: " << e.what() << endl;
            continue;
        }

        do {
            testiApdorojima = gautiTinkamaSymboli("\nAr norite nuskaityti kita faila? (Iveskite 't' - TAIP arba 'n' - NE): ", "'t' arba 'n'!");
        } while (tolower(testiApdorojima) != 't' && tolower(testiApdorojima) != 'n');
    }
}


// Funkcija greicio analizei atlikti
void greicioAnalize() {
    string failoPavadinimas;
    char testiApdorojima = 't';
    list<string> apdorotiFailai;

    while (testiApdorojima == 't' || testiApdorojima == 'T') {
        list<Studentas> studentai;

        cout << "\nIveskite failo pavadinima (pvz., studentai.txt): ";
        getline(cin, failoPavadinimas);
        
        try {
            nuskaitytiDuomenisIsFailo(failoPavadinimas, studentai);
        } catch (const runtime_error& e) {
            cout << e.what() << " Bandykite dar karta." << endl;
            continue;
        }

        char metodoPasirinkimas = gautiTinkamaSymboli(
            "\nAr norite skaiciuoti studento galutini bala pagal:\n"
            "* vidurki (v),\n"
            "* mediana (m)?\n"
            "(Iveskite 'v' arba 'm'!): ",
            "'v' arba 'm'!\n");
        bool naudotiVidurki = (metodoPasirinkimas == 'v');

        char strategijaPasirinkimas = gautiTinkamaSymboli(
            "\nPasirinkite rusiavimo strategija:\n"
            "* 1 - strategija (du nauji konteineriai)\n"
            "* 2 - strategija (vienas naujas konteineris)\n"
            "* 3 - strategija (optimizuota)\n"
            "* 4 - palyginti visas strategijas\n"
            "(Iveskite '1', '2', '3' arba '4'): ",
            "'1', '2', '3' arba '4'!\n");

        
        if (strategijaPasirinkimas == '4') {
            list<Studentas> studentaiKopija = studentai;
            palygintiStrategijas(studentaiKopija, naudotiVidurki);
            
            testiApdorojima = gautiTinkamaSymboli(
                "\nAr norite nuskaityti kita faila?\n"
                "(Iveskite 't' - TAIP arba 'n' - NE): ",
                "'t' arba 'n'!\n");
            continue;
        }

        
        char rusiavimoPasirinkimas = gautiTinkamaSymboli(
            "\nKaip norite surusiuoti studentus?\n"
            "* 1 - pagal galutini bala didejimo tvarka\n"
            "* 2 - pagal galutini bala mazejimo tvarka\n"
            "* 3 - pagal varda\n"
            "* 4 - pagal pavarde\n"
            "* 5 - pagal varda ir pavarde\n"
            "(Iveskite '1', '2', '3', '4' arba '5'!): ",
            "'1', '2', '3', '4' arba '5'!\n");

        
        if (rusiavimoPasirinkimas == '1') {
            studentai.sort(RusiuotiPagalGalutiniBalaDidejanciai(naudotiVidurki));
        } else if (rusiavimoPasirinkimas == '2') {
            studentai.sort(RusiuotiPagalGalutiniBalaMazejanciai(naudotiVidurki));
        } else if (rusiavimoPasirinkimas == '3') {
            studentai.sort(lygintiPagalVarda);
        } else if (rusiavimoPasirinkimas == '4') {
            studentai.sort(lygintiPagalPavarde);
        } else {
            studentai.sort(lygintiPagalVardaIrPavarde);
        }

        list<Studentas> vargsiukai, galvociai;
        double rusiavimoLaikas = 0.0;
        list<Studentas> studentaiKopija = studentai;
        
        switch(strategijaPasirinkimas) {
            case '1':
                rusiavimoLaikas = rusiuotiStudentusStrategija1(studentai, vargsiukai, galvociai, naudotiVidurki);
                break;
            case '2':
                rusiavimoLaikas = rusiuotiStudentusStrategija2(studentaiKopija, vargsiukai, naudotiVidurki);
                galvociai = studentaiKopija;
                break;
            case '3':
                rusiavimoLaikas = rusiuotiStudentusStrategija3(studentaiKopija, vargsiukai, naudotiVidurki);
                galvociai = studentaiKopija;
                break;
        }

        veikimoGreicioRezultatai[failoPavadinimas].setRusiavimoLaikas(rusiavimoLaikas);

        string vargsiukaiFailoPavadinimas = "vargsiukai_" + failoPavadinimas;
        string galvociaiFailoPavadinimas = "galvociai_" + failoPavadinimas;

        double rasymoLaikasVargsiukai = rasytiStudentusIFaila(vargsiukai, vargsiukaiFailoPavadinimas, naudotiVidurki);
        double rasymoLaikasGalvociai = rasytiStudentusIFaila(galvociai, galvociaiFailoPavadinimas, naudotiVidurki);

        veikimoGreicioRezultatai[failoPavadinimas].setRasymoLaikasVargsiukai(rasymoLaikasVargsiukai);
        veikimoGreicioRezultatai[failoPavadinimas].setRasymoLaikasGalvociai(rasymoLaikasGalvociai);

        cout << "\nStudentai buvo surusiuoti ir irasyti i failus '" 
             << vargsiukaiFailoPavadinimas << "' ir '" 
             << galvociaiFailoPavadinimas << "'." << endl;

        apdorotiFailai.push_back(failoPavadinimas);

        testiApdorojima = gautiTinkamaSymboli(
            "\nAr norite nuskaityti kita faila?\n"
            "(Iveskite 't' - TAIP arba 'n' - NE): ",
            "'t' arba 'n'!\n");
    }

    if (!apdorotiFailai.empty()) {
    cout << "\nRezultatai visiems apdorotiems failams:" << endl;
    double bendrasLaikas = 0.0;

    for (const auto& failoPav : apdorotiFailai) {
        rodytiGreicioAnalizesRezultatus(failoPav);
        cout << endl;
        
        auto& rezultatai = veikimoGreicioRezultatai[failoPav];
        
        bendrasLaikas += rezultatai.getSkaitymoLaikas() + rezultatai.getRusiavimoLaikas() + rezultatai.getRasymoLaikasVargsiukai() + rezultatai.getRasymoLaikasGalvociai();
    }

    double vidutinisLaikas = bendrasLaikas / apdorotiFailai.size();
    cout << "Vidutinis laikas visiems apdorotiems failams: " << vidutinisLaikas << " sekundziu" << endl;
    }
}
