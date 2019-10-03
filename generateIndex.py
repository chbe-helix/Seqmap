import pandas as pd
from argparse import ArgumentParser


def makeIndex(inputDataBaseName, outputDir):
    subsetPickleDf = pd.read_csv(inputDataBaseName, sep = '\t')
    subsetPickleDf = subsetPickleDf.sort_values(by=['ScientificName'])

    col = subsetPickleDf['ScientificName']
    ScientificNmaeDic = {}
    for count, name in enumerate(col):
        line = count + 1
        if name in ScientificNmaeDic:
            ScientificNmaeDic[name][1] = line
        else:
            ScientificNmaeDic[name] = [line, line]

    #subsetPickleDf['proj_accession_Published'] = subsetPickleDf['proj_accession_Published'].str.replace('-', '')

    output_sra_dump_dir_index = outputDir + '/sra_dump_index.tsv'
    f_index = open(output_sra_dump_dir_index, 'w')
    for name, line in ScientificNmaeDic.items():
        f_index.write('%s\t%s\t%s\n' % (name, line[0], line[1]))
    f_index.close()

if __name__ == '__main__':

    parser = ArgumentParser()

    parser.add_argument("--out-dir", "-o",
                        dest="out_dir",
                        type=str,
                        default=".",
                        help="Directory name for output files")
    parser.add_argument("-i",
                        dest="dataBaseName",
                        type=str,
                        default="",
                        help="")

    args = parser.parse_args()

    outputDir = args.out_dir
    inputDataBaseName = args.dataBaseName
    makeIndex(inputDataBaseName, outputDir)
