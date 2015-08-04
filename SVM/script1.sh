for i in {70..99} ; 
	do 
		for j in {1..10} ; 
			do 
				./svm_multiclass_classify example5/MetalTest/metal$i/metal.000$i.$j.txt example5/model.dat output$j.dat; 
			done ;
			
			gcc last_step.c && ./a.out ;
			rm result.txt; 
	done