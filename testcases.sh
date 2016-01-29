OriginalDir=./testcases/original
ResultDir=./testcases/result

Compare() {
    if diff $OriginalDir/$File $ResultDir/$File >/dev/null ; then
	echo "TEST $File PASSED"
    else
	echo ">>>>>>>>>>>>>>>>>>> TEST $File FAILED"
    fi
}

Init() {
  rm -f testcases/*.out
  rm -rf testcases/*.generated
}

Finalize() {
  rm -f testcases/*.out
  rm -rf testcases/*.generated
  rm -f *~
}

Main() {
    Init $*

    File="syntax_num.test"
    ./cscheme ./testcases/syntax_num.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="syntax_lambda.test"
    ./cscheme ./testcases/syntax_lambda.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="syntax_app.test"
    ./cscheme ./testcases/syntax_app.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="syntax_if.test"
    ./cscheme ./testcases/syntax_if.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="syntax_set.test"
    ./cscheme ./testcases/syntax_set.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="syntax_define.test"
    ./cscheme ./testcases/syntax_define.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="syntax_load.test"
    ./cscheme ./testcases/syntax_load.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="syntax_complex.test"
    ./cscheme ./testcases/syntax_complex.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="syntax_error_1.test"
    ./cscheme ./testcases/syntax_error_1.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="syntax_error_2.test"
    ./cscheme ./testcases/syntax_error_2.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="syntax_error_3.test"
    ./cscheme ./testcases/syntax_error_3.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="syntax_error_4.test"
    ./cscheme ./testcases/syntax_error_4.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="syntax_error_5.test"
    ./cscheme ./testcases/syntax_error_5.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="cyclic_load.test"
    ./cscheme ./testcases/cyclic_load.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="interpreter_1.test"
    ./cscheme ./testcases/interpreter_1.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="interpreter_2.test"
    ./cscheme ./testcases/interpreter_2.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="interpreter_3.test"
    ./cscheme ./testcases/interpreter_3.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="cyclic_load.test"
    ./cscheme ./testcases/cyclic_load.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*
    
    File="print_0_interpreter.test"
    ./cscheme ./testcases/print_0.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="print_80_interpreter.test"
    ./cscheme ./testcases/print_80.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="print_80_compiler.test"
    ./cschemec ./testcases/print_80.scm
    ./testcases/print_80.scm.out > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="read_write_interpreter.test"
    ./cscheme ./testcases/input.scm < ./testcases/input > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="read_write_compiler.test"
    ./cschemec ./testcases/input.scm
    ./testcases/input.scm.out < ./testcases/input > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="pretty_print.test"
    ./cschemea ./testcases/pretty_print.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    Finalize $*
}

echo "Starging testsuite"
echo "------------------"
echo ""

Main $*

echo "------------------"
echo ""
echo "Testuite complete"
