import subprocess

import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns

from matplotlib.backends.backend_pdf import PdfPages

sns.set(context="paper", font="serif", style="ticks", palette="deep")

# Maybe...
# matplotlib.rcParams["text.usetex"] = True

def savefig(name, **kwargs):
    fn = "../paper/fig/{}.pdf".format(name)
    pp = PdfPages(fn)
    pp.savefig(dpi=1200, bbox_inches="tight", **kwargs)
    pp.close()
    print("saving and cropping ", fn)
    subprocess.run(["pdfcrop", fn, fn])