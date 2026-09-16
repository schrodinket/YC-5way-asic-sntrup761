import re, sys

infile, outfile, mainfunc = sys.argv[1], sys.argv[2], sys.argv[3]
src = open(infile).read()

# Functions to count: multiply and recombination (last argument is the size n)
fns = ['sb_comba_real','sb_comba_complex','ka2_real','ka2_real_32','ka2_real_64',
       'ka2_real_258','v1_complex','ub_real_29','ub_real_31','ub_real_33',
       'ub_real_65','ub_real_129','ub_complex_29','ub_complex_31','b1_real',
       'ue_v11_real','ue_v12_real','ue_v12_complex','ue_v14_real',
       'sum','sum_neg_neg','sum_multiply_w','difference','negation',
       'sum_real','sum_neg_neg_real','sum_re_im_real','sum_re_neg_im_real',
       'sum_im_im_real','sum_im_neg_im_real','difference_real','negation_real',
       'create_term','create_term_neg','phase_change']

lines = src.split('\n')
# add global counters: number of calls and total size per function
header = []
for f in fns:
    header.append(f'long CNT_{f}=0; long SZ_{f}=0;')
header_str = '\n'.join(header)

# add a counter increment at the start of each function body
out=[]
i=0
# insert the counter declarations after the includes
inserted=False
for idx,l in enumerate(lines):
    out.append(l)
    if not inserted and l.startswith('#include') and (idx+1>=len(lines) or not lines[idx+1].startswith('#include')):
        out.append(header_str)
        inserted=True

src2='\n'.join(out)
lines=src2.split('\n')
out=[]
for l in lines:
    out.append(l)
    m=re.match(r'void (\w+)\(.*int n\)\{?\s*$', l) or re.match(r'void (\w+)\(.*int n\)\s*\{\s*$', l)
    # function header of the form: void FN(... int n){
    m2=re.match(r'\s*void\s+(\w+)\s*\(.*\bint\s+n\)\s*\{?\s*$', l)
    if m2 and m2.group(1) in fns:
        fn=m2.group(1)
        # eger { ayni satirda degilse sonraki satira eklenecek; basitce hemen ekle
        if l.rstrip().endswith('{'):
            out.append(f'  CNT_{fn}++; SZ_{fn}+=n;')
        else:
            out.append('{')  # safety
            out.append(f'  CNT_{fn}++; SZ_{fn}+=n;')
src3='\n'.join(out)

# print the report at the end of main, just before return 0
report='\n  fprintf(stderr,"=== INVENTORY ===\\n");\n'
for f in fns:
    report += f'  if(CNT_{f}>0) fprintf(stderr,"%-24s calls=%ld total_size=%ld\\n","{f}",CNT_{f},SZ_{f});\n'

src3 = src3.replace('\treturn 0;', report+'\treturn 0;',1)
if report not in src3:
    src3 = src3.replace('return 0;', report+'return 0;',1)

open(outfile,'w').write(src3)
print(f"instrumented: {outfile}")
