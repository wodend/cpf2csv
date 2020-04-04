#!/usr/bin/python

import ast
import re
import pandas as pd

def getclasses(row, course):
    pdfstr = row["pdfstr"]
    row["classes"] = re.findall(course, pdfstr)
    return row

def main():
    df = pd.read_csv("flowchart.csv")
    course = re.compile(r"[A-Z]{2,4}\s\d{3}")
    df = df.apply(getclasses, 1, args=(course,)).drop("pdfstr", 1)
    df = pd.DataFrame(df.classes.tolist(), index=[df.major, df.quarter])\
        .stack().reset_index().drop("level_2", 1)\
        .rename(columns={0: "classes"})
    df.to_csv("classes.csv")

if __name__ == "__main__":
    main()
