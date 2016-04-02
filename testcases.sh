OriginalDir=./testcases/original
ResultDir=./testcases/result
CSCHEME="./cscheme --debug "
CSCHEMEC="./cschemec --debug "
CSCHEMEA="./cschemea --debug "

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
    $CSCHEME ./testcases/syntax_num.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="syntax_lambda.test"
    $CSCHEME ./testcases/syntax_lambda.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="syntax_app.test"
    $CSCHEME ./testcases/syntax_app.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="syntax_if.test"
    $CSCHEME ./testcases/syntax_if.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="syntax_set.test"
    $CSCHEME ./testcases/syntax_set.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="syntax_define.test"
    $CSCHEME ./testcases/syntax_define.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="syntax_load.test"
    $CSCHEME ./testcases/syntax_load.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="syntax_complex.test"
    $CSCHEME ./testcases/syntax_complex.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="syntax_error_1.test"
    $CSCHEME ./testcases/syntax_error_1.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="syntax_error_2.test"
    $CSCHEME ./testcases/syntax_error_2.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="syntax_error_3.test"
    $CSCHEME ./testcases/syntax_error_3.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="syntax_error_4.test"
    $CSCHEME ./testcases/syntax_error_4.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="syntax_error_5.test"
    $CSCHEME ./testcases/syntax_error_5.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="cyclic_load.test"
    $CSCHEME ./testcases/cyclic_load.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="interpreter_1.test"
    $CSCHEME ./testcases/interpreter_1.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="interpreter_2.test"
    $CSCHEME ./testcases/interpreter_2.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="interpreter_3.test"
    $CSCHEME ./testcases/interpreter_3.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="cyclic_load.test"
    $CSCHEME ./testcases/cyclic_load.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*
    
    File="print_0_interpreter.test"
    $CSCHEME ./testcases/print_0.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="print_80_interpreter.test"
    $CSCHEME ./testcases/print_80.scm > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="print_80_compiler.test"
    $CSCHEMEC ./testcases/print_80.scm
    ./testcases/print_80.scm.out > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="read_write_interpreter.test"
    $CSCHEME ./testcases/input.scm < ./testcases/input > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="read_write_compiler.test"
    $CSCHEMEC ./testcases/input.scm
    ./testcases/input.scm.out < ./testcases/input > $ResultDir/$File 2> $ResultDir/$File
    Compare $*

    File="pretty_print.test"
    $CSCHEMEA ./testcases/pretty_print.scm > $ResultDir/$File 2> $ResultDir/$File
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
