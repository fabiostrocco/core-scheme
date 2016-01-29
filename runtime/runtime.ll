@default_plus = global i32 0
@default_minus = global i32 0
@default_times = global i32 0
@default_divide = global i32 0
@default_equals = global i32 0
@default_less = global i32 0
@default_greater = global i32 0
@default_input = global i32 0
@default_output = global i32 0

;These functions have to be provided into a native file
;for example a runtime C file and liked to the
;binary generated after the linkage with llvm
declare i32 @printf(i8*, ...)
declare i32 @gcc_print(i32)
declare i32 @gcc_input()

@.out = private constant [12 x i8] c"Output: %d\0A\00"
@.in = private constant [11 x i8] c"Input: %d\0A\00"
@.infmt = private unnamed_addr constant [3 x i8] c"%d\00", align 1

define i32 @init() {
entry:
  %plus = ptrtoint i32 (i32, i32)* @internal_add to i32
  store i32 %plus, i32 *@default_plus

  %minus = ptrtoint i32 (i32, i32)* @internal_sub to i32
  store i32 %minus, i32 *@default_minus

  %mult = ptrtoint i32 (i32, i32)* @internal_times to i32
  store i32 %mult, i32 *@default_times

  %divs = ptrtoint i32 (i32, i32)* @internal_divide to i32
  store i32 %divs, i32 *@default_divide

  %eq = ptrtoint i32 (i32, i32)* @internal_equals to i32
  store i32 %eq, i32 *@default_equals

  %lt = ptrtoint i32 (i32, i32)* @internal_lt to i32
  store i32 %lt, i32 *@default_less

  %gt = ptrtoint i32 (i32, i32)* @internal_gt to i32
  store i32 %gt, i32 *@default_greater

  %print = ptrtoint i32 (i32)* @gcc_print to i32
  store i32 %print, i32 *@default_output

  %input = ptrtoint i32 ()* @gcc_input to i32
  store i32 %input, i32 *@default_input

  ret i32 0
}

define i32 @internal_add(i32 %a, i32 %b) {
entry:
  %x = add i32 %a, %b
  ret i32 %x
}

define i32 @internal_sub(i32 %a, i32 %b) {
entry:
  %x = sub i32 %a, %b
  ret i32 %x
}

define i32 @internal_times(i32 %a, i32 %b) {
entry:
  %x = mul i32 %a, %b
  ret i32 %x
}

define i32 @internal_divide(i32 %a, i32 %b) {
entry:
  %x = udiv i32 %a, %b
  ret i32 %x
}

define i32 @internal_equals(i32 %a, i32 %b) {
entry:
  %res = icmp eq i32 %a, %b
  br i1 %res, label %else, label %then

then:
  ret i32 0

else:
  ret i32 1
}

define i32 @internal_lt(i32 %a, i32 %b) {
entry:
  %res = icmp slt i32 %a, %b
  br i1 %res, label %else, label %then

then:
  ret i32 0

else:
  ret i32 1
}

define i32 @internal_gt(i32 %a, i32 %b) {
entry:
  %res = icmp sgt i32 %a, %b
  br i1 %res, label %else, label %then

then:
  ret i32 0

else:
  ret i32 1
}
