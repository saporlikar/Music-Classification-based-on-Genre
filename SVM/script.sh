gcc mfcc_opt1.c -lm -o mfcc_opt1 && ./mfcc_opt1 $1;
mv *.txt example5/;
for i in {1..10} ; do ./svm_multiclass_classify example5/$1.$i.txt example5/model.dat output$i.dat; done
gcc last_step.c;
./a.out