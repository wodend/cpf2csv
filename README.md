# cpf2csv
Create .csv files from California Polytechnic State University
[degree flowcharts](https://flowcharts.calpoly.edu/).

# Requirements
- C compiler
- [Poppler](https://poppler.freedesktop.org/) PDF rendering library (glib)

# Usage
./cpf2csv [OPTIONS] PDF [CSV]
Create .csv from PDF, defaulting to the same file name if CSV isn't specified.

  -n  Specify the number of quarters for the major (default 20)

The resulting .csv will be a list of required classes for the major including
what quarter the class is requried in.

# FAQ
- **What is this for?**  
  - This was made for a research project. We needed a list of required classes
  for each major, and the university was unable to provide us with this, so I
  wrote this to scrape the data from public sources.
