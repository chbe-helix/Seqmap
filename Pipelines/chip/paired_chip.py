#run at '/cellar/users/btsui/Project/METAMAP/notebook/RapMapTest/Pipelines/'
import os,sys
sys.path+=['/cellar/users/btsui/Project/METAMAP/notebook/RapMapTest/Pipelines/']
import pandas as pd
import subprocess
from multiprocessing import Process
import param
def startChildren(cmd):
    def f():
        os.system(cmd) 
    p = Process(target=f)
    p.start()
    return p
TEST=False

#srrRun=sys.argv[1]

#specie=sys.argv[2]
if TEST:
    srrRun='SRR5048916'
    specie='Homo_sapiens'
else:
    srrRun=sys.argv[1]
    specie=sys.argv[2]
    
print srrRun,specie
baseGenomesDir='/cellar/users/btsui/Data/BOWTIE2_GENOME_INDEX/'
#out_dir='
base_dict_dir='/cellar/users/btsui/Data/Project/Skymap/ChipSeq/empty_references/'
log_out_dir=param.log_out_dir#'/cellar/users/btsui/Data/SRA/all_seq/log/'
count_out_dir=param.count_out_dir#'/cellar/users/btsui/Data/SRA/all_seq/chip/'
genomeDir=baseGenomesDir+specie+'/'
tmp_dir="/tmp/SRA_DATA/"

count_script_dir='/cellar/users/btsui/Project/METAMAP/notebook/RapMapTest/Chip-seq/a.out'
SRA_FASTQ_TOOL_DIR="/cellar/users/btsui/Program/SRA_TOOL_KIT/sratoolkit.2.4.2-ubuntu64/bin/fastq-dump.2.4.2"
myoption=r'"/cellar/users/btsui/.aspera/connect/bin/ascp|/cellar/users/btsui/.aspera/connect/etc/asperaweb_id_dsa.openssh"'
trim_galore_Dir='/cellar/users/btsui/Program/TRIMAGLORE//trim_galore'
base_sra_dir='/tmp/btsui/METH/sra/'

##make temperoary directory
job_tmp_dir=tmp_dir+srrRun+"/"
subprocess.call(["rm", "-rf",job_tmp_dir])#remove the directory before running STAR
subprocess.call(["mkdir", "-p",job_tmp_dir]) #tmp dir for processing
os.chdir(job_tmp_dir)
##download sra file
downloadCommand=['prefetch','-t','ascp','--ascp-path',myoption,' --ascp-options "-l 2000000" ',srrRun]
os.system(' '.join(downloadCommand))

##identify adaptor contents using first 10000 reads
#--paired
myCmd='{sra_dump_dir} --split-files --stdout {sra_file_dir} |head -n 16000 > head.fq'.format(sra_dump_dir=SRA_FASTQ_TOOL_DIR,                                    sra_file_dir=base_sra_dir+srrRun+'.sra',trim_galore_Dir=trim_galore_Dir)
os.system(myCmd) 
os.system('/cellar/users/btsui/Project/METAMAP/notebook/RapMapTest/Pipelines/decouple_fastq.py head.fq')

os.system(trim_galore_Dir+' --paired '+' head.fq_R1 head.fq_R2')
##identify adaptor contents
fq_trimming_report_S=pd.Series.from_csv('head.fq_R1_trimming_report.txt',index_col=None,sep='\t\t\t\t\\t\t\tt')
adapter_sequence_1=fq_trimming_report_S.str.extract("Adapter sequence: '(\w+)'",expand=False).dropna().iloc[0]


fq_trimming_report_S=pd.Series.from_csv('head.fq_R2_trimming_report.txt',index_col=None,sep='\t\t\t\t\\t\t\tt')
adapter_sequence_2=fq_trimming_report_S.str.extract("Adapter sequence: '(\w+)'",expand=False).dropna().iloc[0]

##set up pipe for counting the data
##remove pipes if they exist 
os.system('rm {srrRun}_bowtie {srrRun}_count file_sorted'.format(srrRun=srrRun))
os.system('mkfifo {srrRun}_bowtie {srrRun}_count file_sorted'.format(srrRun=srrRun))

#my_dict_dir=base_dict_dir+specie+'.size.tsv'
#cmd_0='{count_script_dir} {my_dict_dir} {srrRun}_count'.format(
#    count_script_dir=count_script_dir,srrRun=srrRun,my_dict_dir=my_dict_dir)
if TEST:#in test case, for min read length
    cmd_4="genomeCoverageBed -pc -ibam file_sorted -bg| awk '{ if ($4 >= 1) { print } }' >out.bg"
else:
    cmd_4="genomeCoverageBed -pc -ibam file_sorted -bg| awk '{ if ($4 >= 5) { print } }' >out.bg"
    
cmd_0='samtools view -bS {srrRun}_count | samtools sort -  > file_sorted'.format(srrRun=srrRun)
cmd_1='bowtie2  -x {ref} --interleaved {srrRun}_bowtie --threads 2 > {srrRun}_count 2>bowtie_log.txt'.format(ref=genomeDir,srrRun=srrRun)
#take in and sort it 

if TEST:
    cmd_2_fmt='{sra_dump_dir} --stdout --split-files {sra_file_dir} | head -n 80000 |cutadapt --interleaved -a {adapter_sequence_1} -A {adapter_sequence_2} - > {srrRun}_bowtie'
else:
    cmd_2_fmt='{sra_dump_dir} --stdout --split-files {sra_file_dir} |cutadapt --interleaved -a {adapter_sequence_1} -A {adapter_sequence_2} - > {srrRun}_bowtie'
    
cmd_2=cmd_2_fmt.format(
 sra_file_dir=base_sra_dir+srrRun+'.sra',srrRun=srrRun,adapter_sequence_1=adapter_sequence_1,adapter_sequence_2=adapter_sequence_2,sra_dump_dir=SRA_FASTQ_TOOL_DIR)
print cmd_0
print cmd_1
print cmd_2
my_p_l=[startChildren(cmd_0),startChildren(cmd_1),startChildren(cmd_2)]
os.system(cmd_4)
#os.system('gzip out.bg')
tmp_cmd='gzip -c out.bg >  {count_out_dir}/{srrRun}.bg.gz'.format(count_out_dir=count_out_dir,srrRun=srrRun)
os.system(tmp_cmd)

os.system('echo ">>>>bowtie2 log" >log.txt')
os.system('tail -n 10 bowtie_log.txt >>log.txt')
os.system('echo ">>>>head.fq_trimming_report.txt">>log.txt')
os.system('cat head.fq_R1_trimming_report.txt >> log.txt')
os.system('cat head.fq_R2_trimming_report.txt >> log.txt')

os.system('cp log.txt {log_out_dir}/{srrRun}.log'.format(log_out_dir=log_out_dir,
                                                        srrRun=srrRun))


os.system('rm -r '+job_tmp_dir)
os.system('rm '+base_sra_dir+srrRun+'.sra')
os._exit(0)