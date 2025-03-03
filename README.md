# Baza szeregów czasowych

## 1. Wprowadzenie

Celem tego projektu jest stworzenie bazy zoptymalizowanej do zastosowań z danymi o charakterystyce szeregów czasowych.

## 2. Charakterystyka problemu

Bazy szeregów czasowych przechowują olbrzymią ilość danych, które są zapisywane i
odczytywane w kontekście momentu ich "utworzenia". Najpowszechniejszymi zastosowaniami takich baz to zbieranie metryk czy wydarzeń mających
wyłącznie znaczenie w kontekście ich czasu wystpąpienia. Dane takie charakteryzują się dużą powtarzalnością oraz bardzo dużą częstotliwością zapisów
(dużo większą niż odczytów co jest nietypowe w kontekście baz danych). Dodatkowo ze względu na charakterystyke zastosowań bazy takie zwykle nie wymagają ściłej
spójności (w przypadku skalowania bazy na wiele instancji).

W związku z powyższym kluczowe jest aby impementacja takiej bazy była zoptymalizowana pod kątem
**nieblokujących i efektywnych zapisów**.

## 3. Obecne rozwiązania

Popularne rozwiązania oferujące bazy szeregów czasowych takie jak Prometheus, ScyllaDB czy Gorilla wykorzystują mechanizmy
takie jak Log structured merge tree (LSM-tree), SSTables czy Memtables w roli write back cache'u. Są to podstawowe bloki
pozwalające na budowe bazy szeregów czasowych. Poprawne implementacja wyłącznie tych struktur powinna wystarczyć do uzyskania
zadawalających wyników.

## 4. Wymagania funkcjonalne

- odczyt danych na podstawie zakresu sygnatury czasowej rekordów
- wartości rekordów powinny być typu liczbowego (całkowitoliczbowego lub zmiennoprzecinkowego), analogicznie jak w Prometheus.
- zapisywanie nowych rekordów wraz z ich sygnaturą czasową

## 5. Wymagania niefunkcjonalne

- optymalizacja nieblokujących zapisów do bazy danych
- wykorzystanie plików w systemie hosta do zapisu stanu bazy danych

## 6. Architektura systemu

System powinien korzystać z mechanizmów stanowiących state-of-the-art w kontekście baz time-series.

Zapis do bazy podzielony jest na dwa etapy:

- Zapis do memtable, zaimplementowanej jako drzewo czerwono-czarne
- Zapisz rekordu do commit loga (aby nie utracić danych w przypadku awarii)
- Przy wypełneniu memtable zapis do SSTable
- Na podstawie określonych kryterów (czasowe lub pamięciowe), powstałe SSTable zostają scalane aby zoptymalizować zapytania Dzięki podejściu append-only, większość operacji może nie być blokujących

Odczyt do bazy jest wykonywany poprzez sprawdzenie obecności rekordów z danego zakresu w memtable oraz wszystkich odpowiadających sstables.
TSDB będzie przechowywała pojedyncze sygnatury czasowe szeregów czasowych z odniesieniem do pliku
I offsetu w którym się znajduje. Korzystając z faktu że SSTables posiadają dane, które
są posortowane to pozwoli nam to na ograniczenie przeszukiwania kluczy z całego pliku do
tylko określonego zakresu, którego wielkośc może byś dynamicznie dostosowywana (kosztem zużycia pamięci)

Każdy szereg posiada osobny mechanizm LSM ze względu na typową charakterystyke zapytań, która nie miesza szeregów.

## 7. Przykładowe przypadki użycia

- rejestrowanie metryk z systemów komputerowych (zużycie procesora, statystyki sieci/pakietów, wykonań konkretnych ścieżek w kodzie źródłowym)
- dane analityczne związane z systemami webowymi (analogiczne do google analytics / clickhouse)

## 8. Podsumowanie i dalsze kroki

W ramach realizacji projektu zadaniami z największym priorytetem są:

- implementacja LSM-tree / memtable / SSTables do efektywnego zapisu I odczytu danych z pliku
- implementacja metod do interfejsowania z bazą danych w ramach biblioteki w C++
- wsparcie dla własnościowego języka zapytań, jego parsera i executora

Dodatkowymi zaganieniami wartymi poruszenia w przypadku nadmiernego czasu:

- wsparcie etykiet / tagów pozwalających na wygodniejsze zastosowanie w metrykach (analogicznie do Prometheus)
- wsparcie efektywnego kodowania rekordów korzystając z powtarzalnej natury rekordów
- wykorzystanie Bloom filter do optymalizacji zapytań
- dodanie serwera TCP/UDP do obsługi zapytań poprzez sieć

## 7. Środowisko programistyczne

- Edytor kodu: Nvim (clangd, lldb) , Clion
- test suite: googletest
- kompilator: gcc
- build system: cmake
- zewnętrzne zależności: boost
- System kontroli wersji: git

## 8. Źródła, referencje, inspiracje:

- https://www.alibabacloud.com/blog/598114?spm=a3c0i.29367734.6737026690.4.235c7d3fJQAJtP
- https://opensource.docs.scylladb.com/stable/architecture/sstable/sstable3/sstables-3-data-file-format.html
- https://nakabonne.dev/posts/write-tsdb-from-scratch/
- https://en.wikipedia.org/wiki/Bloom_filter
- https://www.vldb.org/pvldb/vol8/p1816-teller.pdf
- https://www.scylladb.com/glossary/log-structured-merge-tree/
- https://kvs-vishnu23.medium.com/deep-dive-into-prometheus-4790a5916370
- https://www.geeksforgeeks.org/introduction-to-log-structured-merge-lsm-tree/
- https://www.geeksforgeeks.org/sstable-in-apache-cassandra/
- https://blog.cloudflare.com/how-cloudflare-runs-prometheus-at-scale/
- https://cloud.google.com/bigtable/docs/schema-design-time-series
