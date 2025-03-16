all: afl-min afl-tmin
afl-min:
		rm -rf cmin_inputs
		afl-cmin -i all_inputs -o cmin_inputs -- ../worker @@

afl-tmin:
		./make_tmin	

copy_all_files:
		./copy_all_inputs.sh
